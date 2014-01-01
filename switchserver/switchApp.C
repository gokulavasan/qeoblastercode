#include "switchApp.h"

using boost::asio::ip::tcp;

std::mutex vLock; //Lock for reading/manipulating allMap
std::vector<config> allMap; //All event/state reaction lookup map
std::map<int, Device *> devIdMap; //DevID to Name Map
std::map<std::string, bool> availRemotes; //Avail Remote Names
std::map<std::string, bool> validKeyNames; //Valid Remote Key Names
char currPath[FILENAME_MAX]; //CurrPath

const int IR_EVENTS_DEFAULT = 0; //0 -> Read from lircd.conf
const std::string LIRCDFILE = "/etc/lirc/lircd.conf";

static qeo_event_reader_listener_t kinect_event_listener;
int Device::globalDevId = 0;
int kinectDevId = 0;
int zigBeeDevId = 0;
int zWaveDevId = 0;
int irBlasterDevId = 0;

//Function declarations
void deviceEvent (std::string kCmd, int devId);
void readLircConfig(std::vector<std::string> &cmd);
std::string addNewRemote (const std::string &remName, tcp::socket& socket);

//Return 0 if the command is EndConversation else return 1 with  to send in the nextSendMsg
int processCommand (std::string cmd, std::string &nextSendMsg, tcp::socket& socket) {
  int returnCode = 1;
  if (cmd.find(CommandListNames[GET_ECHO_COMMAND]) != std::string::npos) {
    boost::regex re ("GET_ECHO_(.*)");
    boost::cmatch cm;
    std::stringstream ss;
    if (boost::regex_match(cmd.c_str(), cm, re)) {
      std::string remote (cm[1].first, cm[1].second);
      ss << remote << "\n";
      nextSendMsg = ss.str();
    } else {
      nextSendMsg = "ECHO\n";
    }
  } else if (cmd.find(CommandListNames[GET_NUM_DEVICES]) != std::string::npos) {
    //GET_NUM_DEVICES command - send number of devices present in the system
    std::stringstream ss;
    ss << devIdMap.size() << "\n";
    nextSendMsg = ss.str();
    std::cout << "GET_NUM_DEVICES command - Sending : " << nextSendMsg << std::endl;
  } else if (cmd.find(CommandListNames[GET_DEVICE_IDS]) != std::string::npos) {
    std::map<int, Device *>::iterator devItr;
    std::stringstream ss;
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
    std::string devIdStr;
    if (boost::regex_match(cmd.c_str(), cm, re)) {
      std::string val (cm[1].first, cm[1].second);
      devIdStr = val;
    }
    std::stringstream ss;
    std::stringstream devSS (devIdStr);
    int devId;
    devSS >> devId;
    if (devIdMap.find(devId) != devIdMap.end()) 
      ss << (devIdMap[devId])->getDeviceInfo() << "\n";
    else 
      ss << "NOTFOUND" << devId << "\n";
    
    nextSendMsg = ss.str();
    std::cout << cmd << " received - Sending : " << nextSendMsg << std::endl;
  } else if (cmd.find(CommandListNames[SET_MAP_ACTION]) != std::string::npos) {
    //SET_MAP_ACTION -- add a config - currently works only for event -> event based mapping!
    boost::regex re ("SET_MAP_(.*?)_(.*?)_(.*?)_(.*?)_(.*?)_(.*)");
    boost::cmatch cm;
    if (boost::regex_match(cmd.c_str(), cm, re)) {
      std::string sId(cm[1].first, cm[1].second); //source devId
      std::string sES(cm[2].first, cm[2].second); //source event/state
      std::string sName(cm[3].first, cm[3].second); //source action name
      std::string dId(cm[4].first, cm[4].second);  //destionation devId
      std::string dES(cm[5].first, cm[5].second);  //destination event/state
      std::string dName(cm[6].first, cm[6].second); //destination action name
      int sDevId = atoi(sId.c_str());
      int dDevId = atoi(dId.c_str());
      config newConfig;
      newConfig.cause_device_id = sDevId;
      newConfig.cause_state_event = (sES == "S") ? true : false;
      strcpy(newConfig.cause_name, sName.c_str());
      newConfig.effect_device_id = dDevId;
      newConfig.cause_state_event = (dES == "S") ? true : false; 
      strcpy(newConfig.effect_name, dName.c_str());
      vLock.lock();
      allMap.push_back(newConfig);
      vLock.unlock();
      std::cout << "Config Added : Source = " << sId << "::" << sName << " to " << "Dest = " << dId << "::" << dName << std::endl;
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
      std::string sId (cm[1].first, cm[1].second);
      std::string sES (cm[2].first, cm[2].second);
      std::string sName (cm[3].first, cm[3].second);
      int sDevId = atoi(sId.c_str());
      bool SorE = (sES == "S") ? true : false;
      vLock.lock();
      std::vector<config>::iterator it = allMap.begin();
      while (it != allMap.end()) {
        config c = *it;
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
    std::stringstream ss;
    vLock.lock();
    for (const config &c : allMap) {
      std::string es = (c.cause_state_event) ? "S" : "E";
      std::string es2 = (c.effect_state_change) ? "S" : "E";
      std::string ename (c.cause_name);
      std::string ename2 (c.effect_name);
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
      std::string kCmd (cm[1].first, cm[1].second);
      std::cout << kCmd << " received Kinect Command from WebServer " << std::endl;
      deviceEvent(kCmd, kinectDevId);
      nextSendMsg = "KINECTCMDOK\n";
    } else {
      nextSendMsg = "KINECTCMDFAIL\n";
    }
  } else if (cmd.find(CommandListNames[GET_CURRENT_REMOTE_NAMES]) != std::string::npos) {
    std::string ret = "";
    std::map<std::string, bool>::iterator it = availRemotes.begin();
    for ( ; it != availRemotes.end(); it++) {
      ret.append(it->first);
      ret.append(",");
    }
    ret.append("\n");
    std::cout << "Sending Remote Names : " << ret << std::endl;
    nextSendMsg = ret;
  } else if (cmd.find(CommandListNames[GET_REMOTE_VALID_KEYS]) != std::string::npos) {
    std::string ret = "";
    std::map<std::string, bool>::iterator it = validKeyNames.begin();
    for ( ; it != validKeyNames.end(); it++) {
      ret.append(it->first);
      ret.append(",");
    }
    ret.append("\n");
    std::cout << "Sending Valid Remote Keys : " << ret << std::endl;
    nextSendMsg = ret;
  } else if (cmd.find(CommandListNames[ADD_REMOTE]) != std::string::npos) {
    //ADD_REMOTE_<Name> - start adding a new Remote to the system
    boost::regex re ("ADD_REMOTE_(.*)");
    boost::cmatch cm;
    if (boost::regex_match(cmd.c_str(), cm, re)) {
      std::string remName (cm[1].first, cm[1].second);
      //Client should make sure it does not accept remote name with already existing name
      //Delete any remote file config in the current folder with the same name
      std::string cPath (currPath);
      cPath.append("/");
      cPath.append(remName);
      remove(cPath.c_str());
      cPath.append(".conf");
      remove(cPath.c_str());
      nextSendMsg = addNewRemote (remName, socket);
      if (nextSendMsg.find("FAIL") == std::string::npos) {
        //Success adding new remote, modifying lircd.conf; So restart lircd to make it recognize new remote
        //Lock config before restarting lircd, since we might have some commands issuing irsend
        std::cout << "Restarting LIRCD daemon" << std::endl;
        vLock.lock();
        system("sudo /etc/init.d/lirc restart");
        vLock.unlock();
      }
    } else {
      nextSendMsg = "ADDREMOTEFAIL\n";
    }
  } else if (cmd.find(CommandListNames[END_CONVERSATION]) != std::string::npos) {
    //END_CONVERSATIOn - ending the conversation with the client
    std::cout << cmd << " received - Ending Conversation Successfully ! " << std::endl;
    returnCode = 0;
  } else {
    std::cout << cmd << " received - Unknown Command ! " << std::endl;
    nextSendMsg = "UNKNOWNCMD\n";
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
    std::string rCmd;
    std::istream (&rbuf) >> rCmd;
    if (processCommand (rCmd, message, socket) > 0)
      continue;
    else
      break;
  }
  return returnCode;
}

void serveRequests (short port) {
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
      
      std::string rCmd;
      std::istream (&rbuf) >> rCmd; 
      if (rCmd == "JavaReady") {
        if (converse(socket) > 0) {
          std::cout << "Something happenend in Converse " << std::endl;
          break;
        }
      } else {
        std::cout << "Received Wrong Ack : " << rCmd << std::endl;
        break;
      }
    }
  }
}

