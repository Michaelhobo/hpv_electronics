#include "mbed.h"
#include <limits.h>
#include <stdio.h>
#include "xbee.h"
#include "nRF24L01P.h"
#include "constants/constants.h"
#include "main.h"

#define XBEE_SEND_INTERVAL 2
#define PC_SEND_INTERVAL 1
#define MY_ADDR 0

DigitalOut led1(LED1);
DigitalOut led2(LED2);
DigitalOut led3(LED3);
DigitalOut led4(LED4);
Serial pc(USBTX, USBRX); // tx, rx
xbee xbee(p13, p14, p12);
nRF24L01P rf24(p5, p6, p7, p8, p9, p10);

InterruptIn button(p15);

Ticker events;
Timeout timeout;
double speed = 0.0; //calculated speed
double cadence = 0.0;
char receive_buffer[RF24_TRANSFER_SIZE];
char speed_buffer[RF24_TRANSFER_SIZE];
char send_buffer[RF24_TRANSFER_SIZE];
const char *sensor_names[255] = {0};
void (*sensor_handlers[255])(char *data);
uint8_t sensor_states[255];

/* Sends speed to XBEE. */
void send_xbee_speed() {
	sprintf(speed_buffer, "%f\n", speed);
}

/* Initialize xbee for telemetry. */
void telemetry_init() {
	// convert units before to minimize calculations in critical section
	pc.printf("Initializing.\n");
}

/* Initialize one sensor's values. */
void init_sensor(int id, const char *name, void (*handler)(char *)) {
	sensor_names[id] = name;
	sensor_handlers[id] = handler;
	sensor_states[id] = (uint8_t) DISCONNECTED;
}


void send_one() {
	pc.printf("h sent\r\n");
	uint8_t sent = send_sensor(1, "h");
	pc.printf("send %d\r\n", sent);
	//rf24.setTxAddress(0x01F0F0F0F1, 5);
}

/* Initialize everything necessary for the scripts. */
void init() {
	pc.printf("init");
	telemetry_init();
	rf24_init();
	button.rise(&send_one);
}

/* Send to a sensor with an id. */
uint8_t send_sensor(uint8_t id, char *data) {
	pc.printf("send_sensor\r\n");
	send_buffer[0] = id;
	sprintf(send_buffer + 1, "%s", data);
	uint64_t pipe_addr = 0x01F0F0F0F1;//0x00F0F0F0F1 | (1LL << 32);
	num_received = rf24.write(NRF24L01P_PIPE_P0, send_buffer, RF24_TRANSFER_SIZE);
	rf24.setReceiveMode();
	pc.printf("sent.\r\n");
	return num_received;
}

/* Gets the id from the name of the sensor. */
uint8_t find_id(char *name) {
	uint8_t id = 0;
	while (strcmp(sensor_names[id], name)) {
		id += 1;
	}
	return id;
}

/* Send to a sensor with name.
 * Sending format: [dest_address(1), src_address(1), data(30)]
 */
void send_sensor_name(char *name, char *data) {
	uint8_t id = find_id(name);
	send_sensor(id, data);
}

/* Main. */
int main() {
	init();
	//pc.printf("Starting Logging.\n");
	//events.attach(&send_xbee_speed, XBEE_SEND_INTERVAL);
}
