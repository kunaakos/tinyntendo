# tinyntendo

ATtiny based Nintendo to USB adapter firmware to be used with Digispark boards (you can also make your own). See [Digispark product page](http://digistump.com/products/1) for schematics and other info, code for controller wiring.

**Project is set up for the [USBasp programmer](http://www.fischl.de/usbasp/).** Because it's cheap and brilliant, that's why.

Let's assume you have [avrdude](http://www.nongnu.org/avrdude/) and [avr-gcc](http://www.nongnu.org/avr-libc/user-manual/overview.html) working on your machine.

To compile for **NES** controllers:

```
$ make clean && make nes
```

To compile for **SNES** controllers:

```
$ make clean && make snes
```

... after compiling you can **either**:

### A) flash and set fuses with a USBasp

This overwrites the bootloader on the Digispark boards, but you'll get a bog standard HID device.

```
$ make install
```

... **or**:

### B) upload with the [factory bootloader](https://github.com/micronucleus) on Digispark boards

This way you don't need an AVR programmer, but:

 * you'll have to wait roughly 5 seconds every time after plugging the controller in
 * the controller will probably be less compatible with devices, since the bootloader runs before the device switches into HID mode
 * you might have trouble uploading with some laptops, USB hubs and/or cables

But hey, upgrading firmware is easier using a bootloader.

If you're on linux/osx and have the micronucleus command line executable symlinked to **/usr/local/bin**, just run:

```
$ sudo make upload
```
... and plug the thing in when prompted.

You can also mess with [UDEV rules](https://github.com/micronucleus/micronucleus/blob/master/commandline/49-micronucleus.rules) to avoid sudoing.


If you didn't do the symlinking or you're running windows, just use the [micronucleus command line utility](https://github.com/micronucleus/micronucleus/tree/master/commandline) any way you usually would, and upload **main.hex**

**Don't forget to run this step as root!**

# todo

* other controllers? N64 and GC would require some Digispark modding.
* smarter **make** wizardry and project setup
