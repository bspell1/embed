TARGETNAME  = 	locomoto
MODULES     = 	proto stepmoto locomoto
FWMODULES   =  uart i2cmast sx1509 tlc5940
DEVICE      = 	atmega328p
PARAMETERS	= 	F_CPU=16000000 															\
				 	I2C_FREQUENCY=400000														\
				 	I2C_BUFFER_SIZE=6															\
				 	UART_SEND=1																	\
				 	UART_RECV=1																	\
				 	UART_RECV_BUFFER_SIZE=0

CC      = avr-gcc
CCFLAGS = -std=gnu99 -Wall -Wextra -Winline 											\
			 -Wno-missing-field-initializers 											\
			 -O3 -funroll-loops -fdata-sections -ffunction-sections				\
			 -mint8 -mmcu=$(DEVICE)															\
			 -I..\fw																				\
			 $(PARAMETERS:%=-D% )
LDFLAGS = -flto -Wl,--gc-sections,--relax

all: bin bin/$(TARGETNAME).hex bin/$(TARGETNAME).eep

clean: ; rm -fr bin/*

rebuild: clean all

debug:
	$(MAKE) PARAMETERS='$(PARAMETERS) DEBUG' FWMODULES='$(FWMODULES) debug'

asm: $(MODULES:%=bin/%.S)

dump-size: bin/$(TARGETNAME).elf
	avr-size -C --mcu=$(DEVICE) $<

write-flash:  bin bin/$(TARGETNAME).hex
	avrdude -q -patmega328p -cusbtiny -B 1 -U flash:w:bin/$(TARGETNAME).hex
write-eeprom: bin bin/$(TARGETNAME).eep
	avrdude -q -patmega328p -cusbtiny -B 1 -U eeprom:w:bin/$(TARGETNAME).eep

bin:
	mkdir bin

bin/fw%.o: ../fw/%.c makefile ../fw/%.h
	$(CC) $(CCFLAGS) -c -o $@ $<

bin/%.o: %.c makefile $(MODULES:%=%.h)
	$(CC) $(CCFLAGS) -c -o $@ $<

bin/%.S: %.c makefile $(MODULES:%=%.h)
	$(CC) $(CCFLAGS) -S -o $@ $<

bin/%.elf: $(FWMODULES:%=bin/fw%.o) $(MODULES:%=bin/%.o)
	$(CC) $(CCFLAGS) -o $@ $^ $(LDFLAGS)

bin/%.hex: bin/%.elf
	avr-objcopy -j .text -j .data -O ihex $< $@

bin/%.eep: bin/%.elf
	avr-objcopy -j .eeprom -O ihex $< $@

# override make to keep intermediate files for incremental
.SECONDARY: 
