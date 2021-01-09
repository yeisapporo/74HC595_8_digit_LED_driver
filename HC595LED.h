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
const unsigned int _setNumber[] = {
  /*  0 */ 0b00000011, // 0
  /*  1 */ 0b10011111, // 1
  /*  2 */ 0b00100101, // 2
  /*  3 */ 0b00001101, // 3
  /*  4 */ 0b10011001, // 4
  /*  5 */ 0b01001001, // 5
  /*  6 */ 0b01000001, // 6
  /*  7 */ 0b00011011, // 7
  /*  8 */ 0b00000001, // 8
  /*  9 */ 0b00001001, // 9
  /* 10 */ 0b11111111, // blank
  /* 11 */ 0b01101111, // colon
  /* 12 */ 0b11111110, // .
  /* 13 */ 0b00010001, // A
  /* 14 */ 0b11000001, // B
  /* 15 */ 0b01100011, // C
  /* 16 */ 0b10000101, // D
  /* 17 */ 0b01100001, // E
  /* 18 */ 0b01110001, // F
  /* 19 */ 0b01000011, // G
  /* 20 */ 0b10010001, // H
  /* 21 */ 0b11011111, // I
  /* 22 */ 0b10000111, // J
  /* 23 */ 0b01010001, // K
  /* 24 */ 0b11100011, // L
  /* 25 */ 0b00010011, // M
  /* 26 */ 0b11010101, // N
  /* 27 */ 0b11000101, // O
  /* 28 */ 0b00110001, // P
  /* 29 */ 0b00100001, // Q
  /* 30 */ 0b11110101, // R
  /* 31 */ 0b11001001, // S
  /* 32 */ 0b11100001, // T
  /* 33 */ 0b11000111, // U
  /* 34 */ 0b10000011, // V
  /* 35 */ 0b10000001, // W
  /* 36 */ 0b10010001, // X
  /* 37 */ 0b10001001, // Y
  /* 38 */ 0b00100111, // Z

};

// character map
const unsigned char _charMap[256] = {
//       0   1   2   3   4   5   6   7   8   9   a   b   c   d   e   f
/* 0 */ 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 
/* 1 */ 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 
/* 2 */ 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 12, 10, 
/* 3 */  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 11, 10, 10, 10, 10, 10,
/* 4 */ 10, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 
/* 5 */ 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 10, 10, 10, 10, 10, 
/* 6 */ 10, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 
/* 7 */ 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 10, 10, 10, 10, 10, 
/* 8 */ 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 
/* 9 */ 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 
/* a */ 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 
/* b */ 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 
/* c */ 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 
/* d */ 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 
/* e */ 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 
/* f */ 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 
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
