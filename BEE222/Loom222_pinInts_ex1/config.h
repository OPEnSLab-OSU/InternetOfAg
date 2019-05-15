// ================================================================ 
// ===                   CONFIGURATION FILE                     === 
// ================================================================
//
// Use this file to configure the device you are flashing the firmware of
//
// This file is divided into two sections:
//
//  Options: The top half that can be modified to change common settings
//
//  Advanced Options: the bottom section that is at least partially set based on  
//    the top half but may be modified for more specific behavior. 
//    Also contains other specific options that don't need to change as frequently
//
// Definitions that should not be modified by the user are
//   located in the library and are not present here

// ================================================================
// ================================================================ 
// ===                     PRIMARY OPTIONS                      === 
// ================================================================
// ================================================================



// ================================================================ 
// ===                  IDENTIFICATION OPTIONS                  === 
// ================================================================
// --- Device Identification --- 
#define FAMILY     "LOOM"    // Will usually be "LOOM", you can change this if you are setting up your own network
#define FAMILY_NUM       0    // Specifies the subnet of the family that the device is on. 0 for elevated permissions (can communicate with any subnet), 1-9 for normal
#define CHANNEL          1    // Channel to use. Set to 1-8 for channels A-H, respectively (on WiFi), LoRa can use 1-9. Alternatively can define to -1 to used advanced option INIT_INST
#define REQUEST_SETTINGS 0    // 1 to request dynamic channel settings (i.e. next available channel) from MaxMSP Channel Manager, 0 to not
#define AUTO_NAME        0    // 1 to enable naming device based on configured settings (if not set manual name in advanced options)
#define CUSTOM_NAME "BEE222"  // This is only used if Auto_name is set to be 0


// ================================================================ 
// ===                      SERIAL OPTIONS                      === 
// ================================================================
// --- Debugging --- 
#define LOOM_DEBUG    1     // Set to 1 if you want Serial statements from various functions to print
                // NOTE: Serial monitor must be opened for device to setup if this is enabled
                //   Device will freeze if this in enabled and device does not get plugged into Serial
                // LOOM_DEBUG_Print* are Serial prints that are removed if debugging is off

#define dynamic_serial_output 1 // These only apply if LOOM_DEBUG is enabled
                // 0 is standard operation 
                //   - Serial monitor needs to start / be open for device to setup
                //   - Serial (USB) can generally be detached after setup without issue 
                //   - Note that you may need to reopen the Serial monitor if it was open before the device was plugged back in
                // 1 Allows serial monitor to device to setup when LOOM_DEBUG is enabled without Serial (USB) being attached 
                //   - Note that you probably want to have the serial monitor open before uploading to the device, else you may miss 
                //     the first Serial prints, as enabling this option means that the device does not wait for you to open Serial

#define wake_delay 0      // 1 to enable wait 5 seconds upon awaking from sleep to (re)start Serial before program continue (useful for reprogramming if device quickly goes back to sleep)


#define prevent_platform_compile_error 0    // 0: Allow errors to be triggered if the program calls 
                      //     functionality of platforms / devices that are not enabled
                      // 1: Permits the code to compile and the device will skip functionality
                      //     of platforms / devices that are not enabled, informing the user of such

#define display_config 1    // 1 to print the uploaded configuration at start of setup, 0 to not

// ================================================================ 
// ===                  DEVICE TELEMETRY TYPE                   === 
// ================================================================
#define hub_node_type 1   // 0: Hub, 1: Node

// ================================================================ 
// ===                 COMMUNICATION PLATFORMS                  === 
// ================================================================
#define is_wifi       1   // 1 to enable WiFi
#define is_lora       0   // 1 to enable LoRa (cannot be used with nRF) (Further customization in advanced options)
#define is_nrf        0   // 1 to enable nRF (cannot be used with LoRa) (Further customization in advanced options)
#define is_ethernet   0   // 1 to enable Ethernet (a number of options below might auto enable this anyway though)
#define is_fona       0   // 1 to enable cellular via Fona (808 version)

// ================================================================ 
// ===                         SENSORS                          === 
// ================================================================

// --- Button Options ---
#define is_button     1 // 1 to enable button
#define button_pin    6 // Pin button is attached to 

