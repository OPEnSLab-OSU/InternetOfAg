// ================================================================
// ===                        LIBRARIES                         ===
// ================================================================
#include <RTClibExtended.h>
// NOTE: Must include the following line in the RTClibExtended.h file to use with M0:
// #define _BV(bit) (1 << (bit))


#if is_rtc3231 == 1
	#define EI_NOTEXTERNAL
	#include <EnableInterrupt.h>
#endif

// ================================================================ 
// ===                       DEFINITIONS                        === 
// ================================================================

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};


// const int hour_adjustment   = 0;
// const int minute_adjustment = 0;

// ================================================================ 
// ===                        STRUCTURES                        === 
// ================================================================ 
struct state_rtc_t {
	char    timestring[20], datestring[20], weekday[10];
	int     year;
	uint8_t month, day, hour, minute, second;
};


// ================================================================ 
// ===                   GLOBAL DECLARATIONS                    === 
// ================================================================

struct state_rtc_t state_rtc;

// Select correct RTC object based on which is being used
#if  is_rtc3231 == 1
	RTC_DS3231  rtc_inst;
#elif is_rtc8523 == 1
	RTC_PCF8523 rtc_inst;
#endif


	// Some of these should be deleted 

volatile bool alarm_flag = true;  // Flag is set with external Pin A0 Interrupt by RTC
volatile bool LEDState   = false; // flag to toggle LED

// #if LOOM_DEBUG == 0
// 	volatile int WakePeriodMin = 15;  // Period of time to take sample in Min, reset alarm based on this period (Bo - 5 min)
// #elif  LOOM_DEBUG == 1
// 	volatile int WakePeriodMin = 1;
// #endif // LOOM_DEBUG

// ================================================================ 
// ===                   FUNCTION PROTOTYPES                    === 
// ================================================================
void  setup_rtc();
void  measure_rtc();
char* get_datestring(); 
char* get_timestring();
char* get_weekday();

#if  is_rtc3231 == 1
	void setAlarmFunction();
	void clearRTCAlarms();
	void rtc_interrupt_reset();
	void wake_RTC_ISR();
#endif

void print_time();
void print_DateTime(DateTime time);


// ================================================================
// ===                          SETUP                           ===
// ================================================================


void setup_rtc() {
	if (!rtc_inst.begin()) {
		LOOM_DEBUG_Println("Couldn't find RTC");
		while (1);
	}

	// The following section checks if RTC is running, else sets 
	// the time to the time that the sketch was compiled
	#if is_rtc3231 == 1
		// This may end up causing a problem in practice - what if RTC looses power in field? Shouldn't happen with coin cell batt backup
		if (rtc_inst.lostPower()) {
			LOOM_DEBUG_Println("RTC lost power, lets set the time!");
			rtc_inst.adjust(DateTime(F(__DATE__), F(__TIME__)));
		}
	#elif is_rtc8523 == 1
		if (!rtc_inst.initialized()) {
			LOOM_DEBUG_Println("RTC is NOT running!");
			rtc_inst.adjust(DateTime(F(__DATE__), F(__TIME__)));
		}
	#endif

	#if  is_rtc3231 == 1
		clearRTCAlarms(); 	//clear any pending alarms

		//Set SQW pin to OFF (in my case it was set by default to 1Hz)
		//The output of the DS3231 INT pin is connected to this pin
		rtc_inst.writeSqwPinMode(DS3231_OFF);

		// Configure RTC Interrupt pin to be input
		pinMode(RTC_pin, INPUT_PULLUP);

	#endif	

	// Query Time and print
	#if LOOM_DEBUG == 1
		print_time();
	#endif

}


// ================================================================
// ===                        FUNCTIONS                         ===
// ================================================================

void measure_rtc() { 
	DateTime now = rtc_inst.now();

	state_rtc.year   = now.year();
	state_rtc.month  = now.month();
	state_rtc.day    = now.day();
	state_rtc.hour   = now.hour();
	state_rtc.minute = now.minute();
	state_rtc.second = now.second();

	sprintf(state_rtc.weekday, "%s", daysOfTheWeek[now.dayOfTheWeek()]);
	
	String(String(state_rtc.year,   DEC) + '/' 
	     + String(state_rtc.month,  DEC) + '/' 
	     + String(state_rtc.day,    DEC))
	     .toCharArray(state_rtc.datestring, 20);

	String(String(state_rtc.hour,   DEC) + ':' 
	     + String(state_rtc.minute, DEC) + ':' 
	     + String(state_rtc.second, DEC))
	     .toCharArray(state_rtc.timestring, 20);
}

