#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>

#include <util/delay.h>

// oscillator calibration routine
#include "libs-device/osccal.h"

// v-usb lib
#include "usbconfig.h"
#include "usbdrv/usbdrv.h"
// #include "usbdrv/oddebug.h"

#include "snes/snes.h"

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

  initController();

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
        readController();
        usbSetInterrupt((void *)&reportBuffer, sizeof(reportBuffer));
    }

  }
  // MAIN LOOP end

}
