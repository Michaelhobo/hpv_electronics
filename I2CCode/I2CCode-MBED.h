#include "mbed.h";

I2c i2c (p9, p10); //Temporary for now; select it later.

uint8_t ArduinoAddress = 0x01000000;

const int TRANSFER_SIZE = 5;
char data[TRANSFER_SIZE];





//Codes to tell which sensor to send the data to.
char codes[5] = "ABCDE";
const int sensor_1 = 0;
const int sensor_2 = 1;
const int sensor_3 = 2;
const int sensor_4 = 3;
const int sensor_5 = 4;


char* readData(){
	i2c.write(ArduinoAddress, data, TRANSFER_SIZE-1);
	i2c.read(ArduinoAddress, data, TRANSFER_SIZE);
	return data;
}

void sendDataToArduino(sendData){
	i2c.write(ArduinoAddress, sendData, TRANSFER_SIZE);
}

InterruptIn buttonTest(p5);


//Function that determines what sensor to send the information.
void sendToSensor(sensor, info){
	data[0] = codes[sensor];
	for (int i = 0; i < 2; i++){
		data[i+1]= info[i];
	}
	sendDataToArduino(data);
}

void test(){
	sendDataToArduino("AO");
}

char* requestData(char dataType){
	char req[2];
	req[0] = "R";
	req[1] = dataType;
	sendDataToArduino(req);
}


buttonTest.rise(&test);

//Triggered when data is written from slave to master
InterruptIn receivedSignal(p8);
receivedSignal.rise(&readData);