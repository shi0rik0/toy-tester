
#include <Adafruit_PCD8544.h>
#include <math.h>




// -----------------全局变量定义----------------------

// Software SPI (slower updates, more flexible pin options):
// pin 3 - Serial clock out (SCLK)
// pin 4 - Serial data out (DIN)
// pin 5 - Data/Command select (D/C)
// pin 7 - LCD chip select (CS)
// pin 6 - LCD reset (RST)
Adafruit_PCD8544 lcd = Adafruit_PCD8544(3, 4, 5, 7, 6);

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
enum PortType { SMALL = 0, BIG = 1, READ = 2 };
const byte PORT[3][3] = {
  {SMALL_1, BIG_1, READ_1},
  {SMALL_2, BIG_2, READ_2},
  {SMALL_3, BIG_3, READ_3},
};

const float VCC = 5;
const float R_BIG = 470e3;
const float R_SMALL = 680;

// 三极管的两种类型
enum BJTType { PNP, NPN };

// 端口自身的内阻，似乎用不上
const float R_HIGH = 22;
const float R_LOW = 19;

const byte ARRAY_LEN = 100;

const byte NUM_PREFIXES_SMALL = 4;
const char *const PREFIXES_SMALL[NUM_PREFIXES_SMALL] = {"m", "u", "n", "p"};
const byte NUM_PREFIXES_BIG = 2;
const char *const PREFIXES_BIG[NUM_PREFIXES_BIG] = {"k", "M"};

const char *STATUS_RUNNING = "Wait...";
const char *STATUS_OK = "        Done!";


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

// 让 port1 串上小电阻并接地，port2 接上 VCC，另外一个端口悬空
void switchToSmallResistor(byte port1, byte port2);
// 让 port1 串上大电阻并接地，port2 接上 VCC，另外一个端口悬空
void switchToBigResistor(byte port1, byte port2);

void measureResistor(byte port1, byte port2);
// 在 port1 串上电阻 r0 并接地，port2 接上 VCC 的情况下测量电阻，返回阻值
float getResistance(byte port1, byte port2, float r0);

void measureCapacitor(byte port1, byte port2);

void measureResistorOrCapacitor(byte port1, byte port2);

void measure();

// -------------------函数声明结束-----------------

// ------------------setup & loop----------------

void setup() {
  Serial.begin(9600);

  // 启动 LCD
  lcd.begin();
  lcd.setContrast(60); // 参数是对比度，张书源买的屏幕用60合适
  clearLCD();
}

void loop() {
  measure();
  //  lcd.fillRect(0, 10, 10, 5, BLACK);
  //  lcd.display();
  delay(500);
}

// ------------------setup & loop 结束----------------

// ------------------函数定义-----------------------

void clearLCD() {
  lcd.clearDisplay();
  lcd.display();
}

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


template <typename T>
void printLine(byte row, const T &x) {
  lcd.setCursor(0, row * 8);
  lcd.fillRect(0, row * 8, 84, 8, WHITE); // 清除一行，一行显示14个字符，一共6行
  lcd.setCursor(0, row * 8);
  lcd.print(x);
  lcd.display();
}

// 把 port 的电压打印到第 row 行
void printVoltage(byte port, byte row) {
  goToLine(row);
  lcd.print(getVoltage(port));
}

float adcToVoltage(word adc) {
  return adc / 1023.0 * VCC;
}

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



void printValue(float val, const char *unit) {
  goToLine(1);
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
        lcd.display();
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
        lcd.display();
        return;
      }
    }
    prefix = PREFIXES_BIG[i];
  }
  lcd.print(val);
  lcd.print(' ');
  lcd.print(prefix);
  lcd.print(unit);
  lcd.display();
}

void printStatus(const char *str) {
  goToLine(5);
  lcd.print(str);
  lcd.display();
}

//void printDebug_(const char *str) {
//  goToLine(4);
//  lcd.print(str);
//  lcd.display();
//}

byte getOtherPort(byte port1, byte port2) {
  static const byte TABLE[3][3] = {
    { -1, 2, 1},
    {2, -1, 0},
    {1, 0, -1},
  };
  return TABLE[port1][port2];
}

void switchToSmallResistor(byte port1, byte port2) {
  resetPort(getOtherPort(port1, port2));
  setPort(port1, SMALL, LOW);
  setPort(port2, READ, HIGH);
}

void switchToBigResistor(byte port1, byte port2) {
  resetPort(getOtherPort(port1, port2));
  setPort(port1, BIG, LOW);
  setPort(port2, READ, HIGH);
}

void discharge(byte port1, byte port2, PortType type) {
  resetPort(getOtherPort(port1, port2));
  setPort(port1, type, LOW);
  setPort(port2, type, LOW);
}

void dischargeBySmallResistor(byte port1, byte port2, word dischargeTime) {
  discharge(port1, port2, SMALL);
  delay(dischargeTime);
}

