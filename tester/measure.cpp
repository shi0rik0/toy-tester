#include "measure.h"
#include "draw.h"

#include <math.h>

const byte ARRAY_LEN = 100;

const char *STATUS_RUNNING = "Wait...";
const char *STATUS_OK = "        Done!";

// 以 interval(单位：ms) 为间隔测量 times 次，然后取平均值
float getAvgVoltage(byte port, word times, word interval) {
  float s = 0;
  for (word i = 0; i < times; ++i) {
    s += getVoltage(port, 1);
    delay(interval);
  }
  return s / times;
}

float getResistance(byte port1, byte port2, float r0) {
  float v = getAvgVoltage(port1, 10, 10);
  float r = r0 * (VCC - v) / v;
  return r;
}

void measureResistor(byte port1, byte port2) {
  // 这个阈值是计算得到的
  static const float THRESHOLD = 4.8;

  resetOtherPort(port1, port2);
  clearLCD();
  printLine(0, "Resistor");
  printLine(5, STATUS_RUNNING);
  refreshLCD();
  float r0 = R_BIG;
  setPort(port1, PortType::BIG, LOW);
  setPort(port2, PortType::READ, HIGH);
  float v = getVoltage(port1, 3);
  if (v > THRESHOLD) {
    r0 = R_SMALL;
    setPort(port1, PortType::SMALL, LOW);
  }
  float r = getResistance(port1, port2, r0);
  printValue(1, r, "Ohm");
  printLine(5, STATUS_OK);
  refreshLCD();
}

void switchToSmallResistor(byte port1, byte port2) {
  resetOtherPort(port1, port2);
  setPort(port1, PortType::SMALL, LOW);
  setPort(port2, PortType::READ, HIGH);
}

void switchToBigResistor(byte port1, byte port2) {
  resetOtherPort(port1, port2);
  setPort(port1, PortType::BIG, LOW);
  setPort(port2, PortType::READ, HIGH);
}

void discharge(byte port1, byte port2, PortType type) {
  resetOtherPort(port1, port2);
  setPort(port1, type, LOW);
  setPort(port2, type, LOW);
}

void dischargeBySmallResistor(byte port1, byte port2, word dischargeTime) {
  discharge(port1, port2, PortType::SMALL);
  delay(dischargeTime);
}

void dischargeByBigResistor(byte port1, byte port2, word dischargeTime) {
  discharge(port1, port2, PortType::BIG);
  delay(dischargeTime);
}



//// bBigR=0 B port uses 680 ohm, else 470k ohm.
//void initPNP(byte B, byte C, byte E, bool bBigR) {
//  if (!bBigR) {
//    pinMode(PORT[B][SMALL], OUTPUT);
//    pinMode(PORT[B][BIG], INPUT);
//    digitalWrite(PORT[B][SMALL], LOW);
//  } else {
//    pinMode(PORT[B][SMALL], INPUT);
//    pinMode(PORT[B][BIG], OUTPUT);
//    digitalWrite(PORT[B][HIGH], LOW);
//  }
//  pinMode(PORT[B][READ], INPUT);
//  pinMode(PORT[C][READ], OUTPUT);
//  pinMode(PORT[C][SMALL], INPUT);
//  pinMode(PORT[C][BIG], INPUT);
//  pinMode(PORT[E][READ], INPUT);
//  pinMode(PORT[E][SMALL], OUTPUT);
//  pinMode(PORT[E][BIG], INPUT);
//  digitalWrite(PORT[E][SMALL], HIGH);
//  digitalWrite(PORT[C][SMALL], LOW);
//}
//

void initNPN(byte B, byte C, byte E) {
  setPort(B, PortType::BIG, HIGH);
  setPort(C, PortType::SMALL, HIGH);
  setPort(E, PortType::READ, LOW);
}




// 返回值：true: 电压下降值可观; false: 电压基本不变
bool recordVoltages(float *vArr, byte port1, int cnt, int interval) {
  float THRESH = 0.5;
  for (int i = 0; i < cnt; ++i) {
    vArr[i] = getVoltage(port1, 1);
    delayMicroseconds(interval);
  }
  return (vArr[0] - vArr[cnt - 1] > THRESH);
}

