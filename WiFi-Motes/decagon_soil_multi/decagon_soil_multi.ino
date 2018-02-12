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

#include "SDI12.h"
#include <string.h>
#include "SensorList.h"

#define DATAPIN1 A0  // change to the proper pin
#define DATAPIN2 A1
#define DATAPIN3 A2
#define DATAPIN4 A3
#define DATAPIN5 A4
#define DATAPIN6 A5
#define SENSOR_ADDRESS "?"

SDI12 mySDI12[6] = {DATAPIN1, DATAPIN2, DATAPIN3, DATAPIN4, DATAPIN5, DATAPIN6};

//SDI12 mySDI12(DATAPIN1);

struct SensorList data;

String sdiResponse = "";
char buf[20];
char *p = buf;
float dielec_p = 0;
float temp = 0;
float elec_c = 0;

void setup() {
  Serial.begin(9600);
  delay(2000);
  
  for (int i = 0; i < 6; i++) {
    mySDI12[i].begin();
    mySDI12[i].forceHold();
  }
}

void loop() {

  // ===== Poll Sensors =====

  data = poll_sensors(mySDI12);

  // ===== Send Measure to Active Sensors =====
  
  for (int i = 0; i < data.count; i++) {
    int num = data.list[i];
    get_measure(&mySDI12[num]);
  }

  delay(1000);                 // delay between taking reading and requesting data

  // ===== Retrieve Data from Active Sensors =====

  for (int i = 0; i < data.count; i++) {
    int num = data.list[i];
    sdiResponse = get_data(&mySDI12[num]);
    Serial.print("Data from pin A");
    Serial.print(num);
    Serial.print(": ");
    Serial.println(sdiResponse);
    sdiResponse = "";
  }
  if (data.count == 0) Serial.println("No sensors found");
}
