#include "qeoxbeelib.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

#define MODEM "/dev/ttyUSB0"
#define BAUDRATE B19200

struct termios old_stdio;
int tty_fd;

void fixTTY()
{
	close(tty_fd);
	tcsetattr(STDOUT_FILENO, TCSANOW, &old_stdio);
}

int connect_xbee()
{
	struct termios tio;
	struct termios stdio;
	int flags;
	unsigned char c = -1;
	tcgetattr(STDOUT_FILENO, &old_stdio);
	memset(&stdio, 0, sizeof(stdio));
	stdio.c_iflag = 0;
	stdio.c_oflag = 0;
	stdio.c_cflag = 0;
	stdio.c_lflag = 0;
	stdio.c_cc[VMIN] = 1;
	stdio.c_cc[VTIME] = 0;
	tcsetattr(STDOUT_FILENO, TCSANOW, &stdio);
	tcsetattr(STDOUT_FILENO, TCSAFLUSH, &stdio);
	fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
	memset(&tio, 0, sizeof(tio));
	tio.c_iflag = 0;
	tio.c_oflag = 0;
	tio.c_cflag = CS8 | CREAD | CLOCAL;
	tio.c_lflag = 0;
	tio.c_cc[VMIN] = 1;
	tio.c_cc[VTIME] = 5;
	if((tty_fd = open(MODEM, O_RDWR | O_NONBLOCK)) == -1)
	{
		printf("Error while opening\n");
		fixTTY();
		return -1;
	}
	cfsetospeed(&tio, BAUDRATE);
	cfsetispeed(&tio, BAUDRATE);
	tcsetattr(tty_fd, TCSANOW, &tio);
	return 0;
}

int disconnect_xbee()
{
	fixTTY();
	return 0;
}

int get_xbee_signal()
{
	char buf[255];
	int cnt = 0;
	char c = 'D';
	for(;;)
	{
		int ret = read(tty_fd, &buf[cnt], 1);
		if(ret > 0)
			cnt += ret;
		if(cnt > 1 && buf[0] == 'O')
		{
			if(buf[1] == 'N') 
			{
				printf("got ON signal\r\n");
				cnt = 0;
			}
			if(cnt == 3 && buf[1] == 'F' && buf[2] == 'F') 
			{
				printf("got OFF signal\r\n");
				cnt = 0;
			}
			else if(cnt > 2)
				cnt = 0;
		}
	}
	return 0;
}

int fire_xbee_signal(char* signal)
{
	return 0;
}

