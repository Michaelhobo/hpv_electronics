#include "mbed.h"
#include <limits.h>
#include <stdio.h>
#include "xbee.h"
#include "nRF24L01P.h"
#include "TextLCD.h"
#include "constants/constants.h"
#include <string>
#include <sstream>

using namespace std;
#define XBEE_SEND_INTERVAL 2
#define PC_SEND_INTERVAL 1
#define MY_ADDR 0

DigitalOut led1(LED1);
DigitalOut led2(LED2);
DigitalOut led3(LED3);
DigitalOut led4(LED4);
Serial pc(USBTX, USBRX); // tx, rx
xbee xbee(p13, p14, p12);
nRF24L01P rf24(p5, p6, p7, p8, p9, p10);
TextLCD lcd(p21, p22, p23, p24, p25, p26); // rs, e, d4-d7

Ticker events;
Timeout timeout;
double speed = 0.0; //calculated speed
double cadence = 0.0;
int gear_val = 0;
clock_t begin, end;
double last_time;
int landing_gear = 1; //1 for down, 0 for up
int landing_gear_mode = 0; //0 for manual, 1 for auto

char receive_buffer[RF24_TRANSFER_SIZE];
char speed_buffer[RF24_TRANSFER_SIZE];
char send_buffer[RF24_TRANSFER_SIZE];
const char *sensor_names[255] = {0};
void (*sensor_handlers[255])(char *data);
char sensor_states[255] = {DISCONNECTED};

/* RF24 Handlers. They must all take in a char* parameter. */
void receiver_handler(char *data);
void speed_handler(char *data);
void cadence_handler(char *data);
void rear_lights_handler(char *data);
void front_lights_handler(char *data);
void shifter_handler(char *data);

/* Prints speed to terminal through a usb. */
void show_usbterm_speed() {
	pc.printf("speed: %f\n", speed);
}

/* Sends speed to XBEE. */
void send_xbee_speed() {
	sprintf(speed_buffer, "%f\n", speed);
}

/* Initialize xbee for telemetry. */
void telemetry_init() {
	// convert units before to minimize calculations in critical section
	pc.printf("Initializing.\n");
}

/* Initialize one sensor's values. */
void init_sensor(int id, const char *name, void (*handler)(char *)) {
	sensor_names[id] = name;
	sensor_handlers[id] = handler;
	sensor_states[id] = 0;
}

/* initialize all rf24 sensor data. */
void rf24_init() {
	pc.printf("rf24 init\r\n");
	pc.printf( "nRF24L01+ Frequency    : %d MHz\r\n",  rf24.getRfFrequency() );
	pc.printf( "nRF24L01+ Output power : %d dBm\r\n",  rf24.getRfOutputPower() );
	pc.printf( "nRF24L01+ Data Rate    : %d kbps\r\n", rf24.getAirDataRate() );
	pc.printf( "nRF24L01+ TX Address   : 0x%010llX\r\n", rf24.getTxAddress() );
	pc.printf( "nRF24L01+ RX Address   : 0x%010llX\r\n", rf24.getRxAddress() );
	init_sensor(0, "receiver", &receiver_handler);
	init_sensor(1, "speed", &speed_handler);
	init_sensor(2, "cadence", &cadence_handler);
	init_sensor(3, "rear_lights", &rear_lights_handler);
	init_sensor(4, "front_lights", &front_lights_handler);
	init_sensor(5, "shifter", &shifter_handler);
	rf24.powerUp();
	rf24.setRfFrequency(2501);
	rf24.setTransferSize(RF24_TRANSFER_SIZE);
	rf24.setCrcWidth(8);
	rf24.enableAutoAcknowledge(NRF24L01P_PIPE_P0);
	//rf24.enableAutoAcknowledge(NRF24L01P_PIPE_P2);
	//rf24.enableAutoAcknowledge(NRF24L01P_PIPE_P3);
	//rf24.enableAutoAcknowledge(NRF24L01P_PIPE_P4);
	//rf24.enableAutoAcknowledge(NRF24L01P_PIPE_P5);
	rf24.setRxAddress(0x00F0F0F0F0, paddr_size, NRF24L01P_PIPE_P0);
	//rf24.setRxAddress(0x0000000002, paddr_size, NRF24L01P_PIPE_P2);
	//rf24.setRxAddress(0x0000000003, paddr_size, NRF24L01P_PIPE_P3);
	//rf24.setRxAddress(0x0000000004, paddr_size, NRF24L01P_PIPE_P4);
	//rf24.setRxAddress(0x0000000005, paddr_size, NRF24L01P_PIPE_P5);
	rf24.setReceiveMode();
	rf24.enable();
	pc.printf("MASTER: rf24 init finished\r\n");
}

void lcd_update_gear() {
	lcd.character(7,1,'0'+(char)( gear_val/10));
	lcd.character(8,1,0'0'+(char)(gear_val%10));
}

void lcd_update_cadence() {
	lcd.character(15,1,'0'+(char)((int)cadence/100));
	lcd.character(16,1,'0'+(char)((int)cadence/10%10));
	lcd.character(17,1,0'0'+(char)((int)cadence%10));
}

