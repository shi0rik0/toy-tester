#pragma once

#include <Adafruit_PCD8544.h>

// 可以取0、1、2
using PortNum = byte;
using PinNum = byte;

// 用来控制液晶屏
extern Adafruit_PCD8544 lcd;



enum class PortType { SMALL = 0, BIG = 1, READ = 2 };
PinNum getPinNum(PortNum port, PortType type);

// 一些参数
extern const float VCC;
extern const float R_BIG;
extern const float R_SMALL;


// 端口自身的内阻，似乎用不上
extern const float R_HIGH;
extern const float R_LOW;


// 让 port 的三个引脚全部悬空
void resetPort(PortNum port);
// 让 port 的 type 引脚接到 HIGH 或者 LOW，剩下的悬空
void setPort(PortNum port, PortType type, byte highOrLow);
// 让不是 port1 和 port2 的另外一个端口悬空
void resetOtherPort(PortNum port1, PortNum port2);
// 获得电压，测量 times 次，取最后一次的结果
float getVoltage(PortNum port, byte times);
