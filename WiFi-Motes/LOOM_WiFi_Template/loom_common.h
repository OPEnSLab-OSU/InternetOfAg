// Common global variables
int           led =  LED_BUILTIN;             // LED pin number
volatile bool ledState = LOW;                 // State of LED
float         vbat = 3.3;                     // Place to save measured battery voltage (3.3V max)
char          packetBuffer[255];              // Buffer to hold incoming packet
char          ReplyBuffer[] = "acknowledged"; // A string to send back
const byte    memValidationValue = 99;        // Used to check if configuration has been written to (number will only be 99 if mem has been written to)
OSCErrorCode  error;                          // Hold errors from OSC
uint32_t      button_timer;                   // For time button has been held
char addressString[255];

extern void switch_to_AP(OSCMessage &msg);  // Reference to externally defined function called by msg_router




// --- SET INSTANCE NUMBER ---
// Updates device's identifying instance number
// Arguments: msg (OSC message with new instance number)
// Return:    none
void set_instance_num(OSCMessage &msg) 
{
  configuration.instance_number = msg.getInt(0);
  sprintf(configuration.packet_header_string, "%s%d\0", PacketHeaderString, configuration.instance_number);
  
  #if DEBUG == 1
    Serial.print("new address header: ");
    Serial.println(configuration.packet_header_string);
  #endif
  write_non_volatile();
  //flash_config.write(configuration);
}




// --- MESSAGE ROUTER ---
// Used to route OSC messages to the correct function to handle it
// Arguments: msg (OSC message to route subroutine that handles it), 
//   addrOffset (used to determine where to start check of message header string)
// Return:    none
void msg_router(OSCMessage &msg, int addrOffset) {
  #if DEBUG == 1
    char buffer[100];
    msg.getAddress(buffer, addrOffset);
    Serial.print("Parsed ");
    Serial.println(buffer);
  #endif
  #if num_servos > 0
    msg.dispatch("/Servo/Set", set_servo, addrOffset);
  #endif
  #if is_relay == 1
    msg.dispatch("/Relay/State", handleRelay, addrOffset);
  #endif
  #if is_mpu6050 == 1
    msg.dispatch("/MPU6050/cal", calMPU6050_OSC, addrOffset);
  #endif
  #if is_neopixel == 1
    msg.dispatch("/Neopixel", setColor, addrOffset);
  #endif

  #if is_wifi == 1
    msg.dispatch("/Connect/SSID", set_ssid, addrOffset);
    msg.dispatch("/Connect/Password", set_pass, addrOffset);
    msg.dispatch("/wifiSetup/AP", switch_to_AP, addrOffset);
    msg.dispatch("/SetPort", set_port, addrOffset);
    msg.dispatch("/requestIP", broadcastIP, addrOffset);
  #endif
  msg.dispatch("/SetID", set_instance_num, addrOffset);
}



// --- INITIAL CONFIG ---
// Handles configuration of reading device config from flash (if applicable)
//   else uses config.h specific settings, and writes these to flash
// Arguments: none
// Return:    none
void init_config()
{
  #if is_wifi == 1
    packet_header_string = configuration.packet_header_string;
    link_config_wifi(&configuration.config_wifi);
  #endif
  #if is_mpu6050 == 1
    link_config_mpu6050(&configuration.config_mpu6050);
  #endif
  #if MEM_TYPE == MEM_FLASH || MEM_TYPE == MEM_EEPROM
    read_non_volatile(); //reads configuration from non_volatile memory
    
    #if DEBUG == 1
      Serial.println("Reading from non-volatile memory...");
      Serial.print("Checksum: ");
      Serial.println(configuration.checksum);
    #endif
    
    if (configuration.checksum != memValidationValue) {     // Write default values to flash
          configuration.instance_number = INIT_INST;
          sprintf(configuration.packet_header_string,"%s%d\0",PacketHeaderString,configuration.instance_number);
          #if DEBUG == 1
            Serial.print("expecting OSC header ");
            Serial.println(configuration.packet_header_string);
          #endif
          
          #if is_wifi == 1
            configuration.config_wifi.my_ssid = AP_NAME;                  // Default AP name
            strcpy(configuration.config_wifi.ssid,DEFAULT_NETWORK);       // Default network name
            strcpy(configuration.config_wifi.pass,DEFAULT_PASSWORD);      // AP password (needed only for WEP, must be exactly 10 or 26 characters in length)
            configuration.config_wifi.keyIndex = 0;                       // Your network key Index number (needed only for WEP)
            configuration.config_wifi.ip_broadcast = "255.255.255.255";     // IP to Broadcast data 
            configuration.config_wifi.localPort = INIT_PORT;              // Local port to listen on
            configuration.config_wifi.wifi_mode = DEFAULT_MODE;           // WiFi mode to start in (AP_MODE, WPA_CLIENT_MODE, WEP_CLIENT_MODE)
          #endif
          // Add any other behavior/calibration wrapped in an #ifdef is_something preprocessor directive HERE
          
          #if is_mpu6050 == 1
            calMPU6050();                                     // Calibration writes memValidationValue for us
          #endif
          
          configuration.checksum = memValidationValue;      // Configuration has been written successfully, so we write the checksum
          

          #if DEBUG == 1
            Serial.println("Writing to flash for the first time.");
          #endif
          write_non_volatile();
                                                              // Flash memory has limited writes and we don't want to waste it on unnecessary tests
    } // of if (configuration.checksum != memValidationValue)
    #endif //of MEM_TYPE
}



// --- CHECK BUTTON HELD ---
// Checked each iteration of main loop if the device's button has been held
// If so, restart in access point mode
// Arguments: none
// Return:    none
#ifdef button
void check_button_held()
{
  if ( (uint32_t)digitalRead(button) ){
  button_timer = 0;
  } else {
    #ifdef is_sleep_period
      button_timer += is_sleep_period;
    #else
      button_timer++;
    #endif
    if (button_timer >= 5000) { // ~about 8 seconds
      #if DEBUG == 1
        Serial.println("Button held for 8 seconds, resetting to AP mode");
      #endif
      button_timer = 0;
   
      OSCMessage temp;          // Dummy message not used by function, but it expects an OSCMessage normally
      switch_to_AP(temp);       // Change to AP mode
    } 
  } // of else 
}
#endif



// --- LOOM BEGIN ---
// Called by setup(), handles calling of any LOOM specific individual device setups
// Starts Wifi or Lora and serial if debugging prints are on
// Arguments: none
// Return:    none 
void LOOM_begin()
{
  //Initialize serial and wait for port to open:
  #if DEBUG == 1
    Serial.begin(9600);
    while(!Serial);        // Ensure Serial is ready to go before anything happens in DEBUG mode.
  #endif
  
  // Set the button pin mode to input
  #ifdef button
    pinMode(button, INPUT_PULLUP); 
  #endif

  #ifdef is_mpu6050
    setup_mpu6050();
  #endif


  // Primary configuratation, such as writing config to non-volatile memory
  init_config();


  // Actuator-specific setups
  #if is_neopixel == 1
    neopixel_setup();
  #endif
  #if num_servos > 0
    servo_setup();
  #endif
  #if is_relay == 1
    relay_setup();
  #endif
  
 
  
  #if is_wifi == 1
    wifi_setup();
  #endif

  #if is_lora == 1
    lora_setup(&rf95, &manager);
  #endif
  
}



