#include <SPI.h>
#include <RH_RF95.h>
#include <RHReliableDatagram.h>
#include <OSCBundle.h>
 
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

void setup() {
  Serial.begin(9600);
  if (!manager.init())
    Serial.println("init failed");

  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }
  
  rf95.setTxPower(23, false);
  
}

void loop() {
  // put your main code here, to run repeatedly:
  if (manager.available()) {
    uint8_t len = sizeof(buf);
    uint8_t from;
    memset(buf, '\0', RH_RF95_MAX_MESSAGE_LEN);
    if (manager.recvfromAck(buf, &len, &from)) {
      OSCBundle bndl;
      get_OSC_bundle((char*)buf, &bndl);

      //Insert operation code here
      //OSCBundle should container received data

      Serial.print("Received message from ");
      Serial.print(from);
      Serial.print(": ");
      Serial.println((char*)buf);
      bndl.send(Serial);
      Serial.println("");
    }
  }
}
