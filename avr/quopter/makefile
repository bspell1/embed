TARGETNAME 	= 	quopter
MODULES    	= 	quopter quadmpu quadrotr
FWMODULES   =  pid tlc5940 i2cmast mpu6050 uart
DEVICE     	= 	atmega328p
PARAMETERS	= 	F_CPU=16000000																\
					UART_SEND=1																	\
					I2C_FREQUENCY=400000														\
					I2C_BUFFER_SIZE=16														\
					TLC5940_COUNT=1															\
					QUADCTRL_SAMPLE_RATE=0.01f

include ../fw/base.mak
