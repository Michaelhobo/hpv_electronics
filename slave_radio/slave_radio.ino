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

//
// Hardware configuration
//

// Set up nRF24L01 radio on SPI bus plus pins 9 & 10

RF24 radio(8, 7);


// Radio pipe addresses for the 2 nodes to communicate.
//const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL, 0xF0F0F0F0D2LL };
//const uint64_t pipes[3] = { 0x01F0F0F0F0LL, 0x02F0F0F0F0LL, 0x03F0F0F0F0LL };


// The various roles supported by this sketch

// The debug-friendly names of those roles
const char* role_friendly_name[] = { "invalid", "Ping out"};

// The role of the current running sketch
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

  //
  // Setup and configure rf radio
  //

  radio.begin();

  // optionally, increase the delay between retries & # of retries
  radio.setRetries(15,15);

  // optionally, reduce the payload size.  seems to
  // improve reliability
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

  //
  // Start listening
  //

  radio.startListening();
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
    bool ok = radio.write( &time, sizeof(unsigned long) );
    
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
