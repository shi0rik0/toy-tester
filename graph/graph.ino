#include <PCD8544.h>
#include <math.h>
PCD8544 lcd;

// 以下是引脚定义，SMALL表示小电阻，BIG表示大电阻，READ表示模拟输入
const byte SMALL_1 = 8;
const byte BIG_1 = 9;
const byte READ_1 = A2;
const byte SMALL_2 = 10;
const byte BIG_2 = 11;
const byte READ_2 = A1;
const byte SMALL_3 = 12;
const byte BIG_3 = 13;
const byte READ_3 = A0;

// 把引脚写在一个数组里面，方便取用，比如说第一组的小电阻端口就是 PORT[0][SMALL]
const byte SMALL = 0;
const byte BIG = 1;
const byte READ = 2;
const byte PORT[3][3] = {
  {SMALL_1, BIG_1, READ_1},
  {SMALL_2, BIG_2, READ_2},
  {SMALL_3, BIG_3, READ_3},
};


const float VCC = 5;
const float R_BIG = 470e3;
const float R_SMALL = 680;

// 端口自身的内阻，似乎用不上
const float R_HIGH = 22;
const float R_LOW = 19;


const int ARRAY_LEN = 100; 
static const byte LCD_WIDTH = 84;
static const byte LCD_HEIGHT = 48;
static const byte CHART_HEIGHT = 5;

#define DEBUG

#ifdef DEBUG
#define printDebug(x) (printDebug_(x))
#else
#define printDebug(x) ((void)0)
#endif


void fillArr(uint8_t *arr, int len, int val){
  for(int i = 0; i < len; ++i){
    arr[i] = val;
  }
}
// 与lcd.drawBitmap(data, WIDTH, HEIGHT); 搭配使用
// x, y : 相对于cursor位置的xy坐标，坐标系为左下角(0,0) 和数学课定义的一样
// state: 0或者1
// W, H : 画图区域的宽和高
void setPixel(uint8_t* arr, int x, int y, bool state, int W, int H){ 
  int lineOffset = y / 8;
  arr[W - x + W * lineOffset] |=  1 << (y - 8 * lineOffset); 
  // 数组中一个8bit数代表屏幕上竖着8个点  
}

void testGraph(int cnt){ // 画测试图像
  const int WIDTH = 80;
  const int HEIGHT = 20;
  int len = WIDTH * HEIGHT;
  uint8_t data[len];
  fillArr(data,len,0);
  lcd.setCursor(3,8);
  for(int x = 0; x < WIDTH; ++x){
    setPixel(data, x, (int)sqrt(x) , 1, WIDTH, HEIGHT);
  }
  lcd.drawBitmap(data, WIDTH, HEIGHT);
  
}

void setup() {
  lcd.begin(84, 48);
  Serial.begin(9600);

}

float v;

void loop() {

  testGraph(10);
  delay(1000);
  lcd.clear();


}
