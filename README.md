# tinyntendo

ATtiny based Nintendo to USB adapter firmware to be used with Digispark boards (you can also make your own). See [Digispark product page](http://digistump.com/products/1) for schematics and other info, code for controller wiring.

Project is set up for USBasp programmer.

To compile for NES and flash just:

```
$ make clean && make nes && make install
```

Same for SNES:

```
$ make clean && make snes && make install
```

## todo

NES compatibility and eventually other controllers, too - although N64 and GC would require spme Digispark modding.
