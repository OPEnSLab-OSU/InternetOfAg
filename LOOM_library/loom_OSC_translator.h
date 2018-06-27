// ================================================================
// ===                        LIBRARIES                         ===
// ================================================================
#include <string.h>


// ================================================================ 
// ===                       DEFINITIONS                        === 
// ================================================================
// enum DATA_FORMAT {
// 	BUNDLE_MULTI,
// 	BUNDLE_SINGLE,
// 	STRING_MULTI,
// 	STRING_SINGLE,
// 	ARRAY_KEY_VALUE,
// 	ASSOC_ARRAY,
// 	INVALID
// };


// ================================================================ 
// ===                   FUNCTION PROTOTYPES                    === 
// ================================================================

#if LOOM_DEBUG == 1
	void print_bundle(OSCBundle *bndl);
#endif
int    get_bundle_bytes(OSCBundle *bndl); 					// relatively untested
String get_data_value(OSCMessage* msg, int pos);
void   deep_copy_bundle(OSCBundle *srcBndl, OSCBundle *destBndl);

// Conversions between bundles and strings
void convert_OSC_string_to_bundle(char *osc_string, OSCBundle* bndl);
void convert_OSC_bundle_to_string(OSCBundle *bndl, char *osc_string);

// Conversion between bundle formats
void convert_OSC_multiMsg_to_singleMsg(OSCBundle *bndl, OSCBundle *outBndl);
void convert_OSC_multiMsg_to_singleMsg(OSCBundle *bndl);
void convert_OSC_singleMsg_to_multiMsg(OSCBundle *bndl, OSCBundle *outBndl);
void convert_OSC_singleMsg_to_multiMsg(OSCBundle *bndl);

// Conversion from bundle to array formats
void convert_OSC_to_array_key_value(OSCBundle *bndl, String key_values[], int kv_len);
void convert_OSC_to_arrays_assoc(OSCBundle *bndl, String keys[], String values[], int assoc_len);

// Conversion from array to bundle formats
void convert_OSC_key_value_array_to_singleMsg(String key_values [], OSCBundle *bndl, char packet_header[], int kv_len, int interpret);
void convert_OSC_key_value_array_to_singleMsg(String key_values [], OSCBundle *bndl, char packet_header[], int kv_len);
void convert_OSC_key_value_array_to_multiMsg( String key_values [], OSCBundle *bndl, char packet_header[], int kv_len, int interpret);
void convert_OSC_key_value_array_to_multiMsg( String key_values [], OSCBundle *bndl, char packet_header[], int kv_len);
void convert_OSC_assoc_arrays_to_singleMsg(String keys [], String values [], OSCBundle *bndl, char packet_header[], int assoc_len, int interpret);
void convert_OSC_assoc_arrays_to_singleMsg(String keys [], String values [], OSCBundle *bndl, char packet_header[], int assoc_len);
void convert_OSC_assoc_arrays_to_multiMsg( String keys [], String values [], OSCBundle *bndl, char packet_header[], int assoc_len, int interpret);
void convert_OSC_assoc_arrays_to_multiMsg( String keys [], String values [], OSCBundle *bndl, char packet_header[], int assoc_len);

// Conversion between array formats
void convert_array_key_value_to_assoc(String key_values [], String keys [], String values [], int kv_len, int assoc_len);
void convert_array_assoc_to_key_value(String keys [], String values [], String key_values [], int assoc_len, int kv_len);


// ================================================================ 
// ===                        STRUCTURES                        === 
// ================================================================
union data_value { // Used in translation between OSC and strings
	int32_t i;
	float f;
	uint32_t u;
};

// ================================================================
// ===                    GENERAL FUNCTIONS                     ===
// ================================================================


