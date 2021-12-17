#include "globals.h"

// pin 3 - Serial clock out (SCLK)
// pin 4 - Serial data out (DIN)
// pin 5 - Data/Command select (D/C)
// pin 7 - LCD chip select (CS)
// pin 6 - LCD reset (RST)
Adafruit_PCD8544 lcd(3, 4, 5, 7, 6);


// 以下是引脚定义，SMALL表示小电阻，BIG表示大电阻，READ表示模拟输入
const PinNum SMALL_1 = 8;
const PinNum BIG_1 = 9;
const PinNum READ_1 = A2;
const PinNum SMALL_2 = 10;
const PinNum BIG_2 = 11;
const PinNum READ_2 = A1;
const PinNum SMALL_3 = 12;
const PinNum BIG_3 = 13;
const PinNum READ_3 = A0;

const PinNum PORT[3][3] = {
    {SMALL_1, BIG_1, READ_1},
    {SMALL_2, BIG_2, READ_2},
    {SMALL_3, BIG_3, READ_3},
};

const float VCC = 5;
const float R_BIG = 470e3;
const float R_SMALL = 680;


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

float adcToVoltage(word adc)
{
  return adc / 1023.0 * VCC;
}

float getVoltage(PortNum port, byte times) {
  PinNum pin = getPinNum(port, PortType::READ);
  for (byte i = 0; i < times - 1; ++i) {
    analogRead(pin);
  }
  return adcToVoltage(analogRead(pin));
}
