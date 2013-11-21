TARGETNAME 	= 	quopter
MODULES    	= 	quopter quadmpu quadrotr
FWMODULES   =  pid tlc5940 i2cmast mpu6050 uart
DEVICE     	= 	atmega328p
PARAMETERS	= 	F_CPU=16000000																\
					I2C_FREQUENCY=400000														\
					I2C_BUFFER_SIZE=16														\
					TLC5940_COUNT=1															\
					QUADMPU_SAMPLE_TIME=0.004f												\
					QUADROTOR_THRUST_MAX=0.75f												\
					QUADROTOR_PID_PGAIN=0.25f												\
					QUADROTOR_PID_IGAIN=0.0f												\
					QUADROTOR_PID_DGAIN=0.75f

include ../fw/base.mak
