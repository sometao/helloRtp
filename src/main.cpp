#include "littleGift.h"
#include "seeker/logger.h"
#include <iostream>
#include <string>


using std::cout;
using std::endl;
using std::string;


extern void startClient(const string& targetIp, int port);
extern void startServer(int port);

namespace {



  void testUdpServer(){
    startServer(54321);
  }

  void testUdpClient(){
    startClient("127.0.0.1", 54321);
  }


}



int main(int argc, char* argv[]) {
  seeker::Logger::init();

  if( argc == 2 ) {
    const string cmd = argv[1];
    if(cmd == "server") {
      testUdpServer();
    } else if ( cmd == "client") {
      testUdpClient();
    } else {
      cout << "input error: " << cmd << endl;
    }
  
  } else {
    cout << "input error." << endl;
  }

  //testUdp();

  I_LOG("DONE.");


}