void addValidKeyWords (std::map<std::string, bool> &validKeys) {
  //List obtained from "irrecord --list-namespace" ==> Get all the valid keyNames from here
  validKeys.insert(std::pair<std::string, bool> ("KEY_PLAY", true));
  validKeys.insert(std::pair<std::string, bool> ("KEY_PAUSE", true));
  validKeys.insert(std::pair<std::string, bool> ("KEY_POWER", true));
  validKeys.insert(std::pair<std::string, bool> ("KEY_STOP", true));
  validKeys.insert(std::pair<std::string, bool> ("KEY_NEXT", true));
  validKeys.insert(std::pair<std::string, bool> ("KEY_PREVIOUS", true));
}

void populateIREvents (Device * dev) {
  if (IR_EVENTS_DEFAULT) { //Populate it with default commands
    dev->addSubEvent("SONYNEW:KEY_PLAY");
    dev->addSubEvent("SONYNEW:KEY_PAUSE");
  } else { //Read the .conf file and populate the commands
    std::vector<std::string> commands;
    addValidKeyWords(validKeyNames);
    readLircConfig (commands);
    for (const std::string &s : commands)
      dev->addSubEvent(s);
  }
}

void populateZigBEvents (Device * dev) {
  dev->addPubEvent ("SWITCHON");
  dev->addPubEvent ("SWITCHOFF"); 
}

