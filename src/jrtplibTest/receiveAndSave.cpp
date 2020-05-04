#include "jrtplib3/rtpsession.h"
#include "jrtplib3/rtpsessionparams.h"
#include "jrtplib3/rtpudpv4transmitter.h"
#include "jrtplib3/rtpipv4address.h"
#include "jrtplib3/rtptimeutilities.h"
#include "jrtplib3/rtppacket.h"
#include <stdlib.h>
#include <iostream>
#include "seeker/common.h"
#include "seeker/loggerApi.h"

using namespace jrtplib;

int recevieAndSave() {
#ifdef RTP_SOCKETTYPE_WINSOCK
  WSADATA dat;
  WSAStartup(MAKEWORD(2, 2), &dat);
#endif  // RTP_SOCKETTYPE_WINSOCK

  RTPSession session;

  RTPSessionParams sessionparams;
  sessionparams.SetOwnTimestampUnit(1.0 / 90000.0);

  RTPUDPv4TransmissionParams transparams;
  transparams.SetPortbase(8000);

  int status = session.Create(sessionparams, &transparams);
  if (status < 0) {
    std::cerr << RTPGetErrorString(status) << std::endl;
    exit(-1);
  }

  uint8_t localip[] = {127, 0, 0, 1};
  RTPIPv4Address addr(localip, 9000);

  status = session.AddDestination(addr);
  if (status < 0) {
    std::cerr << RTPGetErrorString(status) << std::endl;
    exit(-1);
  }

  // session.SetDefaultPayloadType(96);
  // session.SetDefaultMark(false);
  // session.SetDefaultTimestampIncrement(160);

  uint8_t silencebuffer[160];
  for (int i = 0; i < 160; i++) silencebuffer[i] = 128;

  RTPTime delay(0.005);
  RTPTime starttime = RTPTime::CurrentTime();

  int c = 0;
  bool done = false;
  while (!done) {
    // std::cout << "send pkt" << std::endl;
    // status = session.SendPacket(silencebuffer, 160);
    // if (status < 0) {
    //  std::cerr << RTPGetErrorString(status) << std::endl;
    //  exit(-1);
    //}

    session.BeginDataAccess();
    if (session.GotoFirstSource()) {
      do {
        RTPPacket *packet;

        while ((packet = session.GetNextPacket()) != 0) {
          c += 1;
          auto msg = fmt::format(
              "[{}] Got pkt SequenceNumber={} extendedSequenceNumber={} SSRC={} length={}",
              c,
              packet->GetSequenceNumber(),
              packet->GetExtendedSequenceNumber(),
              packet->GetSSRC(),
              packet->GetPacketLength());
          std::cout << msg << std::endl;
          session.DeletePacket(packet);
        }
      } while (session.GotoNextSource());
    }
    session.EndDataAccess();

    RTPTime::Wait(delay);

    RTPTime t = RTPTime::CurrentTime();
    t -= starttime;
    if (t > RTPTime(60.0)) done = true;
  }

  delay = RTPTime(10.0);
  session.BYEDestroy(delay, "Time's up", 9);

#ifdef RTP_SOCKETTYPE_WINSOCK
  WSACleanup();
#endif  // RTP_SOCKETTYPE_WINSOCK
  return 0;
}
