// a driver for 8-digit LED driven dynamically by two 74HC595's.
// optimized to arduino ESP32-WROOM-32D (PlatformIO).

#ifndef __H_HC595LED_H__
#define __H_HC595LED_H__

#include <Arduino.h>
#include <cstdio>

  static volatile char _cRam[17];
  static int _pinClk;     // SCK
  static int _pinDat;     // DIO
  static int _pinLatch;   // RCK

// first byte (LSB first, inverted)
const unsigned int _setNumber[12] = {
  0b00000011, // 0
  0b10011111, // 1
  0b00100101, // 2
  0b00001101, // 3
  0b10011001, // 4
  0b01001001, // 5
  0b01000001, // 6
  0b00011011, // 7
  0b00000001, // 8
  0b00001001, // 9
  0b11111111, // blank
  0b11101101, // colon
};

// character map
const unsigned char _charMap[256] = {
  10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 
  10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 
  10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 
   0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 11, 10, 10, 10, 10, 10,
  10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 
  10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 
  10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 
  10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 
  10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 
  10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 
  10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 
  10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 
  10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 
  10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 
  10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 
  10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 
}; 
// second byte (LSB first)
const unsigned int _setDigit[8] = {
  0b00010000,
  0b00100000,
  0b01000000,
  0b10000000,
  0b00000001,
  0b00000010,
  0b00000100,
  0b00001000,
};

class HC595LED {
  private:
  static void task7segDrv(void *param) {
    for(;;) {
      for(int i = 0; i < 8; i++) {
        shiftOut(_pinDat, _pinClk, LSBFIRST, _cRam[i]);
        digitalWrite(_pinLatch, LOW);
        shiftOut(_pinDat, _pinClk, LSBFIRST, _setDigit[i]);
        digitalWrite(_pinLatch, HIGH);
        delay(1);
      }
    }
  }
  public:
  HC595LED(int pinClk, int pinDat, int pinLatch) {
    memset((void *)_cRam, 0, sizeof(_cRam));

    _pinClk = pinClk;
    _pinDat = pinDat;
    _pinLatch = pinLatch;

    pinMode(_pinClk, OUTPUT);
    pinMode(_pinDat, OUTPUT);
    pinMode(_pinLatch, OUTPUT);

    for(int i = 0; i < 16; i++) {
      digitalWrite(_pinLatch, LOW);
      shiftOut(_pinDat, _pinClk, LSBFIRST, 0b11111111);
      digitalWrite(_pinLatch, HIGH);
    }
    xTaskCreateUniversal(task7segDrv, "task7segDrv", 1024, NULL, 1, NULL, 1);
    
  }

  int setToLED(char *str) {
    int len;

    len = strlen(str);
    int i;
    for(i = 0; i < len; i++) {
      if(str[i] == 0) {
        break;
      }
      char c = (str[i]);
      _cRam[len - 1 - i] = _setNumber[_charMap[c]];
    }
    _cRam[i] = 0;

    return 0;
  }
};

#endif /* __H_HC595LED_H__ */
