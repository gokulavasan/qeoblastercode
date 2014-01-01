#include "qeoxbeelib.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <signal.h>

#define MODEM "/dev/ttyUSB0"
#define BAUDRATE B19200

struct termios old_stdio;
int tty_fd = -1;

void fixTTY()
{
	close(tty_fd);
	tcsetattr(STDOUT_FILENO, TCSANOW, &old_stdio);
}

static void terminal_signal(int signum)
{
	if(tty_fd != -1)
		fixTTY();
	_exit(128 + signum);
}

int connect_xbee()
{
	if(tty_fd != -1)
		return 0;
	struct termios tio;
	struct termios stdio;
	int flags;
	unsigned char c = -1;
	tcgetattr(STDOUT_FILENO, &old_stdio);
	memset(&stdio, 0, sizeof(stdio));
	tcgetattr(STDOUT_FILENO, &stdio);
	stdio.c_iflag &= ~IGNBRK;
	stdio.c_iflag |= BRKINT;
	stdio.c_oflag = 0;
	stdio.c_cflag = 0;
	stdio.c_lflag |= ISIG | ECHO;
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

	if(atexit(fixTTY))
	{
		printf("Failed to set exit command");
		return -1;
	}
	struct sigaction act;
	sigemptyset(&act.sa_mask);
	act.sa_handler = terminal_signal;
	act.sa_flags = 0;
	if(sigaction(SIGHUP, &act, NULL) ||
	   sigaction(SIGINT, &act, NULL) ||
	   sigaction(SIGQUIT, &act, NULL) ||
	   sigaction(SIGTERM, &act, NULL) ||
	   sigaction(SIGPIPE, &act, NULL) ||
	   sigaction(SIGALRM, &act, NULL))
	{
		printf("Failed setting signal handler\n");
		return -1;
	}
	return 0;
}

int disconnect_xbee()
{
	//fixTTY();
	return 0;
}

int get_xbee_signal(char* text, int len)
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
				snprintf(text, len, "ON");
				return 0;
			}
			if(cnt == 3 && buf[1] == 'F' && buf[2] == 'F') 
			{
				snprintf(text, len, "OFF");
				return 0;
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