// --- PRINT BUNDLE ---
//
// ** NOTE: Can make bundles unreadable after printing, use with caution **
//
// Print out the contents and structure of an OSC bundle
//
// @param bndl  An OSC bundle to be printed
//
#if LOOM_DEBUG == 1
void print_bundle(OSCBundle *bndl)
{
	int n = 0;
	char buf[50];
	char data_type;
	Serial.println("Bundle Size: ");
	Serial.println(bndl->size());
	OSCMessage *msg = bndl->getOSCMessage(n);
	
	while (msg != NULL) {
		msg->getAddress(buf, 0);
		Serial.print("Address ");
		Serial.print(n + 1);
		Serial.print(": ");
		Serial.println(buf);

		int m = 0;
		data_type = msg->getType(m);
		while (data_type != '\0') {
			Serial.print("Value ");
			Serial.print(m + 1);
			Serial.print(": ");
			if (data_type == 'f') {
				Serial.print("(f) ");
				Serial.println(msg->getFloat(m));
			}
			else if (data_type == 'i') {
				Serial.print("(i) ");
				Serial.println(msg->getInt(m));
			}
			else if (data_type == 's') {
				Serial.print("(s) ");
				msg->getString(m, buf, 50);
				Serial.println(buf);
			}

			m++;
			data_type = msg->getType(m);
		}
		n++;
		msg = bndl->getOSCMessage(n);
	}
}
#endif // of LOOM_DEBUG == 1



// --- GET BUNDLE BYTES ---
//
// Gets the size of a bundle in bytes
//
// @param bndl  The bndl to get the size of
// 
// @return The size of the bundle in bytes
// 
int get_bundle_bytes(OSCBundle *bndl)
{
	int total = 0;
	for (int i = 0; i < bndl->size(); i++) {
		total += bndl->getOSCMessage(i)->bytes();
	}
}



// --- GET DATA VALUE ---
// 
// @param msg  An OSCMessage, 
// @param pos  The position of an argument inside the OSCMessage.
//
// @return The value of the argument as a string.
//
String get_data_value(OSCMessage* msg, int pos) 
{
	if (pos < msg->size()) {
		switch (msg->getType(pos)) {
			case 'i':
				return String(msg->getInt(pos));
				break;
			case 'f':
				return String(msg->getFloat(pos));
				break;
			case 's':
				char buf[80];
				msg->getString(pos, buf, 80);
				return String(buf);
				break;
			default:
				LOOM_DEBUG_Println("Unsupported data data_type.");
				return String("");
		}
	}
	LOOM_DEBUG_Println2("Message does not have an argument with position: ", pos);

	return String("");
}



// --- DEEP COPY BUNDLE ---
// 
// Takes two bundle pointers,
// Copies the data of the first into the second
// 
// @param srcBndl   The source bundle to be copied
// @param destBndl  The bundle to copied into
//
void deep_copy_bundle(OSCBundle *srcBndl, OSCBundle *destBndl) {
	destBndl->empty();
	OSCMessage *msg;
	OSCMessage tmpMsg;
	char buf[50];
	for (int i = 0; i < srcBndl->size(); i++) { 	// for each message
		msg = srcBndl->getOSCMessage(i);
		for (int j = 0; j < msg->size(); j++) { 	// for each argument 

			switch (msg->getType(j)) {
	 			case 'i': tmpMsg.add(msg->getInt(j));	break;
	 			case 'f': tmpMsg.add(msg->getFloat(j));	break;
	 			case 's': char buf[80];  msg->getString(j, buf, 80);  tmpMsg.add(buf);  break;
	 			default: LOOM_DEBUG_Println("Unsupported data data_type.");
	 		}
 		}
 		msg->getAddress(buf);
		tmpMsg.setAddress(buf);

		destBndl->add(tmpMsg);
		tmpMsg.empty();		
	} 
}



// ================================================================
// ===         CONVERSIONS BETWEEN BUNDLES AND STRINGS          ===
// ================================================================



// --- CONVERT STRING TO OSC ---
// 
// Converts string used by LoRa to an equivalent OSCBundle 
//
// @param osc_string  A char * created through the use of convert_OSC_bundle_to_string(), 
// @param bndl        The OSC bundle to be populated
//
void convert_OSC_string_to_bundle(char *osc_string, OSCBundle* bndl) 
{
	bndl->empty();
	data_value value_union;
	char buf[strlen(osc_string)+1];
	char *p = buf, *p2 = NULL;
	char *token = NULL, *msg_token = NULL; 
	strcpy(buf, osc_string);
	OSCMessage *msg;
	msg_token = strtok_r(p, " ", &p);
	while (msg_token != NULL & strlen(msg_token) > 0) {
		p2 = msg_token;
		token = strtok_r(p2, ",", &p2);
		msg = &(bndl->add(token));
		token = strtok_r(NULL, ",", &p2);
		int k = 1;
		while (token != NULL & strlen(token) > 0) {
			if (token[0] == 'f') {
				value_union.u = strtoul(&token[1], NULL, 0);
				msg->add(value_union.f);
			}
			else if (token[0] == 'i') {
				value_union.u = strtoul(&token[1], NULL, 0);
				msg->add(value_union.i);
			}
			else if (token[0] == 's') {
				msg->add(&token[1]);
			}
			token = strtok_r(p2, ",", &p2);
		}
		msg_token = strtok_r(p, " ", &p);
	}
}


