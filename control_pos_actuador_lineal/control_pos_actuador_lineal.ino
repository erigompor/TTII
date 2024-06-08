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

  //________________________________________________________________________________
  if (! rtc.begin()) 
  {       // Si no encuentra el módulo:
    Serial.println("¡Modulo RTC no encontrado!");  // Muestra mensaje de error.
    while (1);         // Bucle infinito que impide la ejecución del programa.
  }
  //DateTime(2022, 10, 6, 12, 0, 0) // Fecha de prueba.
  rtc.adjust(DateTime(__DATE__, __TIME__));  // Función que permite establecer fecha y horario.
            // Al momento de la compilacion, comentar esta linea.
            // y volver a subir el programa para operación normal.
  //________________________________________________________________________________
  
  Serial.println("Iniciando programa...");
  Serial.println("Actuador retrayéndose.");
  driveActuator(-1, 255);
  delay(40000);
  driveActuator(0, 0);
  delay(1000);
  pos = 0;
  Serial.println("Actuador colocado en el origen.");
}

void loop() {
  Serial.println("______________INICIO______________");
  DateTime fecha = rtc.now(); // Crea objeto DateTime "fecha" y guarda en él la fecha y hora actuales. 
  Serial.println("Fecha y hora del día:");
  Serial.print(fecha.day());
  Serial.print("/");
  Serial.print(fecha.month());
  Serial.print("/");
  Serial.print(fecha.year());
  Serial.print("/");
  Serial.print(fecha.hour());
  Serial.print(":");
  Serial.print(fecha.minute());
  Serial.print(":");
  Serial.println(fecha.second());
  delay(5000);

  switch (horasDelDia[n]) 
  {
    case 5:
      target = map(60, -80, 80, 0, 1207);
      Serial.print(target);
      break;
    case 6:
      target = map(70, -80, 80, 0, 1207);
      Serial.print(target);
      break;
    case 7:
      target = map(72, -80, 80, 0, 1207);
      Serial.print(target);
      break;
    case 8:
      target = map(75, -80, 80, 0, 1207);
      Serial.print(target);
      break;
    case 9:
      target = map(77, -80, 80, 0, 1207);
      Serial.print(target);
      break;
    case 10:
      target = map(75, -80, 80, 0, 1207);
      Serial.print(target);
      break;
    case 11:
      target = map(72, -80, 80, 0, 1207);
      Serial.print(target);
      break;
    case 12:
      target = map(0, -80, 80, 0, 1207);
      Serial.print(target);
      break;
    case 13:
      target = map(-70, -80, 80, 0, 1207);
      Serial.print(target);
      break;
    case 14:
      target = map(-77, -80, 80, 0, 1207);
      Serial.print(target);
      break;
    case 15:
      target = map(-75, -80, 80, 0, 1207);
      Serial.print(target);
      break;
    case 16:
      target = map(-72, -80, 80, 0, 1207);
      Serial.print(target);
      break;
    case 17:
      target = map(-70, -80, 80, 0, 1207);
      Serial.print(target);
      break;
    case 18:
      target = map(-65, -80, 80, 0, 1207);
      Serial.print(target);
      break;
    case 19:
      target = map(-62, -80, 80, 0, 1207);
      Serial.print(target);
      break;
  }
  
  
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
    Serial.println(pos);
    Serial.println(target);
    Serial.println("-----------");
  }
  while(pos != target);
  
  driveActuator(0, 0);
  Serial.println("Llegó al objetivo (case):");
  Serial.println(horasDelDia[n]);
  Serial.println("__________________________________");

  if (n < 15)
  {
    n++;
  }
  else
  {
    n = 0;
  }
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
