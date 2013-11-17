PLATFORM = i686-linux
CC = gcc

TARGET = camInfoBlaster 
LIBDIR = $(QEO_HOME)/c/lib/$(PLATFORM)
LIBS = -lqeo -lqeoutil -lrt -lpthread
CFLAGS = -m32 -g -Wall  -I$(QEO_HOME)/c/include

ifeq ($(QEO_HOME),)
$(error QEO_HOME is not defined)
endif

.PHONY: default all clean

default: $(TARGET)
all: default

OBJECTS = $(patsubst %.c,%.o,$(wildcard *.c))
HEADERS = $(wildcard *.h)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

QMasterCam.o: ../masterapp/QMasterCam.c ../masterapp/QMasterCam.h 
	$(CC) $(CFLAGS) -c $< -o $@

QIRBlaster.o: ../irblasterpi/QIRBlaster.c ../irblasterpi/QIRBlaster.h
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJECTS) QMasterCam.o QIRBlaster.o
	$(CC) -pthread -m32 -L$(LIBDIR) -Wl,-rpath,$(LIBDIR) $(OBJECTS) -Wall $(LIBS) -o $@ QMasterCam.o QIRBlaster.o
	@echo "$(TARGET) created."

clean:
	-rm -f *.o
	-rm -f $(TARGET)
