#include <elapsedMillis.h>
elapsedMillis timeElapsed;

#include <Wire.h>     // Incluye libreria para interfaz I2C.
#include <RTClib.h>   // Incluye libreria para el manejo del módulo RTC.

RTC_DS3231 rtc;       // Crea objeto del tipo RTC_DS3231.

int RPWM = 6;
int LPWM = 5;
int opticalPin = 2;

volatile long lastDebounceTime = 0;

int Speed = 255;

#define falsepulseDelay 20  // Tiempo mínimo para evitar rebote. 
volatile int counter;       // Para contar pulsos del encoder.
volatile int prevCounter;   // Contador anterior.
int Direction; // -1 = retayéndose,
               //  0 = detenido,
               //  1 = extendiéndose.
               
int pos;
long prevT = 0;
float eprev = 0;
float eintegral = 0;
int dir;
int horasDelDia[] = {5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19};         
int target; // Conteo total: 1207 pulsos.
int n = 0;

void setup() {
  pinMode(RPWM, OUTPUT);
  pinMode(LPWM, OUTPUT);
  pinMode(opticalPin, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(opticalPin), count, RISING);

  Serial.begin(9600);

  
  driveActuator(-1, 255);
  delay(40000);
  driveActuator(0, 0);
  delay(1000);
  pos = 0;
}

void loop() {
    
  int target = 250;
  
  float kp = 1;

  long currT = millis();

  float deltaT = ((float)(currT-prevT)) / 1.0e3;
  prevT = currT;

  // Error:
  int e = pos - target;

  // Señal de control:
  float u = kp * e;
  
  // Define la velocidad del driver:
  float vel = 255;

  int U_pwm = map(u, 0, 1207, 0, 255);
  int pwr = fabs(U_pwm);

  if(pwr > 255)
  {
    pwr = 255;
  }

  if(pwr < 12)
  {
    pwr = 12;
  }
  
  do{
    if(pos > target)
    {
      dir = -1;
    }
    else
    {
      dir = 1;
    }
    driveActuator(dir, vel);
    Serial.print(target);
    Serial.print(" ");
    Serial.println(pos);
    Serial.println();
  }
  while(pos != target);
  
  driveActuator(0, 0);

  
}

void driveActuator(int Direction, int Speed){
  int rightPWM = RPWM;
  int leftPWM = LPWM;

  switch(Direction){
  case 1: // Extensión.
    analogWrite(rightPWM, Speed);
    analogWrite(leftPWM, 0);
  break;

  case 0: // Sin dirección, detenido.
    analogWrite(rightPWM, 0);
    analogWrite(leftPWM, 0);
  break;

  case -1: // Retracción.
    analogWrite(rightPWM, 0);
    analogWrite(leftPWM, Speed);
  break;
  }
}

void count(){
  if ((millis() - lastDebounceTime) > falsepulseDelay) // Reduce el ruido evitando el rebote.
  { 
    lastDebounceTime = millis();
    if(dir == 1){
      pos++;
    }
    if(dir == -1){
      pos--;
    }
  }
}
