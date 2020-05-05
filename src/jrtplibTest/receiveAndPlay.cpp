#include "jrtplib3/rtpsession.h"
#include "jrtplib3/rtpsessionparams.h"
#include "jrtplib3/rtpudpv4transmitter.h"
#include "jrtplib3/rtpipv4address.h"
#include "jrtplib3/rtptimeutilities.h"
#include "jrtplib3/rtppacket.h"
#include <stdlib.h>
#include <iostream>
#include <list>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <fstream>
#include "seeker/common.h"
#include "seeker/loggerApi.h"
#include "H264Decoder.h"



using namespace jrtplib;


class Player {
  std::condition_variable cv{};
  std::mutex pktListMutex{};
  std::list<std::unique_ptr<AVPacket>> packetList{};
  CH264Decoder decoder{};

  void worker() {
    AVFrame* frame = nullptr;
    AVPacket* targetPkt = nullptr;
    while (true) {
      {//locked scope.
        std::unique_lock<std::mutex> lk(pktListMutex);
        cv.wait(lk, [this] { return !packetList.empty(); });
        auto pkt = std::move(packetList.front());
        if (pkt == nullptr) {
          targetPkt = nullptr;
        } else {
          targetPkt = pkt.release();
          packetList.pop_front();
        }
      }
      if( targetPkt != nullptr) {
        frame = decoder.decodePacket(targetPkt);
        //TODO draw frame on SDL.
      } else {
        std::cout << "no more pkt, work finish." << std::endl;
        break;
      }
    }
  }

 public:
  void start() {}

  void pushPacket(std::unique_ptr<AVPacket> pkt) {
    {
      std::lock_guard<std::mutex> lg(pktListMutex);
      packetList.push_back(std::move(pkt));
    }
    cv.notify_one();
  }
};



int recevieAndPlay() {
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
  uint64_t totalExtLength = 0;
  uint64_t totalPktLength = 0;
  uint64_t totalPayLoadLength = 0;
  bool done = false;

  std::ofstream fout("rtp_received.h264", std::ios::binary);


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
        RTPPacket* packet;
        // std::cout << "check packet" << std::endl;
        while ((packet = session.GetNextPacket()) != 0) {
          if (packet->GetPayloadType() != 96) {
            std::cout << "PayloadType != 96 : [" << packet->GetPayloadType() << "]"
                      << std::endl;
            continue;
          }

          c += 1;
          totalPktLength += packet->GetPacketLength();
          totalPayLoadLength += packet->GetPayloadLength();
          totalExtLength += packet->GetExtensionLength();

          uint8_t* payload = packet->GetPayloadData();


          fout.write((char*)payload, packet->GetPayloadLength());

          if (c % 100 == 0) {
            auto msg = fmt::format(
                "[{}] Got pkt SequenceNumber={} extendedSequenceNumber={} SSRC={} length={}",
                c,
                packet->GetSequenceNumber(),
                packet->GetExtendedSequenceNumber(),
                packet->GetSSRC(),
                packet->GetPacketLength());
            std::cout << msg << std::endl;
          }

          session.DeletePacket(packet);
        }
      } while (session.GotoNextSource());
    }
    session.EndDataAccess();
    RTPTime::Wait(delay);

    RTPTime t = RTPTime::CurrentTime();
    t -= starttime;
    if (t > RTPTime(20.0)) done = true;
  }



  std::string msg =
      fmt::format("total pktCount={}, pktLen={} byte,  payLoadLen={} byte,  extLen={}",
                  c,
                  totalPktLength,
                  totalPayLoadLength,
                  totalExtLength);
  std::cout << msg << std::endl;


  delay = RTPTime(3.0);
  session.BYEDestroy(delay, "Time's up", 9);

#ifdef RTP_SOCKETTYPE_WINSOCK
  WSACleanup();
#endif  // RTP_SOCKETTYPE_WINSOCK
  return 0;
}