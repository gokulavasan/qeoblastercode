#ifndef REMOTE_COMMAND_H_
#define REMOTE_COMMAND_H_

typedef enum {
	REMOTE_PLAY,
	REMOTE_PAUSE,
	REMOTE_PREV,
	REMOTE_NEXT,
	REMOTE_POWER,
	REMOTE_STOP,
	REMOTE_LAST_CMD
} remoteCmd;

static char * remoteCmdNames [] = {
	"PLAY",
	"PAUSE",
	"PREV",
	"NEXT",
	"POWER",
	"STOP",
	"LAST_CMD"
};

#endif
