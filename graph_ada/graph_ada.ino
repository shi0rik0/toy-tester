/*********************************************************************
This is an example sketch for our Monochrome Nokia 5110 LCD Displays
  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/products/338
These displays use SPI to communicate, 4 or 5 pins are required to
interface
Adafruit invests time and resources providing this open source code,
please support Adafruit and open-source hardware by purchasing
products from Adafruit!
Written by Limor Fried/Ladyada  for Adafruit Industries.
BSD license, check license.txt for more information
All text above, and the splash screen must be included in any redistribution
*********************************************************************/

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include "mygraph.h"

// Software SPI (slower updates, more flexible pin options):
// pin 7 - Serial clock out (SCLK)
// pin 6 - Serial data out (DIN)
// pin 5 - Data/Command select (D/C)
// pin 4 - LCD chip select (CS)
// pin 3 - LCD reset (RST)

// Hardware SPI (faster, but must use certain hardware pins):
// SCK is LCD serial clock (SCLK) - this is pin 13 on Arduino Uno
// MOSI is LCD DIN - this is pin 11 on an Arduino Uno
// pin 5 - Data/Command select (D/C)
// pin 4 - LCD chip select (CS)
// pin 3 - LCD reset (RST)
// Adafruit_PCD8544 display = Adafruit_PCD8544(5, 4, 3);
// Note with hardware SPI MISO and SS pins aren't used but will still be read
// and written to during SPI transfer.  Be careful sharing these pins!

// 目前配置：
// pin 3 - Serial clock out (SCLK)
// pin 4 - Serial data out (DIN)
// pin 5 - Data/Command select (D/C)
// pin 7 - LCD chip select (CS)
// pin 6 - LCD reset (RST)



// Adafruit_PCD8544 lcd = Adafruit_PCD8544(3,4,5,7,6);
// 在mygraph.cpp内设置

void setup()   {
  Serial.begin(9600);
  Serial.println("START!");
  __initLCD(50);
  delay(50);
}

void arctanTest(){
  lcd.setCursor(0,0);
  lcd.println("arctan test");
  const int len = 80;
  float x[len], y[len];
  for(int i = 0; i < len; i++){
    x[i] = (float)i;
    y[i] = 5.0 * atan(x[i] / 10.0);
  }
  graph(x, y, 0, 80, 0, 10, len);
}
void loop() {
  arctanTest();
  
  /*
  for(int i = 0; i < len; i++){
    x[i] = i;
    y[i] = 2 * i;
  }
  graph(x,y, 0, 40, 0, 27, len);
  */
  
  /*
  for(int i = 0; i < len; i++){
    x[i] = i;
    y[i] = i * i / 4;
  }
  graph(x,y, 0, 40, 0, 27, len);
  */
  

  /*
  for(int i = 0; i < len; i++){
    x[i] = i;
    y[i] = (i)/4;
  }
  graph(x,y,len);
  delay(10);
  
  for(int i = 0; i < len / 2; i++){
    x[i] = i;
    y[i] = (i);
  }
  graph(x,y,len);
  delay(10);
  */
  delay(5000);
  lcd.clearDisplay();
}
