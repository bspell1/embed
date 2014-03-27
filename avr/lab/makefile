TARGETNAME 	= 	lab
MODULES    	= 	lab
FWMODULES   =	uart i2cmast spimast mpu6050 tlc5940 nrf24
DEVICE     	= 	atmega328p
PARAMETERS	= 	F_CPU=16000000																\
					DEBUG_TRACE=UartTrace													\
					UART_BAUD=57600															\
					UART_BAUD_2X=0																\
					UART_SEND=1																	\
					UART_RECV=1																	\
					I2C_BUFFER_SIZE=16														\
					SPI_BUFFER_SIZE=33														\
				 	SPI_FREQUENCY=8000000													\
					TLC5940_FREQ=390.625														\
					TLC5940_BLSCALE=256														\
					TLC5940_BLTICK=1															\



include ../fw/base.mak
