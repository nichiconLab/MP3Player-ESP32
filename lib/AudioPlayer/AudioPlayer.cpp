
#include "AudioPlayer.h"

AudioOutputI2S *out;
AudioGeneratorMP3 *mp3 = NULL;
AudioFileSourceSD *file;

String fileList[200];
static int FileNum = 0;
String PlayList[100];
static int ListNum = 0;

static uint8_t VolumeIndex = 3;
static float AudioVolume = 0.015;

static bool PlayFlag = false;
static bool PauseFlag = false;

static uint8_t PlayMode = MODE_NORMAL;

SPIClass hspi = SPIClass(HSPI);

static int FileIndex = 0;
static int FileIndexOld = 0;

static int ListIndex = 0;
static int ListIndexOld = 0;

static bool PowerSaveFlag = false;


void AudioPlayer_Init(){
    Serial.begin(115200);
    
    Serial2.end();
    pinMode(LCD_POWER_PIN, OUTPUT);
    digitalWrite(LCD_POWER_PIN, LOW);
    pinMode(POWER_SW_PIN, INPUT_PULLUP);
    //vTaskDelay(100/portTICK_PERIOD_MS);
    Interface_Init();
    /*
    hspi.end();
    hspi.begin();
    */
    hspi.setHwCs(false);  //20241103
    while(!SD.begin(27, hspi, 24000000, "/sd")){
        char ErrorMessage[15] = {"(SD not found)"};
        Set_NextTitle(ErrorMessage, 3);
        vTaskDelay(1000);
    }

    File root = SD.open("/music", FILE_READ);
    if(!root){
        Serial.println("root error");
        while(1){
            vTaskDelay(10/portTICK_PERIOD_MS);
        }
        return;
    }
    while(1){
        File entry = root.openNextFile();
        if(!entry){
            Serial.println("!entry");
            ListNum--;
            break;
        }
        if(entry.isDirectory()){
            PlayList[ListNum] = entry.name();
            ListNum++;
        }else{
            /*
            String fileName = entry.name();
            String ext = fileName.substring(fileName.lastIndexOf('.'));
            if(ext.equalsIgnoreCase(".mp3")){
                PlayList[ListNum] = fileName;
                ListNum++;
            }
            */
        }
    }
    Serial.println("End initialization");

    xTaskCreatePinnedToCore(Interface_Task, "InterfaceTask", 4096, NULL, 1, NULL, 0);
    xTaskCreatePinnedToCore(AudioControl_Task, "AudioControl", 4096, NULL, 1, NULL, 0);
    xTaskCreatePinnedToCore(PowerControl_Task, "PowerControl", 4096, NULL, 1, NULL, 0);
    xTaskCreatePinnedToCore(AudioStream_Loop, "AudioStream", 4096, NULL, 2, NULL, 1);  //8192->4096
}


void Get_FileList(uint8_t ListNumber){
    String FolderPath = "/music/" + PlayList[ListNumber];
    char str[50];
    FolderPath.toCharArray(str, 50);
    File root = SD.open(str, FILE_READ);
    FileNum = 0;
    while(1){
        File entry = root.openNextFile();
        if(!entry){
            FileNum--;
            break;
        }
        if(!entry.isDirectory()){
            String fileName = entry.name();
            String ext = fileName.substring(fileName.lastIndexOf('.'));
            if(ext.equalsIgnoreCase(".mp3")){
                fileList[FileNum] = fileName;
                FileNum++;
            }
        }
    }
}

static bool MusicStarted = false;
static uint8_t MusicPos = 0;

