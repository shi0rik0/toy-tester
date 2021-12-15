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

// Software SPI (slower updates, more flexible pin options):
// pin 7 - Serial clock out (SCLK)
// pin 6 - Serial data out (DIN)
// pin 5 - Data/Command select (D/C)
// pin 4 - LCD chip select (CS)
// pin 3 - LCD reset (RST)

// 目前配置：
// pin 3 - Serial clock out (SCLK)
// pin 4 - Serial data out (DIN)
// pin 5 - Data/Command select (D/C)
// pin 7 - LCD chip select (CS)
// pin 6 - LCD reset (RST)
Adafruit_PCD8544 lcd = Adafruit_PCD8544(3,4,5,7,6);

// Hardware SPI (faster, but must use certain hardware pins):
// SCK is LCD serial clock (SCLK) - this is pin 13 on Arduino Uno
// MOSI is LCD DIN - this is pin 11 on an Arduino Uno
// pin 5 - Data/Command select (D/C)
// pin 4 - LCD chip select (CS)
// pin 3 - LCD reset (RST)
// Adafruit_PCD8544 display = Adafruit_PCD8544(5, 4, 3);
// Note with hardware SPI MISO and SS pins aren't used but will still be read
// and written to during SPI transfer.  Be careful sharing these pins!

const uint8_t LCD_WIDTH = 84, LCD_HEIGHT = 48;

inline void _setPixel(int x, int y, bool color){
  lcd.drawPixel(x, LCD_HEIGHT - y, color);
}
void setup()   {
  Serial.begin(9600);
  Serial.println("PCD test");
  lcd.begin();
  lcd.setContrast(50);
  lcd.clearDisplay();
  // draw a single pixel
  //for(int i = 0; i < 80; ++i)
  //lcd.drawPixel(i, 10, BLACK);
  delay(1000);
}

void graph(uint8_t *x, uint8_t *y, int len){ // 画测试图像
  int WIDTH = len;
  const int HEIGHT = LCD_HEIGHT;
  int SIZE = WIDTH * HEIGHT;
  
  //arr = new uint8_t[SIZE];
  //fillArr(arr, SIZE, 0);
  //if(!k){
    //drawXAxis(arr, WIDTH, HEIGHT);
  //}
  //drawYAxis(arr, WIDTH, HEIGHT);
  for(int i = 0; i < WIDTH; ++i){
    _setPixel(x[i], y[i] , BLACK);
  }
  
  lcd.display();
}

void loop() {
  const int len = LCD_WIDTH;
  uint8_t x[len], y[len];
  for(int i = 0; i < len; i++){
    x[i] = i;
    y[i] = (i)/2;
  }
  graph(x,y,len);
  delay(10);
  
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
  
  //lcd.clearDisplay();
}