void lcd_update_speed() {
	lcd.character(2,3,'0'+(char)((int)speed/10));
	lcd.character(3,3,'0'+(char)((int)speed%10));
}

void lcd_update_time() {
	end = clock();
	last_time = (double)(end - begin) / CLOCKS_PER_SEC;
	lcd.character(8,3,54);
	lcd.character(8,3,'0'+(char)((int)last_time/60/100); 
	lcd.character(9,3, '0' + (char)((int)last_time/60/10%10));
	lcd.character(10,3,'0' + (char)((int)last_time/60%10));
	lcd.character(11,3,58);
	lcd.character(12,3,'0'+(char)((int)last_time%60/10));
	lcd.character(13,3,'0'+(char)((int)last_time%60%10));
}

void lcd_update_landing_gear() {
	if (landing_gear_mode == 0) { //if "a"uto
		lcd.character(3,0,065);		
	} else if (landing_gear_mode ==1) { //if "m"anual
		lcd.character(3,0,077);
	}
	if (landing_gear == 0) { //if "u"p
		lcd.character(3,0,85); 		
	} else if (landing_gear_mode ==1) { //if "d"own
		lcd.character(3,0,068);
	}
}

void lcd_display_init() {
	lcd.character(6,0,71);
	lcd.character(7,0,69);
	lcd.character(8,0,65);
	lcd.character(9,0,82);

	lcd.character(0,0,076);
	lcd.character(1,0,071);
	lcd.character(2,0,058);

	lcd.character(13,0,67);
	lcd.character(14,0,65);
	lcd.character(15,0,68);
	lcd.character(16,0,69);
	lcd.character(17,0,78);
	lcd.character(18,0,67);
	lcd.character(19,0,69);
	
	lcd.character(0,2,83);
	lcd.character(1,2,80);
	lcd.character(2,2,69);
	lcd.character(3,2,69);
	lcd.character(4,2,68);
	
	lcd.character(9,2,84);
	lcd.character(10,2,73);
	lcd.character(11,2,77);
	lcd.character(12,2,69);

	lcd_update_gear();
	lcd_update_cadence();
	lcd_update_speed();
	lcd_update_time();
	lcd_update_landing_gear();
}


/* Initialize everything necessary for the scripts. */
void init() {
	pc.printf("init");
	telemetry_init();
	rf24_init();
	begin = clock();
	lcd_display_init();
}

/* Send to a sensor with an id. */
bool send_sensor(uint8_t id, char *data) {
	send_buffer[0] = id;
	sprintf(send_buffer + 1, "%s", data);
	uint64_t pipe_addr = (id << 8) & 1;
	rf24.setTxAddress(pipe_addr, paddr_size);
	bool received = false;
	for (int i = 0; i < 10 && !received; i++) {
		received = rf24.write(NRF24L01P_PIPE_P0, send_buffer, RF24_TRANSFER_SIZE);
	}
	return received;
}
/* Process a connection. */
void process_connection() {
	uint8_t src_addr = (uint8_t) receive_buffer[0];
	if (sensor_states[src_addr] == DISCONNECTED) {
		if (strstr(receive_buffer + 1, "connect") == (receive_buffer + 1)) {
			led4 = 1;
			sensor_states[src_addr] = CONNECTED;
		}
	}
}
/* Process RF24 input and send it to the correct handler. */
void process_rf_input() {
	uint8_t src_addr = (uint8_t) receive_buffer[0];
	led2 = 1;
	if (sensor_handlers[src_addr]){
		sensor_handlers[src_addr](receive_buffer + 1);
		//update lcd display after receiving and sending data to handler
		//write to LCD screen
        //if (t.read() > last_time + min_time_update){
            //format of the screen is
            //cadence          dist
            //______________________
            //|GEAR         CADENCE|
            //| XX            XXX  |
            //|SPEED       TIME    |
            //| XX        XXX:XX   |
            //|---------------------
            //gear              time
            //lcd.cls();
						

						//int last_time = 60;//FOR NOW
            /*std::ostringstream format_data;
            format_data << "GEAR         CADENCE";
            //line 2
            format_data << " ";
            format_data.width(2);
            format_data << gear_val;
            format_data << "            ";
            format_data.width(3);
            format_data << cadence;
            format_data << "  ";
            //third line
            format_data << "SPEED       TIME    ";
            //fourth line
            format_data << " ";
            format_data.width(2);
            format_data << speed;
            format_data << "        ";
            int minutes = (int)last_time/60;
            format_data.width(3);
            format_data << minutes;
            format_data << ":";
            int seconds = ((int)last_time) % 60;
            format_data.width(2);
            format_data << seconds;
            format_data << "   ";
            lcd.printf(format_data.str().c_str());*/
	}
}

/* Main sending loop. */
int main() {
	init();
	pc.printf("Starting Logging.\n");
	//events.attach(&send_xbee_speed, XBEE_SEND_INTERVAL);
	//events.attach(&show_usbterm_speed, PC_SEND_INTERVAL);
	int x=0;
	while(x==0) {
		if (rf24.readable(NRF24L01P_PIPE_P0)) {
			led1 = 1;
			pc.printf("rf24 connection.\r\n");
			rf24.read(NRF24L01P_PIPE_P0, receive_buffer, RF24_TRANSFER_SIZE);
			pc.printf(receive_buffer);
			pc.printf("\r\n");
			process_connection();
		} else if (rf24.readable(NRF24L01P_PIPE_P2)) {
			led1 = 1;
			rf24.read(NRF24L01P_PIPE_P2, receive_buffer, RF24_TRANSFER_SIZE);
			process_rf_input();
		} else if (rf24.readable(NRF24L01P_PIPE_P3)) {
			led1 = 1;
			rf24.read(NRF24L01P_PIPE_P3, receive_buffer, RF24_TRANSFER_SIZE);
			process_rf_input();
		} else if (rf24.readable(NRF24L01P_PIPE_P4)) {
			led1 = 1;
			rf24.read(NRF24L01P_PIPE_P4, receive_buffer, RF24_TRANSFER_SIZE);
			process_rf_input();
		} else if (rf24.readable(NRF24L01P_PIPE_P5)) {
			led1 = 1;
			rf24.read(NRF24L01P_PIPE_P5, receive_buffer, RF24_TRANSFER_SIZE);
			process_rf_input();
		}
		lcd_update_time();
	}
}

/* Gets the id from the name of the sensor. */
uint8_t find_id(char *name) {
	uint8_t id = 0;
	while (strcmp(sensor_names[id], name)) {
		id += 1;
	}
	return id;
}

/* Send to a sensor with name.
 * Sending format: [dest_address(1), src_address(1), data(30)]
 */
void send_sensor_name(char *name, char *data) {
	uint8_t id = find_id(name);
	send_sensor(id, data);
}

/* RF24 Handlers. They must all take in a char* parameter. */

/* Gets the seqno from the data packet. */
unsigned int get_seqno(char *data) {
	uint8_t msb = (uint8_t) data[0];
	uint8_t lsb = (uint8_t) data[1];
	unsigned int seqno = msb * 255 + lsb;
	return seqno;
}

/* Sends an ack to the relevant sensors. */
void send_ack(int id, unsigned int seqno, char *data) {
	uint8_t msb = (uint8_t) seqno >> 8;
	uint8_t lsb = (uint8_t) seqno & 0x0F;
	char *send_data = (char *) malloc(RF24_TRANSFER_SIZE - 2);
	send_data[0] = msb;
	send_data[1] = lsb;
	sprintf(send_data + 2, "%s", data);
	send_sensor(id, send_data);
}

/* The receiver's handler should never really do anything.
 * If the receiver gets a message from itself, something went wrong.
 */
void receiver_handler(char *data) {
	led4 = 1; //We really shouldn't be doing anything. This is to warn us if this handler is ever triggered.
}

/* Get speed from data packet. */
double get_speed(char *data) {
	double spd;
	sscanf(data, "%lf", &spd);
	return spd;
}

/* Speed handler
 * Receive data in the format [seqno(2), speed(8)]
 * speed is a double.
 */
unsigned int speed_seqno = 0;
char *spd_string = (char *) malloc(8);
void speed_handler(char *data) {
	led3 = 1;
	unsigned int seqno = get_seqno(data);
	if (seqno > speed_seqno) {
		speed = get_speed(data); //should we update anything?
		sprintf(spd_string, "%3.4f", speed);
		send_ack(1, speed_seqno, spd_string);
	} else if (seqno == 0) {
		send_ack(1, 0, (char *) "ack");
		if (speed_seqno > 0) {
			send_ack(1, speed_seqno, spd_string);
		}
	}
}

/* Get cadence from data packet. */
double get_cadence(char *data) {
	double cad;
	sscanf(data, "%lf", &cad);
	lcd_update_cadence();
	return cad;
}

/* Cadence handler
 * Receive data in the format [seqno(2), cadence(8)]
 * cadence is a double
 */
unsigned int cadence_seqno = 0;
char *cad_string = (char *) malloc(8);
void cadence_handler(char *data) {
	unsigned int seqno = get_seqno(data);
	if (seqno > cadence_seqno) {
		cadence = get_cadence(data);
		sprintf(cad_string, "%3.4f", cadence);
		send_ack(2, cadence_seqno, cad_string);
	} else if (seqno == 0) {
		send_ack(2, 0, (char *) "ack");
		if (cadence_seqno > 0) {
			send_ack(2, cadence_seqno, cad_string);
		}
	}
}

/* Rear Light handler
 */
void rear_lights_handler(char *data) {
}

/* Front Light handler
 * Not sure what goes here yet. Probably messages like battery, change of status, etc
 */
void front_lights_handler(char *data) {
}

/* Shifter handler
 * not sure what goes here yet. Probably messages like battery, change of status, etc
 */
void shifter_handler(char *data) {
}

/* Landing Gear handler
 * Does landing gear status need a handler?
 * Need to know whether or not landing gear is on auto or manual,
 * and whether it is down or up.
 */
void landing_gear_handler(char *data) {
}