
#include "Interface.h"

static ButtonStatus_t myStatus;

SPIClass vspi = SPIClass(VSPI);
Adafruit_ILI9341 tft = Adafruit_ILI9341(&vspi, TFT_DC, TFT_CS, TFT_RST);
TSC2046 touch = TSC2046(&vspi, 4);

const uint16_t GUI_X0 = 120;
const uint16_t GUI_Y0 = 220; //220->230
const uint16_t GUI_D0 = 70;  //70->60

//const String versionString = "ver.3.2.1";  //<-20240505
//const String versionString = "ver.3.2.2";  //<-20240715
//const String versionString = "ver.3.2.3";  //<-20240815
//const String versionString = "ver.4.1.1";  //<-20240924
//const String versionString = "ver.4.2.1";  //<-20241020
const String versionString = "ver.4.3.1";  //<-20241102

void Clear_ButtonStatus(ButtonStatus_t *BtnStatus){
    BtnStatus->BtnL = true;
    BtnStatus->BtnC = true;
    BtnStatus->BtnR = true;
    BtnStatus->BtnT = true;
    BtnStatus->BtnB = true;
    BtnStatus->BtnM = true;
    BtnStatus->BtnPL = true;
    BtnStatus->BtnPR = true;
}

void Interface_Init(){

    ledcSetup(0, 16000, 4);     //<-20240505
    ledcAttachPin(TFT_LED, 0);
    //ledcWrite(0, 15);
    ledcWrite(0, 0);

    //pinMode(TFT_RST, OUTPUT);
    //digitalWrite(TFT_RST, LOW);
    //vTaskDelay(100);
    //digitalWrite(TFT_RST, HIGH);
    tft.begin();
    tft.setRotation(2);
    tft.fillScreen(ILI9341_BLACK);

    ledcWrite(0, 15);  //<-20240815

    Set_FirstDisplay();
    
    tft.drawTriangle(GUI_X0-10, GUI_Y0-10, GUI_X0-10, GUI_Y0+10, GUI_X0+10, GUI_Y0, ILI9341_BLUE);  //Center: Play button
    tft.drawTriangle(GUI_X0-10, GUI_Y0-GUI_D0, GUI_X0+10, GUI_Y0-GUI_D0, GUI_X0, GUI_Y0-GUI_D0-20, ILI9341_BLUE);  //Up button
    tft.drawTriangle(GUI_X0-10, GUI_Y0+GUI_D0, GUI_X0+10, GUI_Y0+GUI_D0, GUI_X0, GUI_Y0+GUI_D0+20, ILI9341_BLUE);  //Down button
    tft.drawTriangle(GUI_X0-GUI_D0-20,  GUI_Y0, GUI_X0-GUI_D0, GUI_Y0-10, GUI_X0-GUI_D0, GUI_Y0+10, ILI9341_BLUE); //Left button
    tft.drawFastVLine(GUI_X0-GUI_D0-20, GUI_Y0-10, 20, ILI9341_BLUE);
    tft.drawTriangle(GUI_X0+GUI_D0, GUI_Y0-10, GUI_X0+GUI_D0, GUI_Y0+10, GUI_X0+GUI_D0+20, GUI_Y0, ILI9341_BLUE);  //Right button
    tft.drawFastVLine(GUI_X0+GUI_D0+20, GUI_Y0-10, 20, ILI9341_BLUE);
    tft.setTextSize(2);
    
    Clear_ButtonStatus(&myStatus);
    touch.Init();
}

void Set_Brightness(uint8_t level){
    uint8_t value;
    if(level > 15){
        value = 15;
    }else{
        value = level;
    }
    ledcWrite(0, value);
}

ButtonStatus_t Get_ButtonStatus(){
    ButtonStatus_t result = myStatus;
    
    Clear_ButtonStatus(&myStatus);
    return result;
}

void Set_TitleString(char *str){
    tft.fillRect(20, 90, 220, 16, ILI9341_BLACK);
    tft.setTextColor(ILI9341_CYAN);
    tft.setCursor(20, 90);
    tft.print(str);
}

