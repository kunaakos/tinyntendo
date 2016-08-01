#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>  /* for sei() */
#include <avr/wdt.h>  /* watchdog timer */

#include <util/delay.h>     /* for _delay_ms() */

// oscillator calibration routine
#include "osccal/osccal.h"

// #include "snes.h"

// v-usb lib
#include "usbconfig.h"
#include "usbdrv/usbdrv.h"
#include "usbdrv/oddebug.h"        /* debug macros */

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

const PROGMEM char usbHidReportDescriptor[42] = {
    0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
    0x09, 0x05,                    // USAGE (Game Pad)
    0xa1, 0x01,                    // COLLECTION (Application)
    0x09, 0x01,                    //   USAGE (Pointer)
    0xa1, 0x00,                    //   COLLECTION (Physical)
    0x09, 0x30,                    //     USAGE (X)
    0x09, 0x31,                    //     USAGE (Y)
    0x15, 0x00,                    //     LOGICAL_MINIMUM (0)
    0x25, 0x02,                    //     LOGICAL_MAXIMUM (2)
    0x75, 0x02,                    //     REPORT_SIZE (2)
    0x95, 0x02,                    //     REPORT_COUNT (2)
    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
    0xc0,                          //   END_COLLECTION
    0x05, 0x09,                    //   USAGE_PAGE (Button)
    0x19, 0x01,                    //   USAGE_MINIMUM (Button 1)
    0x29, 0x08,                    //   USAGE_MAXIMUM (Button 8)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x25, 0x01,                    //   LOGICAL_MAXIMUM (1)
    0x75, 0x01,                    //   REPORT_SIZE (1)
    0x95, 0x08,                    //   REPORT_COUNT (8)
    0x81, 0x02,                    //   INPUT (Data,Var,Abs)
    0xc0                           // END_COLLECTION
};

// USB reset hook, see usbconfig.h
void hadUsbReset(void) {
  cli();  // usbMeasureFrameLength() counts CPU cycles, so disable interrupts.
  calibrateOscillator();
  sei();
}

usbMsgLen_t usbFunctionSetup(uchar data[8]) {
  return 0; // Nothing implemented
}

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

/* SNES STUFF START */

#define CLOCK 2
#define STROBE 1
#define DATA 0

static void initSNES( void )
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

static uint8_t readBit( void )
{
  uint8_t val;
  // read DATA pin, remember: LOW means pressed
  val = (~PINB & (1 << DATA));
  strobeCLock();
  return val;
}

static void readSNES( void )
{
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

/* SNES STUFF END */

int main(void) {

  // enable Watchdog Timer
  wdt_enable(WDTO_1S);

  // Disable analog comparator to save power
  ACSR |= (1<<ACD);

  // force USB re-enumeration
  usbDeviceDisconnect();
  _delay_ms(500);
  usbDeviceConnect();

  initSNES();

  // init USB
  usbInit();

  // enable interrupts
  sei();

  // MAIN LOOP START
  for(;;) {

    // pet the Watchdog
    wdt_reset();

    // poll USB
    usbPoll();

    // send report if host is ready
    if(usbInterruptIsReady()){
        readSNES();
        usbSetInterrupt((void *)&reportBuffer, sizeof(reportBuffer));
    }

  }
  // MAIN LOOP end

}
