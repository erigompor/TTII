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
  
int target;                         // Conteo total: 1207 pulsos.
int d_actuador;

int hora_actual = 0;

int hora_control = 0;
int minutos_control = 0;

int hora_actual_control = 0;
int vector_control[661][2] = {{700, 0},{701, 0},
{702, 0},
{703, 0},
{704, 0},
{705, 0},
{706, 0},
{707, 0},
{708, 0},
{709, 0},
{710, 0},
{711, 0},
{712, 0},
{713, 0},
{714, 0},
{715, 0},
{716, 0},
{717, 0},
{718, 0},
{719, 0},
{720, 0},
{721, 0},
{722, 0},
{723, 0},
{724, 0},
{725, 0},
{726, 0},
{727, 0},
{728, 0},
{729, 0},
{730, 0},
{731, 0},
{732, 0},
{733, 0},
{734, 0},
{735, 0},
{736, 0},
{737, 0},
{738, 0},
{739, 0},
{740, 0},
{741, 0},
{742, 0},
{743, 0},
{744, 0},
{745, 0},
{746, 0},
{747, 0},
{748, 0},
{749, 0},
{750, 0},
{751, 0},
{752, 0},
{753, 1},
{754, 1},
{755, 1},
{756, 1},
{757, 1},
{758, 2},
{759, 2},
{800, 2},
{801, 2},
{802, 4},
{803, 4},
{804, 4},
{805, 4},
{806, 4},
{807, 6},
{808, 6},
{809, 6},
{810, 6},
{811, 8},
{812, 8},
{813, 8},
{814, 8},
{815, 8},
{816, 10},
{817, 10},
{818, 10},
{819, 10},
{820, 12},
{821, 12},
{822, 12},
{823, 12},
{824, 12},
{825, 14},
{826, 14},
{827, 14},
{828, 14},
{829, 16},
{830, 16},
{831, 16},
{832, 16},
{833, 19},
{834, 19},
{835, 19},
{836, 19},
{837, 19},
{838, 21},
{839, 21},
{840, 21},
{841, 21},
{842, 23},
{843, 23},
{844, 23},
{845, 23},
{846, 23},
{847, 25},
{848, 25},
{849, 25},
{850, 25},
{851, 27},
{852, 27},
{853, 27},
{854, 27},
{855, 27},
{856, 30},
{857, 30},
{858, 30},
{859, 30},
{900, 32},
{901, 32},
{902, 32},
{903, 32},
{904, 32},
{905, 34},
{906, 34},
{907, 34},
{908, 34},
{909, 37},
{910, 37},
{911, 37},
{912, 37},
{913, 39},
{914, 39},
{915, 39},
{916, 39},
{917, 39},
{918, 42},
{919, 42},
{920, 42},
{921, 42},
{922, 44},
{923, 44},
{924, 44},
{925, 44},
{926, 44},
{927, 46},
{928, 46},
{929, 46},
{930, 46},
{931, 49},
{932, 49},
{933, 49},
{934, 49},
{935, 51},
{936, 51},
{937, 51},
{938, 51},
{939, 51},
{940, 54},
{941, 54},
{942, 54},
{943, 54},
{944, 56},
{945, 56},
{946, 56},
{947, 56},
{948, 56},
{949, 59},
{950, 59},
{951, 59},
{952, 59},
{953, 61},
{954, 61},
{955, 61},
{956, 61},
{957, 64},
{958, 64},
{959, 64},
{1000, 64},
{1001, 64},
{1002, 66},
{1003, 66},
{1004, 66},
{1005, 66},
{1006, 69},
{1007, 69},
{1008, 69},
{1009, 69},
{1010, 69},
{1011, 72},
{1012, 72},
{1013, 72},
{1014, 72},
{1015, 74},
{1016, 74},
{1017, 74},
{1018, 74},
{1019, 77},
{1020, 77},
{1021, 77},
{1022, 77},
{1023, 77},
{1024, 79},
{1025, 79},
{1026, 79},
{1027, 79},
{1028, 79},
{1029, 82},
{1030, 82},
{1031, 82},
{1032, 82},
{1033, 85},
{1034, 85},
{1035, 85},
{1036, 85},
{1037, 87},
{1038, 87},
{1039, 87},
{1040, 87},
{1041, 90},
{1042, 90},
{1043, 90},
{1044, 90},
{1045, 90},
{1046, 92},
{1047, 92},
{1048, 92},
{1049, 92},
{1050, 95},
{1051, 95},
{1052, 95},
{1053, 95},
{1054, 95},
{1055, 98},
{1056, 98},
{1057, 98},
{1058, 98},
{1059, 100},
{1100, 100},
{1101, 100},
{1102, 100},
{1103, 103},
{1104, 103},
{1105, 103},
{1106, 103},
{1107, 106},
{1108, 106},
{1109, 106},
{1110, 106},
{1111, 106},
{1112, 108},
{1113, 108},
{1114, 108},
{1115, 108},
{1116, 111},
{1117, 111},
{1118, 111},
{1119, 111},
{1120, 111},
{1121, 113},
{1122, 113},
{1123, 113},
{1124, 113},
{1125, 116},
{1126, 116},
{1127, 116},
{1128, 116},
{1129, 119},
{1130, 119},
{1131, 119},
{1132, 119},
{1133, 119},
{1134, 121},
{1135, 121},
{1136, 121},
{1137, 121},
{1138, 124},
{1139, 124},
{1140, 124},
{1141, 124},
{1142, 126},
{1143, 126},
{1144, 126},
{1145, 126},
{1146, 126},
{1147, 129},
{1148, 129},
{1149, 129},
{1150, 129},
{1151, 131},
{1152, 131},
{1153, 131},
{1154, 131},
{1155, 134},
{1156, 134},
{1157, 134},
{1158, 134},
{1159, 134},
{1200, 136},
{1201, 136},
{1202, 136},
{1203, 136},
{1204, 139},
{1205, 139},
{1206, 139},
{1207, 139},
{1208, 139},
{1209, 142},
{1210, 142},
{1211, 142},
{1212, 142},
{1213, 144},
{1214, 144},
{1215, 144},
{1216, 144},
{1217, 147},
{1218, 147},
{1219, 147},
{1220, 147},
{1221, 149},
{1222, 149},
{1223, 149},
{1224, 149},
{1225, 149},
{1226, 151},
{1227, 151},
{1228, 151},
{1229, 151},
{1230, 154},
{1231, 154},
{1232, 154},
{1233, 154},
{1234, 154},
{1235, 156},
{1236, 156},
{1237, 156},
{1238, 156},
{1239, 159},
{1240, 159},
{1241, 159},
{1242, 159},
{1243, 161},
{1244, 161},
{1245, 161},
{1246, 161},
{1247, 161},
{1248, 164},
{1249, 164},
{1250, 164},
{1251, 164},
{1252, 166},
{1253, 166},
{1254, 166},
{1255, 166},
{1256, 168},
{1257, 168},
{1258, 168},
{1259, 168},
{1300, 168},
{1301, 171},
{1302, 171},
{1303, 171},
{1304, 171},
{1305, 173},
{1306, 173},
{1307, 173},
{1308, 173},
{1309, 175},
{1310, 175},
{1311, 175},
{1312, 175},
{1313, 175},
{1314, 177},
{1315, 177},
{1316, 177},
{1317, 177},
{1318, 180},
{1319, 180},
{1320, 180},
{1321, 180},
{1322, 180},
{1323, 182},
{1324, 182},
{1325, 182},
{1326, 182},
{1327, 184},
{1328, 184},
{1329, 184},
{1330, 184},
{1331, 186},
{1332, 186},
{1333, 186},
{1334, 186},
{1335, 186},
{1336, 189},
{1337, 189},
{1338, 189},
{1339, 189},
{1340, 191},
{1341, 191},
{1342, 191},
{1343, 191},
{1344, 191},
{1345, 193},
{1346, 193},
{1347, 193},
{1348, 193},
{1349, 195},
{1350, 195},
{1351, 195},
{1352, 195},
{1353, 197},
{1354, 197},
{1355, 197},
{1356, 197},
{1357, 197},
{1358, 199},
{1359, 199},
{1400, 199},
{1401, 199},
{1402, 204},
{1403, 204},
{1404, 204},
{1405, 204},
{1406, 204},
{1407, 203},
{1408, 203},
{1409, 203},
{1410, 203},
{1411, 205},
{1412, 205},
{1413, 205},
{1414, 205},
{1415, 207},
{1416, 207},
{1417, 207},
{1418, 207},
{1419, 207},
{1420, 209},
{1421, 209},
{1422, 209},
{1423, 209},
{1424, 211},
{1425, 211},
{1426, 211},
{1427, 211},
{1428, 211},
{1429, 213},
{1430, 213},
{1431, 213},
{1432, 213},
{1433, 214},
{1434, 214},
{1435, 214},
{1436, 214},
{1437, 215},
{1438, 215},
{1439, 215},
{1440, 215},
{1441, 215},
{1442, 218},
{1443, 218},
{1444, 218},
{1445, 218},
{1446, 220},
{1447, 220},
{1448, 220},
{1449, 220},
{1450, 220},
{1451, 222},
{1452, 222},
{1453, 222},
{1454, 222},
{1455, 223},
{1456, 223},
{1457, 223},
{1458, 223},
{1459, 225},
{1500, 225},
{1501, 225},
{1502, 225},
{1503, 225},
{1504, 227},
{1505, 227},
{1506, 227},
{1507, 227},
{1508, 227},
{1509, 228},
{1510, 228},
{1511, 228},
{1512, 228},
{1513, 230},
{1514, 230},
{1515, 230},
{1516, 230},
{1517, 231},
{1518, 231},
{1519, 231},
{1520, 231},
{1521, 231},
{1522, 233},
{1523, 233},
{1524, 233},
{1525, 233},
{1526, 234},
{1527, 234},
{1528, 234},
{1529, 234},
{1530, 234},
{1531, 236},
{1532, 236},
{1533, 236},
{1534, 236},
{1535, 237},
{1536, 237},
{1537, 237},
{1538, 237},
{1539, 239},
{1540, 239},
{1541, 239},
{1542, 239},
{1543, 239},
{1544, 240},
{1545, 240},
{1546, 240},
{1547, 240},
{1548, 241},
{1549, 241},
{1550, 241},
{1551, 241},
{1552, 241},
{1553, 243},
{1554, 243},
{1555, 243},
{1556, 243},
{1557, 244},
{1558, 244},
{1559, 244},
{1600, 244},
{1601, 244},
{1602, 244},
{1603, 244},
{1604, 244},
{1605, 244},
{1606, 244},
{1607, 244},
{1608, 244},
{1609, 244},
{1610, 244},
{1611, 244},
{1612, 244},
{1613, 244},
{1614, 244},
{1615, 244},
{1616, 244},
{1617, 244},
{1618, 244},
{1619, 244},
{1620, 244},
{1621, 244},
{1622, 244},
{1623, 244},
{1624, 244},
{1625, 244},
{1626, 244},
{1627, 244},
{1628, 244},
{1629, 244},
{1630, 244},
{1631, 244},
{1632, 244},
{1633, 244},
{1634, 244},
{1635, 244},
{1636, 244},
{1637, 244},
{1638, 244},
{1639, 244},
{1640, 244},
{1641, 244},
{1642, 244},
{1643, 244},
{1644, 244},
{1645, 244},
{1646, 244},
{1647, 244},
{1648, 244},
{1649, 244},
{1650, 244},
{1651, 244},
{1652, 244},
{1653, 244},
{1654, 244},
{1655, 244},
{1656, 244},
{1657, 244},
{1658, 244},
{1659, 244},
{1700, 244},
{1701, 244},
{1702, 244},
{1703, 244},
{1704, 244},
{1705, 244},
{1706, 244},
{1707, 244},
{1708, 244},
{1709, 244},
{1710, 244},
{1711, 244},
{1712, 244},
{1713, 244},
{1714, 244},
{1715, 244},
{1716, 244},
{1717, 244},
{1718, 244},
{1719, 244},
{1720, 244},
{1721, 244},
{1722, 244},
{1723, 244},
{1724, 244},
{1725, 244},
{1726, 244},
{1727, 244},
{1728, 244},
{1729, 244},
{1730, 244},
{1731, 244},
{1732, 244},
{1733, 244},
{1734, 244},
{1735, 244},
{1736, 244},
{1737, 244},
{1738, 244},
{1739, 244},
{1740, 244},
{1741, 244},
{1742, 244},
{1743, 244},
{1744, 244},
{1745, 244},
{1746, 244},
{1747, 244},
{1748, 244},
{1749, 244},
{1750, 244},
{1751, 244},
{1752, 244},
{1753, 244},
{1754, 244},
{1755, 244},
{1756, 244},
{1757, 244},
{1758, 244},
{1759, 244},
{1800, 244},
};

