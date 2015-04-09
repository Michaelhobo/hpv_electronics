/* User Functions - Sensor Template
 * These functions should perform appropriate actions at every cycle of the loop.
 * Collected and formatted data should be stored in write_buffer[1]
 * This file is meant to pull user-defined functions out of the base template,
 * so the template can change without fear of merge conflicts.
 */
/* Allows user to set things up. */
void user_setup() {
  attachInterrupt(0, tick, RISING);
}
byte spokes = 0;
void tick()
{
  spokes++;
}

/* This function is called before we send. */
void data_manipulation() {
  write_buffer[1] = (char)spokes;
  spokes = 0;
}

/* These functions are called after we send data,
 * and we're figuring out what state 
 * we're going to switch to.
 */
void connected_action() {}

void sleep_action() {}

void deep_sleep_action() {}
