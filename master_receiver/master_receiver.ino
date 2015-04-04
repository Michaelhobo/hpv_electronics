#include <nRF24L01.h>
#include <RF24.h>
#include <RF24_config.h>

#include <Wire.h>
#include <SPI.h>
#include <stdio.h>
#include "constants.h"

#define NUM_SENSORS 4
#define NUM_ACTUATORS 4

int ledPin = 4;
char sensor_data[NUM_SENSORS];
char rf24_in[RF24_TRANSFER_SIZE + 1];
RF24 rf24  (8, 7);
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

        /* Serial Setup */
        Serial.begin(9600);
        
        pinMode(ledPin, OUTPUT);
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
	bool ok = rf24.write(&data, 1);
	rf24.startListening();
}

/* Send updates back to the master controller. */
void send_update() {
	Wire.write(sensor_data, NUM_SENSORS);
	for (int i = 0; i < NUM_SENSORS; i++) {
		sensor_data[i] = 255;
	}
}

void lightblink()
 {
   digitalWrite(ledPin, HIGH);
   delay(100);
   digitalWrite(ledPin, LOW);
   delay(100);
 }

void up()
{
  digitalWrite(ledPin, HIGH);
}

void blink2()
{
  digitalWrite(ledPin, HIGH);
   delay(100);
   digitalWrite(ledPin, LOW);
   delay(1000);
}

void blink3()
{
  digitalWrite(ledPin, HIGH);
   delay(500);
   digitalWrite(ledPin, LOW);
   delay(500);
}

void loop() {
	if (rf24.available()) {
		rf24.read(rf24_in, RF24_TRANSFER_SIZE + 1);
		switch (rf24_in[0]) {
			case 0: //speed
				sensor_data[0] = rf24_in[1];
                                Serial.println("Sending data to speed sensor");
				break;
			case 1: //cadence
				sensor_data[1] = rf24_in[1];
                                Serial.println("Sending data to cadence sensor");
				break;
			case 2: //temp
				sensor_data[2] = rf24_in[1];
                                Serial.println("Sending data to some sort of sensor");
				break;
			case 3: //brightness
                                Serial.println("Sending data to brightness sensor");
				sensor_data[3] = rf24_in[1];
				break;

			case 'g': //gear shift
                                lightblink();
				break;
			case 'f': //front light
                                up();
				break;
			case 'r': //rear light
                                blink2();
				break;
			case 'l': //landing gear
                                blink3();
				break;
		}
	}
}
