// ================================================================ 
// ===                ACCESSSING SENSOR STRUCTS                 === 
// ================================================================

// state_tsl2561_0x29.lightIR
// state_tsl2561_0x29.lightFull

// state_tsl2561_0x39.lightIR
// state_tsl2561_0x39.lightFull

// state_tsl2561_0x49.lightIR
// state_tsl2561_0x49.lightFull

// ================================================================ 
// ===                        LIBRARIES                         === 
// ================================================================
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_TSL2561_U.h>

// ================================================================ 
// ===                       DEFINITIONS                        === 
// ================================================================


// ================================================================ 
// ===                        STRUCTURES                        === 
// ================================================================

struct state_tsl2561_t {
	int lightIR;
	int lightFull;
};



// ================================================================ 
// ===                   GLOBAL DECLARATIONS                    === 
// ================================================================

#if i2c_addr_tsl2561_0x29 == 1
	struct state_tsl2561_t state_tsl2561_0x29;
	Adafruit_TSL2561_Unified inst_tsl2561_0x29 = Adafruit_TSL2561_Unified(TSL2561_ADDR_LOW, 25611);
#endif
#if i2c_addr_tsl2561_0x39 == 1
	struct state_tsl2561_t state_tsl2561_0x39;
	Adafruit_TSL2561_Unified inst_tsl2561_0x39 = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 25612);
#endif
#if i2c_addr_tsl2561_0x49 == 1
	struct state_tsl2561_t state_tsl2561_0x49;
	Adafruit_TSL2561_Unified inst_tsl2561_0x49 = Adafruit_TSL2561_Unified(TSL2561_ADDR_HIGH,  25613);
#endif


// ================================================================ 
// ===                   FUNCTION PROTOTYPES                    === 
// ================================================================
bool setup_tsl2561();
void package_tsl2561(OSCBundle *bndl, char packet_header_string[], uint8_t port);
void package_tsl2561(OSCBundle *bndl, char packet_header_string[]);
void measure_tsl2561();
void details_tsl2561();
void configure_tsl2561();


// ================================================================ 
// ===                          SETUP                           === 
// ================================================================
//
// Runs any TSL2561 setup
//
// @return  Whether or not sensor initialization was successful
//
bool setup_tsl2561() 
{
	LOOM_DEBUG_Println("Setting up TSL2561");

	#if i2c_addr_tsl2561_0x29 == 1
		if (!inst_tsl2561_0x29.begin()) {
			LOOM_DEBUG_Println("No TSL2561 sensor found with I2C selection to ground ... check your wiring?");
			// while (1);
		}
	#endif
	#if i2c_addr_tsl2561_0x39 == 1
		if (!inst_tsl2561_0x39.begin()) {
			LOOM_DEBUG_Println("No TSL2561 sensor found with I2C selection to float ... check your wiring?");
			// while (1);
		}
	#endif
	#if i2c_addr_tsl2561_0x49  == 1
		if (!inst_tsl2561_0x49.begin()) {
			LOOM_DEBUG_Println("No TSL2561 sensor found with I2C selection to high ... check your wiring?");
			// while (1);
		}
	#endif

	configure_tsl2561();

	LOOM_DEBUG_Println("TSL2561 setup complete");
}


// ================================================================ 
// ===                        FUNCTIONS                         === 
// ================================================================