void dischargeByBigResistor(byte port1, byte port2, word dischargeTime) {
  discharge(port1, port2, BIG);
  delay(dischargeTime);
}

void safeDischarge() {
  // 策略是随便写的，可以改进
  for (byte i = 0; i < 3; ++i) {
    setPort(i, SMALL, LOW);
  }
  delay(10);
  for (byte i = 0; i < 3; ++i) {
    setPort(i, READ, LOW);
  }
  for (byte i = 0; i < 100; ++i) {
    delay(1);
    //    for (byte i = 0; i < 3; ++i) {
    //      getVoltage(i);
    //    }
  }
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

float getResistance(byte port1, byte port2, float r0) {
  float v = getAvgVoltage(port1, 10, 10);
  float r = r0 * (VCC - v) / v;
  return r;
}

// 返回 port1 -> port2 是不是导通的
bool testConnectivity(byte port1, byte port2, byte bigOrSmall, word delayUs) {
  const float THRESHOLD_LOW = 0.05;
  const float THRESHOLD_HIGH = 4.95;
  safeDischarge();
  if (bigOrSmall == BIG) {
    switchToBigResistor(port1, port2);
  } else {
    switchToSmallResistor(port1, port2);
  }
  delayMicroseconds(delayUs);
  float v = getVoltage(port1);
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

void measureResistor(byte port1, byte port2) {
  // 这个阈值是计算得到的
  static const float THRESHOLD = 4.8;

  lcd.clearDisplay();
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

  printValue(cap, "F");
  printStatus("        Done!");
}

void testBJT(byte C, byte B, byte E) {
  lcd.clearDisplay();
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

byte countTrue(bool (*arr)[3]) {
  byte count = 0;
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 3; ++j) {
      count += static_cast<byte>(arr[i][j]);
    }
  }
  return count;
}

bool isSymmetric(bool (*arr)[3]) {
  for (int i = 0; i < 3; ++i) {
    for (int j = i + 1; j < 3; ++j) {
      if (arr[i][j] != arr[j][i]) {
        return false;
      }
    }
  }
  return true;
}

void getTwoPorts(bool (*arr)[3], byte *port1, byte *port2) {
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 3; ++j) {
      if (arr[i][j]) {
        *port1 = i;
        *port2 = j;
        return;
      }
    }
  }
}

void getBJTInfo(bool (*arr)[3], byte *b, byte *ce1, byte *ce2, BJTType *type) {
  bool findFirst = false;
  byte i0;
  byte j0;
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 3; ++j) {
      if (arr[i][j]) {
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

void measureResistorOrCapacitor(byte port1, byte port2) {
  float cap = getCapacitance2(port1, port2);
  if (cap < 0) {
    printLine(0, "R");
  } else {
    printLine(0, "C");
  }
}

void measure() {
  clearLCD();
  printLine(5, STATUS_RUNNING);
  static bool connectivity[3][3];
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 3; ++j) {
      if (i != j) {
        connectivity[i][j] = testConnectivity(i, j, SMALL, 100);
      }
    }
  }
  bool detected = false;
  byte count = countTrue(connectivity);
  if (count == 1) {
    detected = true;
    // 是二极管
    byte port1;
    byte port2;
    getTwoPorts(connectivity, &port1, &port2);
    // measureDiode(port1, port2);
    printLine(0, "Diode");
  } else if (count == 2) {
    if (isSymmetric(connectivity)) {
      detected = true;
      // 是电容或者电阻
      byte port1;
      byte port2;
      getTwoPorts(connectivity, &port1, &port2);
      measureResistorOrCapacitor(port1, port2);
      //      printLine(0, "R or C");
    } else {
      detected = true;
      byte b;
      byte ce1;
      byte ce2;
      BJTType type;
      getBJTInfo(connectivity, &b, &ce1, &ce2, &type);
      if (type == PNP) {
        // measurePNPBJT(b, ce1, ce2);
        printLine(0, "PNP");
      } else {
        // measureNPNBJT(b, ce1, ce2);
        printLine(0, "NPN");
      }
    }
  }
  if (!detected) {
    for (int i = 0; i < 3; ++i) {
      for (int j = 0; j < 3; ++j) {
        if (i != j) {
          connectivity[i][j] = testConnectivity(i, j, BIG, 10000);
        }
      }
    }

    byte count = countTrue(connectivity);
    if (count == 2) {
      if (isSymmetric(connectivity)) {
        detected = true;
        // 是电容或者电阻
        byte port1;
        byte port2;
        getTwoPorts(connectivity, &port1, &port2);
        measureResistorOrCapacitor(port1, port2);
      }
    }
  }

  if (!detected) {
    printLine(0, "No supported");
    printLine(1, "component de-");
    printLine(2, "tected.");
  }
  printLine(5, STATUS_OK);
}

// ------------------函数定义结束-----------------------
