#pragma once

#include <Adafruit_PCD8544.h>
#include <math.h>

// 可以取0、1、2
using PortNum = byte;
using PinNum = byte;

// 用来控制液晶屏
extern Adafruit_PCD8544 lcd;

// 临时测试用
void setHigh();

enum class PortType { SMALL = 0, BIG = 1, READ = 2 };
PinNum getPinNum(PortNum port, PortType type);

// 一些参数
extern const float VCC;
extern const float R_BIG;
extern const float R_SMALL;

// 端口自身的内阻，似乎用不上
extern const float R_HIGH;
extern const float R_LOW;

extern const PinNum WRITE_1;
extern const PinNum WRITE_2;
extern const PinNum WRITE_3;

// 采样一次所用时间(us)，实测得到
const word READ_TIME = 112;

// 让 port 的三个引脚全部悬空
void resetPort(PortNum port);
// 让 port 的 type 引脚接到 HIGH 或者 LOW，剩下的悬空
void setPort(PortNum port, PortType type, byte highOrLow);
// 让不是 port1 和 port2 的另外一个端口悬空
void resetOtherPort(PortNum port1, PortNum port2);
// 获得电压，测量 times 次，取最后一次的结果
float getVoltage(PortNum port, byte times = 1);
// 设置port口电压（需要支持PWM的口）
void setVoltage(PinNum pin, float voltage);

enum class ButtonPressed {
  NONE, SHORT, LONG
};

ButtonPressed getButtonStatus();
