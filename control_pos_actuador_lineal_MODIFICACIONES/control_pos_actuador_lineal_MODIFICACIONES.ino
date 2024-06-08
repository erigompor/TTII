#include <elapsedMillis.h>
elapsedMillis timeElapsed;

#include <Wire.h>                   // Incluye libreria para interfaz I2C.
#include <RTClib.h>                 // Incluye libreria para el manejo del módulo RTC.
#include <math.h>

RTC_DS3231 rtc;                     // Crea objeto del tipo RTC_DS3231.

int RPWM = 6;
int LPWM = 5;
int opticalPin = 2;

volatile long lastDebounceTime = 0;

int Speed = 255;

#define falsepulseDelay 20          // Tiempo mínimo para evitar rebote. 
volatile int counter;               // Para contar pulsos del encoder.
volatile int prevCounter;           // Contador anterior.
int Direction;                      // -1 = retayéndose,
                                    //  0 = detenido,
                                    //  1 = extendiéndose.
               
int pos;
long prevT = 0;
float eprev = 0;
float eintegral = 0;
int dir;
// int horasDelDia[] = {5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19};   
int target;                         // Conteo total: 1207 pulsos.
// int n = 0;

float d[180];                       // Distancia necesaria de extensión del actuador.
float ang[180];
float L1 = 0.5675;
float L2 = 0.15;

void setup() {
  pinMode(RPWM, OUTPUT);
  pinMode(LPWM, OUTPUT);
  pinMode(opticalPin, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(opticalPin), count, RISING);

  Serial.begin(9600);
  
//____________________________________________________________________________________
  for(int i = 0; i < 181; i++)
  {
    //Cálculo de la distancia del actuador necesaria.
    //Serial.println("______________C. DISTANCIA______________");
    ang[i] = (i + 1);
    d[i] = distancia_act(ang[i]);

    //Serial.println(ang[i]);
    //Serial.println(d[i]);
  }
//____________________________________________________________________________________

  if (! rtc.begin()) 
  {                                                   // Si no encuentra el módulo:
    Serial.println("¡Modulo RTC no encontrado!");     // Muestra mensaje de error.
    while (1);                                        // Bucle infinito que impide la ejecución del programa.
  }
  //rtc.adjust(DateTime(2022, 12, 14, 16, 0, 0));       // Fecha de prueba.
  rtc.adjust(DateTime(__DATE__, __TIME__));         // Función que permite establecer fecha y horario.
                                                      // Al momento de la compilacion, comentar esta linea.
                                                      // y volver a subir el programa para operación normal.
  //________________________________________________________________________________

  // Al iniciar el programa por primera vez, posiciona el actuador en 0.
  Serial.println("Iniciando programa...");
  Serial.println("Actuador retrayéndose.");
  driveActuator(-1, 255);
  //delay(40000); // Espera 40 segundos.
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

  Serial.println("Hora actual:");
  Serial.println(fecha.hour());
  
  switch (fecha.hour()) 
  {
    case 1:
      // Posiciona el actuador en el origen a la 1 am.
      driveActuator(-1, 255);
      delay(40000); // Espera 40 segundos.
      delay(1000);
      driveActuator(0, 0);
      delay(1000);
      pos = 0;
      target = 0;
      Serial.print(target);
      break;
    case 2:
      target = 0;
      Serial.print(target);
      break;
    case 3:
      target = 0;
      Serial.print(target);
      break;
    case 4:
      target = 0;
      Serial.print(target);
      break;
    case 5:
      // map recibe la distancia requerida por el actuador (0:717);
      // la "mapea" para obtener los pulsos del encoder (0:1207).
      target = map(0, 0, 302, 0, 1207);
      Serial.print(target);
      break;
    case 6:
      target = map(0, 0, 302, 0, 1207);
      Serial.print(target);
      break;
    case 7:
      //target = map(0, 0, 302, 0, 1207);
      target = 0;
      Serial.print(target);
      break;
    case 8:
      //target = map(0, 0, 302, 0, 1207);
      target = 0;
      Serial.print(target);
      break;
    case 9:
      target = map(32, 0, 302, 0, 1207);
      Serial.print(target);
      break;
    case 10:
      target = map(64, 0, 302, 0, 1207);
      Serial.print(target);
      break;
    case 11:
      target = map(100, 0, 302, 0, 1207);
      Serial.print(target);
      break;
    case 12:
      target = map(136, 0, 302, 0, 1207);
      Serial.print(target);
      break;
    case 13:
      target = map(168, 0, 302, 0, 1207);
      Serial.print(target);
      break;
    case 14:
      target = map(199, 0, 302, 0, 1207);
      Serial.print(target);
      break;
    case 15:
      target = map(225, 0, 302, 0, 1207);
      Serial.print(target);
      break;
    case 16:
      target = map(244, 0, 302, 0, 1207);
      Serial.print(target);
      break;
    case 17:
      target = map(244, 0, 302, 0, 1207);
      Serial.print(target);
      break;
    case 18:
      target = map(244, 0, 302, 0, 1207);
      Serial.print(target);
      break;
    case 19:
      target = map(244, 0, 302, 0, 1207);
      Serial.print(target);
      break;
    case 20:
      target = map(244, 0, 302, 0, 1207);
      Serial.print(target);
      break;
    case 22:
      target = map(244, 0, 302, 0, 1207);
      Serial.print(target);
      break;
    case 23:
      target = map(244, 0, 302, 0, 1207);
      Serial.print(target);
      break;
    case 00:
      target = map(244, 0, 302, 0, 1207);
      Serial.print(target);
      break;
  }
  
  
  float kp = 1;         // Control proporcional, solo usa la ganancia "kp".

  long currT = millis();

  float deltaT = ((float)(currT-prevT)) / 1.0e3;
  prevT = currT;

  // Error:
  float e = pos - target;

  // Señal de control:
  float u = kp * e;
  
  // Define la velocidad del driver:
  float vel = 255;
  
  do
  {
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
  Serial.println(fecha.hour());
  Serial.println("__________________________________");

  
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

void count()
{
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

float distancia_act(float angulo)
{
  float argumento = angulo*(3.1416 / 180.00);
  float distancia_m = sqrt(L1*L1 + L2*L2 - 2.00 * L1 * L2 * cos(argumento));
  return distancia_m;
}
