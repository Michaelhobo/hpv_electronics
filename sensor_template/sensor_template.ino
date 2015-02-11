#include <SPI.h>
#include "nRF24L01.h"
#include <stdio.h>
#include "RF24.h"
#include "println.h"
#include "constants.h"
#include "LowPower.h"
#include <avr/io.h>

#define MY_ADDR 1
RF24 rf24(8,7); //change to 7,8 because 9,10 are pwm pins

/* For serial debugging. */
int serial_console_putc(char c, FILE *) {
	Serial.write(c);
	return 0;
}

uint8_t state; //state that the sensor is in. 0 = connected, 1 = connected, 2 = sleep
char *name = "template";
uint64_t master_general_address = ((MY_ADDR % 4) + 2) & 0x00F0F0F0F0; //master will read on this address
uint64_t master_connection_address = 0x00F0F0F0F1; //this is the address we write to connect to the master
char read_buffer[RF24_TRANSFER_SIZE];
char write_buffer[RF24_TRANSFER_SIZE];
char *w_data;
uint8_t disconnected_count = 0;

/* Power Saving code.
 * Pull down all unused (floating) pins, set pull-up resistors, etc.
 */
void powerSetup() {
	DDRD = DDRD | B11111100
		
	//Write Code Here!
}

/* Prints only if debug is true. */
void prt(const char* msg) {
	Serial.println(msg);
}

/* Run setup code. */
void setup() {
	if (debug) Serial.begin(9600); //This allows serial output to a connected serial terminal.
	state = DISCONNECTED;

	// RF24 Setup
	write_buffer[0] = MY_ADDR;
	w_data = (char *) (write_buffer + 1);
	rf24.begin();
	rf24.setDataRate(RF24_1MBPS);
	rf24.setCRCLength(RF24_CRC_8);
	rf24.setPayloadSize(RF24_TRANSFER_SIZE);
	rf24.setChannel(101);
	rf24.setAutoAck(true);

	//Power Saving Setup
	powerSetup();

	/* For debugging, comment out when not needed. */
	fdevopen(&serial_console_putc, NULL);
	rf24.printDetails();
}

/* Connect this sensor to the master. */
bool connect_master() {
	prt("connecting master...");
	char *start_msg = (char *) malloc(RF24_TRANSFER_SIZE);
	sprintf(start_msg, "%c%s", MY_ADDR, "connect");
	rf24.openWritingPipe(master_connection_address);
	//rf24.openWritingPipe(0x00F0F0F0F0);
	//rf24.openReadingPipe(1, 0xF0F0F0F0D2);
	bool connected = false;
	int timeout = 100; //timeout in ms;
	while (!connected) {
		rf24.stopListening();
		connected = rf24.write( start_msg,sizeof(char)*10);
		if (connected) {
			prt("connect ok...\n\r"); 
		} else  {
			prt("connect failed.\n\r");
		}
		delay(100);
		rf24.startListening();

	}
	free(start_msg);
	return true;
}

/* Shut down this sensor.
 * This should be called when a message to shut down is called.
 */
void shutdown() {
	rf24.powerDown();
	LowPower.powerDown(SLEEP_8S, ADC_CONTROL_OFF, BOD_OFF);
	rf24.powerUp();
	rf24.startListening();
	//power down antenna, set all unused pins low, put microcontroller to sleep for 1/2(?) second then wake up
}

/* Put the master to sleep.
 * This should be a short sleep period, so don't power everything down,
 * just shut down the rf24 chip and other unnecessary powered devices.
 */
void sleep() {
	r24.powerDown();
	LowPower.idle(SLEEP_1S, ADC_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_OFF, SPI_OFF, USART0_OFF, TWI_OFF);
	rf24.powerUp();
	rf24.startListening();
}

/* Put the master into deep sleep.
 * This should be used for very long sleep periods, when a disconnect may have happened.
 */
void deep_sleep() {
	rf24.powerDown();
	LowPower.powerDown(SLEEP_4S, ADC_CONTROL_OFF, BOD_OFF);
	rf24.powerUp();
	rf24.startListening();
}

/* Writes data to master
 * Use this for sensor-type slaves that gather data locally and send to the master
 * Comment out the write_data line in loop function if not needed.
 * Write data to write_data, which can store a max of (RF24_TRANSFER_SIZE - 1) bytes/chars.
 */
void write_data() {
	/* Write code here. */
	rf24.stopListening();
	bool received = false;
	while (!received) {
		received = rf24.write(write_buffer, sizeof(char) * 10);
		if (received) {
			prt("write ok...\n\r"); 
		} else  {
			prt("write failed.\n\r");
		}
		delay(100);
		rf24.startListening();
	}
}
/* Handles an incoming packet meant for this slave.
 * @data The data packet meant for this slave.
 */
void read_handler(char *data) {
	if (strstr(data, "shutdown") == data) {
		shutdown();
	} else {
		/* Write code here. */
	}
}

/* Reads data from master
 * All slaves should use this, to receive command messages such as shutdown.
 */
void read_data() {
	if (rf24.available()) {
		rf24.read(read_buffer, RF24_TRANSFER_SIZE);
		if (read_buffer[0] == MY_ADDR) {
			read_handler(read_buffer + 2);
		}
	}
}
void loop() {
	// put your main code here, to run repeatedly
	prt("looping...");
	if (state == DISCONNECTED) {
		prt("Disconnected");
		if (!connect_master()) {
			state = SLEEP;
			prt("failed to connect");
		} else {
			state = CONNECTED;
		}
	} else if (state == CONNECTED) {
		prt("Connected");
		read_data();
		write_data();
		sleep();
	} else if (state == SLEEP) {
		prt("Sleep");
		if (connect_master()) {
			state = CONNECTED;
			disconnected_count = 0;
		} else if (disconnected_count < 10) {
			prt("failed to connect");
			disconnected_count += 1;
			sleep();
		} else {
			state = DEEP_SLEEP;
		}
	} else if (state == DEEP_SLEEP) {
		prt("Deep Sleep");
		if (connect_master()) {
			state = CONNECTED;
			disconnected_count = 0;
		} else {
			prt("failed to connect");
			disconnected_count += 1;
			deep_sleep();
		}
	}
}

