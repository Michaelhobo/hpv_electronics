#include "mbed.h"
#include <limits.h>
#include <stdio.h>
#include "xbee.h"
#include "nRF24L01P.h"
#include "../constants.h"

#define XBEE_SEND_INTERVAL 2
#define PC_SEND_INTERVAL 1
#define MY_ADDR 0

DigitalOut led1(LED1);
DigitalOut led2(LED2);
DigitalOut led3(LED3);
DigitalOut led4(LED4);
Serial pc(USBTX, USBRX); // tx, rx

/* Button declarations. */
DigitalIn btn1(p15); //shift up
DigitalIn btn2(p16); //shift down
DigitalIn btn3(p17); //turn right
DigitalIn btn4(p18); //turn left
InterruptIn btn_change(p27);
uint8_t button_state = 0; // 0b0000[btn{4,3,2,1}] we're bit packing this


xbee xbee(p13, p14, p12);
nRF24L01P rf24(p5, p6, p7, p8, p9, p10);

Ticker events;
Timeout timeout;
double speed = 0.0; //calculated speed
double cadence = 0.0;
char receive_buffer[RF24_TRANSFER_SIZE];
char speed_buffer[RF24_TRANSFER_SIZE];
char send_buffer[RF24_TRANSFER_SIZE];
const char *sensor_names[255] = {0};
void (*sensor_handlers[255])(char *data);
uint8_t sensor_states[255];

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

/* Send a value to the shifter. */
void send_shifter() {
	
}




/* Send a value to turn signal. */
void send_rear(const char *val) {
	send_sensor_name("rear_lights", val);
}

/* Sends value to front lights */
void send_front(const char *val){
	send_sensor_name("front_lights", val);
}

/* On button change, read all button data pins
 * Compare state with previous state to see which button changed.
 */
void button_change() {
	NVIC_DisableIRQ(UART0_IRQn);
	if (button_state & 0b00000001 != btn1) { //shift up
		if (btn1) {
			gear += 1;
		}
		NVIC_EnableIRQ(UART0_IRQn);
		if (btn1) {
			send_shifter();
		}
	} else if ((button_state & 0b00000010) >> 1 != btn2) { //shift down
		if (btn2) {
			gear -= 1;
		}
		NVIC_EnableIRQ(UART0_IRQn);
		if (btn2) {
			send_shifter();
		}
	}	else if ((button_state & 0b00000100) >> 2 != btn3) { //turn right
		NVIC_EnableIRQ(UART0_IRQn);
		if (btn3) {
			send_rear("BR");
		} else {
			send_rear("FR");
		}
	}	else if ((button_state & 0b00001000) >> 3 != btn4) { //turn left
		NVIC_EnableIRQ(UART0_IRQn);
		if (btn4) {
			send_rear("BL");
		} else {
			send_rear("FL");
		}
	}
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
	sensor_states[id] = (uint8_t) DISCONNECTED;
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
	rf24.enableAutoAcknowledge(NRF24L01P_PIPE_P2);
	rf24.enableAutoAcknowledge(NRF24L01P_PIPE_P3);
	rf24.enableAutoAcknowledge(NRF24L01P_PIPE_P4);
	rf24.enableAutoAcknowledge(NRF24L01P_PIPE_P5);
	rf24.setRxAddress(0x00F0F0F0F1, paddr_size, NRF24L01P_PIPE_P0);
	rf24.setRxAddress(0x00F0F0F0F2, paddr_size, NRF24L01P_PIPE_P2);
	rf24.setRxAddress(0x00F0F0F0F3, paddr_size, NRF24L01P_PIPE_P3);
	rf24.setRxAddress(0x00F0F0F0F4, paddr_size, NRF24L01P_PIPE_P4);
	rf24.setRxAddress(0x00F0F0F0F5, paddr_size, NRF24L01P_PIPE_P5);
	rf24.setReceiveMode();
	rf24.enable();
	pc.printf("MASTER: rf24 init finished\r\n");
}

/* Initialize Buttons. */
void button_init() {
	btn_change.rise(&button_change);
	btn_change.fall(&button_change);
}

