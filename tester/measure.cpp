#include "draw.h"
#include "identify.h"

#include <math.h>

const byte ARRAY_LEN = 100;

const char *STATUS_RUNNING = "Wait...";
const char *STATUS_OK = "        Done!";

// 以 interval(单位：us) 为间隔测量 times 次，然后取平均值
float getAvgVoltage(byte port, word times, word interval) {
  getVoltage(port, 3);
  float s = 0;
  for (word i = 0; i < times; ++i) {
    s += getVoltage(port, 1);
    delayMicroseconds(interval);
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
  printValue(1, "R", r, "Ohm");
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

void initPNP(byte B, byte C, byte E) {
  setPort(B, PortType::BIG, LOW);
  setPort(C, PortType::SMALL, LOW);
  setPort(E, PortType::READ, HIGH);
}


void initNPN(byte B, byte C, byte E) {
  setPort(B, PortType::BIG, HIGH);
  setPort(C, PortType::SMALL, HIGH);
  setPort(E, PortType::READ, LOW);
}


//float getBetaPNP(byte C, byte B, byte E) {
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

float getBetaPNP(byte B, byte C, byte E) {
  initPNP(B, C, E);
  float vb = getAvgVoltage(B, 10, 100);
  float vc = getAvgVoltage(C, 10, 100);
  float ic = vc / R_SMALL;
  float ib = vb / R_BIG;
  float beta = ic / ib;
  return beta;
}

float getBetaNPN(byte B, byte C, byte E) {
  initNPN(B, C, E);
  float vb = getAvgVoltage(B, 10, 100);
  float vc = getAvgVoltage(C, 10, 100);
  float ic = (VCC - vc) / R_SMALL;
  float ib = (VCC - vb) / R_BIG;
  float beta = ic / ib;
  return beta;
}


//void measureNPNBJT(byte B, byte C, byte E) {
//  clearLCD();
//  printLine(0, "NPN-BJT");
//  printLine(5, STATUS_RUNNING);
//  refreshLCD();
//  float beta1 = getBetaNPN(B, C, E);
//  float beta2 = getBetaNPN(B, E, C);
//  printValue(1, "beta", max(beta1, beta2), "");
//  printLine(5, STATUS_OK);
//  refreshLCD();
//}

//void measurePNPBJT(byte C, byte B, byte E) {
//  clearLCD();
//  printLine(0, "PNP-BJT");
//  printLine(5, STATUS_RUNNING);
//  refreshLCD();
//  float beta1 = getBetaPNP(B, C, E);
//  float beta2 = getBetaPNP(B, E, C);
//  Serial.println(beta1);
//  Serial.println(beta2); 
//  printValue(1, "beta", max(beta1, beta2), "");
//  printLine(5, STATUS_OK);
//  refreshLCD();
//}

const char *getPortAlphabet(PortNum p) {
  static const char* const STR[] = {""};
}


void measureNPNBJT(byte B, byte C, byte E) {
  clearLCD();
  printLine(0, "NPN-BJT");
  goToLine(1);
  clearLine(1);
  lcd.print("B: ");
  printLine(5, STATUS_RUNNING);
  refreshLCD();
  float beta = getBetaNPN(B, C, E);
  printValue(4, "beta", beta, "");
  printLine(5, STATUS_OK);
  refreshLCD();
}

void measurePNPBJT(byte B, byte C, byte E) {
  clearLCD();
  printLine(0, "PNP-BJT");
  printLine(5, STATUS_RUNNING);
  refreshLCD();
  float beta = getBetaPNP(B, C, E);
  Serial.println(beta); 
  printValue(4, "beta", beta, "");
  printLine(5, STATUS_OK);
  refreshLCD();
}


//// 返回值：true: 电压下降值可观; false: 电压基本不变
//bool recordVoltages(float *vArr, byte port1, int cnt, int interval) {
//  float THRESH = 0.5;
//  for (int i = 0; i < cnt; ++i) {
//    vArr[i] = getVoltage(port1, 1);
////    Serial.println(vArr[i]);
//    delayMicroseconds(interval);
//  }
//  return (vArr[0] - vArr[cnt - 1] > THRESH);
//}
//
//float getTao(float *vArr, int pointCnt, int interval) {
//  const float THRESH = 0.1; // 电压为0的阈值
//  float tao = -1;
//  float sumTao = 0;
//  int validCnt = 0;
//  const float validThresh = 0.01;
//  // V = Vcc * e ** (-t / tao)
//  for (int i = 1; i < pointCnt; ++i) { // 计算tao平均值
//    float t = i * interval * 1e-6;
//    float logDiff = log(VCC) - log(vArr[i]);
//    if (abs(logDiff) > validThresh) { // 如果电压接近0 就不计入
//      sumTao += (t / logDiff);
//      ++validCnt;
//      ;
//    }
//  }
//
//  return sumTao / (float)(validCnt);
//}
//
//float getCapacitance(byte port1, byte port2, float r0) {
//  static float vArr[ARRAY_LEN];
//  int pointCnt = 50;  // 取点数目
//  int interval = 10; // 测量间隔(us)
//
//  if (!recordVoltages(vArr, port1, pointCnt, interval)) {
//    return -1;
//  }
//  float tao = getTao(vArr, pointCnt, interval + 20);
//  return tao / r0; // tao = RC
//}



//
//// 这个函数丑了点，一会儿再改
//float getCapacitance2(byte port1, byte port2) {
//
//  const word dischargeTime = 10; // 单位ms. 这个时间怎么确定的？
//  //  printType("Capacitor");
//  //  printDebug("Small");
//  float cap;
//  switchToBigResistor(port1, port2); // 开始充电
//  dischargeByBigResistor(port1, port2, dischargeTime);
//  switchToBigResistor(port1, port2);
//  cap = getCapacitance(port1, port2, R_BIG);
//  dischargeByBigResistor(port1, port2, dischargeTime); //  电容放电
//
//
//  if (cap < 0) {
//    //    printDebug("Big");
//    switchToSmallResistor(port1, port2); // 开始充电
//    dischargeByBigResistor(port1, port2, dischargeTime);
//    switchToBigResistor(port1, port2); // 开始充电
//  
//    cap = getCapacitance(port1, port2, R_SMALL);
//    dischargeBySmallResistor(port1, port2, dischargeTime); //  电容放电
//  }
//  return cap;
//}


float getCapacitance2(byte port1, byte port2) {
  static const float THRESHOLD_1 = 2.0;
  static const float THRESHOLD_2 = 3.0;
  resetPort(port2);
  setPort(port1, PortType::BIG, HIGH);
  setPort(port2, PortType::READ, LOW);
//  float v1;
//  unsigned long time1;
//  float v2 = getVoltage(port1);
//  unsigned long time2 = micros();
//  do {
//    v1 = v2;
//    time1 = time2;
//    v2 = getVoltage(port1);
//    time2 = micros();
//    Serial.println(time2);
//    Serial.println(v2);
//  } while (v2 < THRESHOLD);

//  bool v1_recorded = false;
//  float v1, v2;
//  unsigned long time1, time2;
//  for (;;) {
//    float v = getVoltage(port1);
//    if (v > THRESHOLD_2) {
//      v2 = v;
//      time2 = micros();
//      break;
//    } else if (!v1_recorded && v > THRESHOLD_1) {
//      v1 = v;
//      time1 = micros();
//      v1_recorded = true;
//    }
//  }
//      Serial.println(time1);
//    Serial.println(v1);
//    Serial.println(time2);
//    Serial.println(v2);
//  float tao = (time2 - time1) * 1e-6 / log(v2 / v1);
//  float cap = tao / R_BIG;
//  Serial.println(cap * 1e9);
//  return cap;

  int count = 0;
  for (;;) {
    float v = getVoltage(port1);
    if (v > 4) {
      break;
    }
    ++count;
  }
  float tao = count * READ_TIME * 1e-6 / log(5);
  float cap = tao / R_BIG * 1.38; // 实验测出来的常数
  Serial.println(cap * 1e9);
  return cap;
}

void measureDiode(byte portPos, byte portNeg) {
  setPort(portPos, PortType::BIG, HIGH);
  setPort(portNeg, PortType::READ, LOW);
  delay(10);
  float v = getVoltage(portPos);
  Serial.println(v);
}

void measureCapacitor(byte port1, byte port2) {
  clearLCD();
  printLine(0, "Capacitor");
  printLine(5, STATUS_RUNNING);
  refreshLCD();
  float cap = getCapacitance2(port1, port2);
  printValue(1, "C", cap, "F");
  printLine(5, STATUS_OK);
  refreshLCD();
}
