#include "globals.h"
#include "identify.h"
#include "draw.h"
#include "measure.h"

void setup() {
  Serial.begin(9600);
  initLCD();

  //  pinMode(A0, OUTPUT);
  //  digitalWrite(A0, HIGH);
  //  delay(10);
  //  Serial.println(analogRead(A3));
}

void measure() {
  // 这个地方用 switch 不行，不知道为什么
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
      measurePNPBJT(b.ce1, b.b, b.ce2);
    }
  } else {
    clearLCD();
    printLine(0, "No supported");
    printLine(1, "component de-");
    printLine(2, "tected.");
    refreshLCD();
  }
}


void loop() {

  //  unsigned long t = millis();
  //  for (int i = 0; i < 10000; ++i) {
  //    analogRead(A1);
  //  }
  //  Serial.println(millis() - t);
  for (;;) {
    ButtonPressed b = getButtonStatus();
    if (b == ButtonPressed::SHORT) {
      measure();
    } else if (b == ButtonPressed::LONG) {
      
    }
  }
  

  //  ButtonPressed b = getButtonStatus();
  //  if (b == ButtonPressed::SHORT) {
  //    Serial.println("SHORT");
  //  } else if (b == ButtonPressed::LONG) {
  //    Serial.println("LONG");
  //  }
}
