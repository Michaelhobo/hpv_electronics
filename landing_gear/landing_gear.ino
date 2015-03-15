int inPin_1 = 5;
int inPin_2 = 6;
int outPin_1 = 7;
int outPin_2 = 8;
int var1 = 0;
int var2 = 0;
boolean state = true;
void setup() {
  // put your setup code here, to run once:
    pinMode(inPin_1, INPUT);
    pinMode(inPin_2, INPUT);
    pinMode(outPin_1, OUTPUT);
    pinMode(outPin_2, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  var1 = digitalRead(inPin_1);
  var2 = digitalRead(inPin_2);
  if ( var1 == HIGH && state) {
    digitalWrite(outPin_1, HIGH);
    delay(2000); 
    digitalWrite(outPin_1, LOW);    
    state = false;
  }
  else if ( var2 == HIGH && !state) {
    digitalWrite(outPin_2, HIGH);
    delay(2000); 
    digitalWrite(outPin_2, LOW);
    state = true;
  }
}