// --- Switch Options ---
#define is_switch     1 // 1 to enable button
#define switch_pin    5 // Pin button is attached to 

// --- Analog Setting --- 
#define is_analog     0   // 1 if analog input is being used, 0 if not

  #define is_analog_a0 1 
  #define is_analog_a1 1
  #define is_analog_a2 1
  #define is_analog_a3 0
  #define is_analog_a4 0
  #define is_analog_a5 0

#define analog_samples 16   // Must be 1, 2, 4, 8, or 16 number of analog measurements to sample and average

// Other sensors
#define is_max31856        0 // MAX31856 Thermocouple

// ================================================================ 
// ===                        ACTUATORS                         === 
// ================================================================
#define is_relay      0   // 1 if a relay is being used
#if is_relay == 1
  #define relay_pin 10  // Digital pin connected to the relay enable 
#endif

// ================================================================ 
// ===                     WIFI SETTINGS                        === 
// ================================================================
// --- WiFi Settings ---
// Requires is_wifi to be set to 1 to take effect
#if is_wifi == 1
  #define DEFAULT_MODE      WPA_CLIENT_MODE // AP_MODE, WPA_CLIENT_MODE or WEP_CLIENT_MODE
  #define DEFAULT_NETWORK   "OSU_Access"      // Network SSID / name
  //#define DEFAULT_NETWORK   "Some_SSID"      // Network SSID / name
  //#define DEFAULT_PASSWORD  "Some_Password"    // Network password

#endif


// ================================================================ 
// ===                  DATA LOGGING PLATFORMS                  === 
// ================================================================ 
#define is_googlesheets 1 // 1 to enable google sheets functionality
/** This library is depricated! Please only use it if you have to. */
/** See https://github.com/OPEnSLab-OSU/InternetOfAg/tree/master/PushingBox/GoogleSheets_README.md */
#define is_pushingbox 0    // 1 to enable PushingBox  


// --- RTC Options ---
#define is_rtc        0   // Enable RTC functionality
#if is_rtc == 1
  #define RTC_pin     6 // What pin the RTC interrupt is connected to

  #define use_utc_time  1   // 1 to use UTC time rather than local

  // Ideally the enum wouldn't be declared here ...
  enum TimeZone { 
    WAT, AT, ADT, AST, EDT, EST, CDT, CST, MDT, MST, PDT, PST, ALDT, 
    ALST, HST, SST, GMT, BST, CET, CEST, EET, EEST, BT, ZP4, ZP5, 
    ZP6, ZP7, AWST, AWDT, ACST, ACDT, AEST, AEDT 
  };

  TimeZone timezone = PST;

  // Select only one of the below options
  #define is_rtc3231 0  // RTC DS 3231 Featherwing
  #define is_rtc8523 1  // RTC Adalogger Featherwing with PCF8523 RTC (the one with SD card)


  // Try to set time from internet (Ethernet or Wifi internet connection required)
  // Will default to normal RTC setting routing if this fails
  #define get_time_from_internet 1
  
#endif

// --- SD Options ---
#define is_sd         0   // 1 to enable SD card 
#if is_sd == 1
  // Does NOT automatically save to SD
  // This works more like a filter than an automator,
  // with a minimum of sdSaveMinDelay between saves
  // Uses millis
  #define sdSaveFilter     1  // 1 to enable a millis delay to SD 
  #define sdSaveMinDelay   5  // minimum delay in seconds between saves (only applies sdSaveFilter is 1)

  #define sdBroadcastSave  0  // 1 to broadcast on communication platforms when data is saved to SD, 0 to not 
#endif

// ================================================================ 
// ===                 DATA LOGGING OPTIONS                     === 
// ================================================================

// --- PushingBox Options ---
#if (is_ethernet != 1) && (is_wifi != 1) && (is_fona != 1)
  #define is_pushingbox 0   // Prevent PushingBox if no means of internet access
  #define is_googlesheets 0
#endif

#if (is_wifi != 1) && (is_googlesheets == 1)
  #warn "Cannot use google sheets without wifi enabled in config.h!"
  #define is_googlesheets 0
