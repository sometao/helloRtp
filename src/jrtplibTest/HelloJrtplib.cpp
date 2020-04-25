#include "seeker/logger.h"
#include "seeker/loggerApi.h"
#include "jrtplib3/rtpsession.h"
#include "jrtplib3/rtpudpv4transmitter.h"
#include "jrtplib3/rtpipv4address.h"
#include "jrtplib3/rtpsessionparams.h"
#include "jrtplib3/rtperrors.h"
#include "jrtplib3/rtplibraryversion.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>


using std::cout;
using std::endl;
using std::string;


namespace {

using jrtplib::RTPGetErrorString;
using jrtplib::RTPIPv4Address;
using jrtplib::RTPLibraryVersion;
using jrtplib::RTPPacket;
using jrtplib::RTPSession;
using jrtplib::RTPSessionParams;
using jrtplib::RTPTime;
using jrtplib::RTPUDPv4TransmissionParams;



void checkerror(int rtperr, const string& mark = "") {
  if (rtperr < 0) {
    E_LOG("[{}] ERROR: {}", mark, RTPGetErrorString(rtperr));
    exit(-1);
  }
}

void test1(int argc, char* argv[]) {
#ifdef RTP_SOCKETTYPE_WINSOCK
  WSADATA dat;
  WSAStartup(MAKEWORD(2, 2), &dat);
#endif  // RTP_SOCKETTYPE_WINSOCK
  if( argc != 5 ) {
    cout << "Usage: portBase destIP destPort num" << endl;
    return;
  }

  int portBase = std::stoi(argv[1]);
  string destIpStr = argv[2];
  int destPort = std::stoi(argv[3]);
  int num = std::stoi(argv[4]);

  I_LOG("portBase[{}] send [{}] msg to [{}:{}]", portBase, num, destIpStr, destPort);


  I_LOG("Using version: {}", RTPLibraryVersion::GetVersion().GetVersionString() );

  RTPSessionParams sessparams{};
  sessparams.SetOwnTimestampUnit(1.0 / 10.0);
  sessparams.SetAcceptOwnPackets(true);

  RTPUDPv4TransmissionParams transparams{};
  transparams.SetPortbase(portBase);

  int status;
  RTPSession sess{};
  status = sess.Create(sessparams, &transparams);


  uint32_t destip = inet_addr(destIpStr.c_str());
  if (destip == INADDR_NONE) {
    std::cerr << "Bad IP address specified" << std::endl;
    throw std::runtime_error("Bad IP address specified");
  }
  destip = ntohl(destip);
  RTPIPv4Address addr(destip, destPort);
  status = sess.AddDestination(addr);
  checkerror(status);


  for (int i = 1; i <= num; i++) {
    printf("\nSending packet %d/%d\n", i, num);

    // send the packet
    status = sess.SendPacket((void *)"1234567890", 10, 0, false, 10);
    checkerror(status);

    sess.BeginDataAccess();
    RTPPacket *pack;
    if (sess.GotoFirstSourceWithData()) {
      do {
        while ((pack = sess.GetNextPacket()) != nullptr) {
          printf("Got packet !\n");
          sess.DeletePacket(pack);
        }
      } while (sess.GotoNextSourceWithData());
    }
    sess.EndDataAccess();

#ifndef RTP_SUPPORT_THREAD
    status = sess.Poll();
    checkerror(status);
#endif  // RTP_SUPPORT_THREAD

    RTPTime::Wait(RTPTime(0, 1000000));
  }

  sess.BYEDestroy(RTPTime(10, 0), 0, 0);

#ifdef RTP_SOCKETTYPE_WINSOCK
  WSACleanup();
#endif  // RTP_SOCKETTYPE_WINSOCK
}


}  // namespace


int main(int argc, char* argv[]) {

  for(int i = 0; i < argc; i++) {
    cout << i << ": " << argv[i] << endl;
  }

  seeker::Logger::init();
  cout << "Hello, JRTPLib." << endl;
  test1(argc, argv);
  cout << "done." << endl;
  return 0;
}
