#include "switchApp.h"

using boost::asio::ip::tcp;

mutex vLock; //Lock for reading/manipulating allMap
vector<config> allMap; //All event/state reaction lookup map
map<int, Device *> devIdMap; //DevID to Name Map
QeoFactory factory; //QeoFactory object
const int IR_EVENTS_DEFAULT = 1;

static qeo_event_reader_listener_t kinect_event_listener;

int kinnectDevId = 0;
int zigBeeDevId = 0;
int zWaveDevId = 0;
int irBlasterDevId = 0;


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

void populateIREvents (Device * dev) {
  if (IR_EVENTS_DEFAULT) { //Populate it with default commands

  } else { //Read the .conf file and populate the commands

  }
}

void populateZigBEvents (Device * dev) {
   
}

void populateZWaveEvents (Device * dev) {

}

void populateKinnectEvents (Device * dev) {

}

void initDevices() {
  Device * irB = new Device("irBlaster");
  Device * zigB = new Device ("ZigBee");
  Device * zWave = new Device("ZWave");
  Device * kinDev = new Device("Kinect");
  
  irBlasterDevId = irB->getDeviceId();
  zigBeeDevId = zigB->getDeviceId(); 
  zWaveDevId = zWave->getDeviceId();
  kinnectDevId = kinDev->getDeviceId();

  devIdMap[irBlasterDevId] = irB;
  devIdMap[zigBeeDevId] = zigB;
  devIdMap[zWaveDevId] = zWave;
  devIdMap[kinnectDevId] = kinDev;

  populateIREvents (irB);
  populateZigBEvents (zigB);
  populateZWaveEvents (zWave);
  populateKinnectEvents (kinDev);
}

void sendCmdToZigBee (string cmd) {
  //Send 'cmd' to ZigBee!
}

void sendCmdToZWave (string cmd) {
  //Send 'cmd' to ZWave! 
}

void sendCmdToIRBlaster (string cmd) {
  //Split cmd into remote and the action type (<REMOTE_TYPE>_<ACTION>)
  //then use system call and irsend to perform the action!
}

void performAction (DEVICE_TYPE dType, string cmd) {
  switch (dType) {
    case ZIGBEE_DEVICE: 
    sendCmdToZigBee (cmd);
    break;

    case ZWAVE_DEVICE:
    sendCmdToZWave (cmd);
    break;

    case IRBLASTER_DEVICE:
    sendCmdToIRBlaster (cmd);
    break;

    default:
    std::cout << "Invalid Device_TYpe in performAction " << dType << std::endl;
  }
}

static void on_kinect_event (const qeo_event_reader_t * reader, const void * data, uintptr_t userdata) {
  //Cast into kinectCmd qeo struct and then get the event details
  //Look up the allMap config and issue the command to the device by looking up devIdMap!
  org_qeo_qeoblaster_qeoir_IRCommand_t *msg = (org_qeo_qeoblaster_qeoir_IRCommand_t *)data;
  printf("Kinnect Channel : %s sent Command = %s\n", msg->from, msg->cmd);
  vLock.lock(); //Lock the config structure and walk through it and find out if we have actions to do
  for (const config &c : allMap) {

  }
}

void qeoLoop() {
  factory.Initialize();
  //Connect all the listener callback functions!
  kinect_event_listener.on_data = on_kinect_event;
  kinect_event_listener.on_remove = NULL;
  initDevices(); //Initialize current devices in the system
}

void zigBeeComm() {
  while (1) {
    //Blocking Read -- If ZigBee is sending commands - if we get commands
    //lookup the config map and then act accordingly
    string cmd = "SWITCHOFF"; //TODO : Replace with a blocking read call here!
    std::cout << "SwitchApp : Received " << cmd <<" command from zigBee " << std::endl;
    vLock.lock(); //Lock the config structure and walk through it and find out if have actions to do 
    for (const config &c : allMap) {
      if (c.cause_device_id == zigBeeDevId) {
        if (0 == strcmp(c.cause_name, cmd.c_str())) {
          //Found a match; Ignoring state/event bool for now!
          performAction (devIdToTypeMap(c.effect_device_id), c.effect_name);
        }
      }
    }
    vLock.unlock();
  }
}

void zWaveComm() {
  while (1) {
    //Blocking Read -- If ZWave is sending commands, if we get commands 
    //lookup the config map and then act accordingly
    string cmd = "SWITCHON"; //TODO: Replace with a blocking call here!
    std::cout << "SwitchApp : Received " << cmd <<" command from zWave " << std::endl;
    vLock.lock();
    for (const config &c : allMap) {
      if (c.cause_device_id == zWaveDevId) {
        if (0 == strcmp (c.cause_name, cmd.c_str())) {
          //Found a match; Ignoring state/event bool for now!
          performAction (devIdToTypeMap(c.effect_device_id), c.effect_name);
        }
      }
    }
    vLock.unlock();
  }
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
    boost::thread zWaveAgent(&zWaveComm);
    boost::thread zigBeeAgent(&zBeeComm);
    zWaveAgent.join();
    zigBeeAgent.join();  
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

