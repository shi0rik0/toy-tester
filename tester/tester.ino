#include "globals.h"
#include "identify.h"
#include "draw.h"
#include "measure.h"

void setup() {
  Serial.begin(9600);
  initLCD();

}

void loop() {
  
//  unsigned long t = millis();
//  for (int i = 0; i < 10000; ++i) {
//    analogRead(A1);
//  }
//  Serial.println(millis() - t);
  ComponentInfo info = getComponentInfo();
  if (info.type == ComponentType::RESISTOR) {
    ResistorInfo &r = info.info.resistor;
    measureResistor(r.port1, r.port2);
  }
  else if (info.type == ComponentType::CAPACITOR) {
    CapacitorInfo &c = info.info.capacitor;
    measureCapacitor(c.port1, c.port2);
  } else if (info.type == ComponentType::DIODE) {
    DiodeInfo &d = info.info.diode;
     measureDiode(d.portPos, d.portNeg);
  } else if (info.type == ComponentType::BJT) {
    BJTInfo &b = info.info.bjt;
    if (b.type == BJTType::NPN) {
      measureNPNBJT(b.ce1, b.b, b.ce2);
    } else {
      //        measurePNPBJT(b.ce1, b.b, b.ce2);
    }
  } else {
    clearLCD();
    printLine(0, "No supported");
    printLine(1, "component de-");
    printLine(2, "tected.");
    refreshLCD();
  }

  delay(2000);
}