void Set_VolumeBar(uint8_t v){
    tft.fillRect(10, 30, 5, 100, ILI9341_LIGHTGREY);
    tft.fillRect(10, 130-v*5, 5, v*5, ILI9341_BLUE);
}

void Set_NextTitle(char *str, uint8_t flag){
    tft.fillRect(20, 60, 220, 16, ILI9341_BLACK);
    tft.setTextColor(ILI9341_GREEN);
    tft.setCursor(20, 60);
    tft.print(str);
}

void Set_BatteryLevel(uint8_t v, bool ChargeFlag){
    //tft.fillRect(190, 0, 50, 16, ILI9341_ORANGE);
    //tft.fillRect(187, 4, 3, 8, ILI9341_ORANGE);
    if(ChargeFlag == true){
        tft.fillRect(190, 0, 50, 16, ILI9341_GREEN);
        tft.fillRect(187, 4, 3, 8, ILI9341_GREEN);
    }else{
        tft.fillRect(190, 0, 50, 16, ILI9341_ORANGE);
        tft.fillRect(187, 4, 3, 8, ILI9341_ORANGE);
    }
    tft.setTextColor(ILI9341_BLACK);
    tft.setCursor(197, 1);  //197,0->197,1
    tft.printf("%3d", v);
}

void Set_CenterButton(bool ButtonMode){
    tft.fillRect(GUI_X0-10, GUI_Y0-10, 20, 20, ILI9341_BLACK);
    if(ButtonMode){  //true: Playing and pause button icon
        tft.drawRect(GUI_X0-10, GUI_Y0-10, 8, 20, ILI9341_BLUE);
        tft.drawRect(GUI_X0+2, GUI_Y0-10, 8, 20, ILI9341_BLUE);
    }else{  //false: Pausing and play button icon
        tft.drawTriangle(GUI_X0-10, GUI_Y0-10, GUI_X0-10, GUI_Y0+10, GUI_X0+10, GUI_Y0, ILI9341_BLUE);
    }
}

void Set_PlayMode(uint8_t mode){
    tft.fillRect(GUI_X0+GUI_D0, GUI_Y0-GUI_D0-20, 20, 20, ILI9341_BLACK);
    switch(mode){
        case MODE_NORMAL:
            tft.drawCircle(GUI_X0+GUI_D0+10, GUI_Y0-GUI_D0-10, 8, ILI9341_WHITE);
            tft.drawFastVLine(GUI_X0+GUI_D0+2, GUI_Y0-GUI_D0-8, 3, ILI9341_WHITE);
            tft.drawFastHLine(GUI_X0+GUI_D0+2, GUI_Y0-GUI_D0-8, 3, ILI9341_WHITE);
            tft.drawFastVLine(GUI_X0+GUI_D0+18, GUI_Y0-GUI_D0-15, 3, ILI9341_WHITE);
            tft.drawFastHLine(GUI_X0+GUI_D0+15, GUI_Y0-GUI_D0-12, 3, ILI9341_WHITE);
            tft.drawLine(GUI_X0+GUI_D0+2, GUI_Y0-GUI_D0-20, GUI_X0+GUI_D0+18, GUI_Y0-GUI_D0-1, ILI9341_WHITE);
            break;

        case MODE_LOOP_MUSIC:
            tft.drawCircle(GUI_X0+GUI_D0+10, GUI_Y0-GUI_D0-10, 8, ILI9341_WHITE);
            tft.drawFastVLine(GUI_X0+GUI_D0+2, GUI_Y0-GUI_D0-8, 3, ILI9341_WHITE);
            tft.drawFastHLine(GUI_X0+GUI_D0+2, GUI_Y0-GUI_D0-8, 3, ILI9341_WHITE);
            tft.drawFastVLine(GUI_X0+GUI_D0+18, GUI_Y0-GUI_D0-15, 3, ILI9341_WHITE);
            tft.drawFastHLine(GUI_X0+GUI_D0+15, GUI_Y0-GUI_D0-12, 3, ILI9341_WHITE);
            tft.drawFastVLine(GUI_X0+GUI_D0+10, GUI_Y0-GUI_D0-16, 12, ILI9341_WHITE);
            break;

        case MODE_AUTO:
            tft.drawCircle(GUI_X0+GUI_D0+10, GUI_Y0-GUI_D0-10, 8, ILI9341_WHITE);
            tft.drawFastHLine(GUI_X0+GUI_D0+2, GUI_Y0-GUI_D0-10, 14, ILI9341_WHITE);
            tft.drawTriangle(GUI_X0+GUI_D0+12, GUI_Y0-GUI_D0-12, GUI_X0+GUI_D0+12, GUI_Y0-GUI_D0-8, GUI_X0+GUI_D0+16, GUI_Y0-GUI_D0-10, ILI9341_WHITE);
            break;

        default: break;
    }
}