#endif


// --- PushingBox Deprication ---
#if is_pushingbox == 1 && is_googlesheets == 1
  #error "Cannot use Google Sheets and PushingBox at the same time! Check the config.h (googlesheets may turn on automatically with wifi)"
#endif
#if is_pushingbox
  #warning "You are using PushingBox, which is depracated! More info here: https://github.com/OPEnSLab-OSU/InternetOfAg/tree/master/PushingBox/GoogleSheets_README.md"
#endif

// --- PushingBox/GoogleSheets Options ---
#if is_pushingbox == 1 || is_googlesheets == 1
  // Google Spreadsheet ID
  // (found betweeen the "docs.google.com/spreadsheets/d/" and "/edit..." in the URL; looks like random string of characters)
  #define init_spreadsheet_id "1TlNG52wIreG9Kg6aZygFUJsuaTiAwDAlqlxH5VoiTCI"  // Chet's Test Sheet

  // Required by PushingBox, specific to each scenario
  // Does not always need to change with different spreadsheets, only different PushingBox accounts
  #define device_id "v516AA615ABCA29C"  // BEE222 PushingBox Device ID


  // Google Spreadsheet Sheet/Tab. Sent as parameter to PushingBox/Google Scripts
  // Generally 0 is the preferred setting (i.e. nodes get there own tab),
  //   use 1 to force all node data to go to a single hub-defined sheet
  #define useHubTabID 0   // Toggle option for below settings

  #if useHubTabID == 1  
    // The hub defines tab ID regardless of bundle source
    #define init_tab_id "HubSheet3"      // Defines tab if hub is defining tab instead of nodes
  #else           
    // Use bundle source and below prefix to define tab ID
    #define init_tab_id "Test1"      // Used as a prefix if node is being used to define tab
  #endif  


  #define verify_family_match 0     // 1 to only upload to spreadsheet if source device family matches hub
                      // Can likley be phased out by subnet filtering (e.g. lora_subnet_scope)


///////////////////////

  #define use_pb_sheet_array  0     // 1 to use the following array of spreadsheet_ids to upload to different
                      // sheets depending on the bundle source's device ID
                      // In the form <Family><Family#>/<Device><DevInst>, a Family# of 0 would map to the sheet at index 0
  
  #if use_pb_sheet_array == 1
    // 0–9 : Sheet ID for corresponding subnet #
    // Only fill out number of IDs being used
    const char* spreadsheet_list[8] = {
      "1L6Rr6H-5PFbbZIBKmR5mkGTztqA8xQhJeOW4Bn7ec8c",
      "1vgVBCv2nbCc8RGcb44-bgRtxKWvp2sIu7iu9w8rtWv4",
      "1_VTY97xWlx8WzRJ5r2NbRzPFYiorViDu4RqYo_l_mCA",
      "1zxA4a_w4dzsZHsNXXaqaXSu5cdpg-TzhzlKNBVbFUWg",
      "1yfj8ICAPvqoDP8xMGu5Hm7RQrj1RM0lvDf2PEee2kz4",
      "1G4kxtvN4_Vrzr5typTv3zybjythn25fuJBOgLFILf14",
      "1qajmatMnUyUohkQIJdjNhN1wSZVmDnS7ZSttaI3Jiqk",
      "1GlPYY3-DG9fhB4OVuebWzOBLT-AlLE7JLGFLmMC_UFw"
    };


    #define pb_default_sheet "1FPwr8Q8QyP6-VbW1lp_fyFne38dbWQUhLGtNW7gVmMk"
    // #define pb_default_sheet "1wMNszrEmPt2jN1ZYDNJhO5lJHm4WcMZhzP6ck5omEOU"  // Default sheet to use if family number is out of range, 
                                          // can be duplicate of an element in the array
  #endif

///////////////////////



  // Currently works by only sending a bundle from 
  //  log_bundle/sendToPushingBox if the delay has passed
  // Does NOT automatically upload a bundle, you still have to call sendToPushingBox
  // This works more like a filter than an automator,
  //  with a minimum of pushUploadMinDelay between uploads
  // Uses millis
  // Recommended that pushUploadedFilter is left enabled
  #define pushUploadFilter    0   // 1 to enable a millis delay to uploading to PushingBox
  #define pushUploadMinDelay  5   // delay in seconds
