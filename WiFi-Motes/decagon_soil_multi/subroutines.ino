#include "SensorList.h"

struct SensorList poll_sensors(SDI12 *mySDI12) {
  struct SensorList data;
  String id = "";
  data.count = 0;
  for (int i = 0; i < 6; i++) {
    id = get_identity(&mySDI12[i]);
    if (id.length() > 0) {
      data.list[data.count] = i;
      data.count++;
    }
  }
  return data;
}

String _send_command(SDI12 *mySDI12, String myCommand) {
  String response = "";
  mySDI12->setActive(); //Necessary when using multiple SDI12 objects

  delay(500);

  mySDI12->sendCommand(myCommand);
  delay(30);                     // wait a while for a response

  while (mySDI12->available()) {  // build response string
    char c = mySDI12->read();
    if ((c != '\n') && (c != '\r')) {
      response += c;
      delay(5);
    }
  }

  mySDI12->clearBuffer();

  mySDI12->forceHold();

  return response;
}

String get_measure(SDI12 *mySDI12) {
  String response = "";
  String myCommand = String(SENSOR_ADDRESS) + "M!";
  response = _send_command(mySDI12, myCommand);
  return response;
}

String get_identity(SDI12 *mySDI12) {
  String response = "";
  String myCommand = String(SENSOR_ADDRESS) + "I!";
  response = _send_command(mySDI12, myCommand);
  return response;
}

String get_data(SDI12 *mySDI12) {
  String response = "";
  String myCommand = String(SENSOR_ADDRESS) + "D0!";
  response = _send_command(mySDI12, myCommand);
  return response;
}

