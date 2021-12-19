#pragma once

#include "globals.h"

void initLCD();
void clearLCD();
// 清除一行，屏幕一共6行
void clearLine(byte row);
void goToLine(byte row);
void refreshLCD();

template <typename T>
void printLine(byte row, const T &x) {
  goToLine(row);
  clearLine(row);
  lcd.print(x);
}

void printValue(byte row, const char *descrption, float val, const char *unit);
