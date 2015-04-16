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
#include <avr/power.h>
#include <avr/sleep.h>
#include <avr/wdt.h>


RF24 radio(8, 7);

#define NUM_SENSORS 4

const uint8_t SHUTDOWN_CHAR = 'x';

bool klondike = true;//Set to false if uploading to burnt toast
const uint64_t rf24_addr = klondike? 0x00F0F0F0F0LL : 0x00E0E0E0E0LL;
const uint64_t base_addr = klondike? 0xF0F0F0F000LL : 0xE0E0E0E000LL;
char sensor_data[NUM_SENSORS];
char rf24_in[RF24_TRANSFER_SIZE];
char rf24_out[RF24_TRANSFER_SIZE];
uint8_t check = 0;
uint8_t shutdownOn = 0;


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
  radio.openWritingPipe((const uint64_t) base_addr);
  radio.openReadingPipe(1,rf24_addr);
  radio.startListening();
  /*** Setup the WDT ***/
  
  /* Clear the reset flag. */
  MCUSR &= ~(1<<WDRF);
  
  /* In order to change WDE or the prescaler, we need to
   * set WDCE (This will allow updates for 4 clock cycles).
   */
  WDTCSR |= (1<<WDCE) | (1<<WDE);

  /* set new watchdog timeout prescaler value */
  WDTCSR = 1<<WDP3; /* 4.0 seconds */
  
  /* Enable the WD interrupt (note no reset). */
  WDTCSR |= _BV(WDIE);
}

/* Receive data over I2C and deal with data accordingly. */
void on_receive(int dataSize) {
	uint8_t id = (uint8_t) Wire.read();
	char data = Wire.read();
	send_slave(id, data);
}

void shutdownThis(){
       shutdownOn = 1;
       radio.stopListening();
       set_sleep_mode(SLEEP_MODE_PWR_DOWN);   // sleep mode is set here
       sleep_enable();          // enables the sleep bit in the mcucr register    
       sleep_mode();            // here the device is actually put to sleep!! 
                                // THE PROGRAM CONTINUES FROM HERE AFTER WAKING UP
       sleep_disable();         // first thing after waking from sleep:
                                // disable sleep...
       radio.powerUp();
       radio.openReadingPipe(1, myAddress);
       radio.openWritingPipe(masterAddress);
       radio.startListening();
       delay(400);
}

void initiateShutdown(){
       count = 0;
       send_slave('g', SHUTDOWN_CHAR);
       send_slave('f', SHUTDOWN_CHAR);
       send_slave('r', SHUTDOWN_CHAR);
       send_slave('t', SHUTDOWN_CHAR);
       send_slave('l', SHUTDOWN_CHAR);
       shutdownThis();
}

void send_slave(uint8_t id, char data) {
  rf24_out[0] = 'b';
  rf24_out[1] = data;
  radio.stopListening();
  radio.openWritingPipe(base_addr | id);
  bool ok = radio.write(rf24_out, RF24_TRANSFER_SIZE);
  if (ok)
    Serial.println("ok...");
  else
    Serial.println("failed.");
  radio.startListening();
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
  if (shutdownOn == 1)
  {
      shutdownThis();
  }
    // if there is data ready
  if ( radio.available() )
  {
    radio.read(rf24_in, RF24_TRANSFER_SIZE);
    // Spew it
    Serial.print("Got payload ...");
    Serial.print(rf24_in);
    switch (rf24_in[0]) {
      case 0: //speed
        sensor_data[0] = rf24_in[1];
        Serial.println("from speed sensor");
        break;
      case 1: //cadence
        sensor_data[1] = rf24_in[1];
        Serial.println("from cadence sensor");
        break;
      case 2: //temp
        sensor_data[2] = rf24_in[1];
        Serial.println("from temperature sensor");
        break;
      case 3: //brightness
        Serial.println("from brightness sensor");
        sensor_data[3] = rf24_in[1];
        break;
      case 4: //occupancy
        sensor_data[4] = rf24_in[1];
        if(rf24_in[1] == 0){
          if (count >= 3){
            initiateShutdown();            
          }
          else {
            count++;
          }         
        }
        else if (shutdownOn == 1)
        {
            shutdownOn = 0;
        }
        break;
      case 5: //humidity
        break;
      
      case 'g': //gear shift
        break;
      case 'f': //front light
        break;
      case 'r': //right turn
        break;
      case 't': //left turn
        break;
      case 'l': //landing gear
        break;
    }
  }
}
// vim:cin:ai:sts=2 sw=2 ft=cpp
