#include <SPI.h>
#include "nRF24L01.h"
#include <stdio.h>
#include "RF24.h"

RF24 rf24(8, 7);

uint8_t gear_up_pin = 3;
uint8_t gear_down_pin = 4;
uint8_t gear_down = 1;
uint8_t gear_down_previous = 0;

const uint64_t master_address = 0x00F0F0F0F0LL;   // Address of the main controller.
const uint64_t slave_address = 0xF0F0F0F000LL | 'l';    // Address of this controller.
const int PAYLOAD = 1;

void setup(){
    pinMode(gear_up_pin, OUTPUT);
    pinMode(gear_down_pin, OUTPUT);
  
    rf24.begin();
    rf24.setRetries(15, 15);
    rf24.setPayloadSize(PAYLOAD);
    
    rf24.openWritingPipe(master_address);
    rf24.openReadingPipe(1,slave_address);
    
    rf24.startListening();
}

void loop(){
    if (rf24.available()) {
        rf24.read(&gear_down, sizeof(uint8_t));
    }
  
    //Send pulse to solenoid once only, to conserve power.
    if (gear_down != gear_down_previous) {
        gear_down_previous = gear_down;
        if (gear_down) {
            digitalWrite(gear_down_pin, HIGH);
            delay(500);
            digitalWrite(gear_down_pin, LOW);
        } else {    //want gear to be up
            digitalWrite(gear_up_pin, HIGH);
            delay(500);
            digitalWrite(gear_up_pin, LOW);
        }
    }
}
