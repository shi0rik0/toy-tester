#include "globals.h"
#include "identify.h"
#include "draw.h"
#include "measure.h"

void setup() {
  Serial.begin(9600);
  initLCD();
  Serial.println(3);
}

void loop() {
  ComponentInfo info = getComponentInfo();
  if (info.type == ComponentType::CAPACITOR) {
    CapacitorInfo &c = info.info.capacitor;
    measureCapacitor(c.port1, c.port2);
  }
  if (info.type == ComponentType::BJT) {
    
      BJTInfo &b = info.info.bjt;
      if (b.type == BJTType::NPN) {
        measureNPNBJT(b.ce1, b.b, b.ce2);
      } else {
        //        measurePNPBJT(b.ce1, b.b, b.ce2);
      }
  }
  switch (info.type) {
    case ComponentType::RESISTOR:
      ResistorInfo &r = info.info.resistor;
      measureResistor(r.port1, r.port2);
      break;
    case ComponentType::CAPACITOR:
      Serial.println(12);
      CapacitorInfo &c = info.info.capacitor;
      measureCapacitor(c.port1, c.port2);
      break;
    case ComponentType::DIODE:
      DiodeInfo &d = info.info.diode;
      // measureDiode(i.portPos, i.portNeg);
      break;
    case ComponentType::BJT:
      BJTInfo &b = info.info.bjt;
      if (b.type == BJTType::NPN) {
        measureNPNBJT(b.ce1, b.b, b.ce2);
      } else {
        //        measurePNPBJT(b.ce1, b.b, b.ce2);
      }
      break;
    case ComponentType::UNKNOWN:
      clearLCD();
      printLine(0, "Error");
      refreshLCD();
      break;
  }
  delay(2000);
}
