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
#include <Adafruit_PWMServoDriver.h>

//////////////////////////////////////////////////////////////////////////////
#include <RH_RF95.h>
#include <RHReliableDatagram.h>
#include <OSCBundle.h>
#include <string.h>
#include <SPI.h>
 
#define RFM95_CS 8
#define RFM95_RST 4

#ifdef __SAMD21G18A__
#define RFM95_INT 3 //Use this for the M0
#endif
#ifdef __AVR_ATmega32U4__
#define RFM95_INT 7 //Use this for the 32u4
#endif

#define CLIENT_ADDRESS 1
#define SERVER_ADDRESS 88
 
// Change to 434.0 or other frequency, must match RX's freq!
#define RF95_FREQ 915.0
 
// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

RHReliableDatagram manager(rf95, SERVER_ADDRESS);
uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];

//////////////////////////////////////////////////////////////////////////////
//OSC_interpret
union data_vals {
  int32_t i;
  float f;
  uint32_t u;
};

void get_OSC_string(OSCBundle *bndl, char *string) {
  char buf[50];
  char type;
  int n = 0;
  data_vals value;
  OSCMessage* msg = bndl->getOSCMessage(n);
  
  while(msg != NULL) {
    msg->getAddress(buf, 0);
    type = msg->getType(0);

    strcat(string, buf);

    if (type == 'f') {
      value.f = msg->getFloat(0);
     
      snprintf(buf, 50, " f%lu", value.u);
      strcat(string, buf);
    }
    else if (type == 'i') {
      value.i = msg->getInt(0);
      /*Serial.print("Value ");
      Serial.print(n+1);
      Serial.print(": ");
      Serial.println(value);*/

      snprintf(buf, 50, " i%lu", value.u);
      strcat(string, buf);
    }
    n++;
    msg = bndl->getOSCMessage(n);
    if (msg != NULL) strcat(string, " ");
  }
}

void get_OSC_bundle(char *string, OSCBundle* bndl) {
  bndl->empty();
  data_vals value_union;
  char buf[121];
  char *p = buf;
  char *addr = NULL, *value = NULL;
  strcpy(buf, string);
  addr = strtok_r(p, " ", &p);
  while (addr != NULL & strlen(addr) > 0) {
    value = strtok_r(NULL, " ", &p);
    value_union.u = strtoul(&value[1], NULL, 0);
    if (value[0] == 'f') {
      bndl->add(addr).add(value_union.f);
      Serial.print("Address: ");
      Serial.println(addr);
      Serial.print("Value: ");
      Serial.println(value_union.f);
    }
    else if (value[0] == 'i') {
      bndl->add(addr).add(value_union.i);
      Serial.print("Address: ");
      Serial.println(addr);
      Serial.print("Value: ");
      Serial.println(value_union.i);
    }

    addr = strtok_r(p, " ", &p);
  }
}
//////////////////////////////////////////////////////////////////////////////

// called this way, it uses the default address 0x40
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
// you can also call it with a different address you want eg. extra servo board
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
double pre_pulselength[1] = {0.0};
#elif (num_servos==2)
int predeg[2] = {0, 0};
double pre_pulselength[2] = {0.0, 0.0};
#elif (num_servos==3)
int predeg[3] = {0, 0, 0};
double pre_pulselength[3] = {0.0, 0.0, 0.0};
#elif (num_servos==4)
int predeg[4] = {0, 0, 0, 0};
double pre_pulselength[4] = {0.0, 0.0, 0.0, 0.0};
#elif (num_servos==5)
int predeg[5] = {0, 0, 0, 0, 0};
double pre_pulselength[5] = {0.0, 0.0, 0.0, 0.0, 0.0};
#elif (num_servos==6)
int predeg[6] = {0, 0, 0, 0, 0, 0};
double pre_pulselength[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
#elif (num_servos==7)
int predeg[7] = {0, 0, 0, 0, 0, 0, 0};
double pre_pulselength[7] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
#elif (num_servos==8)
int predeg[8] = {0, 0, 0, 0, 0, 0, 0, 0};
double pre_pulselength[8] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
#endif

//Global variables
int servo0 = 0;
int servo1 = 1;
int degree0, degree1 = 0;

//set_servo_degree function
// get "servo_choice" and "set_degree" from serial monitor
// and contol each servo's degree
void set_servo_degree(int set_degree, int servo_choice){ 
  uint16_t pulselength = map(set_degree, 0, 180, SERVOMIN, SERVOMAX);
  
  //When input degree is less than previous degree, it decreases
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

  //store degrees to display status
  if(servo_choice == servo0)
    degree0 = set_degree;
  else if(servo_choice == servo1)
    degree1 = set_degree;

}

void setup() {
  Serial.begin(9600);
  pwm.begin();         // setup the I2C interface and hardware
  pwm.setPWMFreq(60);  // Analog servos run at ~60 Hz updates
  
  if (!manager.init())
    Serial.println("init failed");

  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }

  rf95.setTxPower(23, false);
  
  yield();
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
  // Drive each servo one at a time
if (manager.available()) {
    uint8_t len = sizeof(buf);
    uint8_t from;
    memset(buf, '\0', RH_RF95_MAX_MESSAGE_LEN);
    if (manager.recvfromAck(buf, &len, &from)) {
      OSCBundle bndl;
      get_OSC_bundle((char*)buf, &bndl);

      //Insert operation code here
      //OSCBundle should container received data

      bndl.route("/LOOM/ServoShield0",handleServo);
       
      Serial.print("Received message from ");
      Serial.print(from);
      Serial.print(": ");
      Serial.println((char*)buf);
      bndl.send(Serial);
      Serial.println("");
    }
    
/*
  //input degree
  if (Serial.available()) {
    Serial.print("\n\nType servo# and degree (eg. 0 90):");
    int servo_choice = Serial.parseInt();
    int set_degree = Serial.parseInt();

    //define maximum and minimum input degree
    if (set_degree > 180) set_degree = 180;
    if (set_degree < 1) set_degree = 0;
    set_servo_degree(servo_choice, set_degree);
    
    //Display status
    Serial.print("\n[Servo0]: "); Serial.print(degree0); Serial.print(" degree");
    Serial.print("\n[Servo1]: "); Serial.print(degree1); Serial.print(" degree");   
*/
  }


}

