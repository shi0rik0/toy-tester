#include "globals.h"

// pin 12 - Serial clock out (SCLK)
// pin 11 - Serial data out (DIN)
// pin 10 - Data/Command select (D/C)
// pin 9 - LCD chip select (CS)
// pin 8 - LCD reset (RST)
Adafruit_PCD8544 lcd(12, 11, 10, 9, 8);

// 以下是引脚定义，SMALL表示小电阻，BIG表示大电阻，READ表示模拟输入

// 这里的9, 10, 11 可以挪到A3, A4, A5, ADC口支持digitalWrite()
const PinNum SMALL_1 = 2;
const PinNum BIG_1 = A2;
const PinNum READ_1 = A5;
const PinNum SMALL_2 = 4;
const PinNum BIG_2 = A1;
const PinNum READ_2 = A4;
const PinNum SMALL_3 = 7;
const PinNum BIG_3 = A0;
const PinNum READ_3 = A3;
// 这三个（可能只需要两个）引脚必须要PWM
// 只能是 3, 5, 6, 9, 10, 11
const PinNum WRITE_1 = 3;
const PinNum WRITE_2 = 5;
const PinNum WRITE_3 = 6;
// 按钮的引脚
const PinNum BUTTON = A7;

const PinNum PORT[3][3] = {
    {SMALL_1, BIG_1, READ_1},
    {SMALL_2, BIG_2, READ_2},
    {SMALL_3, BIG_3, READ_3},
};

const float VCC = 5;
const float R_BIG = 100e3;
const float R_SMALL = 510;

// 端口自身的内阻，似乎用不上
const float R_HIGH = 22;
const float R_LOW = 19;

PinNum getPinNum(PortNum port, PortType type) {
  byte t = static_cast<byte>(type);
  return PORT[port][t];
}

void resetPort(PortNum port) {
  for (byte i = 0; i < 3; ++i) {
    pinMode(PORT[port][i], INPUT);
  }
}

void setPort(PortNum port, PortType type, byte highOrLow) {
  byte t = static_cast<byte>(type);
  for (byte i = 0; i < 3; ++i) {
    if (i == t) {
      pinMode(PORT[port][i], OUTPUT);
      digitalWrite(PORT[port][i], highOrLow);
    } else {
      pinMode(PORT[port][i], INPUT);
    }
  }
}

void setHigh() {
  //  for (byte i = 0; i < 3; ++i) {
  //    for (byte j = 0; j < 3; ++j) {
  //      pinMode(PORT[i][j], OUTPUT);
  //      digitalWrite(PORT[i][j], HIGH);
  //      getVoltage(i, 3);
  //    }
  //  }
  //  delay(100);
  //  for (byte i = 0; i < 3; ++i) {
  //    for (byte j = 0; j < 3; ++j) {
  //      resetOtherPort(i, j);
  //      setPort(i, PortType::BIG, LOW);
  //      setPort(j, PortType::READ, HIGH);
  //      for (word i = 0; i < 1000; ++i) {
  //        getVoltage(i, 1);
  //        getVoltage(j, 1);
  //      }
  //    }
  //  }
}

PortNum getOtherPort(PortNum port1, PortNum port2) {
  static const PortNum TABLE[3][3] = {
      {-1, 2, 1},
      {2, -1, 0},
      {1, 0, -1},
  };
  return TABLE[port1][port2];
}

void resetOtherPort(PortNum port1, PortNum port2) {
  PortNum port = getOtherPort(port1, port2);
  resetPort(port);
}

float adcToVoltage(word adc) { return adc / 1023.0 * VCC; }

float getVoltage(PortNum port, byte times) {
  PinNum pin = getPinNum(port, PortType::READ);
  for (byte i = 0; i < times - 1; ++i) {
    analogRead(pin);
  }
  return adcToVoltage(analogRead(pin));
}

int voltageToPWM(float voltage) {
  if (voltage < 0) {
    return 0;
  }
  if (voltage > 5.0) {
    return 255;
  }
  return round(voltage / VCC * 255.0);
}
void setVoltage(PinNum pin, float voltage) {
  analogWrite(pin, voltageToPWM(voltage));
}


const word THRESHOLD = 512;
const word DELAY = 10; // ms

void waitButtonDown() {
  word v = analogRead(BUTTON);
  if (v > THRESHOLD) {
    delay(DELAY);
    do {
      v = analogRead(BUTTON);
    } while (v > THRESHOLD);
  }
  delay(DELAY);
  do {
    v = analogRead(BUTTON);
  } while (v < THRESHOLD);
}

ButtonPressed getButtonStatus() {
  static const word LONG_THRESHOLD = 1000; // ms
  word v = analogRead(BUTTON);
  if (v < THRESHOLD) {
    return ButtonPressed::NONE;
  }
  unsigned long t = millis();
  delay(DELAY);
  while (millis() - t < LONG_THRESHOLD) {
    v = analogRead(BUTTON);
    if (v < THRESHOLD) {
      return ButtonPressed::SHORT;
    }
  }
  return ButtonPressed::LONG;
}
