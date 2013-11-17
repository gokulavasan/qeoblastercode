#include <iostream>
#include <stdexcept>
#include <functional>
#include <sstream>
#include <string>

extern "C" {
#include <qeo/api.h>
#include "../irblasterpi/QIRBlaster.h"
#include "../caminfopi/QCamData.h"
#include "QMasterCam.h"
}

using namespace std;

static qeo_state_change_reader_listener_t people_state_listener;

static void on_people_state_change(const qeo_state_change_reader_t* reader, const void* data, uintptr_t userdata) {
    cout << "got people state change" << endl;
}

class QeoFactory {
  public:
    void Initialize() {
        qeo_ = qeo_factory_create();
	if(qeo_ == NULL)
	    throw runtime_error("Couldn't create qeo factory");
    }
    ~QeoFactory() {
	if(qeo_ != NULL)
	    qeo_factory_close(qeo_);
    }
    qeo_factory_t* GetFactory() { return qeo_; }
  private:
    qeo_factory_t* qeo_;
};

class MessageWriter {
  public:
    MessageWriter(QeoFactory* qeo_factory, const DDS_TypeSupport_meta* type) {
	msg_writer_ = qeo_factory_create_event_writer(qeo_factory->GetFactory(), type, NULL, 0);
    }
    ~MessageWriter() {
	qeo_event_writer_close(msg_writer_);
    }
  private:
    qeo_event_writer_t* msg_writer_;
};

class StateReader {
  public:
    StateReader(QeoFactory* qeo_factory, const DDS_TypeSupport_meta* type, qeo_state_change_reader_listener_t* listener) {
	state_reader_ = qeo_factory_create_state_change_reader(qeo_factory->GetFactory(), type, listener, 0);
    }
    ~StateReader() {
	qeo_state_change_reader_close(state_reader_);
    }
  private:
    qeo_state_change_reader_t* state_reader_;
};

class MessageReader {
  public:
    MessageReader(QeoFactory* qeo_factory, const DDS_TypeSupport_meta* type, qeo_event_reader_listener_t* listener) {
	msg_reader_ = qeo_factory_create_event_reader(qeo_factory->GetFactory(), type, listener, 0);
    }
    ~MessageReader() {
	qeo_event_reader_close(msg_reader_);
    }
  private:
    qeo_event_reader_t* msg_reader_;
};

QeoFactory qeo;

int main(int argc, const char **argv)
{
    QeoFactory factory;

    people_state_listener.on_data = on_people_state_change;
    people_state_listener.on_remove = NULL;

    MessageWriter ir_writer(&factory, org_qeo_qeoblaster_qeoir_IRCommand_type);
    MessageWriter people_writer(&factory, org_qeo_qeoblaster_qeomastercam_MockCommand_type);
    StateReader people_reader(&factory, org_qeo_qeoblaster_qeokinect_PeoplePresenceState_type, &people_state_listener);
    int done = 0;

    /* local variables for storing the message before sending */
    //char buf[128];
    //org_qeo_qeoblaster_qeokinect_PeoplePresenceState_t people_state = { .cmd = buf };
    //org_qeo_qeoblaster_qeoir_IRCommand_type ir_blast = { .cmd = buf };

    factory.Initialize();
    cout << "Initialized Qeo Factory" << endl;
    string cmd;
    while (!done) {
	cout << "----===== MENU =====----" << endl;
	cout << "1. Set Number of People" << endl;
	cout << "2. Send IR Blast" << endl;
	cout << "3. Exit" << endl;
	cout << "Enter a command: ";
	cin >> cmd;
	stringstream ss(cmd);
	int iCmd;
	ss << iCmd;
	cout << "Got command " << iCmd << endl;

            //if(fgets(buf, sizeof(buf), stdin) != NULL) {
                //chomp(buf);
                //if ('/' == buf[0]) {
                    //handle_command(&buf[1], &chat_msg, &done);
                //}
                //else {
                    //qeo_event_writer_write(msg_writer, &chat_msg);
                //}
            //}
    }
    return 0;
}
