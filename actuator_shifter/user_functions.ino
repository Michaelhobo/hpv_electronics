/* User Functions - Actuator Template
 * ONLY EDIT FUNCTIONS BELOW THIS LINE TO PREVENT MERGE CONFLICTS
 * These functions should perform appropriate actions at every cycle of the loop.
 * Collected and formatted data should be stored in write_buffer[1]
 * This file is meant to pull user-defined functions out of the base template,
 * so the template can change without fear of merge conflicts.
 */
 #include <Servo.h>
Servo servo;
uint8_t gear_positions[11] = {180, 162, 144, 126, 108, 90, 72, 54, 36, 18, 0};

/* Allows user to set things up. */
void user_setup() {
  servo.attach(9);
  servo.write(gear_positions[0]);
}

/* Allows user to clean things up before a long shutdown. */
void user_shutdown() {}

void manipulate_data(char* data) {
  if (data[0] == 'b') {
    servo.write(gear_positions[data[1] - 1]);
  }
}

