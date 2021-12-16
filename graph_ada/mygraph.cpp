#ifndef MYGRAPH_CPP
#define MYGRAPH_CPP
#include "mygraph.h"

extern Adafruit_PCD8544 lcd = Adafruit_PCD8544(3,4,5,7,6);
extern const uint8_t LCD_WIDTH = 84, LCD_HEIGHT = 48;
inline void _setPixel(int x, int y, bool color){
  lcd.drawPixel(x, LCD_HEIGHT - y, color);
}
void __initLCD(byte contrast){
  lcd.begin();
  lcd.setContrast(contrast);
  lcd.clearDisplay();
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

#endif