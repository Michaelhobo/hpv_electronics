#include <Wire.h>

/**
 * Example RF Radio Ping Pair
 *
 * This is an example of how to use the RF24 class.  Write this sketch to two different nodes,
 * connect the role_pin to ground on one.  The ping node sends the current time to the pong node,
 * which responds by sending the value back.  The ping node can then see how long the whole cycle
 * took.
 */

#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
//#include " .h"
//#include "println.h"
#include "constants.h"

#define MYADDR 0;

char write_buffer[RF24_TRANSFER_SIZE];
char read_buffer[RF24_TRANSFER_SIZE];
RF24 radio(8, 7);
uint8_t missed = 0;
uint8_t state;

const uint64_t masterAddress = 0x00F0F0F0F0LL;
const uint64_t myAddress = 0xF0F0F0F000LL | MYADDR;
#define NUM_SENSORS 4

char sensor_data[NUM_SENSORS];

void setup(void)
{

  //
  // Print preamble
  //

  Serial.begin(57600);
  radio.begin();
  radio.setRetries(15,15);
  state = CONNECTED;


  radio.setPayloadSize(RF24_TRANSFER_SIZE);

  //
  // Open pipes to other nodes for communication
  //

  // This simple sketch opens two pipes for these two nodes to communicate
  // back and forth.
  // Open 'our' pipe for writing
  // Open the 'other' pipe for reading, in position #1 (we can have up to 5 pipes open for reading)

    radio.openReadingPipe(1,myAddress);
    radio.openWritingPipe(masterAddress);

  radio.startListening();
}
void data_manipulation(){
    /* Code here, modify write_buffer[1] */
}

bool write_data() {
	bool received = false;
	received = radio.write(write_buffer,  RF24_TRANSFER_SIZE);
	if (received) {
		Serial.println("write ok...\n\r"); 
	} else  {
		Serial.println("write failed.\n\r");
	}
	//delay(100);
	return received;
}


void loop(void)
{
    data_manipulation();
     
    radio.stopListening();
    Serial.println("Sent");
    bool ok = write_data();
    radio.startListening();
    
    	if (state == CONNECTED) {
		Serial.println("Connected");
		if (!ok) {
			missed += 1;
			if (missed > 3) {
				state = SLEEP;
				missed = 0;
			}
		} else {
			missed = 0;
		}
		delay(1000);
	} else if (state == SLEEP) {
		Serial.println("Sleep");
		if (ok) {
			state = CONNECTED;
			missed = 0;
		} else {
			Serial.println("failed to connect");
			missed += 1;
			if (missed > 5) {
				state = DEEP_SLEEP;
				missed = 0;
			}
		}
		delay(2000);
	} else if (state == DEEP_SLEEP) {
		Serial.println("Deep Sleep");
		if (ok) {
			state = CONNECTED;
		} else {
			Serial.println("failed to connect");
		}
		delay(5000);
	}
  }

  //
  // Pong back role.  Receive each packet, dump it out, and send it back
  //
// vim:cin:ai:sts=2 sw=2 ft=cpp
