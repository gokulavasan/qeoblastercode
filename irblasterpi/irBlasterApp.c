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

#include "QIRBlaster.h"

/* ===[ Cmd message listeners ]============================================= */
int pause_play = 0;
char pp[] = "pause_play";

static void on_receive_cmd (const qeo_event_reader_t *reader,
                            const void *data,
                            uintptr_t userdata)
{
    org_qeo_qeoblaster_qeoir_IRCommand_t *msg = (org_qeo_qeoblaster_qeoir_IRCommand_t *)data;

    /* Whenever a new cmd arrives, print it to stdout and issue the comand to Mock */
    printf("Issuer = %s :: Command = %s\n", msg->from, msg->cmd);
    //TODO : Call the IR Blaster Mockup here!
    if (strcmp(msg->cmd, pp) == 0)
    {
        if (pause_play) //Play Command
            system ("irsend SEND_ONCE sony KEY_PLAY");
        else 
            system ("irsend SEND_ONCE sony KEY_PAUSE");
        pause_play ^= 1;
    }
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
    //qeo_event_writer_t *msg_writer; //IR Blaster is not publishing data
    qeo_event_reader_t *msg_reader;
    int done = 0;

    /* local variables for storing the message before sending */
    char buf[128];
    org_qeo_qeoblaster_qeoir_IRCommand_t chat_msg;
    chat_msg.cmd = buf;

    /* initialize */
    qeo = qeo_factory_create();
    if (qeo != NULL){
	//Wait for events and process it!
        //msg_writer = qeo_factory_create_event_writer(qeo, org_qeo_sample_simplechat_ChatMessage_type, NULL, 0);
        msg_reader = qeo_factory_create_event_reader(qeo, org_qeo_qeoblaster_qeoir_IRCommand_type, &_listener, 0);

        /* set up some defaults */
        chat_msg.from = default_user();

        /* start conversing */
        printf("IR Blaster is ready! Type '/help' for commands.\n");
        printf("Waiting for commands on topic - \n");
        while (!done) {

            if(fgets(buf, sizeof(buf), stdin) != NULL) {
                chomp(buf);
                if ('/' == buf[0]) {
                    handle_command(&buf[1], &chat_msg, &done);
                }
                else {
		    //Nothing to write in IR Blaster
                    //qeo_event_writer_write(msg_writer, &chat_msg);
                }
            }

        }

        /* clean up */
        //free(chat_msg.from);
        qeo_event_reader_close(msg_reader);
        //qeo_event_writer_close(msg_writer);
        qeo_factory_close(qeo);
    }
    return 0;
}