void AudioControl_Task(void *arg){
    ButtonStatus_t buttonInfo;
    buttonInfo = Get_ButtonStatus();  //dummy read
    vTaskDelay(200/portTICK_PERIOD_MS);

    Set_VolumeBar(VolumeIndex);
    Set_PlayMode(MODE_NORMAL);  //<-
    char str[20];
    PlayList[0].toCharArray(str, 19);
    Set_PlayListName(str, 2);
    Get_FileList(0);
    fileList[0].toCharArray(str, 19);
    Set_NextTitle(str, 2);

    while(1){
        buttonInfo = Get_ButtonStatus();
        if(buttonInfo.BtnL == false){
            FileIndex--;
            uint8_t flagIndex = 0;
            if(FileIndex < 0){
                FileIndex = 0;
            }
            if(FileIndex == 0){
                flagIndex = 2;
            }
            char str[20];
            fileList[FileIndex].toCharArray(str, 19);  //20->19
            Set_NextTitle(str, flagIndex);
        }
        if(buttonInfo.BtnR == false){
            FileIndex++;
            uint8_t flagIndex = 0;
            if(FileIndex > FileNum){
                FileIndex = FileNum;
            }
            if(FileIndex == FileNum){
                flagIndex = 1;
            }
            char str[20];
            fileList[FileIndex].toCharArray(str, 19);  //20->19
            Set_NextTitle(str, flagIndex);
        }
        if(buttonInfo.BtnC == false){
            if(FileIndex != FileIndexOld || ListIndex != ListIndexOld || PlayFlag == false){
                PlayFlag = false;
                vTaskDelay(500/portTICK_PERIOD_MS);
                char str[20];
                fileList[FileIndex].toCharArray(str, 19);  //20->19
                //Set_DisplayString(str);
                Set_TitleString(str);
                PlayFlag = true;
                PauseFlag = false;
                Set_CenterButton(true);
                FileIndexOld = FileIndex;
                ListIndexOld = ListIndex;
            }else{
                if(PauseFlag == true){
                    PauseFlag = false;
                    Set_CenterButton(true);
                    Serial.println("Not pause");
                }else{
                    PauseFlag = true;
                    Set_CenterButton(false);
                    Serial.println("Pause");
                }
            }
        }
        if(buttonInfo.BtnT == false){
            VolumeIndex++;
            if(VolumeIndex > 20){
                VolumeIndex = 20;
            }
            Set_VolumeBar(VolumeIndex);
            //AudioVolume = VolumeIndex*0.005;
            AudioVolume = 0.010*pow(1.414, (float)VolumeIndex/3.0);
        }
        if(buttonInfo.BtnB == false){
            VolumeIndex--;
            if(VolumeIndex < 1){
                VolumeIndex = 1;
            }
            Set_VolumeBar(VolumeIndex);
            //AudioVolume = VolumeIndex*0.005;
            AudioVolume = 0.010*pow(1.414, (float)VolumeIndex/3.0);
        }

        if(buttonInfo.BtnM == false){
            PlayMode++;
            if(PlayMode > MODE_AUTO){
                PlayMode = MODE_NORMAL;
            }
            Set_PlayMode(PlayMode);
        }

        if(buttonInfo.BtnPL == false){
            ListIndex--;
            if(ListIndex < 0){
                ListIndex = 0;
            }
            uint8_t flag = 0;
            if(ListIndex == 0){
                flag = 2;
            }
            char str[20];
            PlayList[ListIndex].toCharArray(str, 19);
            Set_PlayListName(str, flag);
            Get_FileList(ListIndex);
            fileList[0].toCharArray(str, 19);
            Set_NextTitle(str, 2);
            FileIndex = 0;
        }
        if(buttonInfo.BtnPR == false){
            ListIndex++;
            if(ListIndex > ListNum){
                ListIndex = ListNum;
            }
            uint8_t flag = 0;
            if(ListIndex == ListNum){
                flag = 1;
            }
            char str[20];
            PlayList[ListIndex].toCharArray(str, 19);
            Set_PlayListName(str, flag);
            Get_FileList(ListIndex);
            fileList[0].toCharArray(str, 19);
            Set_NextTitle(str, 2);
            FileIndex = 0;
        }
        
        size_t FileSize = 0, NowPosition = 0;
        if(PlayFlag == true && MusicStarted == true){
            FileSize = file->getSize();
            NowPosition = file->getPos();
            uint8_t pos = 200*((float)NowPosition/FileSize);
            Set_MusicPosition(pos);
        }else{
            Set_MusicPosition(0);
        }
        
        vTaskDelay(150/portTICK_PERIOD_MS);  //150->100
    }
}

void AudioStream_Loop(void *arg){
    bool StartFlag = false;
    float OldVolume = AudioVolume;
    
    //out = new AudioOutputI2S(I2S_NUM_0, 0);
    out = new AudioOutputI2S(I2S_NUM_0, 0, 8, 1);  //Audio PLL enable
    out->SetPinout(BCLKpin, LRCKpin, DATApin);
    out->SetBitsPerSample(16);
    out->SetGain(AudioVolume);

    char str[20];  //18->20

    //size_t Fsize = 0, Fposition = 0;
    //uint8_t pos = 0;
    
    while(1){
        if(PlayFlag == true){
            if(StartFlag == false){
                String fileName = "/music/";
                fileName.concat(PlayList[ListIndex]);  //<-20240505
                fileName.concat("/");                  //<-20240505
                fileName.concat(fileList[FileIndex]);
                int length = fileName.length()+1;
                char buf[length];
                fileName.toCharArray(buf, length);
                file = new AudioFileSourceSD(buf);
                
                mp3 = new AudioGeneratorMP3();
                mp3->begin(file, out);

                //Serial.printf("Start: %s\r\n", fileList[n]);
                Serial.print("Start: ");
                Serial.println(fileList[FileIndex]);
                StartFlag = true;
                MusicStarted = true;  //ver.4.2.1
                //Fsize = file->getSize();
            }
            
            if(PauseFlag == false){
                if(mp3 != NULL){
                    if(AudioVolume != OldVolume){
                        out->SetGain(AudioVolume);
                        OldVolume = AudioVolume;
                    }
                    if(mp3->isRunning()){
                        //Fposition = file->getPos();
                        //pos = 200*((float)Fposition/Fsize);
                        if(!mp3->loop()){
                            mp3->stop();
                            Serial.println("End");
                            switch(PlayMode){
                                case MODE_NORMAL:
                                    PlayFlag = false;
                                    StartFlag = false;
                                    Set_CenterButton(false);
                                    //Set_TitleString("");
                                    break;

                                case MODE_LOOP_MUSIC:
                                    StartFlag = false;
                                    //char str[18];
                                    fileList[FileIndex].toCharArray(str, 19);
                                    Set_TitleString(str);
                                    break;

                                case MODE_AUTO:
                                    FileIndex++;
                                    StartFlag = false;
                                    //char str[18];
                                    if(FileIndex > FileNum){
                                        FileIndex = 0;
                                        PlayFlag = false;
                                        Set_CenterButton(false);
                                        //char str[18];
                                        fileList[FileIndex].toCharArray(str, 19);
                                        Set_NextTitle(str, 2);
                                        //Set_TitleString("");
                                    }else{
                                        //char TitleStr[18];
                                        fileList[FileIndex].toCharArray(str, 19);
                                        Set_TitleString(str);
                                        //fileList[n+1].toCharArray(str, 18);
                                        Set_NextTitle(str, 0);
                                    }
                                    break;
                            } //switch
                        } //if(!mp3->loop())
                    } //if(mp3->isRunning())
                } //if(mp3 != NULL)
            } //if(PauseFlag == false)
        }else{ //PlayFlag == false
            if(mp3 != NULL){
                mp3->stop();
            }
            StartFlag = false;
            MusicStarted = false;
        }
        //yield();
    }
    
}


