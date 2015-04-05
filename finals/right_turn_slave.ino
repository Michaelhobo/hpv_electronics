#include <SPI.h>
#include "nRF24L01.h"
#include <stdio.h>
#include "RF24.h"

RF24 rf24(8, 7);

uint8_t pinList [8] = {3,4,5,6,14,15,16,17};
uint8_t light_on = 0;
uint8_t num;

const uint64_t master_address = 0x00F0F0F0F0LL;   // Address of the main controller.
const uint64_t slave_address = 0xF0F0F0F000LL | 'r';    // Address of this controller.
const int PAYLOAD = 1;

void setup(){
    for (int i = 0; i < 8; i++){
        pinMode(pinList[i], OUTPUT);
    }
  
    rf24.begin();
    rf24.setRetries(15, 15);
    rf24.setPayloadSize(PAYLOAD);
    
    rf24.openWritingPipe(master_address);
    rf24.openReadingPipe(1,slave_address);
    
    rf24.startListening();
}

void loop(){
    if (rf24.available()) {
        rf24.read(&light_on, sizeof(uint8_t));
    }
  
    if (light_on) {
        for (num = 0; num < 8; num++) {
            digitalWrite(pinList[num], HIGH);
            delay(250);
            digitalWrite(pinList[num], LOW);
        }
    }
}