// --- CONVERT OSC TO STRING ---
// 
// Converts an OSC Bundle to equivalent string to be used in LoRa transmissions
// Osc_string's contents will now include the OSCBundle's formatted data.
//
// @param bndl        An OSCBundle to put into string format.
// @param osc_string  A char * to fill with the OSCBundle's data.
//
void convert_OSC_bundle_to_string(OSCBundle *bndl, char *osc_string) 
{
	char data_type;
	data_value value;
	int addr_len = 40;
	OSCMessage* msg; 
	char addr_buf[addr_len];

	memset(osc_string, '\0', sizeof(osc_string));

	for (int i = 0; i < bndl->size(); i++) {
		msg = bndl->getOSCMessage(i);
		memset(addr_buf, '\0', addr_len);
		msg->getAddress(addr_buf, 0);
		strcat(osc_string, addr_buf);

		for (int j = 0; j < msg->size(); j++) {
			data_type = msg->getType(j);
			switch (data_type) {
				case 'f':
					value.f = msg->getFloat(j);
					snprintf(addr_buf, addr_len, ",f%lu", value.u);
					strcat(osc_string, addr_buf);
					break;

				case 'i':
					value.i = msg->getInt(j);
					snprintf(addr_buf, addr_len, ",i%lu", value.u);
					strcat(osc_string, addr_buf);
					break;

				case 's':
					char data_buf[40];
					msg->getString(j, data_buf, sizeof(data_buf));
					snprintf(addr_buf, addr_len, ",s%s", data_buf);
					strcat(osc_string, addr_buf);
					break;

				default:
					if (data_type != '\0') {
						LOOM_DEBUG_Println("Invalid message arg type");
					}
					break;
			}
		}
		if (msg != NULL) strcat(osc_string, " ");
	}
}



// ================================================================
// ===            CONVERSION BETWEEN BUNDLE FORMATS             ===
// ================================================================



// --- CONVERT OSC MULTIMSG TO SINGLEMSG ---
//
// This converts an OSC bundle formated in multiple messages, 
// the end of each address being a key, and only 
// one argument, the corresponding value:
// 		Bundle Size: 3
// 		Address 1: /LOOM/Device7/abc
// 		Value 1: 1
// 		Address 2: /LOOM/Device7/def
// 		Value 1: 2.00
// 		Address 3: /LOOM/Unknown7/ghi
// 		Value 1: 3.20
//
// to an OSC bundle with a single message, in the format:
// 		Bundle Size: 1
//		Address 1 /LOOM/Device7/data abc 1 def 2.0 ghi 3.2 
//
// This is the inverse of convert_OSC_singleMsg_to_multiMsg()
//
// @param bndl     The OSC bundle to be converted
// @param outBndl  Where to store the converted bundle
//
void convert_OSC_multiMsg_to_singleMsg(OSCBundle *bndl, OSCBundle *outBndl)
{
	// Check that bundle is in expected multi-message format
	if (bndl->size() == 1) { // Already in single message format
		LOOM_DEBUG_Println("Bundle already in single-message format");

		// Verify format and return
		// Else error
		return;
	}
	outBndl->empty();

	// Get packet header (will use address of first message)
	char address[50];
	bndl->getOSCMessage(0)->getAddress(address);
	snprintf(address, strrchr(address,'/')-address+1, "%s", address);
	sprintf(address, "%s%s", address, "/data");
	OSCMessage newMsg = OSCMessage(address);
	OSCMessage* msg;     // Temporarily hold message of bundle
	
	// Copy data of messages into new bundle
	for (int i = 0; i < bndl->size(); i++){
		msg = bndl->getOSCMessage(i); 	// Get ith messsage

		// Get ith messsage key and add to new message
		char tmpBuf[50], keyBuf[50];
		msg->getAddress(tmpBuf);
		strncpy(keyBuf, strrchr(tmpBuf,'/')+1, 49);
		newMsg.add(keyBuf);

		// Get ith message data and add to new message
		switch (msg->getType(0)) {
			case 'f': newMsg.add(msg->getFloat(0));	break;
			case 'i': newMsg.add(msg->getInt(0));	break;
			case 's': char buf[80];  msg->getString(0, buf, 50);  newMsg.add(buf);  break;
			default: LOOM_DEBUG_Println("Unsupported data data_type.");
		}
	}
	outBndl->add(newMsg);
}



