#ifndef INTERFACE_H
#define INTERFACE_H

#include <Arduino.h>
#include <freertos/task.h>
#include <freertos/FreeRTOS.h>

#include <SPI.h>
//#include "Adafruit_GFX_Library/Adafruit_GFX.h"
//#include "Adafruit_ILI9341/Adafruit_ILI9341.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include "TSC2046.h"

#include "driver/ledc.h"  //<-20240505

#define TFT_DC   21
#define TFT_CS   5
#define TFT_MOSI 23
#define TFT_MISO 19
#define TFT_CLK  18
#define TFT_RST  22

#define TFT_LED  A4  //32
#define SLEEP_CYCLE 600  //60seconds

#define TOUCH_CS 4


typedef struct{
    bool BtnL;   //Left button
    bool BtnR;   //Right button
    bool BtnC;   //Center button
    bool BtnT;   //Top button (Volume up)
    bool BtnB;   //Bottom button (Volume down)
    bool BtnM;   //Mode button
    bool BtnPL;  //Playlist Left button
    bool BtnPR;  //Playlist Right button
}ButtonStatus_t;

enum : uint8_t{
    MODE_NORMAL = 0,
    MODE_LOOP_MUSIC,
    MODE_AUTO,
    MODE_LOOP_ALL
};

void Clear_ButtonStatus(ButtonStatus_t *BtnStatus);
void Interface_Init();
void Set_Brightness(uint8_t level);
ButtonStatus_t Get_ButtonStatus();
void Set_TitleString(char *str);
void Set_VolumeBar(uint8_t v);
void Set_NextTitle(char *str, uint8_t flag);
//void Set_BatteryLevel(uint8_t v);
void Set_BatteryLevel(uint8_t v, bool ChargeFlag);
void Set_CenterButton(bool ButtonMode);
void Set_PlayMode(uint8_t mode);
void Set_PlayListName(char *str, uint8_t flag);
void Set_FirstDisplay();  //<-
void Set_MusicPosition(uint8_t pos);
void Set_PowerDownDisplay();
void Interface_Task(void* arg);

#endif