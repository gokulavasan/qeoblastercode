#include <sys/wait.h>
#include <tcl8.5/expect.h>
#include <unistd.h>

#include <iostream>
#include <fstream>
#include <string>
#include <iterator>
#include <sstream>
#include <vector>
#include <map>

using namespace std;

map<string, bool> availRemotes; //Remotes that are present already!
map<string, bool> validKeyNames; //Valid Remote button KEY names
char currPath[FILENAME_MAX]; //Current Working Dir

bool checkValidButtonName (const string &sName) {
  return (validKeyNames.find(sName) != validKeyNames.end());
}

int remoteAddTask (const string &command) {
  exp_is_debugging = 0;
  exp_timeout = 30;
  exp_loguser = 0; //To suppress messages from process on STDOUT assign non-zero value
  
  FILE * expect = exp_popen((char *) command.c_str());
  if (expect == 0) {
    return 1;
  }
  
  cout<<"Started the program"<<endl;
  enum {initStage,randKey,firstRandKey,buttonStart,buttonPress,allSuccess};

  struct exp_case cases[] = {
    { "Press RETURN to continue.",  NULL, exp_glob, initStage},
    { "Press RETURN now to start ", NULL, exp_glob, randKey},
    { "Please keep on pressing buttons", NULL, exp_glob, firstRandKey},
    { "Please enter the name for the next button", NULL, exp_glob, buttonStart},
    { "Now hold down button", NULL, exp_glob, buttonPress},
    { "Successfully written config file", NULL, exp_glob, allSuccess},
    0
  };
  
  bool success = true;
  bool done = false;
  bool bDone = false;
  while (1) {
    string response;
    string temp;
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
        cout<<"Reached Arbitrary Key Init : Press OK to continue"<<endl;
        //Here send a command to client telling it to prompt the user
        //to press Arbitrary buttons and then once we get ACK proceed
        getline(cin, response);
        //Wait for OK/ACK from client
        if (response == "OK")
          fprintf(expect, "\n"); //Send empty line to begin recording
        else
          success = false;
        break;
     
      case firstRandKey:
        cout<<"You are doing great! Keep pressing Arb Keys!"<<endl;
        break;

      case buttonStart:
        cout<<"Enter a button name : "<<endl;
        //Here ask for the user to enter a button name; Only valid button
        //names are accepted - generate a list beforehand!
        //List obtained from "irrecord --list-namespace
        getline(cin, response);
        if (response == "bDone") {
          //Option for Done with buttons
          fprintf(expect, "\n");
          cout<<"Press Same button repeatedly now : Press OK to ack"<<endl;
          getline (cin, temp);
          if (temp == "OK") {
            fprintf(expect, "\n");
            bDone = true;
          } else
            success = false;
        } else if (!checkValidButtonName(response)) {
          cout<<"Enter a valid button name : "<<endl;
          fprintf(expect, "%s\n", response.c_str());
        } else {
          cout<<"Press the button Now : Press OK to ack"<<endl;
          getline(cin, temp);
          //Wait for OK from client 
          if (temp == "OK")
            fprintf(expect, "%s\n", response.c_str());
          else
            success = false;
        }
        break;
      case buttonPress:
        break;
      
      case allSuccess:
        cout<<"Config File success!"<<endl;
        done = true;
        break;
       
      case EXP_TIMEOUT:
        success = false;
        break; 
    }
  }

  fclose (expect);
  waitpid(exp_pid, 0, 0);
  if (success) 
    return 0;
  return 1;
}

void initRemoteKeyNames() {
  validKeyNames.insert(pair<string, bool>("KEY_PLAY", true));
  validKeyNames.insert(pair<string, bool>("KEY_PAUSE", true));
  validKeyNames.insert(pair<string, bool>("KEY_STOP", true));
  validKeyNames.insert(pair<string, bool>("KEY_NEXT", true));
  validKeyNames.insert(pair<string, bool>("KEY_PREVIOUS", true));
  validKeyNames.insert(pair<string, bool>("KEY_POWER", true));
}

bool isRemoteNameValid (const string &rName) {
  //Look into availRemotes map
  return true;
}

int removeRemoteFromConfig (const string &rName) {
  //Look for the remoteName in lircd.conf and remove it
  //Any addition lircd.conf will take effect only when lircd is restarted! -- Should be a locked resource so that irsend doesn't 
  //try to issue commands at the same time? 
}

int addRemoteToConfig (const string &rName) {
  //Look for that name in the lircd.conf if NOT present, look for it under custom and add it to lircd.conf
}

int initRemote() {
  int errCode = 0;
  if (!getcwd(currPath, sizeof(currPath))) 
    return errno;

  if (chdir(currPath))
    return errno;
  string cPath(currPath);
  string remoteName;
  string remoteAddCmd = "irrecord -H iguanair ";
  
  do {
   cout<<"Enter the remote Name: "<<endl;
   getline(cin, remoteName);
  } while (!isRemoteNameValid(remoteName));

  cPath.append("/");
  cPath.append(remoteName);
  remoteAddCmd.append(remoteName);
  remove(cPath.c_str());
  if (errCode = remoteAddTask(remoteAddCmd)) {
    //Send this to the client!
    cout<<"Config Didn't Work Properly! Try Again!!"<<endl;
  } else {
    availRemotes.insert(pair<string, bool>(remoteName, true));
    //Move the config file into /usr/shared/lirc/remotes/custom/.
  }
  return errCode;
}

void initRemoteNames() {
  //Go into /etc/lirc/lircd.conf and look for remote paths that are there; Extract the name and populate
  //the availRemotes variable

}

int main() {
  initRemoteNames();
  initRemoteKeyNames();
  //Start a Server and listen to commands from client and act on it ! -- This must be made multithreaded to allow normal
  //QEO activity to continue simultaneously; The commonly accessed data structures should be protected by locks!
  initRemote(); 
  return 0;
} 
