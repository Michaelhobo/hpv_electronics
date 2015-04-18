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


const uint64_t masterAddress = klondike? 0x00F0F0F0F0LL: 0x00E0E0E0E0LL; //Yaaaa ternary operator
const uint64_t myAddress = klondike ?(0xF0F0F0F000LL | MYADDR): (0xE0E0E0E000LL | MYADDR); //Yaaa more Ternary Operator!


void setup(void)
{
	Serial.begin(57600);
	radio.begin();
	radio.setRetries(15,15);
	state = CONNECTED;
	write_buffer[0] = MYADDR;

	radio.setPayloadSize(RF24_TRANSFER_SIZE);
	radio.openReadingPipe(1,myAddress);
	radio.openWritingPipe(masterAddress);

	radio.startListening();
	user_setup();
}

bool write_data() {
	bool received = false;
	received = radio.write(write_buffer,  RF24_TRANSFER_SIZE);
	if (received) {
		Serial.println("write ok...\n\r"); 
	} else  {
		Serial.println("write failed.\n\r");
	}
	return received;
}


void loop(void)
{
	data_manipulation();

	radio.stopListening();
	Serial.println("Sent");
	bool ok = write_data();
	radio.startListening();

	if (state == CONNECTED) {
		Serial.println("Connected");
		connected_action();
		if (!ok) {
			missed += 1;
			if (missed > 3) {
				state = SLEEP;
				missed = 0;
			}
		} else {
			missed = 0;
		}
		delay(CONNECTED_DELAY);
	} else if (state == SLEEP) {
		Serial.println("Sleep");
		if (ok) {
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
		delay(SLEEP_DELAY);
	} else if (state == DEEP_SLEEP) {
		Serial.println("Deep Sleep");
		if (ok) {
			state = CONNECTED;
		} else {
			Serial.println("failed to connect");
		}
		delay(DEEP_SLEEP_DELAY);
	}
}

