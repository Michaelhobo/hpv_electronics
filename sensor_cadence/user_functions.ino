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

int count = 0;
double last = 0;
double secondLast = 0;
void tick(){
  count++;
}

/* This function is called before we send. */
void data_manipulation() {
  delay(9000);
    if (last!= 0){
      if (secondLast != 0){
        write_buffer[1] = (uint8_t)floor((secondLast*5+last*7+(double)count)/22*10+.5);
      }
      else{
        write_buffer[1] = (uint8_t)floor((last*7+(double)count)/12*10+.5);
      }
      secondLast = last;
    }
    else {
      write_buffer[1] = (uint8_t)floor(((double)count/10)*10+.5);
    }
    last = (double)count/10;
    count = 0;
}

/* These functions are called after we send data,
 * and we're figuring out what state 
 * we're going to switch to.
 */
void connected_action() {}

void sleep_action() {}

void deep_sleep_action() {}
