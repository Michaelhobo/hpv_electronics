/* User Functions - Actuator Template
 * These functions should perform appropriate actions at every cycle of the loop.
 * Collected and formatted data should be stored in write_buffer[1]
 * This file is meant to pull user-defined functions out of the base template,
 * so the template can change without fear of merge conflicts.
 */

/* Allows user to set things up. */
void user_setup() {}

/* Allows user to clean things up before a long shutdown. */
void user_shutdown() {}

void manipulate_data(char* data){
    //if (data[0] == 'b'){
      if (data[1] == 's'){
        shutdown_all();
      }
      else{
        /* Code here please */
        uint8_t gear = data[1];
        Serial.print("Switching to gear ");
        Serial.println(gear);
  servo1.write(gear_positions[gear - 1]);
      }
    //}
}
