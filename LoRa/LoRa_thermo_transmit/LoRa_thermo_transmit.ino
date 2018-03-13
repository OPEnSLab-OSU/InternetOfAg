#include <SPI.h>
#include <RH_RF95.h>
#include <RHReliableDatagram.h>
#include <OSCBundle.h>
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
#define DEVICE "/tcouple"
#define INSTANCE_NUM 0  // Unique instance number for this device, useful when using more than one of the same device type in same space

#define IDString FAMILY DEVICE STR(INSTANCE_NUM) // C interprets subsequent string literals as concatenation: "/Loom" "/Ishield" "0" becomes "/Loom/Ishield0"
 
// Change to 434.0 or other frequency, must match RX's freq!
#define RF95_FREQ 915.0
 
// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

RHReliableDatagram manager(rf95, CLIENT_ADDRESS);

// ===== Thermocouple Definitions =====
#define is_tcouple

#ifdef is_tcouple

#include <Adafruit_MAX31856.h>
float CJTemp;
float TCTemp;
float tc_vin;
#endif

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

#ifdef is_tcouple
  tcouple_setup();
#endif
}


void loop() {
  // ===== Take data measurements here =====
  // Run your specific subroutines to take measurements from your sensors here
  
#ifdef is_tcouple
  measure_tcouple();
#endif

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

  bndl.add(IDString "/CJTemp_C").add((float)CJTemp);
  bndl.add(IDString "/TCTemp_C").add((float)TCTemp);
  
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
