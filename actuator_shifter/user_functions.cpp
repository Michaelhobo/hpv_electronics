/* User Functions - Actuator Template
 * These functions should perform appropriate actions at every cycle of the loop.
 * Collected and formatted data should be stored in write_buffer[1]
 * This file is meant to pull user-defined functions out of the base template,
 * so the template can change without fear of merge conflicts.
 */

#define MYADDR 'g'
 
const int servo_pin = 9;
uint8_t gear_positions[11] = {180, 162, 144, 126, 108, 90, 72, 54, 36, 18, 0};
Servo servo1;

/* Allows user to set things up. */
void user_setup() {
	servo1.attach(9);
}

/* Allows user to clean things up before a long shutdown. */
void user_shutdown() {}

void manipulate_data(char* data) {}
