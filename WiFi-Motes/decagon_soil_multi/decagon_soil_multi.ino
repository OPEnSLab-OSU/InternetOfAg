/*
Example F: Basic data request.
Edited by Ruben Kertesz for ISCO Nile 502 2/10/2016
This is a simple demonstration of the SDI-12 library for Arduino.
This is a very basic (stripped down) example where the user initiates a measurement
and receives the results to a terminal window without typing numerous commands into
the terminal.
The SDI-12 specification is available at: http://www.sdi-12.org/
The library is available at: https://github.com/EnviroDIY/Arduino-SDI-12
The forked library with additional example files is available at: https://github.com/rkertesz/Arduino-SDI-12
The circuit: You should not have more than one SDI-12 device attached for this example.
See:
https://raw.github.com/Kevin-M-Smith/SDI-12-Circuit-Diagrams/master/basic_setup_no_usb.png
or
https://raw.github.com/Kevin-M-Smith/SDI-12-Circuit-Diagrams/master/compat_setup_usb.png
Written by Kevin M. Smith in 2013.
Contact: SDI12@ethosengineering.org
Extended by Ruben Kertesz in 2016
Contact: github@emnet.net or @rinnamon on twitter
*/

// NOT TESTED YET


#include "SDI12.h"
#include <string.h>

#define DATAPIN1 11  // change to the proper pin
#define DATAPIN2 10
#define SENSOR_ADDRESS "?"

SDI12 mySDI12(DATAPIN1);
SDI12 mySDI12_2(DATAPIN2);

String sdiResponse = "";
String myCommand = "";
char buf[20];
char *p = buf;
float dielec_p = 0;
float temp = 0;
float elec_c = 0;

void setup() {
  Serial.begin(9600);
  mySDI12.begin();
  mySDI12_2.begin();
}

void loop() {
  mySDI12.setActive();
  //first command to take a measurement
  myCommand = String(SENSOR_ADDRESS) + "M!";
  //Serial.println(myCommand);     // echo command to terminal

  mySDI12.sendCommand(myCommand);
  delay(30);                     // wait a while for a response

  while (mySDI12.available()) {  // build response string
    char c = mySDI12.read();
    if ((c != '\n') && (c != '\r')) {
      sdiResponse += c;
      delay(5);
    }
  }

  mySDI12.clearBuffer();

  sdiResponse = "";

  mySDI12_2.setActive();
  mySDI12_2.sendCommand(myCommand);
  delay(30);                     // wait a while for a response

  while (mySDI12_2.available()) {  // build response string
    char c = mySDI12_2.read();
    if ((c != '\n') && (c != '\r')) {
      sdiResponse += c;
      delay(5);
    }
  }
  //if (sdiResponse.length() > 1) Serial.println(sdiResponse); //write the response to the screen
  mySDI12_2.clearBuffer();


  delay(1000);                 // delay between taking reading and requesting data
  sdiResponse = "";           // clear the response string


// next command to request data from last measurement
  myCommand = String(SENSOR_ADDRESS) + "D0!";

  mySDI12.setActive();
  mySDI12.sendCommand(myCommand);
  delay(30);                     // wait a while for a response

  while (mySDI12.available()) {  // build string from response
    char c = mySDI12.read();
    if ((c != '\n') && (c != '\r')) {
      sdiResponse += c;
      delay(5);
    }
  }

  sdiResponse.toCharArray(buf, sizeof(buf));

  p = buf;

  Serial.print("Decagon Sensor 1: ");
  Serial.println(p);

  mySDI12.clearBuffer();

  sdiResponse = "";

  mySDI12_2.setActive();
  mySDI12_2.sendCommand(myCommand);
  delay(30);                     // wait a while for a response

  while (mySDI12_2.available()) {  // build string from response
    char c = mySDI12_2.read();
    if ((c != '\n') && (c != '\r')) {
      sdiResponse += c;
      delay(5);
    }
  }

  sdiResponse.toCharArray(buf, sizeof(buf));

  p = buf;

  Serial.print("Decagon Sensor 2: ");
  Serial.println(p);

/*
  strtok_r(p, "+", &p);
  dielec_p = atof(strtok_r(NULL, "+", &p));
  temp = atof(strtok_r(NULL, "+", &p));
  elec_c = atof(strtok_r(NULL, "+", &p));
*/
  
  
  //if (sdiResponse.length() > 1) Serial.println(sdiResponse); //write the response to the screen
  /*Serial.print("Dielectric Permittivity: ");
  Serial.println(dielec_p);
  Serial.print("Temperature (C): ");
  Serial.println(temp);
  Serial.print("Electric Conductivity: ");
  Serial.println(elec_c);*/
  
  mySDI12_2.clearBuffer();

//now go back to top and wait until user hits enter on terminal window
}