void populateZWaveEvents (Device * dev) {
  dev->addSubEvent ("TURNON");
  dev->addSubEvent ("TURNOFF");
}

void populateKinnectEvents (Device * dev) {
  dev->addPubEvent ("RIGHTCIRCLE");
  dev->addPubEvent ("LEFTCIRCLE");
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

void sendCmdToZigBee (std::string cmd) {
  //Send 'cmd' to ZigBee!
  std::cout << "Sending " << cmd << " to ZigBee " << std::endl;
}

void sendCmdToZWave (std::string cmd) {
  //Send 'cmd' to ZWave!
  std::cout << "Sending " << cmd << " to ZWave " << std::endl; 
}

void sendCmdToIRBlaster (std::string cmd) {
  //Split cmd into remote and the action type (<REMOTE_TYPE>_<ACTION>)
  //then use system call and irsend to perform the action!
  boost::regex re("(.*):(.*)");
  boost::cmatch cm;
  if (boost::regex_match(cmd.c_str(), cm, re)) {
    std::string remote(cm[1].first, cm[1].second);
    std::string rcmd (cm[2].first, cm[2].second);
    std::cout << "Sending to IRBlaster " << cmd << " Remote : " << remote << " Command : " << rcmd << std::endl;
    std::stringstream ss; 
    ss << "irsend " << "SEND_ONCE " << remote << " " << rcmd;
    system ((ss.str()).c_str());
  } else {
    std::cout << "Illegal Format received in sendCmdtoIRBlaster : " << cmd << std::endl;
  }
}

DEVICE_TYPE devIdToTypeMap (int devId) {
  if (devId == kinectDevId) 
    return KINECT_DEVICE;
  else if (devId == zigBeeDevId)
    return ZIGBEE_DEVICE;
  else if (devId == zWaveDevId) 
    return ZWAVE_DEVICE;
  else if (devId == irBlasterDevId)
    return IRBLASTER_DEVICE;
  else {
    std::cout << "Unknown Device ID! " << devId << std::endl;
    return DEVICE_TYPE_LAST;
  }
}

void performAction (DEVICE_TYPE dType, std::string cmd) {
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

void deviceEvent (std::string kCmd, int devId) {
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
  std::string kCmd(msg->cmd);
  deviceEvent(kCmd, kinectDevId);
}

void qeoLoop() {
  qeo_factory_t * qeo;
  qeo_event_reader_t * msg_reader;
  qeo = qeo_factory_create();
  kinect_event_listener.on_data = on_kinect_event;
  //Connect all the listener callback functions!
  if (qeo != NULL) {
    msg_reader = qeo_factory_create_event_reader(qeo, org_qeo_qeoblaster_qeoir_IRCommand_type, &kinect_event_listener, 0);
  } else {
    std::cout << "ERROR : Couldn't start QEO Listener!" << std::endl;
  }
  initDevices(); //Initialize current devices in the system
  std::cout << "QEO Loop started! " << std::endl;
  while (1) { }
}

void zigBeeComm() {
  while (1) {
    //Blocking Read -- If ZigBee is sending commands - if we get commands
    //lookup the config map and then act accordingly
    std::string cmd = "SWITCHOFF"; //TODO : Replace with a blocking read call here!
    while(1) {
      boost::this_thread::sleep(boost::posix_time::seconds(5));
    }
    std::cout << "SwitchApp : Received " << cmd <<" command from zigBee " << std::endl;
    deviceEvent (cmd, zigBeeDevId);
  }
}

void zWaveComm() {
  while (1) {
    //Blocking Read -- If ZWave is sending commands, if we get commands 
    //lookup the config map and then act accordingly
    std::string cmd = "SWITCHON"; //TODO: Replace with a blocking call here!
    while(1) { 
      boost::this_thread::sleep(boost::posix_time::seconds(5));
    }
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
    
    if (!getcwd(currPath, sizeof(currPath))) 
      return -1;

    if (chdir(currPath))
      return -1;

    using namespace std;
    boost::thread netWork(&serveRequests, atoi(argv[1]));  
    boost::thread qeoWork(&qeoLoop);
    boost::thread zWaveAgent(&zWaveComm);
    boost::thread zigBeeAgent(&zigBeeComm);
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


std::string addNewRemote (const std::string &remName, tcp::socket& socket) {
  using namespace std;
  exp_is_debugging = 0;
  exp_timeout = 30;
  exp_loguser = 0; //To suppress messages from process on STDOUT assign non-zero value
  std::string fail = "ADDREMOTEFAIL\n";
  std::string succ = "ADDREMOTESUCCESS\n";
  std::string command = "irrecord -H iguanaIR ";
  vector<string> keyNames;
  stringstream copy;
  command.append(remName);
  FILE * expect = exp_popen((char *) command.c_str());
  if (expect == 0) {
    return fail;
  }
  
  cout<<"Started Remote Add " << remName << endl;
  enum {initStage,randKey,firstRandKey,buttonStart,buttonPress,allSuccess,noData};
  boost::system::error_code syserr;
  struct exp_case cases[] = {
    { "Press RETURN to continue.",  NULL, exp_glob, initStage},
    { "Press RETURN now to start ", NULL, exp_glob, randKey},
    { "Please keep on pressing buttons", NULL, exp_glob, firstRandKey},
    { "Please enter the name for the next button", NULL, exp_glob, buttonStart},
    { "Now hold down button", NULL, exp_glob, buttonPress},
    { "Successfully written config file", NULL, exp_glob, allSuccess},
    { "irrecord: no data for 10 secs, aborting", NULL, exp_glob, noData},
    0
  };
  
  bool success = true;
  bool done = false;
  bool bDone = false;


  while (1) {
    std::string message = "SendCmd\n";
    boost::asio::streambuf rbuf, rbuf2, rbuf3;
    string response, rCmd, temp, button;
    if (!success || done)
      break;

    switch (exp_fexpectv(expect, cases)) {
      case initStage:
        if (!bDone) {
          cout<<"Reached initial Stage"<<endl;
          fprintf(expect, "\n"); //Send empty line
        }
        break;

      case randKey:
        cout<<"Reached Arbitrary Key Init : Send OK to continue"<<endl;
        //Here send a command to client telling it to prompt the user
        //to press Arbitrary buttons and then once we get ACK proceed
        //getline(cin, response);
        message = "ARBKEYSTART\n"; //Inform User to start pressing Arb Key (after pressing OK in a dialog box?)
        boost::asio::write(socket, boost::asio::buffer(message), syserr);
        if (syserr) {
          std::cout << "Some Error in Writing " << message << std::endl;
          success = false;
        } else {
          boost::asio::read_until (socket, rbuf, "\n", syserr);
          if (syserr) {
            std::cout << "Some Error while reading in AddRemote" << std::endl;
            success = false;
          } else {
            std::istream (&rbuf) >> rCmd;
            if (rCmd.find("OK") != std::string::npos) {
              fprintf(expect, "\n"); //Send empty line to begin recording
            } else {
              std::cout << "ArbKey Response : Expected OK ; Got " << rCmd << std::endl;
              success = false;
            }
          }
        }
        break;
     
      case firstRandKey:
        cout<<"You are doing great! Keep pressing Arb Keys!"<<endl;
        break;

      case buttonStart:
        cout<<"Asking User to Enter a Button Name : "<<endl;
        //Here ask for the user to enter a button name; Only valid button
        //names are accepted - generate a list beforehand!
        //List obtained from "irrecord --list-namespace
        message = "CHOOSEKEYNAME\n"; //Ask user to choose a key name
        boost::asio::write(socket, boost::asio::buffer(message), syserr);
        if (syserr) {
          std::cout << "Some Error in Writing " << message << std::endl;
          success = false;
        } else {
          boost::asio::read_until (socket, rbuf, "\n", syserr);
          if (syserr) {
            std::cout << "Some Error while reading in AddRemote" << std::endl;
            success = false;
          } else {
            std::istream (&rbuf) >> rCmd;
            if (validKeyNames.find(rCmd) != validKeyNames.end()) { //Valid Key Name
              button = rCmd;
              message = "KEYOKPRESSNOW\n"; //Ack that Key is OK and ask user to press button after pressing OK in a dialog box?
              boost::asio::write(socket, boost::asio::buffer(message), syserr);
              if (syserr) {
                std::cout << "Some Error in Writing " << message << std::endl;
                success = false;
              } else {
                boost::asio::read_until (socket, rbuf2, "\n", syserr);
                if (syserr) {
                  std::cout << "Some Error while reading in AddRemote" << std::endl;
                  success = false;
                } else {
                  std::istream (&rbuf2) >> rCmd;
                  if (rCmd.find("OK") != std::string::npos) {
                    fprintf(expect, "%s\n", button.c_str());
                    keyNames.push_back(button);
                  } else {
                    std::cout << "Not received OK for " << button << " received " << rCmd << " instead! " << std::endl;
                    success = false;
                  } //OK for button ACK
                } //OK read
              } //Write KEYOK
            } else if (rCmd.find("DONEWITHKEY") != std::string::npos) {
              //User Done with KEYS! 
              fprintf(expect, "\n");
              cout << "Done with Keys! Instructing User to Press same button " << endl;
              message = "KEYOKPRESSSAME\n";
              boost::asio::write(socket, boost::asio::buffer(message), syserr);
              if (syserr) {
                std::cout << "Some Error in Writing " << message << std::endl;
                success = false;
              } else {
                boost::asio::read_until (socket, rbuf2, "\n", syserr);
                if (syserr) {
                  cout << "Some Error while reading in AddRemote" << std::endl;
                  success = false;
                } else {
                  std::istream (&rbuf2) >> rCmd;
                  if (rCmd.find("OK") != std::string::npos) {
                    fprintf(expect, "\n");
                    bDone = true;
                  } else {
                    std::cout << "Not received OK for PressSameButton " << " received " << rCmd << std::endl;
                    success = false;
                  } // OK for Same Key Ack
                } //OK for read              
              } //Write same Key
            } else {
              cout << "Received wrong Key Name " << rCmd << endl;
              success = false;
            } //Got Key Name 
          } //OK for read
        } //Write error for Choose Key
        break;

      case buttonPress:
        break;
      
      case allSuccess:
      cout<<"Config File success!"<<endl;
      temp = "mv ";
      temp.append(remName);
      temp.append(" ");
      temp.append(remName);
      temp.append(".conf");
      cout << "Executing " << temp << endl;
      system(temp.c_str());
      copy << "sudo cp " << remName << ".conf" << " /usr/share/lirc/remotes/custom/." << "\n";
      cout << "Executing " << copy.str() << endl;
      system((copy.str()).c_str());
      temp = "sudo echo \"include ";
      temp.append("\\\"/usr/share/lirc/remotes/custom/");
      temp.append(remName);
      temp.append(".conf\\\"\" >> /etc/lirc/lircd.conf");
      system(temp.c_str());
      cout << "Executing " << temp << endl;
      {
       Device * irBlast = devIdMap[irBlasterDevId];
       if (irBlast != NULL) {
         for (string &s : keyNames) {
           string val = remName;
           val.append(":");
           val.append(s);
           irBlast->addSubEvent(val);
         }
       }
      }
      availRemotes.insert(std::pair<std::string, bool> (remName, true));
      done = true;
      break;
      
      case noData:
      cout << "No Data Found for 10seconds. Stopping recording!" << endl;
      success = false;
      break;

      case EXP_TIMEOUT:
      cout << "Hit Timeout in expect!" << endl;
      success = false;
      break; 
    }
  }

  fclose (expect);
  waitpid(exp_pid, 0, 0);
  if (success) 
    return succ;
  return fail;
}



std::string Device::getDeviceInfo () {
  std::stringstream ss;
  ss << name << "_" ;
  for (const struct event &e : eventPubList)
    ss << "EP_" << e.name << "_";
  for (const struct state &s : statePubList)
    ss << "SP_" << s.name << "_";
  for (const struct event &e : eventSubList)
    ss << "ES_" << e.name << "_";
  for (const struct state &s : stateSubList)
    ss << "SS_" << s.name << "_";
  return ss.str();
}


void readLircConfig(std::vector<std::string> &cmd) {
  using namespace std;
  std::string file = LIRCDFILE;
  ifstream f (file.c_str());
  if (!f.is_open()) {
    cout << "Couldn't open " << file << endl;
  } else {
    std::string line;
    while (getline(f, line)) {
      boost::regex re ("include \"(.*)\"");
      boost::cmatch cm;
      if (boost::regex_match(line.c_str(), cm, re)) {
        std::string remote (cm[1].first, cm[1].second);
        cout << remote << endl;
        ifstream rf (remote.c_str());
        if (!rf.is_open()) {
          cout << "Couldn't open " << remote << endl;
        } else {
          std::string key;
          string remKey = "";
          while (getline(rf, key)) {
            boost::regex nam (".*name  (.*)");
            boost::cmatch kn;
            string fKey = remKey;
            if (boost::regex_match(key.c_str(), kn, nam)) {
              std::string name (kn[1].first, kn[1].second);
              remKey.append(name);
              fKey = remKey;
              availRemotes.insert(pair<string, bool>(remKey, true));
              cout << "Remote Name " << fKey << endl;
            }

            boost::regex rel (".*(KEY_[A-Z]*).*0x.*");
            boost::cmatch km;
            if (boost::regex_match(key.c_str(), km, rel)) {
              std::string keyl (km[1].first, km[1].second);
              cout << keyl << endl;
              fKey.append(":");
              fKey.append(keyl);
              cout << "Key : " << fKey << endl;
              cmd.push_back(fKey);
            }
          }
        }
      } else {
        cout << "Didn't work" << endl;
      }
    }
  }
}
