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
RF24 radio(8, 7);


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

  write_buffer[0] = 'h';
  write_buffer[1] = 'o';
  radio.startListening();
}

bool write_data() {
	/* Write code here. */
	radio.stopListening();
	bool received = false;
	received = radio.write(write_buffer, sizeof(char) * (RF24_TRANSFER_SIZE));
	if (received) {
		Serial.println("write ok...\n\r"); 
	} else  {
		Serial.println("write failed.\n\r");
	}
	//delay(100);
	radio.startListening();
	return received;
}


void loop(void)
{
  //
  // Ping out role.  Repeatedly send the current time
  //
//    Serial.println("It does go through loop...");
     // First, stop listening so we can talk.
    radio.stopListening();
    Serial.println("Sent");
    // Take the time, and send it.  This will block until complete
    unsigned long time = millis();
    bool ok = radio.write( write_buffer, RF24_TRANSFER_SIZE );

    
    if (ok)
        Serial.println("ok...");
    else
       Serial.println("failed.\n\r");

    // Now, continue listening
    radio.startListening();
    // Wait here until we get a response, or timeout (250ms)
    unsigned long started_waiting_at = millis();
    bool timeout = false;
    while ( ! radio.available() && ! timeout )
      if (millis() - started_waiting_at > 200 )
        timeout = true;

    // Describe the results
    if ( timeout )
    {
       Serial.println("Failed, response timed out.\n\r");
    }
    else
    {
      // Grab the response, compare, and send to debugging spew
      unsigned long got_time;
      radio.read( &got_time, sizeof(unsigned long) );

      // Spew it
    }

    // Try again 1s later
    delay(1000);
  }

  //
  // Pong back role.  Receive each packet, dump it out, and send it back
  //
// vim:cin:ai:sts=2 sw=2 ft=cpp
