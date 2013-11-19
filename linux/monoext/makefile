TARGETPATH = /usr/local/lib
TARGETNAME = monoext
TARGETVER  = 1.0.1
MODULES    = math spi i2c eventfd

CC = gcc
LD = gcc
CP = sudo cp
CCFLAGS = -Wall -Wextra -Winline 														\
			 -Wno-missing-field-initializers 											\
			 -O3 -funroll-loops
LDFLAGS = -shared -Wl,-soname=$(@:bin/%.so.$(TARGETVER)=%.so) -o$@

all: bin/ bin/lib$(TARGETNAME).so.$(TARGETVER)

clean: ; rm -f bin/*

rebuild: clean all

bin/: ; mkdir bin/

%.c: monoext.h

bin/%.o: %.c ; $(CC) $(CCFLAGS) -c $? -o $@

bin/lib$(TARGETNAME).so.$(TARGETVER) : $(MODULES:%=bin/%.o) ; \
   $(LD) $(LDFLAGS) $(MODULES:%=bin/%.o) ; \
   $(CP) $@ $(TARGETPATH)
