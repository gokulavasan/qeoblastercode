PLATFORM = i686-linux
CPP = g++

TARGET = tcpClient
LIBS = -lboost_thread -lboost_system -pthread
CFLAGS = -g -m32 -Wall -std=c++0x

.PHONY: default all clean

default: $(TARGET)
all: default

#OBJECTS = $(patsubst %.C,%.o,$(wildcard *.C))
#HEADERS = $(wildcard *.h)

tcpClient.o: tcpEchoClient.C
	$(CPP) -m32 $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJECTS)
	$(CPP) tcpEchoClient.C -m32 -Wall $(LIBS) -o $@
	@echo "$(TARGET) created."

clean:
	-rm -f *.o
	-rm -f $(TARGET)
