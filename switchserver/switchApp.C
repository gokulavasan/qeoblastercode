#include "switchApp.h"

using boost::asio::ip::tcp;

mutex vLock; //Lock for reading/manipulating allMap
vector<config> allMap; //All event/state reaction lookup map
map<int, Device *> devIdMap; //DevID to Name Map
QeoFactory factory; //QeoFactory object
const int IR_EVENTS_DEFAULT = 0; //0 -> Read from lircd.conf
const std::string LIRCDFILE = "/etc/lirc/lircd.conf";

static qeo_event_reader_listener_t kinect_event_listener;

int kinectDevId = 0;
int zigBeeDevId = 0;
int zWaveDevId = 0;
int irBlasterDevId = 0;


//Return 0 if the command is EndConversation else return 1 with  to send in the nextSendMsg
int processCommand (std::string cmd, std::string &nextSendMsg) {
  int returnCode = 1;
  if (cmd.find(CommandListNames[GET_ECHO_COMMAND]) != std::string::npos) {
    boost::regex re ("GET_ECHO_(.*)");
    boost::cmatch cm;
    stringstream ss;
    if (boost::regex_match(cmd.c_str(), cm, re)) {
      string remote (cm[1].first, cm[1].second);
      ss << remote << "\n";
      nextSendMsg = ss.str();
    } else {
      nextSendMsg = "ECHO\n";
    }
  } else if (cmd.find(CommandListNames[GET_NUM_DEVICES]) != std::string::npos) {
    //GET_NUM_DEVICES command - send number of devices present in the system
    stringstream ss;
    ss << devIdNameMap.size() << "\n";
    nextSendMsg = ss.str();
    std::cout << "GET_NUM_DEVICES command - Sending : " << nextSendMsg << std::endl;
  } else if (cmd.find(CommandListNames[GET_DEVICE_IDS]) != std::string::npos) {
    map<int, Device *>::iterator devItr;
    stringstream ss;
    for (devItr = devIdMap.begin(); devItr != devIdMap.end(); devItr++) {
      ss << devItr->first << "_" ;
    }
    ss << "\n";
    nextSendMsg = ss.str();
    std::cout << "GET_DEVICE_IDS command - Sending : " << nextSendMsg << std::endl;
  } else if (cmd.find(CommandListNames[GET_DEVICE_INFO]) != std::string::npos) {
    //GET_DEVICE_INFO_<DeviceId> - send details of Device corresponding to DeviceId
    boost::regex re ("GET_DEVICE_INFO_(.*)");
    boost::cmatch cm;
    string devIdStr;
    if (boost::regex_match(cmd.c_str(), cm, re)) {
      string val (cm[1].first, cm[1].second);
      devIdStr = val;
    }
    stringstream ss (devIdStr);
    int devId;
    devId << ss;
    if (devIdMap.find(devId) != devIdMap.end()) 
      ss << (deviceIdMap[devId])->getDeviceInfo() << "\n";
    else 
      ss << "NOTFOUND" << devId << "\n";
    
    nextSendMsg = ss.str();
    std::cout << cmd << " received - Sending : " << nextSendMsg << std::endl;
  } else if (cmd.find(CommandListNames[SET_MAP_ACTION]) != std::string::npos) {
    //SET_MAP_ACTION -- add a config - currently works only for event -> event based mapping!
    boost::regex re ("SET_MAP_(.*?)_(.*?)_(.*?)_(.*?)_(.*?)_(.*)");
    boost::cmatch cm;
    if (boost::regex_match(cmd.c_str(), cm, re)) {
      string sId(cm[1].first, cm[1].second); //source devId
      string sES(cm[2].first, cm[2].second); //source event/state
      string sName(cm[3].first, cm[3].second); //source action name
      string dId(cm[4].first, cm[4].second);  //destionation devId
      string dES(cm[5].first, cm[5].second);  //destination event/state
      string dName(cm[6].first, cm[6].second); //destination action name
      int sDevId = atoi(sId);
      int dDevId = atoi(dId);
      config newConfig;
      newConfig.cause_device_id = sDevId;
      newConfig.cause_state_event = (sES == "S") ? true : false;
      strcpy(newConfig.cause_name, sName.c_str());
      newConfig.effect_device_id = dDevId;
      newConfig.cause_state_event = (dES == "S") ? true : false; 
      strcpy(newConfig.effect_name, dName.c_str());
      vLock.lock();
      allMap.push_back(config);
      vLock.unlock();
      std::cout << "Config Added : Source = " << sID << "::" << sName << " to " << "Dest = " << dId << "::dName " << std::endl;
      nextSendMsg = "CONFIGADDED\n";
    } else {
      std::cout << "Config Format not correct received : " << cmd << std::endl;
      nextSendMsg = "CONFIGFAILURE\n";
    }
  } else if (cmd.find(CommandListNames[SET_UNMAP_ACTION]) != std::string::npos) {
    //SET_UNMAP_ACTION -- remove maps correspoding to that event
    boost::regex re ("SET_UNMAP_(.*?)_(.*?)_(.*)");
    boost::cmatch cm;
    if (boost::regex_match(cmd.c_str(), cm, re)) {
      string sId (cm[1].first, cm[1].second);
      string sES (cm[2].first, cm[2].second);
      string sName (cm[3].first, cm[3].second);
      int sDevId = atoi(sId);
      bool SorE = (sES == "S") ? true : false;
      vLock.lock();
      vector<config>::iterator it = allMap.begin();
      while (it != allMap.end()) {
        if ((sDevId == c.cause_device_id) && (SorE == c.cause_state_event) && (0 == strcmp(c.cause_name, sName.c_str()))) {
          it = allMap.erase(it);
        } else {
          ++it;
        }
      }
      vLock.unlock();
      std::cout << "Config Deleted : Source = " << sId << "::" << sName << std::endl;
      nextSendMsg = "UNMAPSUCCESS\n";
    } else {
      std::cout << "Config UnMap Format NOT correct : " << cmd << std::endl;
      nextSendMsg = "UNMAPFAILURE\n";
    }
  } else if (cmd.find(CommandListNames[GET_ALL_MAP]) != std::string::npos) {
    //GET_ALL_MAP -- send the current config map to WebServer!
    stringstream ss;
    vLock.lock();
    for (const config &c : allMap) {
      string es = (c.cause_state_event) ? "S" : "E";
      string es2 = (c.effect_state_event) ? "S" : "E";
      string ename (c.cause_name);
      string ename2 (c.effect_name);
      ss << c.cause_device_id << "_" << es << "_" << ename << "_" << c.effect_device_id << "_" << es2 << "_" << ename2;
      ss << "," ;
    }
    vLock.unlock();
    ss << "\n";
    nextSendMsg = ss.str();
    std::cout << "Sending all Map details " << nextSendMsg; 
  } else if (cmd.find(CommandListNames[KINECT_CMD]) != std::string::npos) {
    //KINECT_CMD_<Command> - perform action on kinect event - for testing purpose
    boost::regex re ("KINECT_CMD_(.*)");
    boost::cmatch cm;
    if (boost::regex_match(cmd.c_str(), cm, re)) {
      string kCmd (cm[1].first, cm[1].second);
      std::cout << kCmd << " received Kinect Command from WebServer " << std::endl;
      deviceEvent(kCmd, kinectDevId);
      nextSendMsg = "KINECTCMDOK\n";
    } else {
      nextSendMsg = "KINECTCMDFAIL\n";
    }
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
    dev->addSubEvent("SONYNEW:KEY_PLAY");
    dev->addSubEvent("SONYNEW:KEY_PAUSE");
  } else { //Read the .conf file and populate the commands
    std::vector<std::string> commands;
    readLircConfig (commands);
    for (const string &s : commands)
      dev->addSubEvent(s);
  }
}