// --- PACKAGE SHT31D --- (Multiplexer Version)
// 
// Adds last read SHT31D sensor readings to provided OSC bundle
//
// @param bndl                  The OSC bundle to be added to
// @param packet_header_string  The device-identifying string to prepend to OSC messages
// @param port                  Which port of the multiplexer the device is plugged into
//
void package_tsl2561(OSCBundle *bndl, char packet_header_string[], uint8_t port)
{
	char address_string[255];
	sprintf(address_string, "%s%s%d%s", packet_header_string, "/port", port, "/tsl2561/data");
	
	OSCMessage msg = OSCMessage(address_string);
	

	// #if i2c_addr_tsl2561_0x29 == 1
	// 	msg.add("lightIR_0x29"    ).add( (int32_t)state_tsl2561_0x29.lightIR );
	// 	msg.add("lightFull_0x29"  ).add( (int32_t)state_tsl2561_0x29.lightFull );
	// #endif
	// #if i2c_addr_tsl2561_0x39 == 1
	// 	msg.add("lightIR_0x39"  ).add( (int32_t)state_tsl2561_0x39.lightIR );
	// 	msg.add("lightFull_0x39").add( (int32_t)state_tsl2561_0x39.lightFull );
	// #endif
	// #if i2c_addr_tsl2561_0x49 == 1
	// 	msg.add("lightIR_0x49"   ).add( (int32_t)state_tsl2561_0x49.lightIR );
	// 	msg.add("lightFull_0x49" ).add( (int32_t)state_tsl2561_0x49.lightFull );
	// #endif
	#if i2c_addr_tsl2561_0x29 == 1
		msg.add("lightIR"  ).add( (int32_t)state_tsl2561_0x29.lightIR );
		msg.add("lightFull").add( (int32_t)state_tsl2561_0x29.lightFull );
	#endif
	#if i2c_addr_tsl2561_0x39 == 1
		msg.add("lightIR"  ).add( (int32_t)state_tsl2561_0x39.lightIR );
		msg.add("lightFull").add( (int32_t)state_tsl2561_0x39.lightFull );
	#endif
	#if i2c_addr_tsl2561_0x49 == 1
		msg.add("lightIR"  ).add( (int32_t)state_tsl2561_0x49.lightIR );
		msg.add("lightFull").add( (int32_t)state_tsl2561_0x49.lightFull );
	#endif



	bndl->add(msg);
}

// Package function when not using multiplexer
#if is_multiplexer != 1
void package_tsl2561(OSCBundle *bndl, char packet_header_string[])
{
	char address_string[255];

	#if i2c_addr_tsl2561_0x29 == 1
		sprintf(address_string, "%s%s%s%s", packet_header_string, "/", tsl2561_0x29_name, "_ir");
		bndl->add(address_string).add( (int32_t)state_tsl2561_0x29.lightIR );
		sprintf(address_string, "%s%s%s%s", packet_header_string, "/", tsl2561_0x29_name, "_full");
		bndl->add(address_string ).add( (int32_t)state_tsl2561_0x29.lightFull );
	#endif
	#if i2c_addr_tsl2561_0x39 == 1
		sprintf(address_string, "%s%s%s%s", packet_header_string, "/", tsl2561_0x39_name, "_ir");
		bndl->add(address_string).add( (int32_t)state_tsl2561_0x39.lightIR );
		sprintf(address_string, "%s%s%s%s", packet_header_string, "/", tsl2561_0x39_name, "_full");
		bndl->add(address_string ).add( (int32_t)state_tsl2561_0x39.lightFull );
	#endif
	#if i2c_addr_tsl2561_0x49 == 1
		sprintf(address_string, "%s%s%s%s", packet_header_string, "/", tsl2561_0x49_name, "_ir");
		bndl->add(address_string).add( (int32_t)state_tsl2561_0x49.lightIR );
		sprintf(address_string, "%s%s%s%s", packet_header_string, "/", tsl2561_0x49_name, "_full");
		bndl->add(address_string ).add( (int32_t)state_tsl2561_0x49.lightFull );
	#endif
}
#endif

// --- MEASURE SHT31D ---
//
// Gets the current sensor readings of the TSL2561s and stores into its state struct
// Takes samples 5 times for average
// 
void measure_tsl2561() 
{
	uint16_t lightIR_ar[5], lightFull_ar[5];

	#if i2c_addr_tsl2561_0x29 == 1
		for (int i = 0; i < 5; i++) {
			inst_tsl2561_0x29.getLuminosity(&lightFull_ar[i], &lightIR_ar[i]);
		}
		state_tsl2561_0x29.lightIR   = (lightIR_ar[0]   + lightIR_ar[1]   + lightIR_ar[2]   + lightIR_ar[3]   + lightIR_ar[4])   / 5;
		state_tsl2561_0x29.lightFull = (lightFull_ar[0] + lightFull_ar[1] + lightFull_ar[2] + lightFull_ar[3] + lightFull_ar[4]) / 5;
	#endif
	#if i2c_addr_tsl2561_0x39 == 1
		for (int i = 0; i < 5; i++) {
			inst_tsl2561_0x39.getLuminosity(&lightFull_ar[i], &lightIR_ar[i]);
		}
		state_tsl2561_0x39.lightIR   = (lightIR_ar[0]   + lightIR_ar[1]   + lightIR_ar[2]   + lightIR_ar[3]   + lightIR_ar[4])   / 5;
		state_tsl2561_0x39.lightFull = (lightFull_ar[0] + lightFull_ar[1] + lightFull_ar[2] + lightFull_ar[3] + lightFull_ar[4]) / 5;
	#endif
	#if i2c_addr_tsl2561_0x49 == 1
		for (int i = 0; i < 5; i++) {
			inst_tsl2561_0x49.getLuminosity(&lightFull_ar[i], &lightIR_ar[i]);
		}
		state_tsl2561_0x49.lightIR   = (lightIR_ar[0]   + lightIR_ar[1]   + lightIR_ar[2]   + lightIR_ar[3]   + lightIR_ar[4])   / 5;
		state_tsl2561_0x49.lightFull = (lightFull_ar[0] + lightFull_ar[1] + lightFull_ar[2] + lightFull_ar[3] + lightFull_ar[4]) / 5;
	#endif
}



