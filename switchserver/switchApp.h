#ifndef SWITCH_APP_H_
#define SWITCH_APP_H_

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <unordered_map>
#include <boost/asio.hpp>
#include <boost/date_time.hpp>
#include <boost/regex.hpp>
#include <boost/thread.hpp>
#include <mutex>

extern "C" {
#include <qeo/api.h>
#include "../irblasterpi/QIRBlaster.h" //Move all header files to one location?
//TODO : Add ZWave and ZigBee header files!
#include "../irblasterpi/config_structs.h"
}


struct event {
  std::string name;
};
  
struct state {
  std::string name;
};

class Device
{
  private:
  static int globalDevId;
  int id;
  std::string name;
  std::vector<event> eventPubList;  //Events published -- Commands from Kinnect for ex
  std::vector<state> statePubList;  //States published -- State from some device for ex
  std::vector<event> eventSubList;  //Events subscribed -- Commands for IR Blaster for ex
  std::vector<state> stateSubList;  //States subscribed -- Not sure where this will be used 
  public:
  Device (std::string name_) : name (name_), id (globalDevId++) 
  { }
  std::string getDeviceName() { return name; }
  std::string getDeviceInfo();
  //virtual void executeEvent(std::string name) = 0;

  int getDeviceId() { return id; }
  void addPubEvent (std::string eventName) {
    event newEvent;
    newEvent.name = eventName;
    eventPubList.push_back(newEvent);
  }
  void addPubState (std::string stateName) {
    state newState;
    newState.name = stateName;
    statePubList.push_back(newState);
  }
  void addSubEvent (std::string eventName) {
    event newEvent;
    newEvent.name = eventName;
    eventSubList.push_back(newEvent);
  }
  void addSubState (std::string stateName) {
    state newState;
    newState.name = stateName;
    stateSubList.push_back(newState);
  }
};

typedef enum DEVICE_TYPE {
  ZIGBEE_DEVICE = 1,
  ZWAVE_DEVICE,
  IRBLASTER_DEVICE,
  KINECT_DEVICE,
  DEVICE_TYPE_LAST
};

enum CommandList {
  GET_ECHO_COMMAND, //Echo Command for testing - return the command part of it!
  GET_NUM_DEVICES, //Number of devices
  GET_DEVICE_IDS,
  GET_DEVICE_INFO, //Device ID => Name, State, Event
  SET_MAP_ACTION, //Action -> Reaction
  SET_UNMAP_ACTION, //Unmap the Action
  GET_ALL_MAP, //Get all current mappings
  KINECT_CMD, //Send Kinect command from WebServer for testing
  END_CONVERSATION //Command to end Conversation
};

static const char * CommandListNames[] = {
  "GET_ECHO_", //partial match - GET_ECHO_<Sentence> => returns the <Sentence> part of the command back
  "GET_NUM_DEVICES",
  "GET_DEVICE_IDS",
  "GET_DEVICE_INFO_", //partial match - GET_DEVICE_INFO_<DeviceID>
  "SET_MAP_", //partial match - SET_MAP_<DevId>_<E>_<ESName>_<DevId>_<E>_<ESName>
  "SET_UNMAP_", //partial match - SET_UNMAP_<DevId>_<E>_<ESName> #Unmaps all actions associated with this occurence
  "GET_ALL_MAP", //send the current mapping config
  "KINECT_CMD_", //partial match - KINECT_CMD_<Cmd> #For testing actions on kinect commands!
  "END_CONVERSATION"
};

#endif
