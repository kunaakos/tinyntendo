#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>  /* for sei() */
#include <avr/wdt.h>  /* watchdog timer */

#include <util/delay.h>     /* for _delay_ms() */

// oscillator calibration routine
#include "libs-device/osccal.h"

#include "snes/snes.h"

// v-usb lib
#include "usbconfig.h"
#include "usbdrv/usbdrv.h"
#include "usbdrv/oddebug.h"        /* debug macros */

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
