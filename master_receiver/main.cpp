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

/* Sensor count goes like this:
0 - speed
1 - cadence
2 - temp
3 - outdoor brightness
*/
#define NUM_SENSORS 4


/* Actuator count goes like this:
0 - gear shifter
1 - front light
2 - rear lights
3 - landing gear
*/
#define NUM_ACTUATORS 4

DigitalOut led1(LED1);
DigitalOut led2(LED2);
DigitalOut led3(LED3);
DigitalOut led4(LED4);
Serial pc(USBTX, USBRX); // tx, rx
xbee xbee(p13, p14, p12);
I2C arduino(p9, p10);
InterruptIn critical(p8);

Ticker events;
Timeout timeout;

/* I2C values */
uint8_t read_addr = 0x7 << 1 | 0x1;
uint8_t write_addr = 0x7 << 1;
char out[2];
char arduino_updates[NUM_SENSORS];

/* Data Values. */
double speed = 0.0; //calculated speed
uint8_t cadence = 0;
int8_t temp = 0;
uint8_t brightness = 0;


/* Actuator Values */
uint8_t gear = 1;
bool front_light = false;
bool rear_light = false;
bool landing_gear = false;

/* Initialize everything necessary for the scripts. */
void init() {
	pc.printf("Human Powered Vehicle Controller");
	critical.rise(&get_updates);
}

/* Get updates from arduino. 
 * A '-' means the value has not been updated from last time.
 */
void get_updates() {
	arduino.read(read_addr, arduino_updates, NUM_SENSORS);
	for (int i = 0; i < NUM_SENSORS; i++) {
		if (arduino_updates[i] != '-') {
			switch (i) {
				case 0:
					speed = (double) ((uint8_t) arduino_updates[i] * 3.0 / 10.0); // speed converion 
					break;
				case 1:
					cadence = arduino_updates[i];
					break;
				case 2:
					temp = arduino_updates[i];
					break;
				case 3:
					brightness = arduino_updates[i];
					break;
			}
		}
	}
}

/* Send to a sensor with an id. */
void send_sensor(uint8_t id, char data) {
	out[0] = id;
	out[1] = data;
	arduino.write(write_addr, out, 2);
}

/* Main. */
int main() {
	init();
}
