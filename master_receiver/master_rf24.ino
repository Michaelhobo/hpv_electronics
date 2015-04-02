#include <Wire.h>
#include <SPI.h>
#include "nRF24L01.h"
#include <stdio.h>
#include "RF24.h"
#include "constants.h"

#define NUM_SENSORS 4
#define NUM_ACTUATORS 4

char sensor_data[NUM_SENSORS];
char rf24_in[RF24_TRANSFER_SIZE + 1];
void setup() {
	/* I2C setup */
	Wire.begin(7);
	Wire.onReceive(on_receive);
	Wire.onRequest(send_update);

	/* RF24 setup */
	rf24.begin();
	rf24.openReadingPipe(1, 0x00F0F0F0F0LL);
	rf24.setRetries(15,15);
	rf24.setPayloadSize(RF24_TRANSFER_SIZE);
	rf24.startListening();
}

/* Process data received. */
void on_receive(int dataSize) {
	uint8_t id = (uint8_t) Wire.read();
	char data = Wire.read();
	send_slave(id, data);
}

/* Send sensor to correct slave. */
void send_slave(uint8_t id, char data) {
	rf24.stopListening();
	bool received = false;
	rf24.openWritingPipe(0xF0F0F0F000LL | id);
	bool ok = radio.write(&data, 1);
	rf24.startListening();
}

/* Send updates back to the master controller. */
void send_update() {
	Wire.write(sensor_data, NUM_SENSORS);
	for (int i = 0; i < NUM_SENSORS; i++) {
		sensor_data[i] = 255;
	}
}

void loop() {
	if (rf24.available()) {
		rf24.read(rf24_in, RF24_TRANSFER_SIZE + 1);
		switch (rf24_in[0]) {
			case 0: //speed
				sensor_data[0] = rf24_in[1];
				break;
			case 1: //cadence
				sensor_data[1] = rf24_in[1];
				break;
			case 2: //temp
				sensor_data[2] = rf24_in[1];
				break;
			case 3: //brightness
				sensor_data[3] = rf24_in[1];
				break;

			case 'g': //gear shift
				break;
			case 'f': //front light
				break;
			case 'r': //rear light
				break;
			case 'l': //landing gear
				break;
		}
	}
}
