#include "identify.h"

// ---------------私有函数声明-----------------------
void safeDischarge();

enum class Connectivity { OPEN = 0, CAPACITOR, NOT_CAPACITOR };
Connectivity getConnectivity(PortNum port1, PortNum port2, PortType type, word maxDelay);


// ---------------私有函数声明结束--------------------

void safeDischarge() {
  // 策略是随便写的，可以改进
  // 先用小电阻放电10ms
  for (byte i = 0; i < 3; ++i) {
    setPort(i, PortType::SMALL, LOW);
  }
  delay(10);
  // 再用更小的电阻放电100ms
  for (byte i = 0; i < 3; ++i) {
    setPort(i, PortType::READ, LOW);
  }
  delay(100);
}

static void initPNP(byte B, byte C, byte E) {
  setPort(B, PortType::BIG, LOW);
  setPort(C, PortType::SMALL, LOW);
  setPort(E, PortType::READ, HIGH);
}


static void initNPN(byte B, byte C, byte E) {
  setPort(B, PortType::BIG, HIGH);
  setPort(C, PortType::SMALL, HIGH);
  setPort(E, PortType::READ, LOW);
}

static float getBetaPNP(byte B, byte C, byte E) {
  initPNP(B, C, E);
  float vb = getVoltage(B, 10);
  float vc = getVoltage(C, 10);
  float ic = vc / R_SMALL;
  float ib = vb / R_BIG;
  float beta = ic / ib;
  return beta;
}

static float getBetaNPN(byte B, byte C, byte E) {
  initNPN(B, C, E);
  float vb = getVoltage(B, 10);
  float vc = getVoltage(C, 10);
  float ic = (VCC - vc) / R_SMALL;
  float ib = (VCC - vb) / R_BIG;
  float beta = ic / ib;
  return beta;
}

void getPNPBJTPorts(PortNum b, PortNum ce1, PortNum ce2, PortNum *c, PortNum *e) {
  float beta1 = getBetaPNP(b, ce1, ce2);
  float beta2 = getBetaPNP(b, ce2, ce1);
  if (beta1 > beta2) {
    *c = ce1;
    *e = ce2;
  } else {
    *c = ce2;
    *e = ce1;
  }
}

void getNPNBJTPorts(PortNum b, PortNum ce1, PortNum ce2, PortNum *c, PortNum *e) {
  float beta1 = getBetaNPN(b, ce1, ce2);
  float beta2 = getBetaNPN(b, ce2, ce1);
  if (beta1 > beta2) {
    *c = ce1;
    *e = ce2;
  } else {
    *c = ce2;
    *e = ce1;
  }
}


// 返回 port1 -> port2 的导通类型
// type: 使用什么种类的电阻
// maxDelay: 最多等待多久(ms)
Connectivity getConnectivity(PortNum port1, PortNum port2, PortType type, word maxDelay) {
  static const float THRESHOLD_LOW = 0.5;
  static const float THRESHOLD_HIGH = 4.5;
  static const float CAPACITOR_THRESHOLD = 0.2;

  safeDischarge();
  resetOtherPort(port1, port2);
  setPort(port2, type, LOW);
  setPort(port1, PortType::READ, HIGH);

  // 多测几次电压才会准，玄学问题
  getVoltage(port2, 3);

  for (word i = 0; i < maxDelay; ++i) {
    delay(1);
    float v = getVoltage(port2, 1);
    if (v > THRESHOLD_LOW && v < THRESHOLD_HIGH) {
      for (word j = i + 1; j < maxDelay; ++j) {
        delay(1);
        float v2 = getVoltage(port2, 1);
        if (v - v2 > CAPACITOR_THRESHOLD) {
          return Connectivity::CAPACITOR;
        }
      }
      return Connectivity::NOT_CAPACITOR;
    }
  }
  return Connectivity::OPEN;
}


void getAllConnectivities(Connectivity (*arr)[3], PortType type, word maxDelay) {
  for (byte i = 0; i < 3; ++i) {
    for (byte j = 0; j < 3; ++j) {
      if (i != j) {
        arr[i][j] = getConnectivity(i, j, type, maxDelay);
      }
    }
  }
}

struct CountResult {
  byte numOpen;
  byte numCapacitor;
  byte numNotCapacitor;
};

