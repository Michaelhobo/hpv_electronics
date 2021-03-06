/* User Functions - Actuator Template
 * ONLY EDIT FUNCTIONS BELOW THIS LINE TO PREVENT MERGE CONFLICTS
 * These functions should perform appropriate actions at every cycle of the loop.
 * Collected and formatted data should be stored in write_buffer[1]
 * This file is meant to pull user-defined functions out of the base template,
 * so the template can change without fear of merge conflicts.
 */

/* Allows user to set things up. */

//Sets up an array of the pins used in the turn signal, the status of the turn signal and the light number that is currently on.
uint8_t pinList [8] = {3,4,5,6,14,15,16,17};
uint8_t light_on = 0;
uint8_t num = 0;
int last_blink = 0;
int blink_delay = 500; //delay in ms between blinks

void user_setup() {
    for (int i = 0; i < 8; i++){
        pinMode(pinList[i], OUTPUT);
        digitalWrite(pinList[i], LOW);
    } 
		
}

/* Allows user to clean things up before a long shutdown. */
void user_shutdown() {
  turnOff();
}

void user_loop() {
  if (light_on && millis() > last_blink + blink_delay) {
    proceedBlink();
  }
}
  
//Proceeds with the blinking sequence
void proceedBlink(){
      digitalWrite(pinList[num], LOW);
      num = (num == 7)? 0: num+1;
      digitalWrite(pinList[num], HIGH);
      last_blink = millis();
}



void turnOn(){
  light_on = 1;
  proceedBlink();
}

void turnOff(){
  light_on = 0;
  digitalWrite(pinList[num], LOW);
  num = 0;
}

void manipulate_data(char* data) {
  if (data[1] == '1'){
    turnOn();
  }
  else if (data[1] == '0'){
    turnOff();
  }
}

