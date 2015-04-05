/*
 Copyright (C) 2011 J. Coliz <maniacbug@ymail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

#include <Wire.h>
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

RF24 radio(8, 7);

#define NUM_SENSORS 4

const uint64_t rf24_addr = 0x00F0F0F0F0LL;
char sensor_data[NUM_SENSORS];

void setup(void)
{

  Serial.begin(57600);

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
  radio.openWritingPipe((const uint64_t) 0xF0F0F0F000LL);
  radio.openReadingPipe(1,rf24_addr);
  radio.startListening();
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
