PLATFORM = arm-linux-gnueabihf
 
CPP = g++
CC = gcc

TARGET = switchApp
LIBDIR = $(QEO_HOME)/c/lib/$(PLATFORM)
LIBS = -L/opt/lib -lqeo -lqeoutil -lrt -lpthread -lboost_regex -lboost_thread -lexpect -ltcl8.5 -lboost_system -lqeoxbee
CFLAGS = -g -Wall -std=c++0x -I$(QEO_HOME)/c/include -I/usr/local/include -I../qeoxbee/include

ifeq ($(QEO_HOME),)
$(error QEO_HOME is not defined)
endif

.PHONY: default all clean

default: $(TARGET)
all: default

OBJECTS = $(patsubst %.C,%.o,$(wildcard *.C))
HEADERS = $(wildcard *.h)

switchApp.o: switchApp.C
	$(CPP) $(CFLAGS) -c $< -o $@
  
QIRBlaster.o: ../irblasterpi/QIRBlaster.c ../irblasterpi/QIRBlaster.h
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJECTS) QIRBlaster.o
	$(CPP) -m32 -pthread -L$(LIBDIR) -I/usr/local/include -Wl,-rpath,$(LIBDIR) QIRBlaster.o $(OBJECTS) -Wall $(LIBS) -o $@
	@echo "$(TARGET) created."

clean:
	-rm -f *.o
	-rm -f $(TARGET)
