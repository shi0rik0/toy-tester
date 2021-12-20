#include "globals.h"
#include "mygraph.h"
#include "graphBJT.h"
#include "measure.h"
#include "draw.h"
// 输出曲线上有几个Ib
const int IB_CNT = 3;
// 一次测试的测试点数量
const int TEST_CNT = 42;
const float ibs[IB_CNT] = {20e-6, 30e6, 40e-6};
const char* marks[IB_CNT] = {"20uA","30uA", "40uA"};
const int mark_Y[IB_CNT] = {32,22,12};
const float ico[IB_CNT] = {1.0,1.5,2.0};
const float R_B = 1e5;
const float R_C = 51;

struct dataset
{
    int len;
    float *x, *y;    
    dataset(int L, float *X=nullptr, float *Y=nullptr):len(L), x(X), y(Y){}
};

float getUbe(PortNum portB, PortNum portE) {
    //return getVoltage(portX,5) - getVoltage(portY,5);
    return 0.7;
}

void graphNPN1(dataset points, PinNum pinC, PinNum pinB, PinNum pinE, PortNum portC, PortNum portB, PortNum portE, float targetIb){
    float Ube = getUbe(portB, portE);
    float Vbb = targetIb * R_B + Ube;
    setVoltage(pinB, Vbb);
    setVoltage(pinE, 0);
    float VcoStep = VCC / (float)points.len; // 均匀取Vco
    
    float Vco = 0.0;
    for(int i = 0; i < points.len; ++i){
        setVoltage(pinC, Vco);
        //float Uce = getAvgVoltage(portC,1000,10);
        float EEE = getAvgVoltage(portB,1000,10);
        float BBB = getAvgVoltage(portC,1000,10);
        float Uce = getAvgVoltage(portE,1000,10) + 0.1;
        points.x[i] = Uce;  // Uce
        points.y[i] = (Vco - Uce) / R_C; // Ic

        Serial.print("Vco:");
        Serial.print(Vco);
        Serial.print("\n");
        
        Serial.print("Uce:");
        Serial.print(Uce);
        Serial.print("\n");

        Serial.print("BBB:");
        Serial.print(BBB);
        Serial.print("\n");


        Serial.print("EEE:");
        Serial.print(EEE);
        Serial.print("\n");

        Serial.print(points.y[i]*1000);
        Vco += VcoStep;
        delay(5);
    }
}
void drawAxis(){
  //lcd.drawLine(1,1,30,1,BLACK);
  //lcd.drawLine(30,1,30,40,BLACK);
  for(int i = 0; i < 84; i++){
    _setPixel(i, 1, BLACK);
  }
  for(int i = 0; i < 48; i++){
    _setPixel(1, i, BLACK);
  }
  lcd.setCursor(8,2);
  lcd.print("Ic");
  lcd.setCursor(64,40);
  lcd.print("Uce");
}
// E 接地, Uc > Ub
void graphNPN(PinNum pinC, PinNum pinB, PinNum pinE, PortNum portC, PortNum portB, PortNum portE){
    // 其他三个口用来测电压
    //resetPort(0); resetPort(1); resetPort(2);
    // 启用三个pin
    pinMode(pinB,OUTPUT);
    pinMode(pinC,OUTPUT);
    pinMode(pinE,OUTPUT);
    resetPort(portC);
    resetPort(portB);
    resetPort(portE);
    float x[TEST_CNT], y[TEST_CNT];
    const float UceMin = 0.8, UceMax = 4.0, IcMin = 0.0, IcMax = 0.015; // 画图坐标轴的上限

    clearLCD();
    drawAxis();
    for(int i = 0; i < IB_CNT; ++i){
        dataset points(TEST_CNT, x, y);
        graphNPN1(points, pinB, pinC, pinE, portB, portC, portE, ibs[i]);
        Serial.println("Measure Done");
        for(int j = 0; j < TEST_CNT; ++j){
          y[j] *= ico[i];
        }
        const int CUT_F = 8, CUT_BK = 20;
        *(x+5) = 0.0; *(y+5)=0.0;
        drawGraph(x + CUT_F, y + CUT_F, UceMin, UceMax, IcMin, IcMax, TEST_CNT - CUT_BK);
        
        
    }
    for(int i = 0; i < IB_CNT; ++i){
      lcd.setCursor(60,mark_Y[i]);
      lcd.print(marks[i]);
    }
    lcd.display();
    pinMode(pinB,INPUT);
    pinMode(pinC,INPUT);
    pinMode(pinE,INPUT);
    delay(10000);
    //clearLCD();
}
