/* User Functions - Actuator Template
 * ONLY EDIT FUNCTIONS BELOW THIS LINE TO PREVENT MERGE CONFLICTS
 * These functions should perform appropriate actions at every cycle of the loop.
 * Collected and formatted data should be stored in write_buffer[1]
 * This file is meant to pull user-defined functions out of the base template,
 * so the template can change without fear of merge conflicts.
 */

uint8_t extendPin = 3;
uint8_t retractPin = 10;

/* Allows user to set things up. */
void user_setup() {
  pinMode(extendPin, OUTPUT);
  pinMode(retractPin, OUTPUT);
  digitalWrite(retractPin, LOW);
  digitalWrite(extendPin, HIGH); //start off in extended state
  delay(1000);
  digitalWrite(extendPin, LOW);
}

/* Allows user to clean things up before a long shutdown. */
void user_shutdown() {
  digitalWrite(extendPin, HIGH); //start off in extended state
  delay(1000);
  digitalWrite(extendPin, LOW);
}

void manipulate_data(char* data) {
  if (data[1] == 0) {
    digitalWrite(retractPin, HIGH);
    delay(1000);
    digitalWrite(retractPin, LOW);
  } else if (data[1] == 1) {
    digitalWrite(extendPin, HIGH);
    delay(1000);
    digitalWrite(extendPin, LOW);
  }
}

