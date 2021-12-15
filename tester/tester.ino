#include <PCD8544.h>
#include <math.h>

// -----------------全局变量定义----------------------
PCD8544 lcd;
// -----------------全局变量定义结束----------------------

// -----------------宏定义--------------------------

#define DEBUG

#ifdef DEBUG
#define printDebug(x) (printDebug_(x))
#else
#define printDebug(x) ((void)0)
#endif

// -----------------宏定义结束--------------------------

// -----------------常量定义------------------------
// 以下是引脚定义，SMALL表示小电阻，BIG表示大电阻，READ表示模拟输入
const byte SMALL_1 = 8;
const byte BIG_1 = 9;
const byte READ_1 = A2;
const byte SMALL_2 = 10;
const byte BIG_2 = 11;
const byte READ_2 = A1;
const byte SMALL_3 = 12;
const byte BIG_3 = 13;
const byte READ_3 = A0;

// 把引脚写在一个数组里面，方便取用，比如说第一组的小电阻端口就是 PORT[0][SMALL]
const byte SMALL = 0;
const byte BIG = 1;
const byte READ = 2;
const byte PORT[3][3] = {
    {SMALL_1, BIG_1, READ_1},
    {SMALL_2, BIG_2, READ_2},
    {SMALL_3, BIG_3, READ_3},
};

const float VCC = 5;
const float R_BIG = 470e3;
const float R_SMALL = 680;

enum BJTType { PNP, NPN };

// 端口自身的内阻，似乎用不上
const float R_HIGH = 22;
const float R_LOW = 19;

const byte ARRAY_LEN = 100;

const byte NUM_PREFIXES_SMALL = 4;
const char *const PREFIXES_SMALL[NUM_PREFIXES_SMALL] = {"m", "u", "n", "p"};
const byte NUM_PREFIXES_BIG = 2;
const char *const PREFIXES_BIG[NUM_PREFIXES_BIG] = {"k", "M"};

// -----------------常量定义结束------------------------

// -------------------函数声明---------------------

// 让 port 的三个引脚全部悬空
void resetPort(byte port);
// 让 port 的 type 引脚接到 HIGH 或者 LOW，剩下的悬空
void setPort(byte port, byte type, byte highOrLow);
void goToLine(byte row);
void printVoltage(byte port, byte row);
float adcToVoltage(word adc);
float getVoltage(byte port);
float getAvgVoltage(byte port, word times, word interval);
void printType(const char *str);
void printValue(float val, const char *unit);
void printStatus(const char *str);
void printDebug_(const char *str);
byte getOtherPort(byte port1, byte port2);

// -------------------函数声明结束-----------------

// ------------------setup & loop----------------

void setup() {
  lcd.begin(84, 48);
  Serial.begin(9600);
}

void loop() {
  goToLine(0);
  lcd.print((int)testConnectivity(0, 1));
  delay(100);
  goToLine(1);
  lcd.print((int)testConnectivity(1, 0));
  delay(100);
}

// ------------------setup & loop 结束----------------

// ------------------函数定义-----------------------

void resetPort(byte port) {
  for (byte i = 0; i < 3; ++i) {
    pinMode(PORT[port][i], INPUT);
  }
}

void setPort(byte port, byte type, byte highOrLow) {
  for (byte i = 0; i < 3; ++i) {
    if (i == type) {
      pinMode(PORT[port][i], OUTPUT);
      digitalWrite(PORT[port][i], highOrLow);
    } else {
      pinMode(PORT[port][i], INPUT);
    }
  }
}

void goToLine(byte row) {
  lcd.setCursor(0, row);
  lcd.clearLine();
}

// 把 port 的电压打印到第 row 行
void printVoltage(byte port, byte row) {
  goToLine(row);
  lcd.print(getVoltage(port));
}

float adcToVoltage(word adc) { return adc / 1023.0 * VCC; }

float getVoltage(byte port) {
  // 多测几次，以免电容干扰
  analogRead(PORT[port][READ]);
  analogRead(PORT[port][READ]);
  return adcToVoltage(analogRead(PORT[port][READ]));
}

// 以 interval(单位：ms) 为间隔测量 times 次，然后取平均值
float getAvgVoltage(byte port, word times, word interval) {
  float s = 0;
  for (word i = 0; i < times; ++i) {
    s += getVoltage(port);
    delay(interval);
  }
  return s / times;
}

void printType(const char *str) {
  lcd.setCursor(0, 0);
  lcd.clearLine();
  lcd.print(str);
}