void Set_PlayListName(char *str, uint8_t flag){
    int x1 = 40, x2 = 210, y = 35;
    if(flag == 0){
        tft.fillTriangle(x1-5, y, x1+5, y-5, x1+5, y+5, ILI9341_WHITE);
        tft.fillTriangle(x2-5, y-5, x2-5, y+5, x2+5, y, ILI9341_WHITE);
    }else if(flag == 1){
        tft.fillTriangle(x1-5, y, x1+5, y-5, x1+5, y+5, ILI9341_WHITE);
        tft.fillTriangle(x2-5, y-5, x2-5, y+5, x2+5, y, ILI9341_BLACK);
    }else if(flag == 2){
        tft.fillTriangle(x1-5, y, x1+5, y-5, x1+5, y+5, ILI9341_BLACK);
        tft.fillTriangle(x2-5, y-5, x2-5, y+5, x2+5, y, ILI9341_WHITE);
    }else{
        tft.fillTriangle(x1-5, y, x1+5, y-5, x1+5, y+5, ILI9341_BLACK);
        tft.fillTriangle(x2-5, y-5, x2-5, y+5, x2+5, y, ILI9341_BLACK);
    }
    tft.fillRect(20, 42, 220, 16, ILI9341_BLACK);
    tft.setTextColor(ILI9341_GREEN);
    tft.setCursor(20, 42);
    tft.print(str);
}

void Set_FirstDisplay(){
    uint16_t x0 = 100, y0 = 160, r = 20;
    uint16_t x1, y1, x2, y2;
    float radian = 0.0;
    tft.setCursor(48, 120);
    tft.setTextSize(2);
    tft.setTextColor(ILI9341_WHITE);
    tft.print("nichicon lab");
    tft.setCursor(66, 304);
    tft.print(versionString);  //<-20240505
    vTaskDelay(500/portTICK_PERIOD_MS);
    tft.drawTriangle(120, 140, 120, 180, 140, 160, ILI9341_WHITE);
    vTaskDelay(300/portTICK_PERIOD_MS);
    x1 = x0 + cos(radian)*r;
    y1 = y0 + sin(radian)*r;

    for(uint8_t n=0; n<7; n++){
        radian += 0.897;
        x2 = x0 + cos(radian)*r;
        y2 = y0 + sin(radian)*r;
        
        uint16_t colorIndex;
        switch(n){  //n -> idx, 20240505
            case 0: colorIndex = ILI9341_RED;
            break;

            case 1: colorIndex = ILI9341_ORANGE;
            break;

            case 2: colorIndex = ILI9341_YELLOW;
            break;

            case 3: colorIndex = ILI9341_GREENYELLOW;
            break;

            case 4: colorIndex = ILI9341_GREEN;
            break;

            case 5: colorIndex = ILI9341_BLUE;
            break;

            case 6: colorIndex = ILI9341_PURPLE;
            break;

            default: colorIndex = ILI9341_BLACK;
            break;
        }
        tft.drawLine(x1, y1, x2, y2, colorIndex);
        x2 = x1;
        y2 = y1;
        vTaskDelay(300/portTICK_PERIOD_MS);
    }
    vTaskDelay(2000/portTICK_PERIOD_MS);
    tft.fillScreen(ILI9341_BLACK);
}

static uint8_t posOld = 0;
void Set_MusicPosition(uint8_t pos){
    if(pos > 200){
        pos = 200;
    }
    if(pos != posOld){
        posOld = pos;
        tft.drawRect(19, 120, 202, 5, ILI9341_CYAN);
        tft.fillRect(20, 121, pos, 3, ILI9341_CYAN);
        tft.fillRect(20+pos, 121, 200-pos, 3, ILI9341_BLACK);
    }
}

