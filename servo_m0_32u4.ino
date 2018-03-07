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
void set_servo_degree(int servo_choice, int set_degree){ 
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

  pwm.begin();

  pwm.setPWMFreq(60);  // Analog servos run at ~60 Hz updates
  
  yield();
}


void loop() {
  // Drive each servo one at a time

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

  }


}

