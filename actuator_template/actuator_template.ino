#include <Servo.h>
#include <Wire.h>
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "constants.h"

#define MYADDR 'z';

const int servo_pin = 9;
uint8_t gear_positions[11] = {180, 162, 144, 126, 108, 90, 72, 54, 36, 18, 0};
Servo servo1;
char write_buffer[RF24_TRANSFER_SIZE];
char read_buffer[RF24_TRANSFER_SIZE];
RF24 radio(8, 7);
uint8_t missed = 0;
uint8_t state;

const uint64_t masterAddress = 0x00F0F0F0F0LL;
const uint64_t myAddress = 0xF0F0F0F000LL | MYADDR;
#define NUM_SENSORS 4

char sensor_data[NUM_SENSORS];

void setup(void)
{
  Serial.begin(57600);
  radio.begin();
  radio.setRetries(15,15);
  state = CONNECTED;
  servo1.attach(9);
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
	bool received = false;
	received = radio.write(write_buffer, sizeof(char) * 10);
	if (received) {
		Serial.println("write ok...\n\r"); 
	} else  {
		Serial.println("write failed.\n\r");
	}
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
			ping_master();
			last_time = millis();
		}
}
// vim:cin:ai:sts=2 sw=2 ft=cpp