int __abs(int x) { return (x < 0 ? -x : x); }

void printValue(float val, const char *unit) {

  lcd.setCursor(0, 1);
  lcd.clearLine();
  const char *prefix = "";
  if (val < 1) {
    val *= 1000;
    byte i = 0;
    while (val < 1) {
      val *= 1000;
      i += 1;
      if (i == NUM_PREFIXES_SMALL) {
        lcd.print("0 ");
        lcd.print(unit);
        return;
      }
    }
    prefix = PREFIXES_SMALL[i];
  } else if (val >= 1000) {
    val /= 1000;
    byte i = 0;
    while (val >= 1000) {
      val /= 1000;
      i += 1;
      if (i == NUM_PREFIXES_BIG) {
        lcd.print("Inf ");
        lcd.print(unit);
        return;
      }
    }
    prefix = PREFIXES_BIG[i];
  }
  lcd.print(val);
  lcd.print(' ');
  lcd.print(prefix);
  lcd.print(unit);
}

void printStatus(const char *str) {
  lcd.setCursor(0, 5);
  lcd.clearLine();
  lcd.print(str);
}

void printDebug_(const char *str) {
  lcd.setCursor(0, 4);
  lcd.clearLine();
  lcd.print(str);
}

byte getOtherPort(byte port1, byte port2) {
  static const byte TABLE[3][3] = {
      {-1, 2, 1},
      {2, -1, 0},
      {1, 0, -1},
  };
  return TABLE[port1][port2];
}

void switchToSmallResistor(byte port1, byte port2) {
  pinMode(PORT[port1][SMALL], OUTPUT);
  pinMode(PORT[port1][BIG], INPUT);
  pinMode(PORT[port1][READ], INPUT);
  pinMode(PORT[port2][SMALL], INPUT);
  pinMode(PORT[port2][BIG], INPUT);
  pinMode(PORT[port2][READ], OUTPUT);
  digitalWrite(PORT[port1][SMALL], LOW);
  digitalWrite(PORT[port1][BIG], LOW);
  digitalWrite(PORT[port1][READ], LOW);
  digitalWrite(PORT[port2][SMALL], LOW);
  digitalWrite(PORT[port2][BIG], LOW);
  digitalWrite(PORT[port2][READ], HIGH);
  resetOther(port1, port2);
}

// void switchToBigResistor(byte port1, byte port2) {
//  resetOther(port1, port2);
//
//  pinMode(PORT[port1][SMALL], OUTPUT);
//  pinMode(PORT[port1][BIG], OUTPUT);
//  pinMode(PORT[port1][READ], OUTPUT);
//  pinMode(PORT[port2][SMALL], INPUT);
//  pinMode(PORT[port2][BIG], INPUT);
//  pinMode(PORT[port2][READ], INPUT);
//  digitalWrite(PORT[port1][SMALL], LOW);
//  digitalWrite(PORT[port1][BIG], LOW);
//  digitalWrite(PORT[port1][READ], LOW);
//  digitalWrite(PORT[port2][SMALL], LOW);
//  digitalWrite(PORT[port2][BIG], LOW);
//  digitalWrite(PORT[port2][READ], LOW);
//
//  delay(100);
//
//  pinMode(PORT[port1][SMALL], INPUT);
//  pinMode(PORT[port1][READ], INPUT);
//  pinMode(PORT[port2][READ], OUTPUT);
//  digitalWrite(PORT[port2][READ], HIGH);
//}

void switchToBigResistor(byte port1, byte port2) {
  resetOther(port1, port2);

  pinMode(PORT[port1][SMALL], INPUT);
  pinMode(PORT[port1][BIG], OUTPUT);
  pinMode(PORT[port1][READ], INPUT);
  pinMode(PORT[port2][SMALL], INPUT);
  pinMode(PORT[port2][BIG], INPUT);
  pinMode(PORT[port2][READ], OUTPUT);
  digitalWrite(PORT[port1][SMALL], LOW);
  digitalWrite(PORT[port1][BIG], LOW);
  digitalWrite(PORT[port1][READ], LOW);
  digitalWrite(PORT[port2][SMALL], LOW);
  digitalWrite(PORT[port2][BIG], LOW);
  digitalWrite(PORT[port2][READ], HIGH);
}

