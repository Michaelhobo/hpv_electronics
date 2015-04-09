#include <Wire.h>
#include <SPI.h>
#include <Servo.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "constants.h"

#define MYADDR 'g'

char write_buffer[RF24_TRANSFER_SIZE];
char read_buffer[RF24_TRANSFER_SIZE];
RF24 radio(8, 7);
uint8_t missed = 0;
uint8_t state;
int last_time = 0;

const uint64_t masterAddress = 0x00F0F0F0F0LL;
const uint64_t myAddress = 0xF0F0F0F000LL | MYADDR;
#define NUM_SENSORS 4

char sensor_data[NUM_SENSORS];

void setup(void)
{
	Serial.begin(57600);
        Serial.print("Starting actuator");
	radio.begin();
	radio.setRetries(15,15);
	state = CONNECTED;
	radio.setPayloadSize(RF24_TRANSFER_SIZE);
	radio.openReadingPipe(1,myAddress);
	radio.openWritingPipe(masterAddress);
	radio.startListening();
	user_setup();
}

void shutdown_all(){
	/* For future work niggas */
	user_shutdown();
}

bool ping_master() {
        radio.stopListening();
	bool received = false;
	received = radio.write(write_buffer, sizeof(char) * 10);
	if (received) {
		Serial.println("write ok...\n\r"); 
	} else  {
		Serial.println("write failed.\n\r");
	}
        radio.startListening();
	return received;
}

bool read_data() {
	bool hasRead;
	while (radio.available()){
		radio.read(read_buffer, RF24_TRANSFER_SIZE);
		Serial.println("Successfully read data");
		manipulate_data(read_buffer);
	}
	return hasRead;
}


void loop(void)
{    
	read_data();
	if (millis() > last_time + PING_DELAY) {
	//	ping_master();
		last_time = millis();
	}
}

/* User Functions - Actuator Template
 * These functions should perform appropriate actions at every cycle of the loop.
 * Collected and formatted data should be stored in write_buffer[1]
 * This file is meant to pull user-defined functions out of the base template,
 * so the template can change without fear of merge conflicts.
 */

const int servo_pin = 9;
uint8_t gear_positions[11] = {180, 162, 144, 126, 108, 90, 72, 54, 36, 18, 0};
Servo servo1;

/* Allows user to set things up. */
void user_setup() {
	servo1.attach(9);
}

/* Allows user to clean things up before a long shutdown. */
void user_shutdown() {}

void manipulate_data(char* data){
    //if (data[0] == 'b'){
      if (data[1] == 's'){
        shutdown_all();
      }
      else{
        /* Code here please */
        uint8_t gear = data[1];
        Serial.print("Switching to gear ");
        Serial.println(gear);
  servo1.write(gear_positions[gear - 1]);
      }
    //}
}
// vim:cin:ai:sts=2 sw=2 ft=cpp
