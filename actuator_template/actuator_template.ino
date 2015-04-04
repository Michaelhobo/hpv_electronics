#include <SPI.h>
#include "nRF24L01.h"
#include <stdio.h>
#include "RF24.h"
#include "println.h"
#include "constants.h"

#define MY_ADDR 't'
RF24 rf24(8,7); //change to 7,8 because 9,10 are pwm pins

uint8_t state; //state that the sensor is in. 0 = connected, 1 = connected, 2 = sleep
uint8_t missed = 0;
uint64_t master_address = 0x00F0F0F0F0LL;

char read_buffer[RF24_TRANSFER_SIZE];
/* Run setup code. */
void setup() {
	Serial.begin(9600);
	state = CONNECTED;
	rf24.begin();
	rf24.setPayloadSize(RF24_TRANSFER_SIZE);
	rf24.openReadingPipe(1, 0xF0F0F0F000LL | MY_ADDR);
	rf24.openWritingPipe(0x00F0F0F0F0LL);
	rf24.setRetries(15, 15);
	rf24.startListening();
}

/* Writes data to master
 * Use this for sensor-type slaves that gather data locally and send to the master
 * Comment out the write_data line in loop function if not needed.
 * Write data to write_data, which can store a max of (RF24_TRANSFER_SIZE - 1) bytes/chars.
 */
bool ping_master() {
	/* Write code here. */
	rf24.stopListening();
	bool received = false;
		received = rf24.write(write_buffer, sizeof(char) * 10);
		if (received) {
			Serial.println("write ok...\n\r"); 
		} else  {
			Serial.println("write failed.\n\r");
		}
		delay(100);
		rf24.startListening();
	return received;
}

/* Shut down this sensor. */
void shutdown() {

	//power down antenna, set all unused pins low, put microcontroller to sleep for 1/2(?) second then wake up
}

void loop() {
	if (state == CONNECTED) {
		Serial.println("Connected");
		if (!write_data()) {
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
		if (write_data()) {
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
		if (write_data()) {
			state = CONNECTED;
		} else {
			Serial.println("failed to connect");
		}
		delay(5000);
	}
}

