PLATFORM = arm-linux-gnueabihf

CPP = g++
CC = gcc

TARGET = master-app
LIBDIR = $(QEO_HOME)/c/lib/$(PLATFORM)
LIBS = -lqeo -lqeoutil -lrt -lpthread
CFLAGS = -g -Wall  -I$(QEO_HOME)/c/include

ifeq ($(QEO_HOME),)
$(error QEO_HOME is not defined)
endif

.PHONY: default all clean

default: $(TARGET)
all: default

OBJECTS = $(patsubst %.cpp,%.o,$(wildcard *.cpp))
HEADERS = $(wildcard *.h)

MasterApp.o: MasterApp.cpp
	$(CPP) $(CFLAGS) -c $< -o $@

QMasterCam.o: QMasterCam.c QMasterCam.h
	$(CC) $(CFLAGS) -c $< -o $@

QCamData.o: ../caminfopi/QCamData.c ../caminfopi/QCamData.h
	$(CC) $(CFLAGS) -c $< -o $@

QIRBlaster.o: ../irblasterpi/QIRBlaster.c ../irblasterpi/QIRBlaster.h
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJECTS) QCamData.o QIRBlaster.o QMasterCam.o
	$(CPP) -m32 -pthread -L$(LIBDIR) -Wl,-rpath,$(LIBDIR) QMasterCam.o QCamData.o QIRBlaster.o $(OBJECTS) -Wall $(LIBS) -o $@
	@echo "$(TARGET) created."

clean:
	-rm -f *.o
	-rm -f $(TARGET)
