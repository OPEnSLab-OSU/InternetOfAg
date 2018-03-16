//Receiver

#include <SPI.h>
#include "RF24.h"

RF24 radio(5,6);
byte addresses[][6] = {"1Node","2Node"};
uint32_t count = 0;
unsigned long start_time;

void setup() {
  Serial.begin(57600);
  // put your setup code here, to run once:
  radio.begin();
  radio.setDataRate(RF24_2MBPS);

  radio.openReadingPipe(1,addresses[1]);

  radio.startListening();
  start_time = millis();
}

int i = 0;

void loop() {
  // put your main code here, to run repeatedly:
  unsigned long dummy[20];

  if ( radio.available()) {
    while (radio.available()) {
      radio.read( &dummy, 20*sizeof(unsigned long));
      count++;
      i++;
      if (i == 1000) {
        i = 0;
        Serial.print("Data rate: ");
        float value = (float)count*20*8*sizeof(unsigned long)/((millis() - start_time));
        Serial.print(value);
        Serial.println(" Kbps");
      }
    }
  }
}
