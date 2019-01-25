
#include "Loom_LoRa.h"
#include "Loom_Macros.h"



// --- CONSTRUCTOR ---
Loom_LoRa::Loom_LoRa( 	char* module_name,
						uint16_t  max_message_len,
						// CommScope subnet_scope,
						bool      compress_messages,

						uint8_t   address,
						uint8_t   friend_address,
						uint8_t   power_level,
						uint8_t   retry_count,
						uint16_t  retry_timeout 	)
	// : LoomCommPlat( module_name, max_message_len, subnet_scope, compress_messages )
	: LoomCommPlat( module_name, max_message_len, compress_messages )

{ 
	// LOOM_DEBUG_Println("LoRa Constructor 1");
	LOOM_DEBUG_Println("LoRa Setup");

	// Create instances of driver and manager
	this->driver         = new RH_RF95(RFM95_CS, RFM95_INT);
	this->manager        = new RHReliableDatagram(*driver, address);
	this->address        = address;
	this->friend_address = friend_address;
	this->power_level    = ( (power_level >= 5) && (power_level <= 23) ) ? power_level : 23;
	this->retry_count    = retry_count;
	this->retry_timeout  = retry_timeout;

	// #if hub_node_type == 0 // if hub
	pinMode(8, INPUT_PULLUP);
	// #endif

	bool status;

	// Initialize Manager
	LOOM_DEBUG_Print("\tInitializing Manager ... ");
	status = manager->init();
	LOOM_DEBUG_Println( (status) ? "Success" : "Failed");
	
	// Set Frequency
	LOOM_DEBUG_Print("\tSetting Frequency ... ");
	status = driver->setFrequency(RF95_FREQ);
	LOOM_DEBUG_Println( (status) ? "Success" : "Failed" );

	// Set Power Level
	LOOM_DEBUG_Println2("\tSetting Power Level to ", this->power_level);
	driver->setTxPower(this->power_level, false);

	// Set Retry Delay
	LOOM_DEBUG_Println3("\tSetting LoRa retry timeout to ", this->retry_timeout, " ms");
	manager->setTimeout(this->retry_timeout);

	// Set Max Retry Count
	LOOM_DEBUG_Println2("\tSetting LoRa max retry count ", this->retry_count);
	manager->setRetries(this->retry_count);
}

// --- DESTRUCTOR ---
Loom_LoRa::~Loom_LoRa() {}


// --- PUBLIC METHODS ---

void Loom_LoRa::print_config() 
{
	LoomCommPlat::print_config();

	LOOM_DEBUG_Println3('\t', "Address             : ", address );
	LOOM_DEBUG_Println3('\t', "Power Level         : ", power_level );
	LOOM_DEBUG_Println3('\t', "Retry Count         : ", retry_count );
	LOOM_DEBUG_Println3('\t', "Retry Timeout       : ", retry_timeout );
}



// Build OSC Bundle from packet if any exists
bool Loom_LoRa::receive_bundle(OSCBundle *bndl) 
{
	if ( manager->available() ) {
		uint8_t len = RH_RF95_MAX_MESSAGE_LEN;
		uint8_t from;
		uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
		memset(buf, '\0', RH_RF95_MAX_MESSAGE_LEN);

		if ( manager->recvfromAck(buf, &len, &from) ) {

			signal_strength = driver->lastRssi();

			LOOM_DEBUG_Println2("Received: ", (const char*)buf);
			LOOM_DEBUG_Println2("Len: ", len);

			// This is done just in case the compressed string, uncompresses to more than 251 characters
			char larger_buf[384];
			memset(larger_buf, '\0', sizeof(larger_buf));
			strcpy(larger_buf, (const char*)buf);

			LOOM_DEBUG_Println("Memset larger_buf");

			LoomCommPlat::convert_string_to_bundle( (char*)larger_buf, bndl ); 

			LOOM_DEBUG_Println("Converted string to bundle");

			// Apply filtering based on family and subnet
			bool in_scope = LoomCommPlat::scope_filter(bndl);
			if (!in_scope) {
				LOOM_DEBUG_Println("Received LoRa bundle out of scope");
			}

			return in_scope;
			 
		} // of if (manager.recvfromAck(buf, &len, &from))
	} // of if (manager.available()) 

	return false;
}




// Send an OSC bundle
// Should overload or use default args to optionally allow default address
bool Loom_LoRa::send_bundle(OSCBundle *bndl, uint16_t destination) 
// bool send_bundle(OSCBundle *bndl, uint16_t destination=01) 
// bool send_bundle(OSCBundle *bndl) 
{
	LOOM_DEBUG_Println2("Sending LoRa bundle to address: ", destination);

	char message[RH_RF95_MAX_MESSAGE_LEN];
	memset(message, '\0', sizeof(message));
	LoomCommPlat::convert_bundle_to_string(bndl, message);

	LOOM_DEBUG_Println(message);
	LOOM_DEBUG_Println2("Message length: ", strlen(message));
	 
	// bool is_sent = manager->sendtoWait( (uint8_t*)message, strlen(message)+1, destination );
		bool is_sent = manager->sendtoWait( (uint8_t*)message, strlen(message), destination );

	LOOM_DEBUG_Println2("Send LoRa bundle " , (is_sent) ? "successful" : "failed" );

	signal_strength = driver->lastRssi(); 

	return is_sent;
}

bool Loom_LoRa::send_bundle(OSCBundle *bndl) 
{
	send_bundle(bndl, friend_address);
}



// Broadcast an OSC bundle
void Loom_LoRa::broadcast_bundle(OSCBundle *bndl) 
{

}


void Loom_LoRa::set_address(uint addr)    // Need to test this
{ 
	address = addr;
	delete manager;
	manager = new RHReliableDatagram(*driver, address);
}


uint Loom_LoRa::get_address() 
{ 
	return address; 
}


void Loom_LoRa::set_friend_address(uint addr) 
{ 
	friend_address = addr; 
}


uint Loom_LoRa::get_friend_address() 
{ 
	return friend_address; 
}
