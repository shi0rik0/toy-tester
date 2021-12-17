#include "globals.h"
#include "identify.h"
#include "draw.h"
#include "measure.h"

void setup() {
  Serial.begin(9600);
  initLCD();
}

void loop() {
  ComponentInfo info = getComponentInfo();
  switch (info.type) {
    case ComponentType::RESISTOR:
      ResistorInfo &r = info.info.resistor;
      measureResistor(r.port1, r.port2);
      break;
    case ComponentType::CAPACITOR:
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
        measurePNPBJT(b.ce1, b.b, b.ce2);
      }
      break;
  }
}
