#include "draw.h"

void initLCD() {
  lcd.begin();
  lcd.setContrast(60); // 参数是对比度，张书源买的屏幕用60合适
  clearLCD();
  refreshLCD();
}

void clearLCD() {
  lcd.clearDisplay();
}

void clearLine(byte row) {
  lcd.fillRect(0, row * 8, 84, 8, WHITE);
}

void refreshLCD() {
  lcd.display();
}

void goToLine(byte row) {
  lcd.setCursor(0, row * 8);
}

const byte NUM_PREFIXES_SMALL = 4;
const char *const PREFIXES_SMALL[NUM_PREFIXES_SMALL] = {"m", "u", "n", "p"};
const byte NUM_PREFIXES_BIG = 2;
const char *const PREFIXES_BIG[NUM_PREFIXES_BIG] = {"k", "M"};

void printValue(byte row, const char *description, float val, const char *unit) {
  goToLine(row);
  clearLine(row);

  lcd.print(description);
  lcd.print("=");
  const char *prefix = "";
  if (val < 1) {
    val *= 1000;
    byte i = 0;
    while (val < 1) {
      val *= 1000;
      i += 1;
      if (i == NUM_PREFIXES_SMALL) {
        lcd.print("0");
        lcd.print(unit);
        lcd.display();
        return;
      }
    }
    prefix = PREFIXES_SMALL[i];
  } else if (val >= 1000) {
    val /= 1000;
    byte i = 0;
    while (val >= 1000) {
      val /= 1000;
      i += 1;
      if (i == NUM_PREFIXES_BIG) {
        lcd.print("Inf");
        lcd.print(unit);
        lcd.display();
        return;
      }
    }
    prefix = PREFIXES_BIG[i];
  }
  lcd.print(val);
  lcd.print(prefix);
  lcd.print(unit);
}