void discharge(byte port1, byte port2) {
  byte port3 = getOtherPort(port1, port2);
  pinMode(PORT[port1][SMALL], OUTPUT);
  pinMode(PORT[port1][BIG], OUTPUT);
  pinMode(PORT[port1][READ], OUTPUT);
  pinMode(PORT[port2][SMALL], OUTPUT);
  pinMode(PORT[port2][BIG], OUTPUT);
  pinMode(PORT[port2][READ], OUTPUT);
  pinMode(PORT[port3][SMALL], OUTPUT);
  pinMode(PORT[port3][BIG], OUTPUT);
  pinMode(PORT[port3][READ], OUTPUT);
  digitalWrite(PORT[port1][SMALL], LOW);
  digitalWrite(PORT[port1][BIG], LOW);
  digitalWrite(PORT[port1][READ], LOW);
  digitalWrite(PORT[port2][SMALL], LOW);
  digitalWrite(PORT[port2][BIG], LOW);
  digitalWrite(PORT[port2][READ], LOW);
  digitalWrite(PORT[port3][SMALL], LOW);
  digitalWrite(PORT[port3][BIG], LOW);
  digitalWrite(PORT[port3][READ], LOW);
}

void dischargeModeSmallR(byte port1, byte port2) { // 让电容放电(在小电阻上)
  setPort(port1, SMALL, LOW);
  setPort(port2, READ, LOW);
}

void dischargeModeBigR(byte port1, byte port2) { // 让电容放电(在大电阻上)
  setPort(port1, BIG, LOW);
  setPort(port2, READ, LOW);
}

void dischargeCapacitorSmallR(byte port1, byte port2, int dischargeTime) {
  dischargeModeSmallR(port1, port2);
  delay(dischargeTime);
}

void initBJT(byte B, byte C, byte E,
             bool bBigR) { // bBigR=0 B port uses 680 ohm, else 470k ohm.
  if (!bBigR) {
    pinMode(PORT[B][SMALL], OUTPUT);
    pinMode(PORT[B][BIG], INPUT);
    digitalWrite(PORT[B][SMALL], HIGH);
  } else {
    pinMode(PORT[B][SMALL], INPUT);
    pinMode(PORT[B][BIG], OUTPUT);
    digitalWrite(PORT[B][HIGH], HIGH);
  }
  pinMode(PORT[B][READ], INPUT);
  pinMode(PORT[C][READ], OUTPUT);
  pinMode(PORT[C][SMALL], INPUT);
  pinMode(PORT[C][BIG], INPUT);
  pinMode(PORT[E][READ], INPUT);
  pinMode(PORT[E][SMALL], OUTPUT);
  pinMode(PORT[E][BIG], INPUT);
  digitalWrite(PORT[E][SMALL], HIGH);
  digitalWrite(PORT[E][SMALL], LOW);
}

void dischargeCapacitorBigR(byte port1, byte port2, int dischargeTime) {
  dischargeModeBigR(port1, port2);
  delay(dischargeTime);
}

float getResistance(byte port1, byte port2, float r0) {
  float v = getAvgVoltage(port1, 10, 10);
  float r = r0 * (VCC - v) / v;
  return r;
}

// 返回 port1 -> port2 是不是导通的
bool testConnectivity(byte port1, byte port2) {
  const float THRESHOLD_LOW = 0.1;
  const float THRESHOLD_HIGH = 4.9;
  setPort(port2, BIG, LOW);
  setPort(port1, READ, HIGH);
  delay(10);
  float v = getVoltage(port2);
  if (v > THRESHOLD_LOW && v < THRESHOLD_HIGH) {
    return true;
  }
  setPort(port2, SMALL, LOW);
  delay(10);
  v = getVoltage(port2);
  if (v > THRESHOLD_LOW && v < THRESHOLD_HIGH) {
    return true;
  }
  return false;
}

// 返回值：true: 电压下降值可观; false: 电压基本不变
bool recordVoltages(float *vArr, byte port1, int cnt, int interval) {
  float THRESH = 0.5;
  for (int i = 0; i < cnt; ++i) {
    vArr[i] = getVoltage(port1);
    delayMicroseconds(interval);
  }
  return (vArr[0] - vArr[cnt - 1] > THRESH);
}

