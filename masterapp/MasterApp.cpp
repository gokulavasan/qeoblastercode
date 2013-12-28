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
static qeo_event_reader_listener_t ir_listener;

static void on_people_state_change(const qeo_state_change_reader_t* reader, const void* data, uintptr_t userdata) {
    org_qeo_qeoblaster_qeokinect_PeoplePresenceState_t* state = (org_qeo_qeoblaster_qeokinect_PeoplePresenceState_t*)data;
    cout << "got people state change " << state->num_people << endl;
}

static void on_ir_event(const qeo_event_reader_t* reader, const void* data, uintptr_t userdata) {
    org_qeo_qeoblaster_qeoir_IRCommand_t* evt = (org_qeo_qeoblaster_qeoir_IRCommand_t*)data;
    cout << "got ir event " << evt->cmd << endl;
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

class PeopleWriter {
  public:
    PeopleWriter(QeoFactory* qeo_factory) {
	msg_writer_ = qeo_factory_create_event_writer(qeo_factory->GetFactory(), org_qeo_qeoblaster_qeomastercam_MockCommand_type, NULL, 0);
    }
    ~PeopleWriter() {
	qeo_event_writer_close(msg_writer_);
    }
    void WriteNumPeople(int num_people) {
	org_qeo_qeoblaster_qeomastercam_MockCommand_t event;
	event.from = "master_app";
	event.number = num_people;
	qeo_event_writer_write(msg_writer_, &event);
    }
  private:
    qeo_event_writer_t* msg_writer_;
};

class PeopleStateReader {
  public:
    PeopleStateReader(QeoFactory* qeo_factory, qeo_state_change_reader_listener_t* listener) {
	state_reader_ = qeo_factory_create_state_change_reader(qeo_factory->GetFactory(), org_qeo_qeoblaster_qeokinect_PeoplePresenceState_type, listener, 0);
    }
    ~PeopleStateReader() {
	qeo_state_change_reader_close(state_reader_);
    }
  private:
    qeo_state_change_reader_t* state_reader_;
};

 class IRWriter {
  public:
    IRWriter(QeoFactory* qeo_factory) {
	msg_writer_ = qeo_factory_create_event_writer(qeo_factory->GetFactory(), org_qeo_qeoblaster_qeoir_IRCommand_type, NULL, 0);
    }
    ~IRWriter() {
	qeo_event_writer_close(msg_writer_);
    }
    void WritePlay() {
        org_qeo_qeoblaster_qeoir_IRCommand_t msg;
        msg.from = "master_app";
        msg.cmd  = "PLAY";
        qeo_event_writer_write(msg_writer_, &msg);
    }
    void WritePause() {
	org_qeo_qeoblaster_qeoir_IRCommand_t msg;
	msg.from = "master_app";
	msg.cmd = "PAUSE";
	qeo_event_writer_write(msg_writer_, &msg);
    }
  private:
    qeo_event_writer_t* msg_writer_;
};

class IRReader {
  public:
    IRReader(QeoFactory* qeo_factory, qeo_event_reader_listener_t* listener) {
	msg_reader_ = qeo_factory_create_event_reader(qeo_factory->GetFactory(), org_qeo_qeoblaster_qeoir_IRCommand_type, listener, 0);
    }
    ~IRReader() {
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
    ir_listener.on_data = on_ir_event;

    factory.Initialize();
    PeopleWriter people_writer(&factory);
    IRWriter ir_writer(&factory);
    IRReader ir_reader(&factory, &ir_listener);
    PeopleStateReader people_reader(&factory, &people_state_listener);

    cout << "Initialized Qeo Factory" << endl;
    int cmd;
    while(true) {
	cout << "----===== MENU =====----" << endl;
	cout << "1. Set Number of People" << endl;
	cout << "2. Send Pause IR Blast" << endl;
        cout << "3. Send Play IR Blast" <<endl;
	cout << "4. Exit" << endl;
	cout << "Enter a command: ";
	cin >> cmd;
	
	cout << "Got command " << cmd << endl;
	if(cmd == 1) {
	    int num;
	    cout << "Enter number of people to set: ";
	    cin >> num;
	    people_writer.WriteNumPeople(num);
	    cout << "Sent num people " << num << endl;
	} else if(cmd == 2) {
	    ir_writer.WritePause();
	    cout << "Sent Pause to IR Blaster" << endl;
        } else if (cmd == 3) {
            ir_writer.WritePlay();
            cout << "Sent Play to IR Blaster" <<endl;
	} else if(cmd == 3) {
	    cout << "Bye Bye!" << endl;
	    return 0;
	} else {
	    cout << "Unrecognized Command" << endl;
	}
	cout << endl;
    }
    return 0;
}
