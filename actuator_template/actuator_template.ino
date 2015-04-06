#include <Servo.h>

#include <Wire.h>

/**
 * Example RF Radio Ping Pair
 *
 * This is an example of how to use the RF24 class.  Write this sketch to two different nodes,
 * connect the role_pin to ground on one.  The ping node sends the current time to the pong node,
 * which responds by sending the value back.  The ping node can then see how long the whole cycle
 * took.
 */
const int servo_pin = 9;
uint8_t gear_positions[11] = {180, 162, 144, 126, 108, 90, 72, 54, 36, 18, 0};
Servo servo1;

#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
//#include " .h"
//#include "println.h"
#include "constants.h"

#define MYADDR 'g';

char write_buffer[RF24_TRANSFER_SIZE];
char read_buffer[RF24_TRANSFER_SIZE];
RF24 radio(8, 7);
uint8_t missed = 0;
uint8_t state;

const uint64_t masterAddress = 0x00F0F0F0F0LL;
const uint64_t myAddress = 0xF0F0F0F000LL | MYADDR;
#define NUM_SENSORS 4

char sensor_data[NUM_SENSORS];

void setup(void)
{

  //
  // Print preamble
  //

  Serial.begin(57600);
  radio.begin();
  radio.setRetries(15,15);
  state = CONNECTED;
  servo1.attach(9);

  radio.setPayloadSize(RF24_TRANSFER_SIZE);

  //
  // Open pipes to other nodes for communication
  //

  // This simple sketch opens two pipes for these two nodes to communicate
  // back and forth.
  // Open 'our' pipe for writing
  // Open the 'other' pipe for reading, in position #1 (we can have up to 5 pipes open for reading)

    radio.openReadingPipe(1,myAddress);
    radio.openWritingPipe(masterAddress);

  radio.startListening();
}

void shutdown_all(){
  /* For future work niggas */
}

bool ping_master() {
	bool received = false;
	received = radio.write(write_buffer, sizeof(char) * 10);
	if (received) {
		Serial.println("write ok...\n\r"); 
	} else  {
		Serial.println("write failed.\n\r");
	}
	return received;
}

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



bool read_data() {
  bool hasRead;
  while (radio.available()){
            radio.read(read_buffer, RF24_TRANSFER_SIZE);
//            if (hasRead){
              Serial.println("Successfully read data");
              manipulate_data(read_buffer);
//            }
//            else{
//              Serial.println("Fatal error in reading data");
//            }
        }

	//delay(100);
	return hasRead;
}


void loop(void)
{    
    read_data();
}
// vim:cin:ai:sts=2 sw=2 ft=cpp
