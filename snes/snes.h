#ifndef SNES_H
#define SNES_H

#include <avr/io.h>

#define CLOCK 2
#define STROBE 1
#define DATA 0

typedef union {
  unsigned data:12;
  struct {
    uint8_t xAxis:2;
    uint8_t yAxis:2;
    uint8_t select:1;
    uint8_t start:1;
    uint8_t a:1;
    uint8_t b:1;
    uint8_t x:1;
    uint8_t y:1;
    uint8_t l:1;
    uint8_t r:1;
  };
} reportBuffer_SNES;

reportBuffer_SNES reportBuffer;

void initSNES( void );
void readSNES( void );

#endif
