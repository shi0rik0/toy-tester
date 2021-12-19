#include "globals.h"
#include "mygraph.h"
#include "graphBJT.h"
#include "measure.h"
#include "draw.h"
// 输出曲线上有几个Ib
const int IB_CNT = 2;
// 一次测试的测试点数量
const int TEST_CNT = 42;
const float ibs[IB_CNT] = {20e-6, 30e-6};
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

// E 接地, Uc > Ub
void graphNPN(PinNum pinC, PinNum pinB, PinNum pinE, PortNum portC, PortNum portB, PortNum portE){
  clearLCD();
  printLine(0, "Plotting...");
  refreshLCD();
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
    const float UceMax = 5.0, IcMax = 0.015; // 画图坐标轴的上限

    clearLCD();
    for(int i = 0; i < IB_CNT; ++i){
        dataset points(TEST_CNT, x, y);
        graphNPN1(points, pinB, pinC, pinE, portB, portC, portE, ibs[i]);
        Serial.println("Measure Done");
        
        drawGraph(x, y, 0, UceMax, 0, IcMax, TEST_CNT);
        
    }
    
    pinMode(pinB,INPUT);
    pinMode(pinC,INPUT);
    pinMode(pinE,INPUT);
}