void Set_PowerDownDisplay(){
    tft.fillScreen(ILI9341_BLACK);
    tft.setCursor(96, 150);
    tft.setTextColor(ILI9341_CYAN);
    tft.print("Shutdown");
}

void Interface_Task(void *arg){
    TickType_t LastWakeTime;
    ButtonStatus_t OldStatus, NowStatus;
    uint16_t SleepCount = 0;
    
    Clear_ButtonStatus(&OldStatus);
    while(1){
        LastWakeTime = xTaskGetTickCount();

        TouchPosition_t position;
        position = touch.GetPositon();
        //Serial.printf("x: %d, y: %d\r\n", position.posX, position.posY);
        
        if(position.posY > GUI_Y0+GUI_D0){
            if(position.posX > GUI_X0-10 && position.posX < GUI_X0+10){
                NowStatus.BtnB = false;
            }
        }else if(position.posY > GUI_Y0+10 && position.posY < GUI_Y0+25){  //Y0-10, Y0+10
            if(position.posX > GUI_X0-GUI_D0-20 && position.posX < GUI_X0-GUI_D0){
                NowStatus.BtnL = false;
            }else if(position.posX > GUI_X0-10 && position.posX < GUI_X0+10){
                NowStatus.BtnC = false;
            }else if(position.posX > GUI_X0+GUI_D0 && position.posX < GUI_X0+GUI_D0+20){
                NowStatus.BtnR = false;
            }
        }else if(position.posY > GUI_Y0-GUI_D0-20 && position.posY < GUI_Y0-GUI_D0){
            if(position.posX > GUI_X0-10 && position.posX < GUI_X0+10){
                NowStatus.BtnT = false;
            }else if(position.posX > GUI_X0+GUI_D0 && position.posX < GUI_X0+GUI_D0+20){
                NowStatus.BtnM = false;
            }
        }else if(position.posY > 15 && position.posY < 35){  //25,45->15,35
            if(position.posX > 30 && position.posX < 50){
                NowStatus.BtnPL = false;
            }else if(position.posX > 200 && position.posX < 220){
                NowStatus.BtnPR = false;
            }
        }else{
            Clear_ButtonStatus(&NowStatus);
        }
        
        if(NowStatus.BtnL != OldStatus.BtnL){
            myStatus.BtnL = NowStatus.BtnL;
        }
        if(NowStatus.BtnC != OldStatus.BtnC){
            myStatus.BtnC = NowStatus.BtnC;
        }
        if(NowStatus.BtnR != OldStatus.BtnR){
            myStatus.BtnR = NowStatus.BtnR;
        }
        if(NowStatus.BtnT != OldStatus.BtnT){
            myStatus.BtnT = NowStatus.BtnT;
        }
        if(NowStatus.BtnB != OldStatus.BtnB){
            myStatus.BtnB = NowStatus.BtnB;
        }
        if(NowStatus.BtnM != OldStatus.BtnM){
            myStatus.BtnM = NowStatus.BtnM;
        }
        if(NowStatus.BtnPL != OldStatus.BtnPL){
            myStatus.BtnPL = NowStatus.BtnPL;
        }
        if(NowStatus.BtnPR != OldStatus.BtnPR){
            myStatus.BtnPR = NowStatus.BtnPR;
        }
        OldStatus = NowStatus;

        //Serial.printf("%d, %d\r\n", position.posX, position.posY);
        if(position.posX < 25){
            SleepCount++;
            if(SleepCount > SLEEP_CYCLE){
                SleepCount = SLEEP_CYCLE;
            }
            if(SleepCount == SLEEP_CYCLE){
                Set_Brightness(0);  //back light OFF
            }else if(SleepCount > (SLEEP_CYCLE-100)){
                Set_Brightness(6);
            }
        }else{
            if(SleepCount > (SLEEP_CYCLE-100)){
                Clear_ButtonStatus(&myStatus);
            }
            SleepCount = 0;
            Set_Brightness(15);  //back light ON
        }

        vTaskDelayUntil(&LastWakeTime, 100/portTICK_PERIOD_MS);
    }
}