// OVERLOADED version of the above function
//
// This one is for converting bundles 'in-place',
// that is, only one bundle needs to be provided,
// rather than a source and destination of the conversion
//
// See above function for further details on conversion
//
// @param bndl  The bundle to be converted 
// 
void convert_OSC_multiMsg_to_singleMsg(OSCBundle *bndl)
{
	OSCBundle outBndl;
	convert_OSC_multiMsg_to_singleMsg(bndl, &outBndl);
	deep_copy_bundle(&outBndl, bndl);
}



// --- CONVERT OSC SINGLEMSG TO MULTIMSG ---
//
// This converts an OSC bundle formated as an 
// OSC bundle with a single message, in the format:
// 		Bundle Size: 1
//		Address 1 /LOOM/Device7/data abc 1 def 2.0 ghi 3.2 
//
// to an OSC bundle with multiple messages, 
// the end of each address being a key, and only 
// one argument, the corresponding value:
// 		Bundle Size: 3
// 		Address 1: /LOOM/Device7/abc
// 		Value 1: 1
// 		Address 2: /LOOM/Device7/def
// 		Value 1: 2.00
// 		Address 3: /LOOM/Unknown7/ghi
// 		Value 1: 3.20
//
// This is the inverse of convert_OSC_multiMsg_to_singleMsg()
//
// @param bndl     The OSC bundle to be converted
// @param outBndl  Where to store the converted bundle
//
void convert_OSC_singleMsg_to_multiMsg(OSCBundle *bndl, OSCBundle *outBndl)
{
	// Check that bundle is in expected single-message format
	// Also needs to be in key - value pairs
	if (bndl->size() != 1) {
		// Error
		LOOM_DEBUG_Println("Bundle not in single-message format");
		return;
	}
	
	outBndl->empty();
 	OSCMessage *msg = bndl->getOSCMessage(0); 
	OSCMessage tmpMsg;
 	char address[50], newAddress[50], keyBuf[50];

	// Save old packet header address
 	msg->getAddress(address);

	// Copy flat data into messages of '/some/address/key value' format
 	for (int i = 1; i < msg->size(); i+=2) {
 		switch (msg->getType(i)) {
 			case 'i': tmpMsg.add(msg->getInt(i));	break;
 			case 'f': tmpMsg.add(msg->getFloat(i));	break;
 			case 's': char buf[80];  msg->getString(i, buf, 80);  tmpMsg.add(buf);  break;
 			default: LOOM_DEBUG_Println("Unsupported data data_type.");
 		}

		// Add message as /address/key value
		snprintf(newAddress, strrchr(address,'/')-address+1, "%s", address);
		msg->getString(i-1, keyBuf, 80);
		sprintf(newAddress, "%s%s%s", newAddress, "/", keyBuf);
		tmpMsg.setAddress(newAddress);

		outBndl->add(tmpMsg);
		tmpMsg.empty();	
 	} 
}

// OVERLOADED version of the above function
//
// This one is for converting bundles 'in-place',
// that is, only one bundle needs to be provided,
// rather than a source and destination of the conversion
//
// See above function for further details on conversion
//
// @param bndl  The bundle to be converted 
// 
void convert_OSC_singleMsg_to_multiMsg(OSCBundle *bndl)
{
	OSCBundle outBndl;
	convert_OSC_singleMsg_to_multiMsg(bndl, &outBndl);
	deep_copy_bundle(&outBndl, bndl);
}