void populateZigBEvents (Device * dev) {
  dev->addPubEvent ("SWITCH_ON");
  dev->addPubEvent ("SWITCH_OFF"); 
}

void populateZWaveEvents (Device * dev) {
  dev->addSubEvent ("TURN_ON");
  dev->addSubEvent ("TURN_OFF");
}

void populateKinnectEvents (Device * dev) {
  dev->addPubEvent ("RIGHT_CIRCLE");
  dev->addPubEvent ("LEFT_CIRCLE");
}

void initDevices() {
  Device * irB = new Device("irBlaster");
  Device * zigB = new Device ("ZigBee");
  Device * zWave = new Device("ZWave");
  Device * kinDev = new Device("Kinect");
  
  irBlasterDevId = irB->getDeviceId();
  zigBeeDevId = zigB->getDeviceId(); 
  zWaveDevId = zWave->getDeviceId();
  kinectDevId = kinDev->getDeviceId();

  devIdMap[irBlasterDevId] = irB;
  devIdMap[zigBeeDevId] = zigB;
  devIdMap[zWaveDevId] = zWave;
  devIdMap[kinectDevId] = kinDev;

  populateIREvents (irB);
  populateZigBEvents (zigB);
  populateZWaveEvents (zWave);
  populateKinnectEvents (kinDev);
}

void sendCmdToZigBee (string cmd) {
  //Send 'cmd' to ZigBee!
  std::cout << "Sending " << cmd " to ZigBee " << std::endl;
}

