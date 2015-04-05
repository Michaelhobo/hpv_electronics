#include <SPI.h>
#include <Servo.h>
#include "nRF24L01.h"
#include <stdio.h>
#include "RF24.h"
#include "println.h"
#include "constants.h"

#define MY_ADDR 1
RF24 rf24(8,7); //change to 7,8 because 9,10 are pwm pins

Servo servo1;
uint8_t gear_positions[11] = {180, 162, 144, 126, 108, 90, 72, 54, 36, 18, 0};

/* For serial debugging. */
int serial_console_putc(char c, FILE *) {
    Serial.write(c);
    return 0;
}
yes
uint8_t state; //state that the sensor is in. 0 = connected, 1 = connected, 2 = sleep
char *name = "template";
uint64_t master_general_address = ((MY_ADDR % 4) + 2) & 0x00F0F0F0F0; //master will read on this address
uint64_t master_connection_address = 0x00F0F0F0F1; //this is the address we write to connect to the master
char read_buffer[RF24_TRANSFER_SIZE];
char write_buffer[RF24_TRANSFER_SIZE];
char *w_data;
/* Run setup code. */
//const int servoSig = 0 //analog out pin for servo
const int servo_pin = 9;

void setup() {
    Serial.begin(9600);  
    servo1.attach(servo_pin);
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
 
/* This slave is a gear shifter.  data should be a character array of length 1.
 * The number in data should be the gear number, which is one off from
 * the gear position array.
 */
void read_handler(char *data) {
    if (strstr(data, "s") == data) {
        shutdown();
    } else {
        servo1.write(gear_positions[*data - 1]);
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