#endif



// --- Adafruit IO Options ---
#if is_adafruitio == 1
  #define AIO_USERNAME    ""
  #define AIO_KEY         ""
#endif


// --- SD Options ---
#if is_sd
  // Timestamp formatting options:
  // 0: none, 1: date, 2: time, 3: date+time two fields, 4: data+time combined field
  // Times are only added if RTC is also enabled
  #define sd_save_time_format 3
#endif



// ================================================================
// ===                    ANALOG CONVERSIONS                    === 
// ================================================================
// Analog conversions are specified by indexing (of function pointers
// in an array). 
// -1 means to not use any conversion
// Numbers >= 0 indicate which conversion to use, as defined by the 
//   array in loom_analog_conversions.h (0 indexed)

// These convesions only apply to the analog readings as read by the 
//   measure_analog() function

#define enable_analog_conversions -1    // 1 to enable the conversion of analog values upon reading

#if enable_analog_conversions == 1

  #if is_analog_a0 == 1
    #define analog_a0_conversion -1
  #endif 
  #if is_analog_a1 == 1
    #define analog_a1_conversion -1
  #endif
  #if is_analog_a2 == 1
    #define analog_a2_conversion -1
  #endif
  #if is_analog_a3 == 1
    #define analog_a3_conversion -1
  #endif
  #if is_analog_a4 == 1
    #define analog_a4_conversion -1
  #endif
  #if is_analog_a5 == 1
    #define analog_a5_conversion -1
  #endif

#endif




// ================================================================
// ================================================================ 
// ===                    ADVANCED OPTIONS                      === 
// ================================================================
// ================================================================

// --- Flash Options ---
#define enable_flash 1        // 1 is the default, 0 to turn of reading and writing to flash/EEPROM memory 

// --- Scripts ---
#define enable_hub_scripts 0    // RPN script parser

// --- Advanced Interdev Communication ---
#define advanced_interdev_comm   0  // 1 to use Max-like functionality
#define build_actuator_cmd_funcs 0  // 1 to include file to make commands for Loom actuators

// --- Set Custom Instance Number and UDP Port ---
// If not using channels, then the following 
// intial instance number and port (if WiFi) will be used
// Interdevice communication is undefined and less supported
// when not using channels
#if CHANNEL == -1
  #define INIT_INST      3  // Initial device instance number (normally 1-9 when using channels)
  #if is_wifi == 1
    #define INIT_PORT 9443  // Initial device UDP port (normally 1-9 when using channels)
  #endif
#endif

#if is_wifi == 1
  #define advanced_interdev_comm 1  // Enable for full Max support 
                    // Default is for this to be 1 when using WiFi
#endif

#if LOOM_DEBUG == 1
  #define SERIAL_BAUD 115200      // Select Serial baudrate, only used supported rates (i.e. 9600, or 115200, etc.)
                    // Make sure the Serial monitor is set to the same baud
#endif




// --- LoRa Options ---
#if is_lora == 1

  // Address of hub / node
  // Self and corresponding device 
  // 10 CLIENT_ADDRESSes belong to each SERVER_ADDRESS,
  // 10-19 for 0, 20 - 29 for 1, etc. 
  #if hub_node_type == 0  // If is hub
    #define LORA_SELF_ADDRESS   1       // Self
    #define LORA_OTHER_ADDRESS  2     // Default node to communicate with
  #else           // If is node
    #define LORA_OTHER_ADDRESS  1     // Hub to communicate with  
    #define LORA_SELF_ADDRESS   CHANNEL   // Self
  #endif

  #define lora_subnet_scope       2   // 1 for subnet, 2 for global/family, 3 for global only (not recommended), 4 for any message. (use 2 if unsure)

  #define RF95_FREQ           915.0 // Hardware specific, Tx must match Rx

  #define package_lora_rssi     0   // 1 to add LoRa last RSSi to bundle build via package bundle; 0 to not



  #define lora_power_level -1 // 5 (low) – 23 (high)   
                // -1 to use our default of 23 (13 is normally the default)
  #define lora_retry_delay -1 // Time to wait between retries in milliseconds
                // -1 to use default (200ms)
  #define lora_retry_count -1 // 0 means no retransmission
                // -1 to use default to (3)


  // #define lora_bundle_fragment 0   // Splits bundles into smaller bundles to avoid overflowing size LoRa can send
                      // Currently unused
