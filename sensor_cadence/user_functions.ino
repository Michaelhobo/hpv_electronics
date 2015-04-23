/* User Functions - Sensor Template
 * These functions should perform appropriate actions at every cycle of the loop.
 * Collected and formatted data should be stored in write_buffer[1]
 * This file is meant to pull user-defined functions out of the base template,
 * so the template can change without fear of merge conflicts.
 */

/* Allows user to set things up. */
void user_setup() {
  attachInterrupt(1, tick, LOW);
}


int counts [60];
int count = 0;
int lasttime;
int currenttime;
int counter = 0;
uint8_t firsttime = 1;
void tick(){
  Serial.println("Triggered");
  currenttime = millis();
  if (currenttime-lasttime >= 90){
    counter++;
  }
  lasttime = currenttime;
  
}

/* This function is called before we send. */
void data_manipulation() {
    double total = 0;
    counts[counter] = count;
    if (counter == 59){
      counter= 0;
      firsttime = 0;
    }
    else{ counter++;}
    if (firsttime){
      for (int i = 0; i< counter; i++)
      {
         total+=counts[counter];
      }
    }
    else{
      for (int i = 0; i< 60; i++)
        {
           total+=counts[counter];
        }
    }
    if (firsttime){
      total = (total/counter)*60;
    }
    write_buffer[1] = (uint8_t) total;
    Serial.println(total);
    Serial.println(count);
    count = 0;
    Serial.println(write_buffer[1]);
}

/* These functions are called after we send data,
 * and we're figuring out what state 
 * we're going to switch to.
 */
void connected_action() {}

void sleep_action() {}

void deep_sleep_action() {}