float getTao(float *vArr, int pointCnt, int interval) {
  const int THRESH = 0.1; // 电压为0的阈值
  float tao = -1;
  float sumTao = 0;
  int validCnt = 0;
  const float validThresh = 0.01;
  // V = Vcc * e ** (-t / tao)
  for (int i = 1; i < pointCnt; ++i) { // 计算tao平均值
    float t = i * interval * 1e-6;
    float logDiff = log(VCC) - log(vArr[i]);
    if (__abs(logDiff) > validThresh) { // 如果电压接近0 就不计入
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

void testResistor(byte port1, byte port2) {
  // 这个阈值是随便取的，我有一个计算更好的值的思路，但是还没算
  const float THRESHOLD = 4.5;
  lcd.clear();
  printType("Resistor");
  printStatus("Wait...");
  float r0 = R_BIG;
  printDebug("Big");
  switchToBigResistor(port1, port2);
  delay(10);
  float v = getVoltage(port1);
  if (v > THRESHOLD) {
    r0 = R_SMALL;
    printDebug("Small");
    switchToSmallResistor(port1, port2);
    delay(10);
  }
  float r = getResistance(port1, port2, r0);
  printValue(r, "Ohm");
  printStatus("        Done!");
}

void testCapacitor(byte port1, byte port2) {
  const int dischargeTime = 1000;
  lcd.clear();
  printType("Capacitor");
  printDebug("Small");
  float cap;
  switchToBigResistor(port1, port2); // 开始充电
  cap = getCapacitance(port1, port2, R_BIG);
  dischargeCapacitorBigR(port1, port2, dischargeTime); //  电容放电

  if (cap < 0) {
    printStatus("Trying big F");
    switchToSmallResistor(port1, port2); // 开始充电
    cap = getCapacitance(port1, port2, R_SMALL);
    dischargeCapacitorSmallR(port1, port2, dischargeTime); //  电容放电
  }

  printValue(cap, "F");
  printStatus("        Done!");
}

void testBJT(byte C, byte B, byte E) {
  lcd.clear();
  printType("BJT");
  printStatus("Trying as PNP");
  initBJT(B, C, E, 0);
  float vb = getVoltage(B);
  float vc = getVoltage(C);
  float ic = vc / R_LOW;
  float ib = vb / (R_LOW + R_SMALL);
  if (vb < 0.01 || vb < vc) {
    printStatus("Attaching 470k ohm to B");
    initBJT(B, C, E, 1);
    vb = getVoltage(B);
    vc = getVoltage(C);
    ic = vc / R_LOW;
    ib = vb / (R_LOW + R_BIG);
  }
  float ve = getVoltage(E);
  float ie = ve / (R_HIGH + R_SMALL);
  float beta = ic / ie;
  printValue(beta, "");
  Serial.println(beta);
  Serial.println(ib);
  Serial.println(ic);
  Serial.println(ie);
}

byte countTrue(bool **arr, byte dim1, byte dim2) {
  byte count = 0;
  for (int i = 0; i < dim1; ++i) {
    for (int j = 0; j < dim2; ++j) {
      count += static_cast<byte>(connectivity[i][j]);
    }
  }
  return count;
}

bool isSymmetric(bool **arr, byte size) {
  for (int i = 0; i < size; ++i) {
    for (int j = i + 1; j < size; ++j) {
      if (connectivity[i][j] != connectivity[j][i]) {
        return false;
      }
    }
  }
  return true;
}

void getTwoPorts(bool **arr, byte *port1, byte *port2) {
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 3; ++j) {
      if (connectivity[i][j]) {
        *port1 = i;
        *port2 = j;
        return;
      }
    }
  }
}

void getBJTInfo(bool **arr, byte *b, byte *ce1, byte *ce2, BJTType *type) {
  bool findFirst = false;
  byte i0;
  byte j0;
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 3; ++j) {
      if (connectivity[i][j]) {
        if (!findFirst) {
          findFirst = true;
          i0 = i;
          j0 = j;
        } else {
          if (i0 == i) {
            *b = i0;
            *ce1 = j0;
            *ce2 = j;
            *type = NPN;
          } else {
            *b = j0;
            *ce1 = i0;
            *ce2 = i;
            *type = PNP;
          }
          return;
        }
      }
    }
  }
}

void measure() {
  static bool connectivity[3][3];
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 3; ++j) {
      if (i != j) {
        connectivity[i][j] = testConnectivity(i, j);
      }
    }
  }
  byte count = countTrue(connectivity, 3, 3);
  if (count == 1) {
    // 是二极管
  } else if (count == 2) {
    if (isSymmetric(connectivity, 3)) {
      byte port1;
      byte port2;
      getSymmetricPorts(connectivity, &port1, &port2);
      // 是电容或者电阻
    } else {
      byte b;
      byte ce1;
      byte ce2;
      BJTType type;
      getBJTInfo(connectivity, &b, &ce1, &ce2, &type);
      // 是三极管
    }
  } else {
    // 啥也不是
  }
}

// ------------------函数定义结束-----------------------