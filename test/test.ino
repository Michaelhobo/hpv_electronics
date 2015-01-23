#include <SPI.h>
#include "nRF24L01.h"
#include <stdio.h>
#include "RF24.h"
#include "println.h"

RF24 radio(9,10);
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };
uint8_t state = 0; //state that the sensor is in. 0 = unconnected, 1 = connected, 2 = sleep
int serial_console_putc(char c, FILE *) {
  Serial.write(c);
  return 0;
}
void setup(void)
{
	Serial.begin(9600);
	Serial.println("\n\rRadio Setup: test\n\r");
	radio.begin();
	radio.setDataRate(RF24_1MBPS);
	radio.setCRCLength(RF24_CRC_8);
	radio.setPayloadSize(10);
	radio.setChannel(101);
	radio.setAutoAck(true);
        fdevopen(&serial_console_putc, NULL);
        //radio.openReadingPipe(1, 0xF0F0F0F0D2);
        radio.printDetails();
        radio.openWritingPipe(0x00F0F0F0F0);

}
void loop(void)
{
     if (state == 0) {
	//unsigned long time = millis();
	Serial.print("Now sending...");
	//radio.printDetails();
        //radio.powerUp();
        radio.stopListening();
	bool ok = radio.write( "hello!!!\r\n",sizeof(char)*10);
	if (ok) {
		Serial.println("ok...\n\r"); 
                state = 1;
	} else  {
		Serial.println("failed.\n\r");
        }
	delay(100);
        radio.startListening();
     } else if (state == 1) {
         Serial.println("Connected");
         delay(2000);
     }
}
