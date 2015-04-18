int switchIn = 7;
int secondSwitchIn = 12;

int light1 = 4;
int light2 = 5;


void setup() {
  // put your setup code here, to run once:
  pinMode(switchIn, INPUT);
  pinMode(secondSwitchIn, INPUT);
  pinMode(light1, OUTPUT);
  pinMode(light2, OUTPUT);
}

void loop() {
  delay(10);
  digitalWrite(light1,digitalRead(switchIn));
  digitalWrite(light2,digitalRead(secondSwitchIn));  
}