float getTao(float *vArr, int pointCnt, int interval) {
  const float THRESH = 0.1; // 电压为0的阈值
  float tao = -1;
  float sumTao = 0;
  int validCnt = 0;
  const float validThresh = 0.01;
  // V = Vcc * e ** (-t / tao)
  for (int i = 1; i < pointCnt; ++i) { // 计算tao平均值
    float t = i * interval * 1e-6;
    float logDiff = log(VCC) - log(vArr[i]);
    if (abs(logDiff) > validThresh) { // 如果电压接近0 就不计入
      sumTao += (t / logDiff);
      ++validCnt;
      ;
    }
  }

  return sumTao / (float)(validCnt);
}

float getCapacitance(byte port1, byte port2, float r0) {
  float vArr[ARRAY_LEN];
  int pointCnt = 50;  // 取点数目
  int interval = 500; // 测量间隔(us)

  if (!recordVoltages(vArr, port1, pointCnt, interval)) {
    return -1;
  }
  float tao = getTao(vArr, pointCnt, interval);
  return tao / r0; // tao = RC
}




// 这个函数丑了点，一会儿再改
float getCapacitance2(byte port1, byte port2) {
  static const word DISCHARGE_TIME_BIG = 100;
  static const word DISCHARGE_TIME_SMALL = 100;

  const word dischargeTime = 1000; // 这个时间怎么确定的？
  lcd.clearDisplay();
  //  printType("Capacitor");
  //  printDebug("Small");
  float cap;
  switchToBigResistor(port1, port2); // 开始充电
  cap = getCapacitance(port1, port2, R_BIG);
  dischargeByBigResistor(port1, port2, dischargeTime); //  电容放电

  if (cap < 0) {
    //    printDebug("Big");
    switchToSmallResistor(port1, port2); // 开始充电
    cap = getCapacitance(port1, port2, R_SMALL);
    dischargeBySmallResistor(port1, port2, dischargeTime); //  电容放电
  }
  return cap;
}

void measureCapacitor(byte port1, byte port2) {
  float cap = getCapacitance2(port1, port2);

  printValue(1, cap, "F");
  printLine(5, "        Done!");
  refreshLCD();
}

//void measurePNPBJT(byte C, byte B, byte E) {
//  lcd.clear();
//  printType("PNP BJT");
//  printStatus("Trying as PNP");
//  initPNP(B, C, E, 0);
//  float vb = getVoltage(B);
//  float vc = getVoltage(C);
//  float ic = vc / R_LOW;
//  float ib = vb / (R_LOW + R_SMALL);
//  if (vb < 0.01 || vb < vc) {
//    printStatus("Attaching 470k ohm to B");
//    initPNP(B, C, E, 1);
//    vb = getVoltage(B);
//    vc = getVoltage(C);
//    ic = vc / R_LOW;
//    ib = vb / (R_LOW + R_BIG);
//  }
//  float ve = getVoltage(E);
//  float ie = (VCC - ve) / (R_HIGH + R_SMALL);
//  float beta = ic / ib;
//  printValue(beta, "beta");
//  Serial.println(beta);
//  Serial.println(vb);
//  Serial.println(vc);
//  Serial.println(ve);
//  Serial.println(ib * 1000);
//  Serial.println(ic * 1000);
//  Serial.println(ie * 1000);
//}

void measureNPNBJT(byte C, byte B, byte E) {
  clearLCD();
  printLine(0, "NPN BJT");
  printLine(5, STATUS_RUNNING);
  initNPN(B, C, E);
  float vb = getVoltage(B, 3);
  float vc = getVoltage(C, 3);
  float ic = (VCC - vc) / (R_SMALL + R_HIGH);
  float ib = (VCC - vb) / (R_HIGH + R_BIG);
  float ve = getVoltage(E, 3);
  float ie = ve / R_LOW;
  float beta = ic / ib;
  printValue(1, beta, "");
  Serial.println(beta);
  Serial.println(vb);
  Serial.println(vc);
  Serial.println(ve);
  Serial.println(ib * 1000);
  Serial.println(ic * 1000);
  Serial.println(ie * 1000);
}
