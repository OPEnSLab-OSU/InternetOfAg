#include <SPI.h>
#include <RH_RF95.h>
#include <RHReliableDatagram.h>
#include <OSCBundle.h>
#include <string.h>
//#include <EnableInterrupt.h>

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

//IDString constructor

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x) //to concatenate a predefined number to a string literal, use STR(x)

#define FAMILY "/LOOM"
#define DEVICE "/ServoShield"
#define INSTANCE_NUM 0  // Unique instance number for this device, useful when using more than one of the same device type in same space

#define IDString FAMILY DEVICE STR(INSTANCE_NUM) // C interprets subsequent string literals as concatenation: "/Loom" "/Ishield" "0" becomes "/Loom/Ishield0"
 
// Change to 434.0 or other frequency, must match RX's freq!
#define RF95_FREQ 915.0
 
// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

RHReliableDatagram manager(rf95, CLIENT_ADDRESS);

//////////////////////////////////////////////////////////////////////////////

//Global variables
int servo0 = 0;
int servo1 = 1;
int degree0, degree1 = 0;

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
void setup() {
  Serial.begin(9600);
  delay(2000);

  Serial.println("Initializing manager...");
  if (!manager.init())
    Serial.println("init failed");

  Serial.println("Setting Frequency...");
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }

  Serial.println("Setting power...");
  rf95.setTxPower(23, false);

}

//uint8_t data[] = "Hello world";

void loop() {
  // ===== Take data measurements here =====
  // Run your specific subroutines to take measurements from your sensors here
  //int servo_choice, set_degree;
  int set_degree0, set_degree1;
  
while (Serial.available() == 0) {}
if (Serial.available()) {
    Serial.print("\n\nType degrees (eg. 0 90):");
    
    //servo_choice = Serial.parseInt();
    
    set_degree0 = Serial.parseInt();
    set_degree1 = Serial.parseInt();
    
    //define maximum and minimum input degree
    if (set_degree0 > 180) set_degree0 = 180;
    if (set_degree0 < 1) set_degree0 = 0;
    //if(servo_choice == 0) degree0 = set_degree;
    //else if(servo_choice == 1) degree1 = set_degree;
    //Display status
    Serial.print("\n[Servo0]: "); Serial.print(set_degree0); Serial.print(" degree");
    Serial.print("\n[Servo1]: "); Serial.print(set_degree1); Serial.print(" degree");
}
  // ===== Create OSCBundle =====
  // Create your own data identification strings
  // Replace "/VWC", "/Temp", etc. with your specific values
  // bndl.add().add() -> The first 'add' adds a message to the bundle.
  //                    All subsequent 'add' operations add data to a given message.
  //
  // Ex: bndl.add("addr1").add(12).add(-6)
  //     This example adds a message with address/routing information "addr1" to the
  //     OSCBundle 'bndl'.  It adds two data values to this message: 12 and -6
  //     NOTE: this version of OSCInterpret does not support multiple data values
  //     attached to a single value.  If this is functionality you desire, let
  //     Kenny know.

  OSCBundle bndl;
  bndl.add(IDString "/Servo0/Set").add((int32_t)set_degree0);
  bndl.add(IDString "/Servo1/Set").add((int32_t)set_degree1);

  // ===== Send message =====
  char message[241];


  memset(message, '\0', 241);
  
  get_OSC_string(&bndl, message);

  //Serial.println(message);
  //Serial.print("Message length: ");
  //Serial.println(strlen(message));
  //Serial.print("Max message length: ");
  //Serial.println(RH_RF95_MAX_MESSAGE_LEN);

  delay(2000);
  
  Serial.print("Sending...");
  if (manager.sendtoWait((uint8_t*)message, strlen(message), SERVER_ADDRESS))
    Serial.println("ok");
  else
    Serial.println("failed");

  delay(2000);
}