/* Initialize everything necessary for the scripts. */
void init() {
	pc.printf("init");
	telemetry_init();
	rf24_init();
	button_init();
	//lcd.putc('0');
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
/* Process RF24 input and send it to the correct handler. */
void process_rf_input() {
	uint8_t src_addr = (uint8_t) receive_buffer[0];
	led2 = 1;
	if (sensor_handlers[src_addr]){
		sensor_handlers[src_addr](receive_buffer + 1);
	}
}

/* Process a connection. */
void process_connection() {
	uint8_t src_addr = (uint8_t) receive_buffer[0];
	if (src_addr >= 0 && src_addr <= 5) {
		//pc.printf("src_addr in range.\r\n");
		if (sensor_states[src_addr] == DISCONNECTED) {
			if (strstr(receive_buffer + 1, "connect") == (receive_buffer + 1)) {
				//led4 = 1;
				pc.printf("sensor_states[%d] = %d\r\n", src_addr, sensor_states[src_addr]);
				sensor_states[src_addr] = (uint8_t) CONNECTED;
			}
		} else if (sensor_states[src_addr] == CONNECTED) {
			pc.printf("CONNECTED");
			process_rf_input();
		}
	} else {
		pc.printf("src_addr out of range\r\n");
	}
}
/* Main sending loop. */
int main() {
	init();
	pc.printf("Starting Logging.\n");
	//events.attach(&send_xbee_speed, XBEE_SEND_INTERVAL);
	//events.attach(&show_usbterm_speed, PC_SEND_INTERVAL);
	while(1) {
		if (rf24.readable(NRF24L01P_PIPE_P0)) {
			led1 = 1;
			pc.printf("rf24 connection.\r\n");
			rf24.read(NRF24L01P_PIPE_P0, receive_buffer, RF24_TRANSFER_SIZE);
			pc.printf(receive_buffer);
			pc.printf("\r\n");
			process_connection();
		} else if (rf24.readable(NRF24L01P_PIPE_P2)) {
			led1 = 1;
			pc.printf("rf24 pipe2.\r\n");
			rf24.read(NRF24L01P_PIPE_P2, receive_buffer, RF24_TRANSFER_SIZE);
			process_rf_input();
		} else if (rf24.readable(NRF24L01P_PIPE_P3)) {
			led3 = 1;
			pc.printf("rf24 pipe3.\r\n");
			rf24.read(NRF24L01P_PIPE_P3, receive_buffer, RF24_TRANSFER_SIZE);
			process_rf_input();
		} else if (rf24.readable(NRF24L01P_PIPE_P4)) {
			led1 = 1;
			pc.printf("rf24 pipe4.\r\n");
			rf24.read(NRF24L01P_PIPE_P4, receive_buffer, RF24_TRANSFER_SIZE);
			process_rf_input();
		} else if (rf24.readable(NRF24L01P_PIPE_P5)) {
			led1 = 1;
			pc.printf("rf24 pipe5.\r\n");
			rf24.read(NRF24L01P_PIPE_P5, receive_buffer, RF24_TRANSFER_SIZE);
			process_rf_input();
		}
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
	pc.printf("speed\r\n");
	/* if the speed is known to decrease significantly, send a signal for break lights
	if (speed < lastSpeed - 3){
		send_sensor_name("rear_lights","NB");
	}
*/ 
/*	unsigned int seqno = get_seqno(data);
	if (seqno > speed_seqno) {
		speed = get_speed(data); //should we update anything?
		sprintf(spd_string, "%3.4f", speed);
		send_ack(1, speed_seqno, spd_string);
	} else if (seqno == 0) {
		send_ack(1, 0, (char *) "ack");
		if (speed_seqno > 0) {
			send_ack(1, speed_seqno, spd_string);
		}
	}*/
}

/* Get cadence from data packet. */
double get_cadence(char *data) {
	double cad;
	sscanf(data, "%lf", &cad);
	return cad;
	
}

/* Cadence handler
 * Receive data in the format [seqno(2), cadence(8)]
 * cadence is a double
 */
unsigned int cadence_seqno = 0;
char *cad_string = (char *) malloc(8);
void cadence_handler(char *data) {
	pc.printf("cadence\r\n");
/*	unsigned int seqno = get_seqno(data);
	if (seqno > cadence_seqno) {
		cadence = get_cadence(data);
		sprintf(cad_string, "%3.4f", cadence);
		send_ack(2, cadence_seqno, cad_string);
	} else if (seqno == 0) {
		send_ack(2, 0, (char *) "ack");
		if (cadence_seqno > 0) {
			send_ack(2, cadence_seqno, cad_string);
		}
	}*/
}

/* Rear Light handler
 */
void rear_lights_handler(char *data) {
	pc.printf("rear_lights\r\n");
}

/* Front Light handler
 * Not sure what goes here yet. Probably messages like battery, change of status, etc
 */
void front_lights_handler(char *data) {
	pc.printf("front_lights\r\n");
}

/* Shifter handler
 * not sure what goes here yet. Probably messages like battery, change of status, etc
 */
void shifter_handler(char *data) {
}

