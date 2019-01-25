
#ifndef LOOM_TSL2591_h
#define LOOM_TSL2591_h


#include "Loom_I2C_Sensor.h"

#include <Adafruit_Sensor.h>
#include <Adafruit_TSL2591.h>


class Loom_TSL2591 : public LoomI2CSensor
{

protected:

	// Add gain and timing settings

	Adafruit_TSL2591 inst_tsl2591;

	uint16_t vis;
	uint16_t ir;
	uint16_t full;

	uint8_t gain_level;
	uint8_t timing_level;

public:

	// --- PUBLIC MEMBERS ---


	// --- CONSTRUCTOR ---
	Loom_TSL2591(	byte i2c_address 			= 0x29,
					
					char* module_name 			= "LIS3DH",
					char* sensor_description 	= "Lux",

					uint8_t gain_level 			= 1, // document 
					uint8_t timing_level 		= 0  // document
				);


	// --- DESTRUCTOR ---
	~Loom_TSL2591();

	void print_measurements();

	void measure();

	void package(OSCBundle* bndl);	

	// void package(OSCBundle* bndl, char* id_prefix);	
	// void package(OSCBundle* bndl) { LoomModule::package(bndl); }


	void package_mux(OSCBundle* bndl, char* id_prefix, uint8_t port);

	void configure_settings();

private:

};


#endif