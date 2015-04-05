#include <Wire.h>

/*
 Copyright (C) 2011 J. Coliz <maniacbug@ymail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

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
#include "printf.h"o

//
// Hardware configuration
//

// Set up nRF24L01 radio on SPI bus plus pins 9 & 10

RF24 radio(8, 7);
// Topology
//

// Radio pipe addresses for the 2 nodes to communicate.
//const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL, 0xF0F0F0F0D2LL };
//const uint64_t pipes[3] = { 0x01F0F0F0F0LL, 0x02F0F0F0F0LL, 0x03F0F0F0F0LL };
const uint64_t pipes[3] = { 0xF0F0F0F000LL | 0, 0x00F0F0F0F0LL, 0xF0F0F0F000LL | 1};

//
// Role management
//
// Set up role.  This sketch uses the same software for all the nodes
// in this system.  Doing so greatly simplifies testing.  The hardware itself specifies
// which node it is.
//
// This is done through the role_pin
//

#define NUM_SENSORS 4

char sensor_data[NUM_SENSORS];

void setup(void)
{

  //
  // Print preamble
  //

  Serial.begin(57600);
  printf_begin();
  printf("\n\rRF24/examples/pingpair/\n\r");

  /* Set up I2C. */
  Wire.begin(7);
  Wire.onReceive(on_receive);
  Wire.onRequest(send_update);

  /* Set up RF24 Radio. */
  radio.begin();
  // optionally, increase the delay between retries & # of retries
  radio.setRetries(15,15);
  // optionally, reduce the payload size.  seems to
  // improve reliability
  radio.setPayloadSize(8);
    radio.openWritingPipe(pipes[1]);
    radio.openReadingPipe(1,pipes[0]);
  radio.startListening();
  radio.printDetails();
}

/* Send data to slave.
 * Called when arduino receives data over I2C
 */
void on_receive(int dataSize) {
	uint8_t id = (uint8_t) Wire.read();
	char data = Wire.read();
	//send_slave(id, data);
}


void send_update() {
  Wire.write(sensor_data, NUM_SENSORS);
  Serial.print("got request from master...");
  for (int i = 0; i < NUM_SENSORS; i++) {
    sensor_data[i] = 255;
  }
  Serial.println("sent updates back");
}

void loop(void)
{
    // if there is data ready
    if ( radio.available() )
    {
      // Dump the payloads until we've gotten everything
      unsigned long got_time;
      bool done = false;
      while (!done)
      {
        // Fetch the payload, and see if this was the last one.
        done = radio.read( &got_time, sizeof(unsigned long) );

        // Spew it
        printf("Got payload %lu...",got_time);

	// Delay just a little bit to let the other unit
	// make the transition to receiver
      delay(20);
    }

    // First, stop listening so we can talk
    radio.stopListening();

    // Send the final one back.
    radio.write( &got_time, sizeof(unsigned long) );
    printf("Sent response.\n\r");

    // Now, resume listening so we catch the next packets.
    radio.startListening();
  }
}
// vim:cin:ai:sts=2 sw=2 ft=cpp
