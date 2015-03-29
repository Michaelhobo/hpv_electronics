#include <Wire.h>;

const int TRANSFER_SIZE = 4;
char data[TRANSFER_SIZE];
void setup() {
  Wire.begin(7);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(receiveRequest);
  Serial.begin(9600);
  pinMode(10, OUTPUT);
  Serial.println("setup done");
}

void loop() {
  // put your main code here, to run repeatedly:

}

void sendToSensor(char sensor, char* data){
}

//When the Arduino receives data from master, gets called
void receiveEvent(int dataSize){
  Serial.println("got smthn");
  int counter= 0;
  digitalWrite(10, LOW);
   while (Wire.available()&& counter < TRANSFER_SIZE){ //Reads data
      data[counter] =  Wire.read(); 
      Serial.print(data[counter]);
      //Serial.print("h");
      counter++;
   }
   if (data[0]== 'S'){ //Case for sending data to a different sensor
     sendToSensor(data[1], data);
   }
   else if (data[0] == 'R')//Case for returning a requestd data to Master
   {
       if (data[1] == 'S'){//Speed
            Wire.write(10);
            Serial.print("request speed");
       }
      else if (data[1]== 'C'){
            Wire.write(20);
      }
      digitalWrite(10, HIGH);
    }
}

void receiveRequest() {
    Serial.println("got request");
    data[0] = 0;
    data[1] = 1;
    data[2] = 2;
    data[3] = 3;
    Wire.write(data, TRANSFER_SIZE);
}
