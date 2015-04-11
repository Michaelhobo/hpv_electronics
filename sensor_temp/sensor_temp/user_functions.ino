/* User Functions - Sensor Template
 * These functions should perform appropriate actions at every cycle of the loop.
 * Collected and formatted data should be stored in write_buffer[1]
 * This file is meant to pull user-defined functions out of the base template,
 * so the template can change without fear of merge conflicts.
 */
#include <dht11.h>
dht11 DHT11;
#define DHT11PIN 3
int i = 0;
/* Allows user to set things up. */
void user_setup() {
  //Celsius to Fahrenheit conversion
  
  Serial.begin(115200);
  Serial.println("DHT11 TEST PROGRAM ");
  Serial.print("LIBRARY VERSION: ");
  Serial.println(DHT11LIB_VERSION);
  Serial.println();
}

/* This function is called before we send. */
void data_manipulation() {
  Serial.println("\n");

  int chk = DHT11.read(DHT11PIN);

  Serial.print("Read sensor: ");
  switch (chk)
  {
    case DHTLIB_OK: 
		Serial.println("OK"); 
		break;
    case DHTLIB_ERROR_CHECKSUM: 
		Serial.println("Checksum error"); 
		break;
    case DHTLIB_ERROR_TIMEOUT: 
		Serial.println("Time out error"); 
		break;
    default: 
		Serial.println("Unknown error"); 
		break;
  }
  
  if (i == 1) {
    Serial.print("Humidity (%): ");
    Serial.println((float)DHT11.humidity, 2);
    write_buffer[1] = (uint8_t) DHT11.humidity;
    i = 0;
    
  } else {
    Serial.print("Temperature (°C): ");
    Serial.println((float)DHT11.temperature, 2);
    write_buffer[1] = (uint8_t) DHT11.temperature;
    i = 1;
  }

 
  delay(2000);
}

/* These functions are called after we send data,
 * and we're figuring out what state 
 * we're going to switch to.
 */
void connected_action() {}

void sleep_action() {}

void deep_sleep_action() {}
