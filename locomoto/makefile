TARGETNAME  = 	locomoto
MODULES     = 	tlc5940 uart i2cmast sx1509 proto stepmoto locomoto
DEVICE      = 	atmega328p
PARAMETERS	= 	F_CPU=16000000 															\
				 	I2C_FREQUENCY=400000														\
				 	I2C_BUFFER_SIZE=6															\
				 	UART_SEND=1																	\
				 	UART_RECV=1																	\
				 	UART_RECV_BUFFER_SIZE=0

CC      = avr-gcc
CCFLAGS = -std=gnu99 -Wall -Wextra -Winline 											\
			 -O3 -funroll-loops 																\
			 -mint8 -mmcu=$(DEVICE)															\
			 $(PARAMETERS:%=-D% )
LDFLAGS = -lc -lm -flto

all: bin\$(TARGETNAME).hex bin\$(TARGETNAME).eep

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