void setup() {
  pinMode(RPWM, OUTPUT);
  pinMode(LPWM, OUTPUT);
  pinMode(opticalPin, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(opticalPin), count, RISING);

  Serial.begin(9600);

// CONFIGURACIÓN DEL MÓDULO RTC DS3231:  
//________________________________________________________________________________________________________________________________
  if (! rtc.begin()) 
  {                                                   // Si no encuentra el módulo,
    Serial.println("¡Modulo RTC no encontrado!");     // muestra mensaje de error.
    while (1);                                        // Ciclo que impide la ejecución del programa.
  }
  //rtc.adjust(DateTime(2022, 12, 16, 7, 51, 0));     // Fecha de prueba.
  rtc.adjust(DateTime(__DATE__, __TIME__));           // Función que permite establecer fecha y horario.
                                                      // Al momento de la compilacion, comentar esta linea.
                                                      // y volver a subir el programa para operación normal.
//________________________________________________________________________________________________________________________________

  // Al iniciar el programa por primera vez, posiciona el actuador en 0.
  Serial.println("Iniciando programa...");
  Serial.println("Actuador retrayendose.");
  driveActuator(-1, 255);
  delay(40000);                                         // Espera 40 segundos.
  driveActuator(0, 0);
  delay(1000);
  pos = 0;
  Serial.println("Actuador colocado en el origen.");
}

