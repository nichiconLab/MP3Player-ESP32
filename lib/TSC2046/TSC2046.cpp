
#include "TSC2046.h"


TSC2046::TSC2046(SPIClass *spi, int CSpin){
    mySPI = spi;
    TouchCS = CSpin;
}

void TSC2046::Init(){
    mySPI->beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE0));
    gpio_set_level(GPIO_NUM_4, LOW);
    mySPI->transfer16(B11010000);
    mySPI->transfer16(0);
    gpio_set_level(GPIO_NUM_4, HIGH);
    mySPI->endTransaction();
}

TouchPosition_t TSC2046::GetPositon(){
    int z1 = 0, z2 = 0;
    int rx = 0, ry = 0, rz = 0;

    mySPI->beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE0));
    gpio_set_level(GPIO_NUM_4, LOW);

    mySPI->transfer(B10110011);
    mySPI->transfer16(B10110011);
    mySPI->transfer16(B10110011);
    mySPI->transfer16(B10110011);
    z1 = mySPI->transfer16(B11000011) >> 3;
    mySPI->transfer16(B11000011);
    mySPI->transfer16(B11000011);
    mySPI->transfer16(B11000011);
    z2 = mySPI->transfer16(B11010011) >> 3;
    mySPI->transfer16(B11010011);
    mySPI->transfer16(B11010011);
    mySPI->transfer16(B11010011);
    rx = mySPI->transfer16(B10010011) >> 3;
    mySPI->transfer16(B10010011);
    mySPI->transfer16(B10010011);
    mySPI->transfer16(B10010000);
    ry = mySPI->transfer16(0) >> 3;

    gpio_set_level(GPIO_NUM_4, HIGH);
    mySPI->endTransaction();
    
    if(z1 > 0 && z2 > 0){
        ////rz = 4096 - (int)((double)z1/z2 * rx/4.0);  //z2/z1 -> z1/z2
        //rz = 4096 - (int)((double)z1/z2 * abs(rx-1745)/2.0);  //<-
        //float k = sqrt(pow(rx-1745, 2)+pow(ry-1770, 2));
        float k = sqrt((rx-1745)*(rx-1745) + (ry-1770)*(ry-1770))/2.0;  //rx width: 3490, ry width: 3539
        rz = 4096 - (int)((double)z1/z2 * k);
    }else{
        //rz = 0;
        rz = 4096;
    }

    //cx = 1744.5, cy = 1769.5
    //int th = 4096 - abs(rx-1745)/67;

    TouchPosition_t result;
    if(rz < 4070){  //4060->4070
        result.posX = map(rx, 4000, 511, 0, 240);  //min:511, max:4000
        result.posY = map(ry, 511, 4050, 0, 320);  //min:255, max:4050, 255->511
    }else{
        result.posX = -1;
        result.posY = -1;
    }
    
    //Serial.printf("x:%d, y:%d, rz:%d\r\n", rx, ry, rz);
    //Serial.printf("x:%d, y:%d, rz:%d\r\n", result.posX, result.posY, rz);

    return result;
}
