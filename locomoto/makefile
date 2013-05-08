TARGETNAME = locomoto
MODULES    = tlc5940 uart i2cmast sx1509 proto stepmoto locomoto
DEVICE     = atmega328p
F_CPU      = 16000000

CC      = avr-gcc
CCFLAGS = -std=gnu99 -Wall -Wextra -Winline 											\
			 -mint8 -O3 -funroll-loops -DF_CPU=$(F_CPU) -mmcu=$(DEVICE)
LDFLAGS = -lc -lm

all: bin bin\$(TARGETNAME).hex

clean: ; rm -fr bin

rebuild: clean all

asm: bin $(MODULES:%=bin\\%.S)

#flash: all ; avrdude -q -patmega328p -carduino -PCOM5 -b57600 -D -Uflash:w:bin/$(TARGETNAME).hex:i
flash: all ; avrdude -q -patmega328p -cusbtiny -B 1 -U flash:w:bin/$(TARGETNAME).hex

bin: ; mkdir bin

bin\\%.o: %.c makefile avrdefs.h $(MODULES:%=%.h) ; $(CC) $(CCFLAGS) -c -o $@ $<

bin\\%.S: %.c makefile avrdefs.h $(MODULES:%=%.h) ; $(CC) $(CCFLAGS) -S -o $@ $<

bin\$(TARGETNAME).elf: $(MODULES:%=bin\\%.o) ; $(CC) $(CCFLAGS) -o $@ $^ $(LDFLAGS)

bin\$(TARGETNAME).hex: bin\$(TARGETNAME).elf; 										\
	rm -f $@ ; 																					\
	avr-objcopy -j .text -j .data -O ihex $(^:bin\\%=bin/%) $(@:bin\\%=bin/%)
