#include <string.h>

union data_vals {
  int32_t i;
  float f;
  uint32_t u;
};

char* get_OSC_string(OSCBundle bndl) {
  char buf[50];
  char string[121];
  char type;
  int n = 0;
  data_vals value;
  OSCMessage* msg = bndl.getOSCMessage(n);

  memset(string, '\0', 121);
  
  while(msg != NULL) {
    msg->getAddress(buf, 0);
    
    /*Serial.print("Address ");
    Serial.print(n+1);
    Serial.print(": ");
    Serial.println(buf);*/

    strcat(string, buf);

    int m = 0;
    type = msg->getType(m);

    while(type != '\0') {

      if (type == 'f') {
        value.f = msg->getFloat(m);
        /*Serial.print("Value ");
        Serial.print(n+1);
        Serial.print(": ");
        Serial.println(value);*/

        snprintf(buf, 50, " f%lu", value.u);
        strcat(string, buf);
      }
      else if (type == 'i') {
        value.i = msg->getInt(m);
        /*Serial.print("Value ");
        Serial.print(n+1);
        Serial.print(": ");
        Serial.println(value);*/

        snprintf(buf, 50, " i%lu", value.u);
        strcat(string, buf);
      }
      else if (type == 's') {
        char val_buf[10];
        msg->getString(m, val_buf);

        snprintf(buf, 50, " s%s", val_buf);
      }
      m++;
      type = msg->getType(m);
    }
    n++;
    msg = bndl.getOSCMessage(n);
    if (msg != NULL) strcat(string, " ");
  }

  char* ret_val = (char*)malloc((strlen(string) + 1) * sizeof(char));
  strcpy(ret_val, string);
  return ret_val;
}

void get_OSC_bundle(char *string, OSCBundle* bndl) {
  bndl->empty();
  data_vals value_union;
  char buf[121];
  char *p = buf;
  char *token = NULL;
  strcpy(buf, string);
  OSCMessage msg;
  token = strtok_r(p, " ", &p);
  while (token != NULL & strlen(token) > 0) {
    if (token[0] == '/') {
      msg = bndl->add(token);
    }
    else if (token[0] == 'f') {
      value_union.u = strtoul(&token[1], NULL, 0);
      msg.add(value_union.f);
    }
    else if (token[0] == 'i') {
      value_union.u = strtoul(&token[1], NULL, 0);
      msg.add(value_union.i);
    }
    else if (token[0] == 's') {
      msg.add(&token[1]);
    }
    
    
    /*value_union.u = strtoul(&token[1], NULL, 0);
    if (token[0] == 'f') {
      bndl->add(addr).add(value_union.f);
      Serial.print("Address: ");
      Serial.println(addr);
      Serial.print("Value: ");
      Serial.println(value_union.f);
    }
    else if (token[0] == 'i') {
      bndl->add(addr).add(value_union.i);
      Serial.print("Address: ");
      Serial.println(addr);
      Serial.print("Value: ");
      Serial.println(value_union.i);
    }

    addr = strtok_r(p, " ", &p);*/
  }
}

void print_bundle(OSCBundle *bndl) {
  int n = 0;
  char buf[50];
  char type;
  OSCMessage *msg = bndl.getOSCMessage(n);
  while(msg != NULL) {
    msg->getAddress(buf, 0);
    Serial.print("Address ");
    Serial.print(n+1);
    Serial.print(": ");
    Serial.println(buf);

    int m = 0;
    type = msg->getType(m);
    while(type != '\0') {
      Serial.print("Value ");
      Serial.print(m+1);
      Serial.print(": ");
      if (type == 'f') {
        Serial.println(msg->getFloat(m));
      }
      else if (type == 'i') {
        Serial.println(msg->getInt(m));
      }
      else if (type == 's') {
        msg->getString(m, buf);
        Serial.println(buf);
      }
      
      m++;
      type = msg->getType(m);
    }
  }
}

