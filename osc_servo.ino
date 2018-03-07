//#include <OSCBoards.h>
//#include <OSCBundle.h>
//#include <OSCData.h>
//#include <OSCMatch.h>
//#include <OSCMessage.h>
//#include <OSCTiming.h>
#include <SLIPEncodedSerial.h>
#include <SLIPEncodedUSBSerial.h>

/*************************************************** 
  This is an example for our Adafruit 16-channel PWM & Servo driver
  Servo test - this will drive 16 servos, one after the other

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/products/815

  These displays use I2C to communicate, 2 pins are required to  
  interface. For Arduino UNOs, thats SCL -> Analog 5, SDA -> Analog 4

  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ****************************************************/

#include <Wire.h>
#include <WiFi101.h>
#include <WiFiUdp.h>
#include <OSCBundle.h>
#include <Adafruit_PWMServoDriver.h>

// called this way, it uses the default address 0x40
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
// you can also call it with a different address you want
//Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x41);

// Depending on your servo make, the pulse width min and max may vary, you 
// want these to be as small/large as possible without hitting the hard stop
// for max range. You'll have to tweak them as necessary to match the servos you
// have!
#define SERVOMIN  150 // this is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  600 // this is the 'maximum' pulse length count (out of 4096)
#define num_servos 2
// our servo # counter
#if (num_servos==1)
  int predeg[1] = {0};
  double pre_pulselength[1]= {0.0};
#elif (num_servos==2)
  int predeg[2] = {0, 0};
  double pre_pulselength[2] = {0.0,0.0};
#elif (num_servos==3)
  int predeg[3] = {0, 0, 0};
  double pre_pulselength[3] = {0.0,0.0,0.0};
#elif (num_servos==4)
  int predeg[4] = {0, 0, 0, 0};
  double pre_pulselength[4] = {0.0,0.0,0.0,0.0};
#elif (num_servos==5)
  int predeg[5] = {0, 0, 0, 0, 0};
  double pre_pulselength[5] = {0.0,0.0,0.0,0.0,0.0};
#elif (num_servos==6)
  int predeg[6] = {0, 0, 0, 0, 0, 0};
  double pre_pulselength[6] = {0.0,0.0,0.0,0.0,0.0,0.0};
#elif (num_servos==7)
  int predeg[7] = {0, 0, 0, 0, 0, 0, 0};
  double pre_pulselength[7] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0};
#elif (num_servos==8)
  int predeg[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  double pre_pulselength[8] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
#endif

// WiFi Stuff here:
int status = WL_IDLE_STATUS;
char ssid[] = "servo-network"; //  your network SSID (name)
char pass[] = "1234567890";    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;            // your network key Index number (needed only for WEP)
unsigned int localPort = 9436;      // local port to listen on

char packetBuffer[255]; //buffer to hold incoming packet
char  ReplyBuffer[] = "acknowledged";       // a string to send back

OSCErrorCode error;

WiFiUDP Udp;
void setup() {
  WiFi.setPins(8,7,4,2);
  Serial.begin(9600);

  pwm.begin();
  
  pwm.setPWMFreq(55);  // Analog servos run at ~60 Hz updates

  yield();
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true);
  }
  status = WiFi.beginAP(ssid);
  if (status != WL_AP_LISTENING) {
    Serial.println("Creating access point failed");
    // don't continue
    while (true);
  }
  delay(10000);

  // you're connected now, so print out the status
  printWiFiStatus();


  Serial.println("\nStarting connection to server...");
  // if you get a connection, report back via serial:
  Udp.begin(localPort);
}

// you can use this function if you'd like to set the pulse length in seconds
// e.g. setServoPulse(0, 0.001) is a ~1 millisecond pulse width. its not precise!
/*void setServoPulse(uint8_t n, double pulse) {
  double pulselength;
  
  pulselength = 1000000;   // 1,000,000 us per second
  pulselength /= 60;   // 60 Hz
  Serial.print(pulselength); Serial.println(" us per period"); 
  pulselength /= 4096;  // 12 bits of resolution
  Serial.print(pulselength); Serial.println(" us per bit"); 
  pulse *= 1000;
  pulse /= pulselength;
  Serial.println(pulse);
  pwm.setPWM(n, 0, pulse);
}
*/

void set_servo_degree(int set_degree, int servo_choice){
  uint16_t pulselength = map(set_degree, 0, 180, SERVOMIN, SERVOMAX);
  if(set_degree < predeg[servo_choice]){
    for (double pulselen = pre_pulselength[servo_choice]; pulselen >= pulselength; pulselen--) {
      pwm.setPWM(servo_choice, 0, pulselen);
    }
  }
  //When input degree is greater than previous degree, it increases
  if(set_degree > predeg[servo_choice]){
    for (double pulselen = pre_pulselength[servo_choice]; pulselen < pulselength; pulselen++) {
      pwm.setPWM(servo_choice, 0, pulselen);
    }
  }
  
  predeg[servo_choice] = set_degree;
  pre_pulselength[servo_choice] = pulselength;
}

void handleServo(OSCMessage &msg, int addrOffset){
  int set_degree;
  set_degree = msg.getInt(0);
  if (msg.fullMatch("/Servo0/Set",addrOffset))
    set_servo_degree(set_degree,0);
  #if num_servos > 1
  else if (msg.fullMatch("/Servo1/Set",addrOffset))
    set_servo_degree(set_degree,1);
  #endif
  // ADD checks for matching on Servo2, Servo3, etc.
}

void loop() {
  OSCBundle bndl;

  int packetSize = Udp.parsePacket();
  if (packetSize > 0){
    Serial.println("=========================================");
    Serial.print("received packet of size: ");
    Serial.println(packetSize);
    while (packetSize--){
      bndl.fill(Udp.read());
    }
    if (!bndl.hasError()){
      bndl.route("/LOOM/ServoShield0",handleServo);
    }
    else {
      error = bndl.getError();
      Serial.print("error: ");
      Serial.println(error);
    }
  }
//input degree
  /*if(Serial.available()){
    degree = Serial.parseInt();
    if(degree > 180) degree = 180;
    if(degree < 1) degree = 0;
    ser_servo_degree(degree);
    }*/
    
}

void printWiFiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

