#include <SPI.h>
#include <Servo.h>
#include "nRF24L01.h"
#include <stdio.h>
#include "RF24.h"


RF24 rf24(8,7); //change to 7,8 because 9,10 are pwm pins
/*
Servo servo1;
const int servo_pin = 9;
uint8_t gear_positions[11] = {180, 162, 144, 126, 108, 90, 72, 54, 36, 18, 0};
*/
/* For serial debugging. */
int serial_console_putc(char c, FILE *) {
    Serial.write(c);
    return 0;
}

const uint64_t master_address = 0x00F0F0F0F0LL;   // Address of the main controller.
const uint64_t servo_address = 0xF0F0F0F000LL | 'g';    // Address of this controller.
const int PAYLOAD = 1;


void setup() {
    Serial.begin(9600);  
    servo1.attach(servo_pin);

    rf24.begin();
    rf24.setRetries(15, 15);
    rf24.setPayloadSize(PAYLOAD);
    
    rf24.openWritingPipe(servo_address);
    rf24.openReadingPipe(1,master_address);
    
    rf24.stopListening();
}

void loop() {

    for(char a = 1; a < 12; a++) {
      rf24.write(&a, sizeof(char));
      delay(1000);
    }
}
