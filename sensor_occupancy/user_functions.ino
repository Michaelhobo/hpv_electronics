/* User Functions - Sensor Template
 * These functions should perform appropriate actions at every cycle of the loop.
 * Collected and formatted data should be stored in write_buffer[1]
 * This file is meant to pull user-defined functions out of the base template,
 * so the template can change without fear of merge conflicts.
 */

int forcepin = 7;


/* Allows user to set things up. */
void user_setup() {
  pinMode(forcepin, INPUT);
}

/* This function is called before we send. */
void data_manipulation() {
  delay(5000); //This doesnt have to be frequent at all, since this doesnt change often. I could probably make it 20000 and probably not affect the effectiveness that much.
  if (digitalRead(forcepin)==HIGH){
    write_buffer[1] = 1;
  }
  else{
    write_buffer[1] = 0;
  }
}

/* These functions are called after we send data,
 * and we're figuring out what state 
 * we're going to switch to.
 */
void connected_action() {}

void sleep_action() {}

void deep_sleep_action() {}
