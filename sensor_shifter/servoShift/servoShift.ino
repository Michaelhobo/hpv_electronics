#include <Servo.h>
//INSERTARRAYHERE
uint8_t listOfGearPositions[5] = {0, 20, 40, 60, 80};
// pins and servo setup
const int servo1 = 5;      // the number of the servo pin
const int pushButtonUp = 3;
const int pushButtonDown = 4;
Servo myservo;

//will change
int buttonUpState = 0;
int buttonDownState = 0;
int currentGear = 0;

void setup() { 
  //start serial connection
  Serial.begin(9600);
  myservo.attach(servo1);
  pinMode(pushButtonUp, INPUT);
  pinMode(pushButtonDown, INPUT);
  digitalWrite(pushButtonUp, LOW);
  digitalWrite(pushButtonDown, LOW);
}

void loop(){
  Serial.print("Current = ");
  Serial.print(currentGear);
  buttonUpState = digitalRead(pushButtonUp);
  buttonDownState = digitalRead(pushButtonDown);
  if (buttonUpState == HIGH) {
    if (currentGear + 1 > 4) {
      currentGear = 3 ;
    }
    Serial.print("Goal = ");
    currentGear = currentGear + 1;
    Serial.print(currentGear);
    myservo.write(listOfGearPositions[currentGear]);
    digitalWrite(pushButtonUp, LOW);
    delay(1000);
  } else if (buttonDownState == HIGH) {
    if (currentGear - 1 < 0) {
      currentGear = 1;
    }
    Serial.print("Goal = ");
    currentGear = currentGear - 1;
    Serial.print(currentGear);
    myservo.write(listOfGearPositions[currentGear]);
    digitalWrite(pushButtonDown, LOW);
    delay(1000);
  }
}
