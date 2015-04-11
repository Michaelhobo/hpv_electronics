#include <Wire.h>
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "constants.h"
#include "addr.h"

char write_buffer[RF24_TRANSFER_SIZE];
char read_buffer[RF24_TRANSFER_SIZE];
RF24 radio(8, 7);
uint8_t missed = 0;
uint8_t state;
int last_time = 0;

const uint64_t masterAddress = 0x00F0F0F0F0LL;
const uint64_t myAddress = 0xF0F0F0F000LL | MYADDR;

void setup(void)
{
	Serial.begin(57600);
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