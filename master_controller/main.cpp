#include "mbed.h"
#include <limits.h>
#include <stdio.h>
#include "xbee.h"
#include "constants/constants.h"
#include "main.h"

//#include "TextLCD.h"
#include <string>
#include <sstream>

#define XBEE_SEND_INTERVAL 2
#define PC_SEND_INTERVAL 1
#define MY_ADDR 0

/* Sensor count goes like this:
0 - speed
1 - cadence
2 - temp
3 - outdoor brightness
*/
#define NUM_SENSORS 4


/* Actuator count goes like this:
g - gear shifter
f - front light
r - right turn
t - left turn
l - landing gear
*/
#define NUM_ACTUATORS 5

DigitalOut led1(LED1);
DigitalOut led2(LED2);
DigitalOut led3(LED3);
DigitalOut led4(LED4);
Serial pc(USBTX, USBRX); // tx, rx
xbee xbee(p13, p14, p12);
//TextLCD lcd(p21, p22, p23, p24, p25, p26); // rs, e, d4-d7
I2C arduino(p9, p10);
//InterruptIn critical(p8);

DigitalIn shift_up(p5);
DigitalIn shift_down(p6);
DigitalIn landing_up(p7);
DigitalIn landing_down(p8);
DigitalIn turn_left(p11);
DigitalIn turn_right(p14);

Ticker tick_arduino, tick_lcd;
Ticker t_gear, t_landing, t_turn_signal;
//Timeout timeout;

/* I2C values */
uint8_t read_addr = 0x7 << 1 | 0x1;
uint8_t write_addr = 0x7 << 1;
char out[2];
char arduino_updates[NUM_SENSORS];

/* Data Values. */
double speed = 0.0; //calculated speed
uint8_t cadence = 0;
int8_t temp = 0;
uint8_t brightness = 0;


/* Actuator Values */
uint8_t gear = 1;
uint8_t front_light = 0;
uint8_t right_turn = 0;
uint8_t left_turn = 0;
uint8_t landing_gear = 0;

/* Initialize everything necessary for the scripts. */
void init() {
	pc.printf("Human Powered Vehicle Controller");
//	critical.rise(&get_updates);
    
	tick_arduino.attach(&get_updates, 1);
//    tick_lcd.attach(&update_lcd, 1);
    
    t_gear.attach(&shift_gear_fn, 0.1);
    t_landing.attach(&landing_fn, 0.5);
    t_turn_signal.attach(&turn_signal_fn, 0.5);
}

/*ID s
gear shifter = 'g'
landing gear = 'l'
right turn = 'r'
left turn = 't'
front light = 'f' //unused
*/

//Debouncer variables for shifting
uint8_t shift_up_hold = 0;
uint8_t shift_down_hold = 0;
void shift_gear_fn() {
    if (shift_up.read()) {
        if (!shift_up_hold && (gear < 11)) {
            gear++;
        }
        shift_up_hold = 1;
    } else {
        shift_up_hold = 0;
    }
    if (shift_down.read()) {
        if (!shift_down_hold && (gear > 1)) {
            gear--;
        }
        shift_down_hold = 1;
    } else {
        shift_down_hold = 0;
    }
    send_sensor('g', gear);
}



//Landing gear: 0 = up, 1 = down.
void landing_fn() {
    if (landing_up.read()) {
        landing_gear = 0;
        send_sensor('l', 0);
    } else if (landing_down.read()) {
        landing_gear = 1;
        send_sensor('l', 1);
    } else {    //Automatic controller
        if (landing_gear && (speed > 10)) { //Landing gear down but fast
            landing_gear = 0;
            send_sensor('l', 0);
        } else if (!landing_gear && speed < 5) { //Landing gear up but slow
            landing_gear = 1;
            send_sensor('l', 1);
        }
    }
}

void turn_signal_fn() {
    if (turn_left.read()) {
        left_turn = 1;
    } else {
        left_turn = 0;
    }
    
    if (turn_right.read()) {
        right_turn = 1;
    } else {
        right_turn = 0;
    }

    send_sensor('t', left_turn);
    send_sensor('r', right_turn);
}

/* Get updates from arduino. 
 * A 255 means the value has not been updated from last time.
 */
void get_updates() {
	pc.printf("get_updates() ...");
	arduino.read(read_addr, arduino_updates, NUM_SENSORS);
	for (int i = 0; i < NUM_SENSORS; i++) {
		if (arduino_updates[i] != 255) {
			switch (i) {
				case 0:
					speed = (double) ((uint8_t) arduino_updates[i] * 3.0 / 10.0); // speed converion 
					break;
				case 1:
					cadence = arduino_updates[i];
					break;
				case 2:
					temp = arduino_updates[i];
					break;
				case 3:
					brightness = arduino_updates[i];
					break;
			}
		}
	}
	pc.printf("done\n\r");
}

/* Send to a sensor with an id. */
void send_sensor(uint8_t id, char data) {
	out[0] = id;
	out[1] = data;
	arduino.write(write_addr, out, 2);
}
/*
void update_lcd() {
	//update lcd display after receiving and sending data to handler
		//write to LCD screen
        //if (t.read() > last_time + min_time_update){
            //format of the screen is
            //cadence          dist
            //______________________
            //|      GEAR   CADENCE|
            //|       XX      XXX  |
            //|SPEED     TIME      |
            //| XX      XXX:XX     |
            //|---------------------
            //gear              time
            lcd.cls();
						int last_time = 60;//FOR NOW
            std::ostringstream format_data;
            format_data << "      GEAR   CADENCE";
            //line 2
            format_data << "       ";
            format_data.width(2);
            format_data << gear_val;
            format_data << "      ";
            format_data.width(3);
            format_data << cadence;
            format_data << "  ";
            //third line
            format_data << "SPEED     TIME      ";
            //fourth line
            format_data << " ";
            format_data.width(2);
            format_data << speed;
            format_data << "      ";
            int minutes = (int)last_time/60;
            format_data.width(3);
            format_data << minutes;
            format_data << ":";
            int seconds = ((int)last_time) % 60;
            format_data.width(2);
            format_data << seconds;
            format_data << "     ";
            lcd.printf(format_data.str().c_str());
}
*/
/* Main. */
int main() {
	init();
}
