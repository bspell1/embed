TARGETNAME 	= 	avrlab
MODULES    	= 	avrlab
DEVICE     	= 	atmega328p
PARAMETERS	= 	F_CPU=16000000

CC      = avr-gcc
CCFLAGS = -std=gnu99 -Wall -Wextra -Winline 											\
			 -Wno-missing-field-initializers 											\
			 -O3 -funroll-loops 																\
			 -mint8 -mmcu=$(DEVICE)															\
			 $(PARAMETERS:%=-D% )
LDFLAGS = -lc -lm -flto

all: bin\$(TARGETNAME).hex bin\$(TARGETNAME).eep

debug: PARAMETERS+=DEBUG UART_SEND_BUFFER_SIZE=128
debug: all

clean: ; rm -fr bin/*

rebuild: clean all

asm: $(MODULES:%=bin\\%.S)

dump-size: bin\$(TARGETNAME).elf
	avr-size -C --mcu=$(DEVICE) $<

write-flash:  bin bin\$(TARGETNAME).hex
	avrdude -q -patmega328p -cusbtiny -B 1 -U  flash:w:bin/$(TARGETNAME).hex
write-eeprom: bin bin\$(TARGETNAME).eep
	avrdude -q -patmega328p -cusbtiny -B 1 -U eeprom:w:bin/$(TARGETNAME).eep

bin:
	mkdir bin

bin\\%.o: %.c bin makefile avrdefs.h $(MODULES:%=%.h)
	$(CC) $(CCFLAGS) -c -o $@ $<

bin\\%.S: %.c bin makefile avrdefs.h $(MODULES:%=%.h)
	$(CC) $(CCFLAGS) -S -o $@ $<

bin\\%.elf: $(MODULES:%=bin\\%.o)
	$(CC) $(CCFLAGS) -o $@ $^ $(LDFLAGS)

bin\\%.hex: bin\%.elf
	avr-objcopy -j .text -j .data -O ihex $(<:bin\\%=bin/%) $(@:bin\\%=bin/%)

bin\\%.eep: bin\%.elf
	avr-objcopy -j .eeprom -O ihex $(<:bin\\%=bin/%) $(@:bin\\%=bin/%)

# override make to keep intermediate files for incremental
.SECONDARY: 
