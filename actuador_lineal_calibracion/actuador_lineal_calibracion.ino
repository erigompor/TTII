#include <elapsedMillis.h>
elapsedMillis timeElapsed;

int RPWM = 6;
int LPWM = 5;
int opticalPin = 2;

volatile long lastDebounceTime = 0;

int Speed = 255;

#define falsepulseDelay 20
volatile int counter;
volatile int prevCounter;
int Direction; // -1 = retayéndose
               //  0 = detenido
               //  1 = extendiéndose
               
int extensionCount;
int retractionCount;
int pulseTotal;

void setup(){
  pinMode(RPWM, OUTPUT);
  pinMode(LPWM, OUTPUT);
  pinMode(opticalPin, INPUT_PULLUP);
  counter = 0;
  prevCounter =0;
  extensionCount = 0;
  retractionCount = 0;
  pulseTotal = 0;

  attachInterrupt(digitalPinToInterrupt(opticalPin), count, RISING);

  Serial.begin(9600);
  Serial.println("Inicializando calibración");
  Serial.println("Actuador retrayéndose...");
  Direction = -1;
  moveTillLimit(Direction, 255);
  Serial.println("Actuador totalmente retraido");
  delay(1000);

  Serial.print("\t\t\t\tActuator ");
  Serial.print(1);

  Direction = 1;
  moveTillLimit(Direction, 255);
  Serial.print("\nConteo de extensión:");
  extensionCount = counter;
  Serial.print("\t\t"); 
  Serial.print(extensionCount);
  Serial.print("\t\t\t"); 

  delay(1000);

  Direction = -1;
  moveTillLimit(Direction, 255);
  Serial.print("\nConteo de retracción:");
  retractionCount = counter;
  Serial.print("\t\t"); 
  Serial.print(abs(retractionCount));
  Serial.print("\t\t\t"); 

  Serial.print("\n");

  Serial.print("\nActuador ");
  Serial.print(1);
  Serial.print(" Pulsos promedio: ");
  pulseTotal = (extensionCount + abs(retractionCount))/2; 
  Serial.print(pulseTotal);
}

void loop() { 
}

void moveTillLimit(int Direction, int Speed){
  //this function moves the actuator to one of its limits
  
  counter = 0; //reset counter variables
  prevCounter = 0;

  do {
    prevCounter = counter;
    timeElapsed = 0;
    while(timeElapsed < 200){ //keep moving until counter remains the same for a short duration of time
      driveActuator(Direction, Speed);
    }
  } while(compareCounter(prevCounter, counter)); //loop until all counts remain the same
}

bool compareCounter(volatile int prevCounter, volatile int counter){
  //compares two arrays and returns false when every element of one array is the same as its corresponding indexed element in the other array
  bool areUnequal = true;
  if(prevCounter == counter){
    areUnequal = false;
  } 
  else{ //if even one pair of elements are unequal the entire function returns true
    areUnequal = true;
  }
  
  return areUnequal;
}

void driveActuator(int Direction, int Speed){
  int rightPWM = RPWM;
  int leftPWM = LPWM;

  switch(Direction){
  case 1: //extension
    analogWrite(rightPWM, Speed);
    analogWrite(leftPWM, 0);
  break;

  case 0: //stopping
    analogWrite(rightPWM, 0);
    analogWrite(leftPWM, 0);
  break;

  case -1: //retraction
    analogWrite(rightPWM, 0);
    analogWrite(leftPWM, Speed);
  break;
  }
}

void count(){
  //This interrupt function increments a counter corresponding to changes in the optical pin status
  if ((millis() - lastDebounceTime) > falsepulseDelay) { //reduce noise by debouncing IR signal 
    lastDebounceTime = millis();
    if(Direction == 1){
      counter++;
    }
    if(Direction == -1){
      counter--;
    }
  }
}
