# README (English version)

Audio player
Firmware ver.6

## Notice
This firmware use "ESP8266Audio" and "Adafruit_GFX" libraries etc.
These libraries are not developed by me. So, I will delete this repository if I pointed out by these developers.

## Functions
・Volume adjust (20 steps)
・music selection
・Play and Pause button
add at ver.2
・Automatic playing (only a music, loop a music, playing all music)
add at ver.3
・Playlist (directory as playlist)

## music format
・mp3 (max 16bit,48kHz)

## hardware specification
### Microcontroller: ESP32-WROOM-32E
  CPU clock: 240MHz
  FLASH: 4MB
### DAC: PCM5102A
  Interface: I2S
  Resolution: 32bit
  Max frequency: 384kHz
  LPF: 470Ω + 2200pF (cut-off: 153.9kHz)
### LCD: MSP2807
  Interface: SPI
  LCD driver: ILI9341
  Touch controller: TSC2046
### Battery: Litium polimer
  Voltage: 3.82V (3.3~4.3V)
  Capacity: 1821mAh
  Manifacturer: Apple Inc. (This battery from iPhone SE2)
### Charge controller: MCP73831
  Charge current: 500mA


## change logs
### ver.3.1.1
Modified to display "nichicon lab" and the logo.

### ver.3.1.2
Fixed the problem that title is not updated when changing music in MODE_LOOP_MUSIC mode.

### ver.3.1.3
Fixed the bug that file path is not correct.

### ver.3.2.1 (2024/05/05)
To reduce power and extend operating time, backlight is turned off if no operation for a minute.
Add the control circuit.

### ver.3.2.2 (2024/07/15)
Changed to the battery of iPhone SE2.
Battery voltage is changed to 4.3V from 4.2V, so, I fixed calculation formula of the battery level.
Add the battery charge controller and be able to control the charging.
STAT pin of MCP73831 is push pull output, it is possible to get the charging status.

### ver.3.2.3 (2024/08/15)
Modified to turn on the backlight after LCD initializaion.

### ver.3.2.4X (NOT implemented)
//Fixed a bug that music is begun with second one if you changed playlist when playing music.

### ver.4.1.1 (2024/09/24)
Modified to adjust the volume match with the sense of hearing of human.

### ver.4.2.1 (2024/10/20)
Modified to display a music playing position in visible.

### ver.4.3.1 (2024/11/02)
Changed the power switch to push switch from slide switch.
Read the switch status at the switch, turn to deep sleep.

### ver.6.1.1X (2025/02/20?)
Add a function that ESP32 gets battery charging status from charging controller.


## rules of version change
ver.A.B.C(X)
A: Changed when large repair, for example, changing the hardware.
B: Changed when add functions by software.
C: Changed when bug fixed.
'X' of version string means unimplemented version.
