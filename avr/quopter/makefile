TARGETNAME 	= 	quopter
MODULES    	= 	quopter quadpsx quadmpu quadrotr quadbay quadtel
FWMODULES   =  pid i2cmast spimast nrf24 mpu6050 tlc5940
DEVICE     	= 	atmega328p
PARAMETERS	= 	F_CPU=16000000																\
					I2C_FREQUENCY=400000														\
					I2C_BUFFER_SIZE=16														\
					SPI_BUFFER_SIZE=32														\
				 	SPI_FREQUENCY=8000000													\
					TLC5940_COUNT=1															\
					TLC5940_FREQ=390.625														\
					TLC5940_BLSCALE=256														\
					TLC5940_BLTICK=1															\
					QUADPSX_ADDRESS=\"Psx00\"												\
					QUADMPU_SAMPLE_TIME=0.0085f											\
					QUADROTOR_THRUST_MAX=0.90f												\
					QUADROTOR_PID_PGAIN=\(0.05f\)											\
					QUADROTOR_PID_IGAIN=\(0.0f\)											\
					QUADROTOR_PID_DGAIN=\(0.01f\)

include ../fw/base.mak
