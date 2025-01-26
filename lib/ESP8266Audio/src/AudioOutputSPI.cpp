/*
  AudioOutputI2S
  Base class for I2S interface port
  
  Copyright (C) 2017  Earle F. Philhower, III

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <Arduino.h>

//#include "driver/i2s.h"
#include <SPI.h>

#include "AudioOutputSPI.h"

SPIClass hspi(HSPI);
SPISettings mySettings = SPISettings(8000000, MSBFIRST, SPI_MODE0);

hw_timer_t *timer = NULL;
uint8_t dataR, dataL;

void IRAM_ATTR sound_out(){
  timerWrite(timer, 0);
  timerRestart(timer);
  digitalWrite(26, LOW);
  hspi.beginTransaction(mySettings);
  hspi.write(dataL);
  digitalWrite(26, HIGH);
  hspi.write(dataR);
  hspi.endTransaction();
  //Serial.println("ISR");
}

AudioOutputSPI::AudioOutputSPI()
{
  //this->portNo = port;
  this->i2sOn = false;
  //this->dma_buf_count = dma_buf_count;
  /*
  if (output_mode != EXTERNAL_I2S && output_mode != INTERNAL_DAC && output_mode != INTERNAL_PDM) {
    output_mode = EXTERNAL_I2S;
  }
  */
  //this->output_mode = output_mode;
  //this->output_mode = EXTERNAL_I2S;
  //this->use_apll = use_apll;

  //set defaults
  mono = false;
  lsb_justified = false;
  bps = 16;
  channels = 2;
  hertz = 44100;
  bclkPin = 14;
  wclkPin = 26;
  doutPin = 13;
  //SetGain(1.0);
  SetGain(0.01);  //<-

  timer = timerBegin(1, 10, true);
  timerAttachInterrupt(timer, &sound_out, true);
  timerAlarmWrite(timer, 179, true);
  timerAlarmDisable(timer);
}


AudioOutputSPI::~AudioOutputSPI()
{
  
    if (i2sOn) {
      audioLogger->printf("UNINSTALL I2S\n");
      //i2s_driver_uninstall((i2s_port_t)portNo); //stop & destroy i2s driver
      hspi.end();
    }
  
  i2sOn = false;
}

bool AudioOutputSPI::SetPinout()
{
    if (output_mode == INTERNAL_DAC || output_mode == INTERNAL_PDM)
      return false; // Not allowed
    
    /*
    i2s_pin_config_t pins = {
        .mck_io_num = 0, // Unused
        .bck_io_num = bclkPin,
        .ws_io_num = wclkPin,
        .data_out_num = doutPin,
        .data_in_num = I2S_PIN_NO_CHANGE};
    i2s_set_pin((i2s_port_t)portNo, &pins);
    */
    
    return true;
  
}

bool AudioOutputSPI::SetPinout(int bclk, int wclk, int dout)
{
  bclkPin = bclk;
  wclkPin = wclk;
  doutPin = dout;
  if (i2sOn)
    return SetPinout();

  return true;
}
bool AudioOutputSPI::SetRate(int hz)
{
  // TODO - have a list of allowable rates from constructor, check them
  this->hertz = hz;
  if (i2sOn)
  {
      //i2s_set_sample_rates((i2s_port_t)portNo, AdjustI2SRate(hz));
  }
  return true;
}

bool AudioOutputSPI::SetBitsPerSample(int bits)
{
  if ( (bits != 16) && (bits != 8) ) return false;
  this->bps = bits;
  return true;
}

bool AudioOutputSPI::SetChannels(int channels)
{
  if ( (channels < 1) || (channels > 2) ) return false;
  this->channels = channels;
  return true;
}

bool AudioOutputSPI::SetOutputModeMono(bool mono)
{
  this->mono = mono;
  return true;
}

bool AudioOutputSPI::SetLsbJustified(bool lsbJustified)
{
  this->lsb_justified = lsbJustified;
  return true;
}

bool AudioOutputSPI::begin(bool txDAC)
{
  
    if(!i2sOn){

    }
  
  pinMode(wclkPin, OUTPUT);
  //hspi.begin(bclkPin, -1, doutPin, wclkPin);
  hspi.begin(14, 12, 13, 15);
  hspi.setHwCs(false);
  timerAlarmEnable(timer);

  i2sOn = true;
  SetRate(hertz); // Default
  return true;
}

bool AudioOutputSPI::ConsumeSample(int16_t sample[2])
{

  //return if we haven't called ::begin yet
  if (!i2sOn)
    return false;

  int16_t ms[2];

  ms[0] = sample[0];
  ms[1] = sample[1];
  MakeSampleStereo16( ms );

  if (this->mono) {
    // Average the two samples and overwrite
    int32_t ttl = ms[LEFTCHANNEL] + ms[RIGHTCHANNEL];
    ms[LEFTCHANNEL] = ms[RIGHTCHANNEL] = (ttl>>1) & 0xffff;
  }
//"i2s_write_bytes" has been removed in the ESP32 Arduino 2.0.0,  use "i2s_write" instead.
//    return i2s_write_bytes((i2s_port_t)portNo, (const char *)&s32, sizeof(uint32_t), 0);

    //size_t i2s_bytes_written;
    //i2s_write((i2s_port_t)portNo, (const char*)&s32, sizeof(uint32_t), &i2s_bytes_written, 0);
    //return i2s_bytes_written;

/*
    SPI.beginTransaction(mySettings);
    digitalWrite(wclkPin, LOW);
    SPI.write16(Amplify(ms[LEFTCHANNEL]));
    digitalWrite(wclkPin, HIGH);
    SPI.write16(Amplify(ms[RIGHTCHANNEL]));
    SPI.endTransaction();
*/
    dataL = (Amplify(ms[LEFTCHANNEL]) + 32768) >> 8;
    dataR = (Amplify(ms[RIGHTCHANNEL]) + 32768) >> 8;
    return true;
}

void AudioOutputSPI::flush()
{
  
    // makes sure that all stored DMA samples are consumed / played
    int buffersize = 128 * this->dma_buf_count;
    int16_t samples[2] = {0x0, 0x0};
    for (int i = 0; i < buffersize; i++)
    {
      while (!ConsumeSample(samples))
      {
        delay(10);
      }
    }
    
}

bool AudioOutputSPI::stop()
{
  if (!i2sOn)
    return false;

  
  //i2s_zero_dma_buffer((i2s_port_t)portNo);
  hspi.end();
  
  i2sOn = false;
  return true;
}
