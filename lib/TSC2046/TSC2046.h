
#ifndef TSC2046_H
#define TSC2046_H

#include <Arduino.h>
#include <SPI.h>
#include <driver/gpio.h>

//#define TouchThreshold (3850)  //3700->3800
#define TouchThreshold (4070)  //4070->4080

typedef struct {
    int posX;
    int posY;
}TouchPosition_t;

class TSC2046 {
    public:
        TSC2046(SPIClass *spi, int CSpin);
        //TSC2046(int CSpin);
        void Init();
        TouchPosition_t GetPositon();

    private:
        SPIClass *mySPI;
        int TouchCS;
};


#endif