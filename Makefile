DEVICE     = attiny85           # it's what we're using
CLOCK      = 16500000           # 16.5Mhz, no external crystal needed
PROGRAMMER = -c usbasp -P usb   # using usbasp, modify if needed
_OBJECTS   := libs-device/osccal.o usbdrv/usbdrv.o usbdrv/usbdrvasm.o usbdrv/oddebug.o main.o

# fuse settings:
# use http://www.engbedded.com/fusecalc
FUSES = -U lfuse:w:0xe1:m -U hfuse:w:0xdd:m -U efuse:w:0xff:m

AVRDUDE = avrdude $(PROGRAMMER) -p $(DEVICE)
COMPILE = avr-gcc -Wall -Os -DF_CPU=$(CLOCK) -mmcu=$(DEVICE) -I. -D $(MODULE)

snes: OBJECTS := $(_OBJECTS) snes/snes.o
snes: MODULE := SNES
snes: snes/snes.o main.hex

nes: OBJECTS := $(_OBJECTS) nes/nes.o
nes: MODULE := NES
nes: nes/nes.o main.hex

.c.o:
	$(COMPILE) -c $< -o $@

.S.o:
	$(COMPILE) -x assembler-with-cpp -c $< -o $@

.c.s:
	$(COMPILE) -S $< -o $@

flash:
	$(AVRDUDE) -U flash:w:main.hex:i

fuse:
	$(AVRDUDE) $(FUSES)

install: flash fuse

clean:
	rm -f main.hex main.elf $(_OBJECTS) snes/snes.o nes/nes.o

# file targets:
main.elf: $(_OBJECTS)
	$(COMPILE) -o main.elf $(OBJECTS)

main.hex: main.elf
	rm -f main.hex
	avr-objcopy -j .text -j .data -O ihex main.elf main.hex
	avr-size --format=avr --mcu=$(DEVICE) main.elf

# Targets for code debugging and analysis:
disasm: main.elf
	avr-objdump -d main.elf

cpp:
	$(COMPILE) -E main.c
