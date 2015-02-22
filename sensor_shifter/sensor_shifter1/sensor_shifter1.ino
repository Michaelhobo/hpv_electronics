#include <SPI.h>
#include "nRF24L01.h"
#include <stdio.h>
#include "RF24.h"
#include "println.h"
#include "constants.h"

#define MY_ADDR 1
RF24 rf24(8,7); //change to 7,8 because 9,10 are pwm pins
//INSERTARRAYHERE
double distances[11]; //Array of 11 different distances for gear shifting.

/* For serial debugging. */
int serial_console_putc(char c, FILE *) {
	Serial.write(c);
	return 0;
}

uint8_t state; //state that the sensor is in. 0 = connected, 1 = connected, 2 = sleep
char *name = "template";
uint64_t master_general_address = ((MY_ADDR % 4) + 2) & 0x00F0F0F0F0; //master will read on this address
uint64_t master_connection_address = 0x00F0F0F0F1; //this is the address we write to connect to the master
char read_buffer[RF24_TRANSFER_SIZE];
char write_buffer[RF24_TRANSFER_SIZE];
char *w_data;
/* Run setup code. */
const int relay1Pin =  3;      // the number of the Relay1 pin
const int relay2Pin =  4;      // the number of the Relay2 pin
const int sensorPin = 0;    // select the input pin for the 
int goalPosition = 350; 
int CurrentPosition = 0; 
boolean Extending = false;
boolean Retracting = false;
void setup() {
	Serial.begin(9600);
        // initialize the relay pin as an output:
        pinMode(relay1Pin, OUTPUT);    
        pinMode(relay2Pin, OUTPUT);    
        
        //preset the relays to LOW
        digitalWrite(relay1Pin, LOW); 
        digitalWrite(relay2Pin, LOW); 
        
	state = DISCONNECTED;
	write_buffer[0] = MY_ADDR;
	w_data = (char *) (write_buffer + 1);
	rf24.begin();
	rf24.setDataRate(RF24_1MBPS);
	rf24.setCRCLength(RF24_CRC_8);
	rf24.setPayloadSize(RF24_TRANSFER_SIZE);
	rf24.setChannel(101);
	rf24.setAutoAck(true);

	/* For debugging, comment out when not needed. */
	fdevopen(&serial_console_putc, NULL);
	rf24.printDetails();

	/*if (!connect_master()) {
		shutdown();
		}*/
	//rf24.openWritingPipe(my_pipe);
}


bool connect_master() {
	Serial.print("connecting master...");
	Serial.flush();
	char *start_msg = (char *) malloc(RF24_TRANSFER_SIZE);
	sprintf(start_msg, "%c%s", MY_ADDR, "connect");
	Serial.print("saved start message");
	Serial.flush();
	rf24.openWritingPipe(master_connection_address);
	//rf24.openWritingPipe(0x00F0F0F0F0);
	//rf24.openReadingPipe(1, 0xF0F0F0F0D2);
	bool connected = false;
	int timeout = 100; //timeout in ms;
	while (!connected) {
		rf24.stopListening();
	connected = rf24.write( start_msg,sizeof(char)*10);
	if (connected) {
		Serial.println("connect ok...\n\r");
	} else  {
		Serial.println("connect failed.\n\r");
        }
	delay(100);
		/*bool connected = rf24.write(start_msg, sizeof(char) * RF24_TRANSFER_SIZE);
		if (!connected) {
			if (timeout > 1000) {
				free(start_msg);
				Serial.println("failed.");
				return false;
			}
			delay(timeout);

			timeout += 100;
			Serial.print("failed...");
		}*/
		rf24.startListening();

	}
	free(start_msg);
	Serial.println("done.");
	return true;
}

/* Writes data to master
 * Use this for sensor-type slaves that gather data locally and send to the master
 * Comment out the write_data line in loop function if not needed.
 * Write data to write_data, which can store a max of (RF24_TRANSFER_SIZE - 1) bytes/chars.
 */
void write_data() {
	/* Write code here. */
	//shift_gear(shiftv);

	rf24.stopListening();
	bool received = false;
	while (!received) {
		received = rf24.write(write_buffer, sizeof(char) * 10);
                	if (received) {
		Serial.println("write ok...\n\r");
	} else  {
		Serial.println("write failed.\n\r");
        }
	delay(100);
		rf24.startListening();
	}
}
/* Shut down this sensor. */
void shutdown() {
	//power down antenna, set all unused pins low, put microcontroller to sleep for 1/2(?) second then wake up
}

/* Handles an incoming packet meant for this slave.
 * @data The data packet meant for this slave.
 */

void read_handler(char *data) {
	if (strstr(data, "shutdown") == data) {
		shutdown();
	} else {
		/* Write code here. */
                // read the value from the sensor:
                CurrentPosition = analogRead(sensorPin); 
                
		//take signal from xbee and moves actuator to desired gear.
                char *a = data[0];
                char *b = data[1];
                int shiftv = 10 * atoi(a)+ atoi(b);
                if (shiftv > 11) {
                  Serial.println("unattainable gear");
                } else {
                    goalPosition = listOfGearPositions[shiftv];
                    if (goalPosition > CurrentPosition) {
                        while (goalPosition > CurrentPosition) {
                            Retracting = false;
                            Extending = true;
                            digitalWrite(relay1Pin, HIGH);  
                            digitalWrite(relay2Pin, LOW);  
                            Serial.println("Extending");     
                            CurrentPosition = analogRead(sensorPin); 
                        }
                        digitalWrite(relay1Pin, LOW);
                        digitalWrite(relay2Pin, LOW);
                        Serial.println("Shifting Done");
                    }      
                    else if (goalPosition < CurrentPosition) {
                         while (goalPosition < CurrentPosition) {
                            Retracting = true;
                            Extending = false;
                            digitalWrite(relay1Pin, LOW);  
                            digitalWrite(relay2Pin, HIGH); 
                            Serial.println("Retracting");         
                        }  
                        digitalWrite(relay1Pin, LOW);
                        digitalWrite(relay2Pin, LOW);
                        Serial.println("Shifting Done");
                    }
                /* I don't know whether this is needed.
                uint8_t seqno = (uint8_t) data[0];
                */
                }
                //master-side: data[1] = (uint8_t) 10;
        }

	}
}

/* Reads data from master
 * All slaves should use this, to receive command messages such as shutdown.
 */
void read_data() {
	if (rf24.available()) {
		rf24.read(read_buffer, RF24_TRANSFER_SIZE);
		if (read_buffer[0] == MY_ADDR) {
			read_handler(read_buffer + 2);
		}
	}
}
void loop() {
	// put your main code here, to run repeatedly
	Serial.println("looping...");
	if (state == DISCONNECTED) {
                Serial.println("Disconnected");
		if (!connect_master()) {
			state = SLEEP;
			Serial.println("failed to connect");
		} else {
                    state = CONNECTED;
                }
	} else if (state == CONNECTED) {
                Serial.println("Connected");
		read_data();
		write_data();
		delay(1000);
	} else if (state == SLEEP) {
                Serial.println("Sleep");
		delay(1000);
		if (connect_master()) {
			state = CONNECTED;
		} else {
			Serial.println("failed to connect");
			//should it wake up?
                }
	} else if (state == DEEP_SLEEP) {
                  Serial.println("Deep Sleep");
			delay(10000);
			if (connect_master()) {
				state = CONNECTED;
			} else {
				Serial.println("failed to connect");
			}
		}
	}

