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
#include "constants.h"

RF24 radio(8, 7);

#define NUM_SENSORS 4

const uint64_t rf24_addr = 0x00F0F0F0F0LL;
char sensor_data[NUM_SENSORS];
char rf24_buffer[RF24_TRANSFER_SIZE];

void setup(void)
{

  Serial.begin(57600);

  /* Set up I2C. */
  Wire.begin(7);
  Wire.onReceive(on_receive);
  Wire.onRequest(send_update);

  /* Set up RF24 Radio. */
  radio.begin();
  radio.setRetries(15,15); //(retry interval, retry number)
  radio.setPayloadSize(RF24_TRANSFER_SIZE);
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

/* Send an update back to the master. */
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
      bool done = false;
      while (!done)
      {
        // Fetch the payload, and see if this was the last one.
        done = radio.read(rf24_buffer, RF24_TRANSFER_SIZE);
        // Spew it
        Serial.print("Got payload ...");
        Serial.print(rf24_buffer);

	// Delay just a little bit to let the other unit
	// make the transition to receiver
      delay(20);
    }

    // First, stop listening so we can talk
    radio.stopListening();

    // Send the final one back.
    radio.write( rf24_buffer, RF24_TRANSFER_SIZE );
    Serial.println("...Sent response.\n\r");

    // Now, resume listening so we catch the next packets.
    radio.startListening();
  }
}
// vim:cin:ai:sts=2 sw=2 ft=cpp