// --- DISPLAY SENSOR DETAILS --- 
//
// Displays information about the connected
// TS2561 sensor(s)
//
void details_tsl2561()
{
	sensor_t sensor;

	for (int i = 0; i < 3; i++) {
		switch(i) {
			case 0: 
				#if i2c_addr_tsl2561_0x29 == 1
					inst_tsl2561_0x29.getSensor(&sensor);
					break;
				#else
					continue;
				#endif
			case 1:
				#if i2c_addr_tsl2561_0x39 == 1
					inst_tsl2561_0x39.getSensor(&sensor);
					break;
				#else
					continue;
				#endif
			case 2:
				#if i2c_addr_tsl2561_0x49 == 1
					inst_tsl2561_0x49.getSensor(&sensor);
					break;
				#else
					continue;
				#endif
		} // of switch

		LOOM_DEBUG_Println("------------------------------------");
		LOOM_DEBUG_Println4("Sensor ", i+1, ": ", sensor.name);
		LOOM_DEBUG_Println2("Driver Ver:   ", sensor.version);
		LOOM_DEBUG_Println2("Unique ID:    ", sensor.sensor_id);
		LOOM_DEBUG_Println3("Max Value:    ", sensor.max_value, " lux");
		LOOM_DEBUG_Println3("Min Value:    ", sensor.min_value, " lux");
		LOOM_DEBUG_Println3("Resolution:   ", sensor.resolution," lux");  
		LOOM_DEBUG_Println("------------------------------------");
	}
}


void configure_tsl2561()
{
	// --- Gain settings
	// You can also manually set the gain or enable auto-gain support 
	// TSL2561_GAIN_1X  : No gain  (use in bright light to avoid sensor saturation) 
	// TSL2561_GAIN_16X : 16x gain (use in low light to boost sensitivity)
	// tsl.enableAutoRange(true);    // Auto-gain ... switches automatically between 1x and 16x 


	// --- Integration time
	// Changing the integration time gives you better sensor resolution (402ms = 16-bit data) 
	// TSL2561_INTEGRATIONTIME_101MS) : medium resolution and speed  
	// TSL2561_INTEGRATIONTIME_402MS) : 16-bit data but slowest conversions 


	#if i2c_addr_tsl2561_0x29 == 1
		inst_tsl2561_0x29.setGain(TSL2561_GAIN_1X);
		#if  tsl2561_res == 1
			inst_tsl2561_0x29.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS);
		#elif tsl2561_res == 2
			inst_tsl2561_0x29.setIntegrationTime(TSL2561_INTEGRATIONTIME_101MS);
		#else 
			inst_tsl2561_0x29.setIntegrationTime(TSL2561_INTEGRATIONTIME_402MS);
		#endif
	#endif
	#if i2c_addr_tsl2561_0x39 == 1
		inst_tsl2561_0x39.setGain(TSL2561_GAIN_1X);
		#if  tsl2561_res == 1
			inst_tsl2561_0x39.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS);
		#elif tsl2561_res == 2
			inst_tsl2561_0x39.setIntegrationTime(TSL2561_INTEGRATIONTIME_101MS);
		#else 
			inst_tsl2561_0x39.setIntegrationTime(TSL2561_INTEGRATIONTIME_402MS);
		#endif
	#endif
	#if i2c_addr_tsl2561_0x49  == 1
		inst_tsl2561_0x49.setGain(TSL2561_GAIN_1X);
		#if  tsl2561_res == 1
			inst_tsl2561_0x49.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS);;
		#elif tsl2561_res == 2
			inst_tsl2561_0x49.setIntegrationTime(TSL2561_INTEGRATIONTIME_101MS);;
		#else 
			inst_tsl2561_0x49.setIntegrationTime(TSL2561_INTEGRATIONTIME_402MS);;
		#endif
	#endif


}


