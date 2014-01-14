TARGET = xbeetest
CC = gcc
CFLAGS = -pedantic -Wall -I../include 
LDFLAGS = -L/opt/lib -lqeoxbee -lpthread -lrt

SOURCES = xbeetest.c
HEADERS = ../include
OBJECTS = $(SOURCES:.c=.o)

BINDIR = $(PREFIX)/bin
PREFIX = $(DESTDIR)/usr/local

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(TARGET) $(OBJECTS)

clean:
	rm $(TARGET); rm *.o