void sendCmdToZWave (string cmd) {
  //Send 'cmd' to ZWave!
  std::cout << "Sending " << cmd << " to ZWave " << std::endl; 
}

void sendCmdToIRBlaster (string cmd) {
  //Split cmd into remote and the action type (<REMOTE_TYPE>_<ACTION>)
  //then use system call and irsend to perform the action!
  boost::regex re("(.*):(.*)");
  boost::cmatch cm;
  if (boost::regex_match(cmd.c_str(), cm, re)) {
    string remote(cm[1].first, cm[1].second);
    string rcmd (cmd[2].first, cm[2].second);
    std::cout << "Sending to IRBlaster " << cmd << " Remote : " << remote << " Command : " << rcmd << std::endl;
    stringstream ss; 
    ss << "irsend " << "SEND_ONCE " << remote << " " << rcmd;
    system (ss.str());
  } else {
    std::cout << "Illegal Format received in sendCmdtoIRBlaster : " << cmd << std::endl;
  }
}

DEVICE_TYPE devIdToTypeMap (int devId) {
  switch (devId) {
    case kinectDevId: 
    return KINNECT_DEVICE;

    case zigBeeDevId:
    return ZIGBEE_DEVICE;

    case zWaveDevId:
    return ZWAVE_DEVICE;

    case irBlasterDevId:
    return IRBLASTER_DEVICE; 

    default:
    std::cout << "Unknown Device ID! " << devId << std::endl;
    return DEVICE_TYPE_LAST;
  }
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

void deviceEvent (string kCmd, int devId) {
  vLock.lock();
  for (const config &c : allMap) {
    if (c.cause_device_id == devId) {
      if (0 == strcmp(c.cause_name, kCmd.c_str())) {
        //Found a match; Ignoring state/event bool for now!
        performAction (devIdToTypeMap(c.effect_device_id), c.effect_name);
      }
    }
  }
  vLock.unlock();
}

static void on_kinect_event (const qeo_event_reader_t * reader, const void * data, uintptr_t userdata) {
  //Cast into kinectCmd qeo struct and then get the event details
  //Look up the allMap config and issue the command to the device by looking up devIdMap!
  org_qeo_qeoblaster_qeoir_IRCommand_t *msg = (org_qeo_qeoblaster_qeoir_IRCommand_t *)data;
  printf("Kinnect Channel : %s sent Command = %s\n", msg->from, msg->cmd);
  string kCmd(msg->cmd);
  deviceEvent(kCmd, kinectDevId);
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
    deviceEvent (cmd, zigBeeDevId);
  }
}

void zWaveComm() {
  while (1) {
    //Blocking Read -- If ZWave is sending commands, if we get commands 
    //lookup the config map and then act accordingly
    string cmd = "SWITCHON"; //TODO: Replace with a blocking call here!
    std::cout << "SwitchApp : Received " << cmd <<" command from zWave " << std::endl;
    deviceEvent (cmd, zWaveDevId);
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
  for (const std::string &eventName : eventPubList)
    ss << "EP_" << eventName << "_";
  for (const std::string &stateName : statePubList)
    ss << "SP_" << stateName << "_";
  for (const std::string &eventName : eventSubList)
    ss << "ES_" << eventName << "_";
  for (const std::string &stateName : stateSubList)
    ss << "SS_" << stateName << "_";
  return ss.str();
}

void readLircConfig(std::vector<std::string> &cmd) {
  using namespace std;
  string file = LIRCDFILE;
  ifstream f (file.c_str());
  if (!f.is_open()) {
    cout << "Couldn't open " << file << endl;
  } else {
    string line;
    while (getline(f, line)) {
      boost::regex re ("include \"(.*)\"");
      boost::cmatch cm;
      if (boost::regex_match(line.c_str(), cm, re)) {
        string remote (cm[1].first, cm[1].second);
        cout << remote << endl;
        ifstream rf (remote.c_str());
        if (!rf.is_open()) {
          cout << "Couldn't open " << remote << endl;
        } else {
          string key;
          while (getline(rf, key)) {
            boost::regex nam (".*name  (.*)");
            boost::cmatch kn;
            string remname;
            if (boost::regex_match(key.c_str(), kn, nam)) {
              string name (kn[1].first, kn[1].second);
              cout << "Remote Name " << name << endl;
              remname = name;
            }

            boost::regex rel (".*(KEY_[A-Z]*).*0x.*");
            boost::cmatch km;
            if (boost::regex_match(key.c_str(), km, rel)) {
              string keyl (km[1].first, km[1].second);
              cout << keyl << endl;
              stringstream ss;
              ss << remname << ":" << keyl;
              cmd.push_back(ss.str());
            }
          }
        }
      } else {
        cout << "Didn't work" << endl;
      }
    }
  }
  return 0;
}
