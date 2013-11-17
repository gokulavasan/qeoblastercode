/************* COPYRIGHT AND CONFIDENTIALITY INFORMATION *********************
 **                                                                          **
 ** Copyright (c) 2012 Technicolor                                           **
 ** All Rights Reserved                                                      **
 **                                                                          **
 ** This program contains proprietary information which is a trade           **
 ** secret of TECHNICOLOR and/or its affiliates and also is protected as     **
 ** an unpublished work under applicable Copyright laws. Recipient is        **
 ** to retain this program in confidence and is not permitted to use or      **
 ** make copies thereof other than as permitted in a written agreement       **
 ** with TECHNICOLOR, UNLESS OTHERWISE EXPRESSLY ALLOWED BY APPLICABLE LAWS. **
 **                                                                          **
 ******************************************************************************/

#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

#include <qeo/api.h>

#include "../irblasterpi/QIRBlaster.h"
#include "../masterapp/QMasterCam.h"
#include "QCamData.h"

static int kinectId = 0;
static int numPplInRoom = 4;
static char * nameUser  = "KinectPI";
static char * roomName  = "LivingRoom";
static char * irCommand = "PAUSE";

static qeo_event_writer_t *ircmd_writer; //Publish cmds to IR
static qeo_state_writer_t *pplstate_writer; //Publish the state to the world
static qeo_event_reader_t *mastercmd_reader; //Read cmds from MasterApp

/* ===[ Master App message listeners ]============================================= */
//For Demo/Testing Purposes only :: camInfoApp subscribes to this topic only for testing/demo purposes
static void on_receive_cmd (const qeo_event_reader_t *reader,
                            const void *data,
                            uintptr_t userdata)
{
    org_qeo_qeoblaster_qeomastercam_MockCommand_t *msg = (org_qeo_qeoblaster_qeomastercam_MockCommand_t *)data;
    /* Whenever a new cmd arrives, print it to stdout and issue the comand to Mock */
    printf("Received from %s : Change people state to %d\n", msg->from, msg->number);
    numPplInRoom = msg->number;
    org_qeo_qeoblaster_qeokinect_PeoplePresenceState_t changeState; //from, room - strings, num_people - int
    changeState.id = kinectId;
    changeState.from = nameUser;
    changeState.room = roomName;
    changeState.num_people = numPplInRoom;
    qeo_state_writer_write(pplstate_writer, &changeState); 
    //Do some intelligent decision here about max people seen so far and depending upon the current state and the max state seen
    //decide to send the command to irBlaster 
    org_qeo_qeoblaster_qeoir_IRCommand_t irCmd; //from, cmd - strings
    irCmd.from = nameUser;
    irCmd.cmd  = irCommand;
    qeo_event_writer_write(ircmd_writer, &irCmd);
}

static qeo_event_reader_listener_t _listener = { .on_data = on_receive_cmd };

/* ===[ Main code ]========================================================== */

static char *chomp(char *s)
{
    char *p = s;

    while ('\0' != *p) {
        if (('\n' == *p) || ('\r' == *p)) {
            *p = '\0';
            break;
        }
        p++;
    }
    return s;
}

static void help(void)
{
    printf("Available commands:\n");
    printf("  /bye           quit irBlaster application\n");
    printf("  /help          display this help\n");
    printf("  /name <name>   change user name\n");
}

static void handle_command(const char *cmd,
                           org_qeo_qeoblaster_qeoir_IRCommand_t *chat_msg,
                           int *done)
{
    if (0 == strcmp("bye", cmd)) {
        *done = 1;
    }
    if (0 == strcmp("help", cmd)) {
        help();
    }
    else if (0 == strncmp("name ", cmd, 5)) {
        free(chat_msg->from);
        chat_msg->from = strdup(&cmd[5]);
    }
}

static char *default_user(void)
{
    struct passwd *pwd = getpwuid(getuid());
    char *name = "";

    if (NULL != pwd) {
        name = strdup(pwd->pw_name);
    }
    return name;
}


int main(int argc, const char **argv)
{
    qeo_factory_t *qeo;
    int done = 0;

    /* local variables for storing the message before sending */
    char buf[128];
    org_qeo_qeoblaster_qeoir_IRCommand_t cmd_msg;
    cmd_msg.cmd = buf;


    /* initialize */
    qeo = qeo_factory_create();
    if (qeo != NULL){
	    
	    ircmd_writer     = qeo_factory_create_event_writer(qeo, org_qeo_qeoblaster_qeoir_IRCommand_type, NULL, 0);
	    pplstate_writer  = qeo_factory_create_state_writer(qeo, org_qeo_qeoblaster_qeokinect_PeoplePresenceState_type, NULL, 0);
	    mastercmd_reader = qeo_factory_create_event_reader(qeo, org_qeo_qeoblaster_qeomastercam_MockCommand_type, &_listener, 0); 

	    //Initialize the system with some arbit value
	    org_qeo_qeoblaster_qeokinect_PeoplePresenceState_t initState; //from, room - strings, num_people - int
            
            initState.id = kinectId;
	    initState.from = nameUser;
	    initState.room = roomName;
	    initState.num_people = numPplInRoom;

	    qeo_state_writer_write(pplstate_writer, &initState);  

	    /* set up some defaults */
	    cmd_msg.from = default_user();

	    /* start conversing */
	    printf("KinectPI is ready! Type '/help' for commands.\n");
	    printf("Waiting for commands on topic - \n");
	    while (!done) {

		    if(fgets(buf, sizeof(buf), stdin) != NULL) {
			    chomp(buf);
			    if ('/' == buf[0]) {
				    handle_command(&buf[1], &cmd_msg, &done);
			    }
			    else {
				    //qeo_event_writer_write(msg_writer, &chat_msg);
			    }
		    }

	    }

	    /* clean up */
	    //free(chat_msg.from);
	    qeo_event_reader_close(mastercmd_reader);
	    qeo_state_writer_close(pplstate_writer);
	    qeo_event_writer_close(ircmd_writer);
	    qeo_factory_close(qeo);
    }
    return 0;
}