void PowerControl_Task(void *arg){
    pinMode(BATTERY_FB_PIN, ANALOG);  //34: ADC1_CHANNEL_6
    adc1_config_channel_atten(BATTERY_FB_ADC, ADC_ATTEN_11db);  //full scale: 3.9V
    adc1_config_width(ADC_WIDTH_10Bit);                         //0-1023
    pinMode(CHARGE_STATUS_PIN, INPUT);

    bool ChargeStatus = false;

    //gpio_num_t LcdPowerPin = GPIO_NUM_17;
    //rtc_gpio_init(LcdPowerPin);
    //rtc_gpio_set_direction(LcdPowerPin, RTC_GPIO_MODE_OUTPUT_ONLY);
    //rtc_gpio_set_level(LcdPowerPin, 0);

    vTaskDelay(100/portTICK_PERIOD_MS);
    int val = 0;
    uint8_t n = 0;
    int BatLevel = 0;
    for(n=0; n<10; n++){
        //val += adc1_get_raw(ADC1_CHANNEL_6);
        val += adc1_get_raw(BATTERY_FB_ADC);
    }
    val /= 10;
    //BatLevel = (int)(((float)val-434)/1.18);
    BatLevel = (int)(((float)val-434)/1.30);
    if(BatLevel < 0){
        BatLevel = 0;
    }else if(BatLevel > 100){
        BatLevel = 100;
    }
    //Set_BatteryLevel(BatLevel);
    if(digitalRead(CHARGE_STATUS_PIN) == LOW){
        ChargeStatus = true;
    }else{
        ChargeStatus = false;
    }
    Set_BatteryLevel(BatLevel, false);
    val = 0;
    n = 0;
    uint8_t cnt = 0, LowCount = 0;
    while(1){
        cnt++;
        if(cnt == 6){
            cnt = 0;
            //val += adc1_get_raw(ADC1_CHANNEL_6);
            val += adc1_get_raw(BATTERY_FB_ADC);
            n++;
            if(n == 10){
                n = 0;
                val /= 10;
                BatLevel = (int)(((float)val-434)/1.30);
                if(BatLevel < 0){
                    BatLevel = 0;
                }else if(BatLevel > 100){
                    BatLevel = 100;
                }
                //Set_BatteryLevel(BatLevel);
                if(digitalRead(CHARGE_STATUS_PIN) == LOW){
                    ChargeStatus = true;
                }else{
                    ChargeStatus = false;
                }
                Set_BatteryLevel(BatLevel, ChargeStatus);
                val = 0;
            }
        }

        if(digitalRead(POWER_SW_PIN) == LOW){
            LowCount++;
            if(LowCount == 5){
                PlayFlag = false;
                vTaskDelay(100/portTICK_PERIOD_MS);
                Set_PowerDownDisplay();
                while(digitalRead(POWER_SW_PIN) == LOW);
                vTaskDelay(100/portTICK_PERIOD_MS);
                //rtc_gpio_set_level(LcdPowerPin, 1);
                digitalWrite(LCD_POWER_PIN, HIGH);
                esp_sleep_enable_ext0_wakeup((gpio_num_t)POWER_SW_PIN, LOW);
                esp_deep_sleep_start();
            }
        }else{
            LowCount = 0;
        }

        vTaskDelay(1000/portTICK_PERIOD_MS);
    }
}

