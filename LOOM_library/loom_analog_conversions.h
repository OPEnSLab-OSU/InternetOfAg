// ================================================================ 
// ===                          NOTES                           === 
// ================================================================
// This file contains functions that are used ot convert analog
// readings from the raw values before they are stored into the 
// analog state struct

// Conversions are selected in the config based on the index of the 
// desired conversion

// All conversion functions should take int and return float

// Defines and constants should be placed immediately above their
// associated function, rather than all at the top

// Functions should be named analog_convert_X



// ================================================================ 
// ===                       DEFINITIONS                        === 
// ================================================================
typedef float (*convertRetFuncPtr)(int); 



// ================================================================ 
// ===                   FUNCTION PROTOTYPES                    === 
// ================================================================
float analog_convert_voltage(int analog); 
float analog_convert_triple(int analog); // example
float analog_convert_thermistor(int analog);
float analog_convert_ph(int analog);

float analog_convert_turbidity(int analog);
float analog_convert_ec(int analog);
float analog_convert_tds(int analog);


// ================================================================ 
// ===                     CONVERSION ARRAY                     === 
// ================================================================

static convertRetFuncPtr conversion_list[] = 
{
	analog_convert_voltage,			// 0
	analog_convert_triple,			// 1 
	analog_convert_thermistor,		// 2
	analog_convert_ph,				// 3
	analog_convert_turbidity,		// 4
	analog_convert_ec,				// 5
	analog_convert_tds 				// 6
};



// ================================================================ 
// ===                         TRIPLE                           === 
// ================================================================

// mostly just an example
float analog_convert_triple(int analog)
{
	return 3*analog;
}


// ================================================================ 
// ===                        VOLTAGE                           === 
// ================================================================

float analog_convert_voltage(int analog)
{
	return analog*3.3/4096.0;
}



// ================================================================ 
// ===                ANALOG CONVERT THERMISTOR                 === 
// ================================================================

#define reverse_connect 1

#define THERMISTORNOMINAL 	10000   // resistance at 25 degrees C
#define TEMPERATURENOMINAL 	25   	// temp. for nominal resistance (almost always 25 C)
#define BCOEFFICIENT 		3950  	// The beta coefficient of the thermistor (usually 3000-4000)
//#define SERIESRESISTOR 10000    
#define SERIESRESISTOR 	29330  	// the value of the 'other' resistor
#define range_resol  		4095

float analog_convert_thermistor(int analog)
{
	float average = analog;

	#if reverse_connect == 0
		average = range_resol / average - 1;
		average = SERIESRESISTOR / average;
	#endif
	#if reverse_connect == 1
		average = range_resol / average - 1; 
		average = SERIESRESISTOR * average;
	#endif
	// Serial.print("Thermistor resistance "); 
	// Serial.println(average);
 
	float steinhart;
	steinhart = average / THERMISTORNOMINAL;     // (R/Ro)
	steinhart = log(steinhart);                  // ln(R/Ro)
	steinhart /= BCOEFFICIENT;                   // 1/B * ln(R/Ro)
	steinhart += 1.0 / (TEMPERATURENOMINAL + 273.15); // + (1/To)
	steinhart = 1.0 / steinhart;                 // Invert
	steinhart -= 273.15;                         // convert to C

	return steinhart;
}


// ================================================================ 
// ===                    ANALOG CONVERT PH                     === 
// ================================================================

#define PH_Offset 0.0

float analog_convert_ph(int analog)
{
	float voltage = analog_convert_voltage(analog);
	
	float pHValue = 3.5*voltage + PH_Offset;

	return pHValue;
}


float analog_convert_turbidity(int analog)
{
	float voltage = analog_convert_voltage(analog);

	return -1120.4 * (voltage * voltage) + (5742.3 * voltage) - 4352.9;
}




#define EC_TEMP 25

float analog_convert_ec(int analog)
{
	float voltage = analog_convert_voltage(analog);

	float compensation_coefficient = 1.0 + 0.02 * (EC_TEMP - 25.0); // temperature compensation formula: fFinalResult(25^C) = fFinalResult(current)/(1.0+0.02*(fTP-25.0));
	float comp_volt = voltage / compensation_coefficient;  			// temperature compensation
	float EC = ( 133.42 * comp_volt * comp_volt * comp_volt - 255.86 * comp_volt * comp_volt + 857.39 * comp_volt ); //convert voltage value to EC value
	
	return EC;	
}

float analog_convert_tds(int analog)
{
	float voltage = analog_convert_voltage(analog);

	float compensation_coefficient = 1.0 + 0.02 * (EC_TEMP - 25.0); // temperature compensation formula: fFinalResult(25^C) = fFinalResult(current)/(1.0+0.02*(fTP-25.0));
	float comp_volt = voltage / compensation_coefficient;  			// temperature compensation
	float TDS = ( 133.42 * comp_volt * comp_volt * comp_volt - 255.86 * comp_volt * comp_volt + 857.39 * comp_volt ) * 0.5; //convert voltage value to TDS value
	
	return TDS;	
}




