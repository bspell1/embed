TARGETPATH = /usr/local/lib
TARGETNAME = monoext
TARGETVER  = 1.0.1
MODULES    = spi i2c

CC = gcc
LD = gcc
CP = sudo cp
CCFLAGS = -c 
LDFLAGS = -shared -Wl,-soname=$(@:bin/%.so.$(TARGETVER)=%.so) -o$@ -lc

all: bin/ bin/lib$(TARGETNAME).so.$(TARGETVER)

clean: ; rm -f bin/*

rebuild: clean all

bin/: ; mkdir bin/

%.c: monoext.h

bin/%.o: %.c ; $(CC) $(CCFLAGS) $? -o $@

bin/lib$(TARGETNAME).so.$(TARGETVER) : $(MODULES:%=bin/%.o) ; \
   $(LD) $(LDFLAGS) $(MODULES:%=bin/%.o) ; \
   $(CP) $@ $(TARGETPATH)