CountResult countConnectivites(Connectivity (*arr)[3]) {
  CountResult r;
  r.numOpen = 0;
  r.numCapacitor = 0;
  r.numNotCapacitor = 0;
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 3; ++j) {
      switch (arr[i][j]) {
        case Connectivity::OPEN:
          ++r.numOpen;
          break;
        case Connectivity::CAPACITOR:
          ++r.numCapacitor;
          break;
        case Connectivity::NOT_CAPACITOR:
          ++r.numNotCapacitor;
          break;
      }
    }
  }
  return r;
}

bool isSymmetric(Connectivity (*arr)[3]) {
  for (int i = 0; i < 3; ++i) {
    for (int j = i + 1; j < 3; ++j) {
      if (arr[i][j] != arr[j][i]) {
        return false;
      }
    }
  }
  return true;
}

void getTwoPorts(Connectivity (*arr)[3], byte *port1, byte *port2) {
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 3; ++j) {
      if (arr[i][j] != Connectivity::OPEN) {
        *port1 = i;
        *port2 = j;
        return;
      }
    }
  }
}



BJTInfo getBJTInfo(Connectivity (*arr)[3]) {
  BJTInfo info;
  bool findFirst = false;
  byte i0;
  byte j0;
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 3; ++j) {
      if (arr[i][j] != Connectivity::OPEN) {
        if (!findFirst) {
          findFirst = true;
          i0 = i;
          j0 = j;
        } else {
          if (i0 == i) {
            info.b = i0;
            info.ce1 = j0;
            info.ce2 = j;
            info.type = BJTType::NPN;
            getNPNBJTPorts(info.b, info.ce1, info.ce2, &info.c, &info.e);
          } else {
            info.b = j0;
            info.ce1 = i0;
            info.ce2 = i;
            info.type = BJTType::PNP;
            getPNPBJTPorts(info.b, info.ce1, info.ce2, &info.c, &info.e);
          }
          return info;
        }
      }
    }
  }
}

ComponentType guessType(Connectivity (*arr)[3]) {
  CountResult r = countConnectivites(arr);
  Serial.println(r.numOpen);
  Serial.println(r.numCapacitor);
  if (r.numNotCapacitor == 1) {
    Serial.println("DIODE");
    return ComponentType::DIODE;
  } else if (r.numCapacitor == 2) {
    Serial.println("CAP");
    return ComponentType::CAPACITOR;
  } else if (r.numNotCapacitor == 2) {
    if (isSymmetric(arr)) {
      
      Serial.println("RESISTOR");
      return ComponentType::RESISTOR;
    } else {
      
      Serial.println("BJT");
      return ComponentType::BJT;
    }
  } else {
    Serial.println("UNKNOWN");
    return ComponentType::UNKNOWN;
  }
}

ComponentInfo getInfo(Connectivity (*arr)[3], ComponentType type) {
  ComponentInfo r;
  r.type = type;
  PortNum port1;
  PortNum port2;
  switch (type) {
    case ComponentType::RESISTOR:
      Serial.println("RESISTOR");
      getTwoPorts(arr, &port1, &port2);
      r.info.resistor.port1 = port1;
      r.info.resistor.port2 = port2;
      break;
    case ComponentType::CAPACITOR:
      Serial.println("CAP");
      getTwoPorts(arr, &port1, &port2);
      r.info.capacitor.port1 = port1;
      r.info.capacitor.port2 = port2;
      break;
    case ComponentType::DIODE:
      getTwoPorts(arr, &port1, &port2);
      r.info.diode.portPos = port1;
      r.info.diode.portNeg = port2;
      break;
    case ComponentType::BJT:
      r.info.bjt = getBJTInfo(arr);
      break;
  }
  return r;
}

ComponentInfo getComponentInfo() {
  static Connectivity conn[3][3];
  getAllConnectivities(conn, PortType::SMALL, 50);
  ComponentType type = guessType(conn);
  if (type != ComponentType::UNKNOWN) {
    return getInfo(conn, type);
  }
  getAllConnectivities(conn, PortType::BIG, 50);
  type = guessType(conn);
  if (type != ComponentType::UNKNOWN) {
    return getInfo(conn, type);
  }
  return getInfo(conn, ComponentType::UNKNOWN);
}
