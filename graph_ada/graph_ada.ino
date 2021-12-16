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
/*
float __fmap(float x, float l1, float r1, float l2, float r2){
  return (x - l1) * (r2 - l2) / (r1 - l1) + l2;
}
*/
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
  delay(50);
}

void testGraph(uint8_t *x, uint8_t *y, int len){ // 画测试图像
  int WIDTH = len;
  const int HEIGHT = LCD_HEIGHT;
  int SIZE = WIDTH * HEIGHT;
  lcd.setCursor(0,0);
  lcd.println("Hello!");
  lcd.setCursor(60,40); // setCursor和之前的库不一样，第一个参数是行，第二个是列，单位是像素
  lcd.println("test");
  for(int i = 0; i < WIDTH; ++i){
    _setPixel(x[i], y[i] , BLACK);
  }
  
  lcd.display();
}

void graph(float *X, float *Y, float xMin, float xMax, float yMin, float yMax, int len) {  // xMin, xMax分别为x轴最小、最大刻度, y同理; len: 数据点个数
  int px, py; // previous x and y
  for(int i = 0; i < len; ++i){
    int x = round(map(X[i], xMin, xMax, 0, LCD_WIDTH - 1));
    int y = round(map(Y[i], yMin, yMax , 0, LCD_HEIGHT - 1));
    Serial.print(x);
    Serial.print(' ');
    Serial.print(y);
    Serial.print('\n');
    _setPixel(x, y , BLACK);
    if(i == 0){
      ;
    }
    // 连线
    
    float incl = x == px ? 1e7 : (float)(y - py) / (float)(x - px);
    if(incl <= 1){
      for(int j = 1; px + j < x; ++j){
      _setPixel(px + j, py + round(j * incl), BLACK);
      }
    }
    else{
      for(int k = 1; py + k < y; ++k){
        _setPixel(px + round(k / incl), py + k ,BLACK);
      }
    }
    delay(5);
    px = x;
    py = y;
  }
  lcd.display();
}
void loop() {
  const int len = 11;
  float x[len], y[len];
  for(int i = 0; i < len; i++){
    x[i] = i;
    y[i] = 2 * i;
  }
  graph(x,y, 0, 40, 0, 27, len);
  for(int i = 0; i < len; i++){
    x[i] = i;
    y[i] = 1 * i;
  }
  graph(x,y, 0, 40, 0, 27, len);
  for(int i = 0; i < len; i++){
    x[i] = i;
    y[i] = i * i / 4;
  }
  graph(x,y, 0, 40, 0, 27, len);
  delay(1000);

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
  //lcd.clearDisplay();
}
