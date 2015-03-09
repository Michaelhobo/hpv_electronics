// constants won't change. They're used here to set pin numbers:
const int servo1 = 9;      // the number of the pushbutton1 pine pushbutton2 pin

// variables will change:
int button1State = 0;         // variable for reading the pushbutton status
int button2State = 0;         // variable for reading the pushbutton status
int sensorValue = 0;  // variable to store the value coming from the sensor

int goalPosition = 350; 
int CurrentPosition = 0; 
boolean Extending = false;
boolean Retracting = false;

void setup() { 


  //start serial connection
  Serial.begin(9600);

  // initialize the pushbutton pin as an input:
  pinMode(button1Pin, INPUT);      
  // initialize the relay pin as an output:
  pinMode(relay1Pin, OUTPUT);    
  pinMode(relay2Pin, OUTPUT);    
  
  //preset the relays to LOW
  digitalWrite(relay1Pin, LOW); 
  digitalWrite(relay2Pin, LOW); 

  
}

void loop(){
  
  // read the value from the sensor:
  CurrentPosition = analogRead(sensorPin); 

  
  // print the results to the serial monitor:
  Serial.print("Current = " );                       
  Serial.print(CurrentPosition);      
  Serial.print("\t Goal = ");      
  Serial.println(goalPosition);  
  
  // read the state of the pushbutton values: CHANGE THIS BEGIN
  desiredGearNum = analogRead(desiredPin);
  // CHANGE THIS END

//CHANGE THIS /*
  if (button1State == HIGH) {     
    // set new goal position
    goalPosition = 300; 
    
    if (goalPosition > CurrentPosition) {
        Retracting = false;
        Extending = true;
        digitalWrite(relay1Pin, HIGH);  
        digitalWrite(relay2Pin, LOW);  
        Serial.println("Extending");     
    }      
    else if (goalPosition < CurrentPosition) {
        Retracting = true;
        Extending = false;
        digitalWrite(relay1Pin, LOW);  
        digitalWrite(relay2Pin, HIGH); 
        Serial.println("Retracting");         
    }  
  }
// END CHANGE THIS

  if (Extending = true && CurrentPosition > goalPosition) {
    //we have reached our goal, shut the relay off
    digitalWrite(relay1Pin, LOW); 
    boolean Extending = false; 
    Serial.println("IDLE");  
  }
  
  if (Retracting = true && CurrentPosition < goalPosition){
    //we have reached our goal, shut the relay off
    digitalWrite(relay2Pin, LOW); 
    boolean Retracting = false; 
    Serial.println("IDLE");  
  }


  
  
}
