#include "globals.h"
#include "mygraph.h"
#include "graphBJT.h"

// 输出曲线上有几个Ib
const int IB_CNT = 2;
// 一次测试的测试点数量
const int TEST_CNT = 42;
const float ibs[IB_CNT] = {20e-6, 40e-6};
const float R_B = 1e5;
const float R_C = 1e5;

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

void graphNPN1(dataset points, PinNum pinB, PinNum pinC, PinNum pinE, PortNum portB, PortNum portC, PortNum portE, float targetIb){
    float Ube = getUbe(portB, portE);
    float Vbb = targetIb * R_B + Ube;
    setVoltage(pinB, Vbb);
    setVoltage(pinE, 0);
    float VcoStep = VCC / (float)points.len; // 均匀取Vco
    float Vco = 0.0;
    for(int i = 0; i < points.len; ++i){
        setVoltage(pinC, Vco);
        float Uce = getVoltage(portC, 5);
        points.x[i] = Uce;  // Uce
        points.y[i] = (Vco - Uce) / R_C; // Ic
    }
}

// E 接地, Uc > Ub
void graphNPN(PinNum pinB, PinNum pinC, PinNum pinE, PortNum portB, PortNum portC, PortNum portE){
    // 其他三个口用来测电压
    resetPort(0); resetPort(1); resetPort(2);
    // 启用三个pin
    pinMode(pinB,OUTPUT);
    pinMode(pinC,OUTPUT);
    pinMode(pinE,OUTPUT);

    float x[TEST_CNT], y[TEST_CNT];
    const float UceMax = 3.0, IcMax = 0.01; // 画图坐标轴的上限
    for(int i = 0; i < IB_CNT; ++i){
        dataset points(TEST_CNT, x, y);
        graphNPN1(points, pinB, pinC, pinE, portB, portC, portE, ibs[i]);
        drawGraph(x, y, 0, UceMax, 0, IcMax, TEST_CNT);
    }

    pinMode(pinB,INPUT);
    pinMode(pinC,INPUT);
    pinMode(pinE,INPUT);
}