// ================================================================
// ===         CONVERSION FROM BUNDLE TO ARRAY FORMATS          ===
// ================================================================




// --- CONVERT OSC TO ARRAY KEY VALUE --- 
//
// Converts an OSC bundle to an array formated as:
// 	 [key1, value1, key2, value2, key3, value3]
// Converts bundle to flat single message if not already
//
// @param bndl        The bundle to use to fill the key value array
// @param key_values  The array to be populated
//
void convert_OSC_to_array_key_value(OSCBundle *bndl, String key_values[], int kv_len)
{	
	// Convert bundle to flat single message if not already
	OSCBundle convertedBndl;	
	if (bndl->size() > 1)  {
		convert_OSC_multiMsg_to_singleMsg(bndl, &convertedBndl); 
	} else {
		convertedBndl = *bndl;
	} 

	// Make sure key_values array is large enough
	if ( convertedBndl.getOSCMessage(0)->size() > kv_len ) {
		LOOM_DEBUG_Println("Key-values array not large enough to hold all of bundle data, cannot convert");
		return;
	}

	// Fill key-value array
	OSCMessage* msg = convertedBndl.getOSCMessage(0);	
	for (int i = 0; i < msg->size(); i++) {
		key_values[i] = get_data_value(msg, i); 
	}
}



// --- CONVERT OSC TO ARRAYS ASSOC --- 
//
// Converts an OSC bundle to two associated arrays,
// formated as:
//   [key1,   key2,   key3]
//   [value1, value2, value3]
// Converts bundle to flat single message if not already
//
// @param bndl   The bundle to use to fill the arrays
// @param keys   The array of keys be populated
// @param value  The array of values to be populated
//
void convert_OSC_to_arrays_assoc(OSCBundle *bndl, String keys[], String values[], int assoc_len)
{
	// Convert bundle to flat single message if not already
	OSCBundle convertedBndl;	
	if (bndl->size() > 1) {
		convert_OSC_multiMsg_to_singleMsg(bndl, &convertedBndl);
	} else {
		convertedBndl = *bndl;
	}

	// Make sure keys and values arrays are large enough
	if ( convertedBndl.getOSCMessage(0)->size() > 2*assoc_len ) {
		LOOM_DEBUG_Println("Key-values array not large enough to hold all of bundle data, cannot convert");
		return;
	}

	// Fill key and value arrays
	OSCMessage* msg = convertedBndl.getOSCMessage(0);
	for (int i = 0; i < msg->size(); i+=2) {
		keys[i/2]   = get_data_value(msg, i);
		values[i/2] = get_data_value(msg, i+1); 
	}
}



// ================================================================
// ===         CONVERSION FROM ARRAY TO BUNDLE FORMATS          ===
// ================================================================

// Interpret is an optional parameter in the following array to bundle functions
// The encoding is as follows:
//   0:  Smart Interpret (int->int, float->float, other->string) 
//                         [This is the default if interpret is not specified]
//   1:  Int             (non-ints will become 0)
//   2:  Float           (non-floats will become 0)
//   3:  String          (does no extra manipulation, leaves as strings)





void convert_OSC_key_value_array_to_singleMsg(String key_values [], OSCBundle *bndl, char packet_header[], int kv_len, int interpret)
{
	bndl->empty();
	OSCMessage tmpMsg;// = bndl->add(packet_header);

	// for (int i = 0; i < kv_len; i++) {
	// 	LOOM_DEBUG_Println(key_values[i]);
	// }

	char buf[50];
	for (int i = 0; i < kv_len; i++) {
		key_values[i].toCharArray(buf, 50);
		tmpMsg.add(buf);
	}

	char address[80];
	sprintf(address, "%s/data", packet_header);
	tmpMsg.setAddress(address);
	bndl->add(tmpMsg);
}

void convert_OSC_key_value_array_to_singleMsg(String key_values [], OSCBundle *bndl, char packet_header[], int kv_len) { 
	convert_OSC_key_value_array_to_singleMsg(key_values, bndl, packet_header, kv_len, 0); 
}



