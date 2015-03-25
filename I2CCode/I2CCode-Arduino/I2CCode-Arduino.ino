#include <Wire.h>;

const int TRANSFER_SIZE = 4;
char data[TRANSFER_SIZE];
void setup() {
  Wire.begin(3);
  Wire.onReceive(receiveEvent);
  Serial.begin(9600);
  pinMode(10, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:

}


char key[] = "ABCDE"; //Sensor keys

void sendToSensor(char sensor, char* data){
}

//When the Arduino receives data from master, gets called
void receiveEvent(int dataSize){
  int counter= 0;
  digitalWrite(10, LOW);
   while (Wire.available()&& counter < TRANSFER_SIZE){ //Reads data
      data[counter] =  Wire.read(); 
      counter++;
   }
   if (data[0]== 'S'){ //Case for sending data to a different sensor
     sendToSensor(data[1], data);
   }
   else if (data[0] == 'R')//Case for returning a requestd data to Master
   {
       if (data[1] == 'S'){//Speed
            Wire.write(10);
       }
      else if (data[1]== 'C'){
            Wire.write(20);
      }
      digitalWrite(10, HIGH);
    }
}
