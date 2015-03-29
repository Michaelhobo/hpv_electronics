#include "mbed.h"

I2C i2c (p9, p10); //Temporary for now; select it later.
InterruptIn receivedSignal(p8);
DigitalOut led1(LED1);
DigitalOut led2(LED2);
DigitalOut led3(LED3);
DigitalOut led4(LED4);
Serial pc(USBTX, USBRX);
uint8_t ArduinoAddress = 2;

const int TRANSFER_SIZE = 2;
char data[TRANSFER_SIZE];
char in[4] = {0};




//Codes to tell which sensor to send the data to.
const uint8_t sensor_1 = 0;
const uint8_t sensor_2 = 1;
const uint8_t sensor_3 = 2;
const uint8_t sensor_4 = 3;
const uint8_t sensor_5 = 4;


void readData(){
	while (!i2c.read(ArduinoAddress, data, TRANSFER_SIZE)) {
	}
}

void sendDataToArduino(char* sendData){
	//i2c.start();
	//bool fail = i2c.write(ArduinoAddress, sendData, TRANSFER_SIZE);
	in[0] = in[1] = in[2] = in[3] = 0;
	//data[0] = 0x7<<1 | 0x0;
	//i2c.write(data[0], "RS", 2);
	data[1] = 0x7 <<1 | 0x1;
	i2c.read(data[1], in, 4);
	//i2c.start();
	//i2c.write(data[1]);
	//i2c.read(data[1], in, 4);
	//in[0] = i2c.read(1);
	//in[1] = i2c.read(1);
	//in[2] = i2c.read(1);
	//in[3] = i2c.read(0);
	//i2c.stop();
	//i2c.read(data[0], in, 4);
	//i2c.stop();
	//if (!fail) led3 = !led3;
	pc.printf("%d%d%d%d", in[0],in[1],in[2],in[3]);
	if (in[1] == 1) led3 = !led3;
	led2 = !led2;
}

InterruptIn buttonTest(p5);


//Function that determines what sensor to send the information.
void sendToSensor(uint8_t sensor, char* info){
	data[0] = sensor;
	for (int i = 0; i < 2; i++){
		data[i+1]= info[i];
	}
	sendDataToArduino(data);
}

void test(){
	led1 = !led1;
	sendDataToArduino("AO");
}

void requestData(char dataType){
	char req[2];
	req[0] = 'R';
	req[1] = dataType;
	sendDataToArduino(req);
}

int main() {
	pc.printf("i2c test\r\n");
	buttonTest.rise(&test);

	//Triggered when data is written from slave to master
	receivedSignal.rise(&readData);
}
