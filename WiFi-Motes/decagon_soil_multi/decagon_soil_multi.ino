/*
Decagon shield polling functionality demo
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

//Declare 6 SDI-12 objects initialized with DATAPIN1-6
SDI12 mySDI12[6] = {DATAPIN1, DATAPIN2, DATAPIN3, DATAPIN4, DATAPIN5, DATAPIN6};

//Struct used to manage sensor state data
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

    set_data(&data, sdiResponse, i);
    /*Serial.print("Data from pin A");
    Serial.print(num);
    Serial.print(": ");
    Serial.println(sdiResponse);*/
    sdiResponse = "";
  }

  // ===== Print Data to Serial Monitor =====

  print_data(data);
  delay(3000);
}
