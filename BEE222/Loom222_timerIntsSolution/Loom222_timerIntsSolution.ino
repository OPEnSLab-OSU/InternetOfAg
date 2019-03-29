// ================================================================
// ===              INCLUDE CONFIGURATION FILE                  ===
// ===    INCLUDE DECLARATIONS, STRUCTS, AND FUNCTIONS FROM     ===
// ===            OTHER FILES AS SET IN CONFIG.H                ===
// ================================================================

// Config has to be first has it hold all user specified options
#include "config.h"

// Preamble includes any relevant subroutine files based 
// on options specified in the above config
#include "loom_preamble.h"
#include <AsyncDelay.h> // Add this line of code to include the Async Delay Library for timed functions
AsyncDelay delay_1000ms; // Create an Async delay object called delay_1000ms. The name is arbitrary to the actual time period set

// ================================================================ 
// ===            Referenced Pin Names and State                ===
// ================================================================ 
// button_pin
// button_state

// switch_pin
// switch_state

// ================================================================ 
// ===                    Interrupt Flags                       ===
// ================================================================ 
// These flags are set true if a pin interrupt has been triggered
volatile bool buttFlag = false;
volatile bool switchFlag = false; 

// ================================================================ 
// ===                    Global Variables                      ===
// ================================================================ 
volatile int count = 0;
// ================================================================ 
// ===                           SETUP                          ===
// ================================================================ 
void setup() 
{


  // LOOM_begin calls any relevant (based on config) LOOM device setup functions
  Loom_begin(); 

  // Any custom setup code goes here
  attachInterrupt(button_pin, button_pressed, CHANGE); // function to add an interrupt service routine to a pin
                                           // Args: Pin#, ISR-name, Condition
                                           // see: https://www.arduino.cc/en/Reference/attachInterrupt
  attachInterrupt(switch_pin, switch_switched, CHANGE);

  // Start counting
  delay_1000ms.start(1000, AsyncDelay::MILLIS); // Function to start the timer
}

// ================================================================ 
// ===                        MAIN LOOP                         ===
// ================================================================ 
void loop() 
{
  
  OSCBundle bndl, send_bndl;      // Bundles to hold incoming and outgoing data

  receive_bundle(&bndl, WIFI);    // Receive messages
  if (bndl.size()) {
    print_bundle(&bndl);      // Print bundle if LOOM_DEBUG enabled
  }
  process_bundle(&bndl);        // Dispatch message to correct handling functions
  measure_sensors();          // Read sensors, store data in sensor structs
  package_data(&send_bndl);     // Copy sensor data from state to provided bundle

  checkInts(&send_bndl);  // Function to check interrupt flags and append any data messages to log or transmit

  // If Switch is on, Increment Count and Report it
  if(switch_state)
  {
    if (delay_1000ms.isExpired())
    {
      count++;
      Serial.print("My Count is ");
      Serial.println(count);
      
      append_to_bundle_key_value(&send_bndl, "countIs", count);
      delay_1000ms.repeat(); // Count from when the delay expired
    }
  }
  
  print_bundle(&send_bndl);
  log_bundle(&send_bndl, GOOGLESHEETS);  // Send bundle to Google Sheet
  additional_loop_checks();     // Miscellaneous checks

}

// ================================================================ 
// ===                 Interrupt Flag Checker                   ===
// ===    Check flags set by ISRs, do conditional things        ===
// ================================================================
void checkInts(OSCBundle *bndl)
{
  if(buttFlag)
  {
    button_state = digitalRead(button_pin) ^ 1; // Invert because Low-True
    if (button_state)
      {
        count = 0;
        Serial.print("My Count is ");
        Serial.println(count);
        delay_1000ms.start(1000, AsyncDelay::MILLIS); // Function to restart the timer
      }
    
    append_to_bundle_key_value(bndl, "buttonIs", button_state); // Function to slip this message into the OSC bundle
    buttFlag = false; // lower the flag
  }

  if(switchFlag)
  {
    int switch_state = digitalRead(switch_pin);
    Serial.print("My switch is  ");
    if (switch_state)
      {
        Serial.println("on!");
        // ReStart counting
        delay_1000ms.start(1000, AsyncDelay::MILLIS); // Function to restart the timer
      }
    else
    {
      Serial.println("off!");
    }
    append_to_bundle_key_value(bndl, "switchIs", switch_state); // Function to slip this message into the OSC bundle
    switchFlag = false; // lower the flag
  }
  
}

// ================================================================ 
// ===                Interrupt Service Routines                ===
// ================================================================
void button_pressed(void)
{
  buttFlag = true; // raise the flag!
}

void switch_switched(void)
{
  switchFlag = true; // raise the flag!
}



// ================================================================ 
// ===                 High-Level API Functions                 === 
// ================================================================ 

// void receive_bundle(OSCBundle *bndl, CommPlatform platform);
// void process_bundle(OSCBundle *bndl);
// void measure_sensors();
// void package_data(OSCBundle *bndl);
// void send_bundle(OSCBundle *bndl, CommPlatform platform, int port);
// void send_bundle(OSCBundle *bndl, CommPlatform platform);
// void log_bundle(OSCBundle *bndl, LogPlatform platform, char* file); // filename required for SD files
// void log_bundle(OSCBundle *bndl, LogPlatform platform);
// bool bundle_empty(OSCBundle *bndl);
// void additional_loop_checks();
// void sleep_for(int amount, TimeUnits units, SleepMode mode);
// void sleep_until_time(SleepMode mode, int hour, int min, int sec);
// void append_to_bundle_key_value(OSCBundle *bndl, char* key, T elem);

// CommPlatforms: WIFI, LORA, NRF
// LogPlatforms:  PUSHINGBOX, SDCARD, OLED
// TimeUnits: MINUTES, SECONDS
// SleepMode: STANDBY, SLEEPYDOG

// Print Macro
// LOOM_DEBUG_Println