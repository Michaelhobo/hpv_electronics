int pinList [8] = {3,4,5,6,14,15,16,17};
int num = 0;
void setup(){
  for (int i = 0; i < 8; i++){
    pinMode(pinList[i], OUTPUT);
  }
}

void loop(){
  digitalWrite(pinList[num], HIGH);
  delay(400);
  digitalWrite(pinList[num], LOW);
  num = (num >= 7)? 0: num+1;
}
