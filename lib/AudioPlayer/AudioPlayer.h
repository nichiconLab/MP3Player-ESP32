
#ifndef AUDIO_PLAYER_H
#define AUDIO_PLAYER_H

#include <Arduino.h>
#include <freertos/task.h>
#include <freertos/FreeRTOS.h>
#include <AudioFileSourceSD.h>
#include <AudioGeneratorMP3.h>
#include <driver/i2s.h>
#include <AudioOutputI2S.h>
#include <driver/adc.h>
#include <driver/rtc_io.h>  //20241102

#include "Interface.h"

//I2S pin config
#define BCLKpin 26
#define LRCKpin 33
#define DATApin 25

#define LCD_POWER_PIN 17
#define POWER_SW_PIN  15

#define BATTERY_FB_PIN 34
#define BATTERY_FB_ADC ADC1_CHANNEL_6
#define CHARGE_STATUS_PIN 35

void AudioPlayer_Init();
void AudioControl_Task(void *arg);
void AudioStream_Loop(void *arg);
void PowerControl_Task(void *arg);
void Get_FileList(uint8_t ListNumber);


#endif