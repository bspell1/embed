TARGETNAME = avrtest
MODULES    = avrtest
DEVICE     = atmega328p
CLOCK      = 16000000

CC = avr-gcc
CCFLAGS = -std=gnu99 -Wall -Wextra -Werror -Winline 								\
			 -mint8 -O3 -funroll-loops -DF_CPU=$(CLOCK) -mmcu=$(DEVICE)
LDFLAGS = -lc -lm

all: bin bin\$(TARGETNAME).hex

clean: ; rm -fr bin

rebuild: clean all

#flash: all ; avrdude -q -patmega328p -carduino -PCOM5 -b57600 -D -Uflash:w:bin/$(TARGETNAME).hex:i
flash: all ; avrdude -q -patmega328p -cusbtiny -B 1 -U flash:w:bin/$(TARGETNAME).hex

bin: ; mkdir bin

bin\\%.o: %.c makefile ; $(CC) $(CCFLAGS) -c -o $@ $<

bin\$(TARGETNAME).elf: $(MODULES:%=bin\\%.o) ; $(CC) $(CCFLAGS) -o $@ $^ $(LDFLAGS)

bin\$(TARGETNAME).hex: bin\$(TARGETNAME).elf; 										\
	rm -f $@ ; 																					\
	avr-objcopy -j .text -j .data -O ihex $(^:bin\\%=bin/%) $(@:bin\\%=bin/%)
