//Transmitter

#include <SPI.h>
#include "RF24.h"

RF24 radio(5,6);
byte addresses[][6] = {"1Node","2Node"};

void setup() {
  Serial.begin(57600);
  // put your setup code here, to run once:
  radio.begin();
  radio.setDataRate(RF24_2MBPS);
  radio.openWritingPipe(addresses[1]);

  //radio.startListening();
}

void loop() {
  // put your main code here, to run repeatedly:
  unsigned long dummy[20];
  if (!radio.write( &dummy, 20*sizeof(unsigned long))) {
    Serial.println(F("failed"));
  }
}
