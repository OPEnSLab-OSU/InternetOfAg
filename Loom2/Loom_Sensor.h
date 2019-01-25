
#ifndef LOOM_SENSOR_h
#define LOOM_SENSOR_h



#include "Loom_Module.h"

#include <OSCBundle.h>



class LoomSensor : public LoomModule
{

protected:
	// --- PROTECTED MEMBERS ---

	char*   sensor_description;

	uint8_t num_samples; 	// How many samples to take

	// struct or individual members of data?
	// might prefer individual, using arrays for similar elements (e.g. different axes)
	// not really a reason for struct unless it is being passes around

public:

	// --- PUBLIC MEMBERS ---


	// --- CONSTRUCTOR ---
	// LoomSensor()
	// {
	// 	LOOM_DEBUG_Println("LoomSensor Default Constructor");		
	// }


	LoomSensor(	char* 	module_name			= "Sensor",
	 
				char* 	sensor_description 	= "Sensor", 
				uint8_t	num_samples 		= 1 

			  );


	// --- DESTRUCTOR ---
	~LoomSensor();


	// --- PUBLIC METHODS ---

	virtual void print_config();

	virtual void print_measurements() = 0;

	virtual void calibrate();

	virtual void measure() = 0;

	virtual void package(OSCBundle* bndl) = 0;

	virtual bool message_route(OSCMessage* msg, int address_offset);

	void set_num_samples(uint8_t n);

	uint8_t get_num_samples();

private:

// --- PRIVATE MEMBERS ---

// --- PRIVATE METHODS --- 



};



#endif