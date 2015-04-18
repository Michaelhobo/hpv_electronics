#include <Wire.h>
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "constants.h"
#include "addr.h"
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>


char write_buffer[RF24_TRANSFER_SIZE];
char read_buffer[RF24_TRANSFER_SIZE];
RF24 radio(8, 7);
uint8_t missed = 0;
uint8_t state;
int last_time = 0;
uint8_t check = 0;
uint8_t onShutdown = 0;

const uint64_t masterAddress = klondike? 0x00F0F0F0F0LL : 0x00E0E0E0E0LL;
const uint64_t myAddress = klondike? (0xF0F0F0F000LL | MYADDR) : (0xE0E0E0E000LL | MYADDR);


ISR(WDT_vect)
{
  if (check == 0){
    check = 1;
  }
  else{
    Serial.println("Fired but not in sleep");
  }
}

void setup(void)
{
	Serial.begin(57600);
	radio.begin();
	radio.setRetries(15,15);
	state = CONNECTED;
	radio.setPayloadSize(RF24_TRANSFER_SIZE);
	radio.openReadingPipe(1,myAddress);
	radio.openWritingPipe(masterAddress);
	radio.startListening();
	user_setup();
        /*** Setup the WDT ***/
  
        /* Clear the reset flag. */
        MCUSR &= ~(1<<WDRF);
  
        /* In order to change WDE or the prescaler, we need to
         * set WDCE (This will allow updates for 4 clock cycles).
         */
        WDTCSR |= (1<<WDCE) | (1<<WDE);

        /* set new watchdog timeout prescaler value */
        WDTCSR = 1<<WDP3; /* 4.0 seconds */
  
        /* Enable the WD interrupt (note no reset). */
        WDTCSR |= _BV(WDIE);
}

void shutdown_all(){
    user_shutdown(); //Prioritize whatever the user wants to shut down first, before the execution of the shutdown of arduino.
    radio.powerDown();
    onShutdown = 1;
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);   // sleep mode is set here
    sleep_enable();          // enables the sleep bit in the mcucr register    
    sleep_mode();            // here the device is actually put to sleep!! 
                              // THE PROGRAM CONTINUES FROM HERE AFTER WAKING UP
    sleep_disable();         // first thing after waking from sleep:
                             // disable sleep...
    radio.powerUp();
    radio.openReadingPipe(1, myAddress);
    radio.openWritingPipe(masterAddress);
    radio.startListening();
    delay(400);
}

bool ping_master() {
        radio.stopListening();
	bool received = false;
	received = radio.write(write_buffer, sizeof(char) * 10);
	if (received) {
		Serial.println("write ok...\n\r"); 
	} else  {
		Serial.println("write failed.\n\r");
	}
	return received;
        radio.startListening();
}

bool read_data() {
	bool hasRead = false; 
	while (radio.available()){
		hasRead=radio.read(read_buffer, RF24_TRANSFER_SIZE);
		Serial.println("Successfully read data");
                if(read_buffer[1] == 'x'){
                    onShutdown = 1;
                }
                else {
		    manipulate_data(read_buffer);
                    onShutdown = 0;
                }
	}
	return hasRead;
}

void loop(void)
{    
	read_data();
        if (onShutdown== 1)
        {
          shutdown_all();
        }
	if (millis() > last_time + PING_DELAY) {
		ping_master();
		last_time = millis();
	}
        
}