void convert_OSC_key_value_array_to_multiMsg(String key_values [], OSCBundle *bndl, char packet_header[], int kv_len, int interpret)
{
	// Convert to single message
	convert_OSC_key_value_array_to_singleMsg(key_values, bndl, packet_header, kv_len, interpret);
	// Then convert bundle to multiple messages
	convert_OSC_singleMsg_to_multiMsg(bndl);
}

void convert_OSC_key_value_array_to_multiMsg(String key_values [], OSCBundle *bndl, char packet_header[], int kv_len) { 
	convert_OSC_key_value_array_to_multiMsg(key_values, bndl, packet_header, kv_len, 0); 
}



void convert_OSC_assoc_arrays_to_singleMsg(String keys [], String values [], OSCBundle *bndl, char packet_header[], int assoc_len, int interpret)
{
	// Convert to single array first 
	int kv_len = 2*assoc_len;
	String key_values[kv_len];
	convert_array_assoc_to_key_value(keys, values, key_values, assoc_len, kv_len);
	// Then convert to single message bundle
	convert_OSC_key_value_array_to_singleMsg(key_values, bndl, packet_header, kv_len, interpret);
}

void convert_OSC_assoc_arrays_to_singleMsg(String keys [], String values [], OSCBundle *bndl, char packet_header[], int assoc_len) { 
	convert_OSC_assoc_arrays_to_singleMsg(keys, values, bndl, packet_header, assoc_len, 0); 
}



void convert_OSC_assoc_arrays_to_multiMsg(String keys [], String values [], OSCBundle *bndl, char packet_header[], int assoc_len, int interpret)
{
	// Convert to single array first 
	int kv_len = 2*assoc_len;
	String key_values[kv_len];
	convert_array_assoc_to_key_value(keys, values, key_values, assoc_len, kv_len);
	// Then convert to single then multi-message bundle
	convert_OSC_key_value_array_to_multiMsg(key_values, bndl, packet_header, kv_len, interpret);
}

void convert_OSC_assoc_arrays_to_multiMsg(String keys [], String values [], OSCBundle *bndl, char packet_header[], int assoc_len) { 
	convert_OSC_assoc_arrays_to_multiMsg(keys, values, bndl, packet_header, assoc_len, 0); 
}



// ================================================================
// ===             CONVERSION BETWEEN ARRAY FORMATS             ===
// ================================================================



// --- CONVERT ARRAY KEY VALUES TO ASSOC ---
// 
// Converts and array formatted as:
//   [key1, value1, key2, value2, key3, value3]
// to two associated arrays formatted as:
//   [key1,   key2,   key3]
//   [value1, value2, value3]
// 
// @param key_values  The flat array of keys and values
// @param keys        The array to be filled with the keys from 'key_values'
// @param values      The array to be filled with the values for 'key_values'
// @param kv_len      The length of the 'key_value' array
// @param assoc_len   The length of the 'keys' and 'values' arrays (should be the same)
//
void convert_array_key_value_to_assoc(String key_values [], String keys [], String values [], int kv_len, int assoc_len)
{
	if (kv_len > 2*assoc_len) {
		LOOM_DEBUG_Println("Keys or values array not large enough to hold all of keys/values, cannot split");
		return;
	}

	for (int i = 0; i < kv_len; i+=2) {
		keys[i/2]   = key_values[i];
		values[i/2] = key_values[i+1]; 	
	}
}



// --- CONVERT ARRAY KEY VALUES TO ASSOC ---
// 
// Converts two associated arrays formatted as:
//   [key1,   key2,   key3]
//   [value1, value2, value3]:
// to single array formatted as:
//   [key1, value1, key2, value2, key3, value3]
// 
// @param keys        The array of keys  
// @param values      The array to values
// @param key_values  The flat array of keys and values to be filled by combining 'keys' and 'values'
// @param assoc_len   The length of the 'keys' and 'values' arrays (should be the same)
// @param kv_len      The length of the 'key_value' array
//
void convert_array_assoc_to_key_value(String keys [], String values [], String key_values [], int assoc_len, int kv_len)
{
	if ( kv_len < 2*assoc_len ) {
		LOOM_DEBUG_Println("Key-values array is not at least twice the size of keys and values arrays, cannot merge");
		return;
	}

	for (int i = 0; i < assoc_len; i++) {
		key_values[i*2]   = keys[i];
		key_values[i*2+1] = values[i];
	}
}














