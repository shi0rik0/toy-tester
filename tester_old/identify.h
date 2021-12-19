#pragma once

#include "globals.h"

// 支持识别4种元件
enum class ComponentType { RESISTOR, CAPACITOR, DIODE, BJT, UNKNOWN };

// 三极管的两种类型
enum class BJTType { PNP, NPN };

struct ResistorInfo {
  PortNum port1;
  PortNum port2;
};

struct CapacitorInfo {
  PortNum port1;
  PortNum port2;
};

struct DiodeInfo {
  PortNum portPos;
  PortNum portNeg;
};

struct BJTInfo {
  PortNum b;
  PortNum ce1; // 废弃
  PortNum ce2; // 废弃
  PortNum c;
  PortNum e;
  BJTType type;
};

struct ComponentInfo {
  ComponentType type;
  union {
    ResistorInfo resistor;
    CapacitorInfo capacitor;
    DiodeInfo diode;
    BJTInfo bjt;
  } info;
};

ComponentInfo getComponentInfo();
