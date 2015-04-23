/* User Functions - Sensor Template
 * These functions should perform appropriate actions at every cycle of the loop.
 * Collected and formatted data should be stored in write_buffer[1]
 * This file is meant to pull user-defined functions out of the base template,
 * so the template can change without fear of merge conflicts.
 */

/* Allows user to set things up. */

int secondInput = 5;

int ledShow = 7;

void user_setup() {
  attachInterrupt(1, tick, RISING);
  pinMode(secondInput, INPUT);
  pinMode(ledShow, OUTPUT);
}


void blinkk(){
  digitalWrite(ledShow, HIGH);
  delay(20);
  digitalWrite(ledShow, LOW);
}
int counts [10];
int count = 0;
uint8_t laststate = 0;
int counter = 0;
uint8_t firsttime = 1;
void tick(){
  uint8_t state = digitalRead(secondInput);
  if (state!=laststate)
  {
    if(state == HIGH){
      count++;

    }
    laststate = state;
    digitalWrite(ledShow, state);
  }
}

/* This function is called before we send. */
void data_manipulation() {
    double total = 0;
    counts[counter] = count;
    if (counter == 9){
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
      for (int i = 0; i< 10; i++)
        {
           total+=counts[counter];
        }
    }
    if (firsttime){
      total = (total/counter)*60;
    }
    else{
      total *= 6;
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