void loop() {
  Serial.println("______________INICIO______________");
  DateTime fecha = rtc.now();                           // Crea objeto DateTime "fecha" y guarda en él la fecha y hora actuales. 
  Serial.println("Fecha y hora del dia:");
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

  hora_actual = fecha.hour();
  minutos_control = fecha.minute();
  Serial.println("Minutos actuales:");
  Serial.println(minutos_control);

  if(hora_actual == 0)
  {
    hora_control = 0;
    Serial.println("Media noche, regresando a inicio...");
    Serial.println("Actuador retrayéndose.");
    driveActuator(-1, 255);
    delay(40000);   // Espera 40 segundos.
    driveActuator(0, 0);
    delay(1000);
    pos = 0;
    Serial.println("Actuador colocado en el origen.");
    Serial.println("Son las 0 horas.");
  }

  if(hora_actual > 0 && hora_actual < 10)
  {
    hora_control = hora_actual * 100;
    Serial.println("Son entre las 0 y las 10.");
  }
  else
  {
    hora_control = hora_actual * 100;
    //Serial.println("Son más de las 18 y menos de las 8.");
  }

  hora_actual_control = hora_control + minutos_control;
  Serial.println(hora_actual_control);


  if(hora_actual_control > 0 && hora_actual_control < 745)
  {
    d_actuador = 0;
    driveActuator(-1, 255);
    delay(40000);   // Espera 40 segundos.
    driveActuator(0, 0);
    delay(1000);
    pos = 0;
  }
  else
  {
    if(hora_actual_control > 1800)
    {
      d_actuador = 244;
      Serial.println("Debe colocarse en la distancia maxima.");
    }
    else
    {
      for(int i = 0; i < 662; i++)
      {
        //Serial.println(vector_control[i][0]);
        if(hora_actual_control == vector_control[i][0])
        {
          
          d_actuador = vector_control[i][1];
          //Serial.println("Sí lo encontró!!! (otra hora que no sea después de las 18 o antes de las 7)");
          Serial.println("vector_control");
          Serial.println(vector_control[i][0]);
          Serial.println("d_actuador");
          Serial.println(d_actuador);
          break;
        }
      }
    }
  }

  target = map(d_actuador, 0, 302, 0, 1207);
  //Serial.println(target);
  
  int kp = 1;         // Control proporcional, solo usa la ganancia "kp".

  long currT = millis();

  float deltaT = ((float)(currT-prevT)) / 1.0e3;
  prevT = currT;

  // Error:
  
  int e = target - pos;

  // Señal de control:
  int u = kp * e;
  
  // Define la velocidad del driver:
  int vel = 255;

  int U_pwm = map(u, 0, 1207, 0, 255);
  int pwr = fabs(U_pwm);

  if(pwr > 255)
  {
    pwr = 255;
  }

  if(pwr < 50)
  {
    pwr = 50;
  }
  
  do
  {
    if(pos > target)
    {
      dir = -1;
      Serial.println("pos > target");
      Serial.println(pos);
      Serial.println(target);
    }
    else if(target > pos)
    {
      dir = 1;
      Serial.println("target > pos");
      Serial.println(pos);
      Serial.println(target);
    }
    else if(pos == target)
    {
      Serial.println("target == pos");
      Serial.println(pos);
      Serial.println(target);
      break;  
    }
    else
    {
      Serial.println("INICIO DEL ERROR, ESTA IMPRIMIENDO:");
      Serial.println("pos");
      Serial.println(pos); 
      Serial.println("target");
      Serial.println(target);
      Serial.println("ERROR EN EL ENCODER");
    }
    // OJO CON LA SEÑAL DE CONTROL!!!!!!!!!!!!!!!!!
    // CON VEL, PWR = 255, HAY QUE USAR U
    driveActuator(dir, pwr);
    Serial.println(target);
    Serial.println(pos);
  }
  while(pos != target);
  
  driveActuator(0, 0);
  Serial.println("Llego al objetivo (target, pulsos):");
  Serial.println(target);
  Serial.println("Hora actual control:");
  Serial.println(hora_actual_control);
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
