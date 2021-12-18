#ifndef MYGRAPH_H
#define MYGRAPH_H
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
//extern Adafruit_PCD8544 lcd;
extern const uint8_t LCD_WIDTH, LCD_HEIGHT;


inline void _setPixel(int x, int y, bool color);
//void __initLCD(byte contrast);
//void testGraph(uint8_t *x, uint8_t *y, int len); // 画测试图像
void drawGraph(float *X, float *Y, float xMin, float xMax, float yMin, float yMax, int len);  // xMin, xMax分别为x轴最小、最大刻度, y同理; len: 数据点个数
 
#endif