#endif

// --- nRF Options --- 
#if is_nrf == 1

  // Node addresses should be 00-05
  #if hub_node_type == 0  // If is hub
    #define NRF_SELF_ADDRESS  00    // Self
    #define NRF_OTHER_ADDRESS 01    // Default node to communicate with 
  #else           // If is node
    #define NRF_OTHER_ADDRESS 00    // Hub to communicate with
    #define NRF_SELF_ADDRESS  01    // Self
  #endif


  #define nrf_subnet_scope      4 // 1 for subnet, 2 for global/family, 3 for global only (not recommended), 4 for any message. (use 2 if unsure)


  #define nrf_data_rate -1    // 1: 250kbs, 2: 1Mbps, 3: 2Mbps
                  // -1: use default (1Mbps)
                  // setting RF24_250KBPS will fail for non-plus units (not sure what this means)

  #define nrf_power_level -1    // 1: RF24_PA_MIN (-18dBm), 2: RF24_PA_LOW (-12dBm), 3: RF24_PA_HIGH (-6dBM), 4: RF24_PA_MAX (0dBm)
                  // -1 to use default (-18dBm)

  // Both of the next two options have to be set for them to take effect
  #define nrf_retry_delay -1    // How long to wait between each retry, in multiples of 250us, max is 15. 0 means 250us, 15 means 4000us.
                  // -1 to use default (5) 
  #define nrf_retry_count -1    // How many retries before giving up, max 15
                  // -1 to use default (15)


  // #define nrf_bundle_fragment 0    // Splits bundles into smaller bundles to avoid overflowing size LoRa can send
                      // Currently unused
#endif

// --- Ethernet Options ---
#if is_ethernet == 1
  #include <Ethernet2.h>        // (this is needed for IPAddress object below, do not remove)
  
  //Use this for OPEnS Lab
  // byte mac[] = {0x98, 0x76, 0xB6, 0x10, 0x61, 0xD6}; 

  byte mac[] = {0x00, 0x23, 0x12, 0x12, 0xCE, 0x7D};    // MAC address of Luke's Ethernet port
  
  IPAddress ip(128,193,56,138);               // device's IP address          try to make this a string, so #include can be move to ethernet file
#endif


// --- FONA Options ---
#if is_fona == 1
  #define LOOM_DEBUG 1  // This is just temporary for now, as all Fona tests are done interactively through serial monitor
  #define fona_test  1    // This enables the Fona interactive testing Loop, note that this will override normal 
              //    Loom loop behavior until command to exit testing loop is issued
  #define fona_type 2G  // Can be '2G' (808) or '3G' (5320A), currently only 2G is supported
#endif


// ================================================================ 
// ===                       HUB OPTIONS                        === 
// ================================================================

#if (hub_node_type == 0) && ((is_wifi == 1) || (is_ethernet == 1) || (is_fona == 1))
  // #define is_pushingbox 1       // Auto enable PushingBox if Ethernet, WiFi, or cellular
  #if is_pushingbox == 0
    #define is_googlesheets 1
  #endif

  #define device_poll_refresh 0   // Seconds between re-searching for devices on the network
                  // Set 0 or negative to disable
#endif



// ================================================================
// ===              BETWEEN LOOP SLEEP OPTIONS                  === 
// ================================================================

// --- Delay between loops ---
#if is_lora == 0            // Cannot not use with LoRa
  #define is_sleep_period 50      // Uncomment to use SleepyDog to transmit at intervals up to 16s and sleep in between. 
                    // Change the value according to the length of your desired transmission interval
                    // 80 seems to be a good amount, around 50 and lower may result in lost packets over WiFi
#else
  #define is_sleep_interrupt 11     // Uncomment to use Low-Power library to sit in idle sleep until woken by pin interrupt, parameter is pin to interrupt
#endif
