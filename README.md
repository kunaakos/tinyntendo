# tinyntendo
---

ATtiny based Nintendo to USB adapter firmware to be used with Digispark boards (you can also make your own). See [Digispark product page](http://digistump.com/products/1) for schematics and other info, code for controller wiring.

Currently works with SNES controllers only, project is set up for USBasp programmer, to compile and write just:

```
$ make clean && make install
```

## todo

NES compatibility and eventually other controllers, too - although N64 and GC would require spme Digispark modding.