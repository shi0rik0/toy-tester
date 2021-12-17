#pragma once

#include "globals.h"


void measureResistor(byte port1, byte port2);
void measurePNPBJT(byte C, byte B, byte E);
void measureNPNBJT(byte C, byte B, byte E);
void measureCapacitor(byte port1, byte port2);
void measureDiode(byte portPos, byte portNeg);
