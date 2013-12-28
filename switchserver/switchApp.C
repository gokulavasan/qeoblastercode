#include "switchApp.h"

using boost::asio::ip::tcp;

mutex vLock; //Lock for reading/manipulating allMap
vector<config> allMap; //All event/state reaction lookup map
map<int, Device *> devIdMap; //DevID to Name Map
QeoFactory factory; //QeoFactory object

static qeo_event_reader_listener_t kinectCmd_event_listener;

static void on_kinectCmd_event (const qeo_event_reader_t * reader, const void * data, uintptr_t userdata) {
  //Cast into kinectCmd qeo struct and then get the event details
  //Look up the allMap config and issue the command to the device by looking up devIdMap!

}

//Return 0 if the command is EndConversation else return 1 with  to send in the nextSendMsg
int processCommand (std::string cmd, std::string &nextSendMsg) {
  int returnCode = 1;
  if (cmd.find(CommandListNames[GET_NUM_DEVICES]) != std::string::npos) {
    //GET_NUM_DEVICES command - send number of devices present in the system
    stringstream ss;
    ss << devIdNameMap.size() << "\n";
    nextSendMsg = ss.str();
    std::cout << "GET_NUM_DEVICES command - Sending : " << nextSendMsg << std::endl;
  } else if (cmd.find(CommandListNames[GET_DEVICE_INFO]) != std::string::npos) {
    //GET_DEVICE_INFO_<DeviceId> - send details of Device corresponding to DeviceId
    cmd.pop_back();
    cmd.pop_back(); //Remove "\n"
    int devId = atoi(cmd.back());
    stringstream ss;
    if (devIdMap.find(devId) != devIdMap.end()) 
      ss << (deviceIdMap[devId])->getDeviceInfo() << "\n";
    else 
      ss << "NOTFOUND" << devId << "\n";
    
    nextSendMsg = ss.str();
    std::cout << cmd << " received - Sending : " << nextSendMsg << std::endl;
  } else if (cmd.find(CommandListNames[END_CONVERSATION]) != std::string::npos) {
    //END_CONVERSATIOn - ending the conversation with the client
    std::cout << cmd << " received - Ending Conversation Successfully ! " << std::endl;
    returnCode = 0;
  } else {
    std::cout << cmd << " received - Unknown Command ! " << std::endl;
    nextSendMessage = "UNKNOWNCMD\n";
  }
  return returnCode;
}


//Return 0 if the whole conversation went successfully, Else send 1
int converse (tcp::socket& socket) {
  int returnCode = 0;
  boost::system::error_code syserr;
  std::string message = "SendCmd\n";
  while (1) {
    boost::asio::streambuf rbuf;
    boost::asio::write(socket, boost::asio::buffer(message), syserr);
    if (syserr) {
      std::cout << "Some Error in Writing " << message << std::endl;
      returnCode = 1;
      break;
    }
    boost::asio::read_until (socket, rbuf, "\n", syserr);
    if (syserr) {
      std::cout << "Some Error while reading in Converse" << std::endl;
      returnCode = 1;
      break;
    }
    if (processCommand (rbuf.str(), message) > 0)
      continue;
    else
      break;
  }
  return returnCode;
}

void netWork (short port) {
  while (1) {
    boost::asio::io_service io_service;
    tcp::acceptor acceptor (io_service, tcp::endpoint(tcp::v4(), port));
    for ( ; ; ) {
      tcp::socket socket(io_service);
      acceptor.accept(socket);
      std::string message = "SwitchAppReady\n";
      boost::system::error_code syserr;
      boost::asio::write (socket, boost::asio::buffer(message), syserr);
      if (syserr) {
        std::cout << "Some Error while writing " << message << std::endl;
        break;
      }
      boost::asio::streambuf rbuf;
      boost::asio::read_until (socket, rbuf, "\n", syserr);
      if (syserr) {
        std::cout << "Some Error while reading" << std::endl;
        break;
      }
      
      if (rbuf.str() == "JavaReady\n") {
        if (converse(socket) > 0) {
          std::cout << "Something happenend in Converse " << std::endl;
          break;
        }
      } else {
        std::cout << "Received Wrong Ack : " << rbuf.str() << std::endl;
        break;
      }
    }
  }
}


//Devices Definitions 

class ZWave : public Device 
{

}

class ZigBee : public Device
{

}

class Kinect : public Device
{

};


class IRBlaster : public Device 
{
  private:
    qeo_event_writer_t * msg_writer_;
  public:
  IRBlaster (QeoFactory * qeo_factory, std:string devName) : Device(devName) {
    msg_writer_ = qeo_factory_create_event_writer (qeo_factory->GetFactory(), org_qeo_qeoblaster_qeoir_IRCommand_type, NULL, 0);
  }
  ~IRBlaster() {
    qeo_event_writer_close(msg_writer_);
  }

  void executeEvent (std::string name) {
    //Command should be of the form REMOTENAME_ACTION, for ex, SONY_PLAY
    //Make sure the command is present in the eventSubList!
    bool found = false;
    for (const event &e : eventSubList) {
      if (e.name == name) {
        found = true;
        break;
      }
    }

    if (!found) {
      std::cout << name << " command not found in IRBlaster device " << std::endl;
      return;
    }

    org_qeo_qeoblaster_qeoir_IRCommand_t msg;
    msg.from = "switchApp";
    msg.cmd  = name;
    qeo_event_writer_write(msg_writer_, &msg);
  }
};


void initDevices() {
  IRBlaster * irB = new IRBlaster (&factory, "IRBlaster");
  irB->addSubEvent("SONY_PLAY");
  irB->addSubEvent("SONY_PAUSE");
}

void qeoLoop() {
  factory.Initialize();
  //Connect all the listener callback functions!
  kinectCmd_event_listener.on_data = on_kinectCmd_event;
  kinectCmd_event_listener.on_remove = NULL;

  initDevices(); //Initialize current devices in the system
}


int main(int argc, char * argv[]) {
  try {
    if (argc != 2) {
      std::cerr << "Usage: switchApp <port> " << std::endl;
      return 1;
    }

    using namespace std;
    boost::thread netWork(&serveRequests, atoi(argv[1]));  
    boost::thread qeoWork(&qeoLoop);  
    netWork.join();
    qeoWork.join();
    //Will not reach here!
  }
  catch (std::exception &e) {
    std::cerr << "Exception : " << e.what() << std::endl;
  }
}

std::string Devices::getDeviceInfo () {
  stringstream ss;
  ss << name << "_" ;
  for (const std::string &eventName : eventList)
    ss << "E_" << eventName << "_";
  for (const std::string &stateName : stateList)
    ss << "S_" << stateName << "_";
  return ss.str();
}

