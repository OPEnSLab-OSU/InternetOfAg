/*

/**
 * Simplest possible example of using RF24Network,
 *
 * RECEIVER NODE
 * Listens for messages from the transmitter and prints them out.
 */

#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>
#include <OSCBundle.h>

RF24 radio(5,6);                // nRF24L01(+) radio attached using Getting Started board 

RF24Network network(radio);      // Network uses that radio
const uint16_t this_node = 01;    // Address of our node in Octal format ( 04,031, etc)
const uint16_t other_node = 00;   // Address of the other node in Octal format
unsigned long start_time = millis();
unsigned long total_bytes = 0;
char message[121];

void setup(void)
{
  Serial.begin(57600);
  Serial.println("RF24Network/examples/helloworld_rx/");
 
  SPI.begin();
  radio.begin();
  network.begin(/*channel*/ 90, /*node address*/ this_node);
}

void loop(void){
  
  network.update();                  // Check the network regularly

  
  while ( network.available() ) {     // Is there anything ready for us?
    
    RF24NetworkHeader header;        // If so, grab it and print it out
    memset(message, '\0', 121);
    network.read(header,&message,120);
 /*   Serial.print("Received packet #");
    Serial.print(payload.counter);
    Serial.print(" at ");
    Serial.println(payload.ms);
    Serial.print("Message: ");
    Serial.print(payload.message); */
    //Serial.print("Message: ");
    //Serial.println(message);

    //total_bytes += strlen(message);

    OSCBundle bndl;
    get_OSC_bundle(message, &bndl);

    bndl.send(Serial);
    Serial.println("");
    //Serial.print("Data rate: ");
    //Serial.print((float)total_bytes/((millis() - start_time)/1000));
    //Serial.println(" Byte per second");
  }
}

