#ifndef SNES_H
#include "snes.h"
#endif

#include <avr/io.h>
#include <util/delay.h>

/* BIT MANIPULATION THINGIES START */

// clear bit
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
// set bit
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

/* BIT MANIPULATION THINGIES END */

static void strobeCLock(void) {
  sbi(PORTB, CLOCK);
  _delay_ms(1);
  cbi(PORTB, CLOCK);
  _delay_ms(1);
}

static void strobeLatch(void) {
  sbi(PORTB, STROBE);
  _delay_ms(1);
  cbi(PORTB, STROBE);
  _delay_ms(1);
}

static uint8_t readBit( void ) {
  uint8_t val;
  // read DATA pin, remember: LOW means pressed
  val = (~PINB & (1 << DATA));
  strobeCLock();
  return val;
}

void initSNES( void )
{
  // CLOCK and STROBE pins are outputs on micro, set corresponding
  // data direction registers to HIGH
  sbi(DDRB, CLOCK);
  sbi(DDRB, STROBE);
  // DATA pin is an input on micro, set DDR bit LOW
  cbi(DDRB, DATA);

  // set CLOCK and STROBE to LOW, as they should be on the start
  // of every controller read cycle
  cbi(PORTB, CLOCK);
  cbi(PORTB, STROBE);
}

void readSNES( void ) {
  reportBuffer.data = 0x05;

  strobeLatch();

  reportBuffer.b = readBit();
  reportBuffer.y = readBit();
  reportBuffer.select = readBit();
  reportBuffer.start = readBit();

  if (readBit()) {
    reportBuffer.yAxis = 0;
  }
  if (readBit()) {
    reportBuffer.yAxis = 2;
  }
  if (readBit()) {
    reportBuffer.xAxis = 0;
  }
  if (readBit()) {
    reportBuffer.xAxis = 2;
  }

  reportBuffer.a = readBit();
  reportBuffer.x = readBit();
  reportBuffer.l = readBit();
  reportBuffer.r = readBit();

  // should we read remaining 4 bits?
}