char* get_datestring() {
	measure_rtc();
	return state_rtc.datestring;
}

char* get_timestring() {
	measure_rtc();
	return state_rtc.timestring;
}

char* get_weekday() {
	measure_rtc();
	return state_rtc.weekday;
}



#if  is_rtc3231 == 1

// --- SET RTC ALARM RELATIVE
void setRTCAlarm_Relative(int hours, int minutes, int seconds)
{
	// Display current time
	LOOM_DEBUG_Println("Current Time:");
	print_DateTime( rtc_inst.now() );

	// Calculate new time
	DateTime future( rtc_inst.now() + TimeSpan(0,hours,minutes,seconds) );
	LOOM_DEBUG_Println("Resetting Alarm 1 for:"); 
	print_DateTime(future);

	// Set alarm 1																	// not sure if this 0 is correct
	rtc_inst.setAlarm(ALM1_MATCH_HOURS, future.second(), future.minute(), future.hour(), 0); 
	rtc_inst.alarmInterrupt(1, true);
}


// --- SET RTC ALARM RELATIVE
void setRTCAlarm_Absolute(int hour, int minute, int second)
{
	DateTime now = rtc_inst.now();

	// Display current time
	LOOM_DEBUG_Println("Current Time:");
	print_DateTime( now );

	// Calculate new time (checking to make sure alarm is not set in past)
	DateTime future = DateTime(now.year(), now.month(), now.day(), hour, minute, second);

	// If that time has already passed on current day, set for next day
	// Using Timespan of 24 hours to prevent errors
	if ( (future - now).totalseconds() < 0 ) {
		LOOM_DEBUG_Println("Time already passed, setting for next day");
		future = future + TimeSpan(1,0,0,0);
	} 

	LOOM_DEBUG_Println("Resetting Alarm 1 for:"); 
	print_DateTime(future);

	// Set alarm 1																	// not sure if this 0 is correct
	rtc_inst.setAlarm(ALM1_MATCH_HOURS, future.second(), future.minute(), future.hour(), 0); 	    								
	rtc_inst.alarmInterrupt(1, true);
}



// --- CLEAR RTC ALARMS ---
//
// Clear any pending alarms on the RTC
//
void clearRTCAlarms() {
	rtc_inst.armAlarm(1, false);
	rtc_inst.clearAlarm(1);
	rtc_inst.alarmInterrupt(1, false);
	rtc_inst.armAlarm(2, false);
	rtc_inst.clearAlarm(2);
	rtc_inst.alarmInterrupt(2, false);
}



void rtc_interrupt_reset()
{
	// Clears any interrupts that may be pending on M0
	detachInterrupt(digitalPinToInterrupt(RTC_pin));
	delay(20);
	attachInterrupt(digitalPinToInterrupt(RTC_pin), wake_RTC_ISR, LOW);
}


// --- WAKE RTC ISR ---
//
// The interrupt service routine called when a device 
// wakes from sleep via an RTC interrupt
//
void wake_RTC_ISR()
{
	detachInterrupt(digitalPinToInterrupt(RTC_pin));
}

#endif // of is_rtc3231 == 1



// --- PRINT TIME ---
//
// Prints the time according to the RTC clock
//
void print_time()
{
	measure_rtc();
	LOOM_DEBUG_Println2("Date: ", get_datestring());
	LOOM_DEBUG_Println2("Time: ", get_timestring());
	LOOM_DEBUG_Println2("Day : ", get_weekday());
}


// --- PRINT DATETIME ---
//
// Prints a DateTime object
//
void print_DateTime(DateTime time) 
{
	#if LOOM_DEBUG == 1
		Serial.print(time.year());   Serial.print('/');
		Serial.print(time.month());  Serial.print('/');
		Serial.print(time.day());    Serial.print(' ');
		Serial.print(time.hour());   Serial.print(':');
		Serial.print(time.minute()); Serial.print(':');
		Serial.print(time.second()); Serial.println();
	#endif
}



