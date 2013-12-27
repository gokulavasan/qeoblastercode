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
#include <boost/thread.hpp>
#include <mutex>

extern "C" {
#include <qeo/api.h>
#include "../irblasterpi/QIRBlaster.h" //Move all header files to one location?
#include "config_structs.h"
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
  vector<event> eventPubList;  //Events published -- Commands from Kinnect for ex
  vector<state> statePubList;  //States published -- State from some device for ex
  vector<event> eventSubList;  //Events subscribed -- Commands for IR Blaster for ex
  vector<state> stateSubList;  //States subscribed -- Not sure where this will be used 
  public:
  Device (std::string name_) : name (name_), id (globalDevId++) 
  { }
  std::string getDeviceName(return name;);
  std::string getDeviceInfo();
  virtual void executeEvent(string name) = 0;

  int getDeviceId() { return id; }
  void addPubEvent (string eventName) {
    event newEvent;
    newEvent.name = eventName;
    eventPubList.push_back(newEvent);
  }
  void addPubState (string stateName) {
    state newState;
    newState.name = stateName;
    statePubList.push_back(newState);
  }
  void addSubEvent (string eventName) {
    event newEvent;
    newEvent.name = eventName;
    eventSubList.push_back(newEvent);
  }
  void addSubState (string stateName) {
    state newState;
    newState.name = stateName;
    stateSubList.push_back(newState);
  }
};

enum CommandList {
  GET_NUM_DEVICES, //Number of devices
  GET_DEVICE_INFO, //Device ID => Name, State, Event
  END_CONVERSATION //Command to end Conversation
};

static const char * CommandListNames[] = {
  "GET_NUM_DEVICES\n",
  "GET_DEVICE_INFO_", //partial match - GET_DEVICE_INFO_<DeviceID>
  "END_CONVERSATION"
};

#endif
