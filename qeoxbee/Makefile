TARGET = libqeoxbee.so
CC = gcc
CFLAGS = -pedantic -Wall -Iinclude -I../libxbee.libxbee-v3
LDFLAGS = -shared

SOURCES = qeoxbeelib.c
HEADERS = include/qeoxbeelib.h
OBJECTS = $(SOURCES:.c=.o)

BINDIR = $(PREFIX)/bin
PREFIX = $(DESTDIR)/usr/local

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(TARGET) $(OBJECTS)

clean:
	rm *.o
