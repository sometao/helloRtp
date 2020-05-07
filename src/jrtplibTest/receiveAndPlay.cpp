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

extern "C" {
#include "SDL/SDL.h"
};

using namespace jrtplib;

using std::cout;
using std::endl;
using std::string;

class Player {
  std::condition_variable cv{};
  std::mutex pktListMutex{};
  std::list<std::unique_ptr<AVPacket, void (*)(AVPacket*)>> packetList{};
  CH264Decoder decoder{};

  int width;
  int height;


  void worker() {
    cout << "Worker start." << endl;
    SDL_Window* screen;
    // SDL 2.0 Support for multiple windows
    screen = SDL_CreateWindow("Simplest Video Play SDL2", SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED, width / 2, height / 2,
                              SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (!screen) {
      string errMsg = "SDL: could not create window - exiting:";
      errMsg += SDL_GetError();
      cout << errMsg << endl;
      throw std::runtime_error(errMsg);
    }

    SDL_Renderer* sdlRenderer = SDL_CreateRenderer(screen, -1, 0);

    // IYUV: Y + U + V  (3 planes)
    // YV12: Y + V + U  (3 planes)
    Uint32 pixformat = SDL_PIXELFORMAT_IYUV;


    SDL_Texture* sdlTexture =
        SDL_CreateTexture(sdlRenderer, pixformat, SDL_TEXTUREACCESS_STREAMING, width, height);

    AVFrame* frame = nullptr;
    AVPacket* targetPkt = nullptr;

    while (true) {
      {  // locked scope.
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
      if (targetPkt != nullptr) {
        frame = decoder.decodePacket(targetPkt);
        av_packet_free(&targetPkt);

        if (frame != nullptr) {
          cout << "draw a picture." << endl;
          SDL_UpdateYUVTexture(sdlTexture,  // the texture to update
                               NULL,  // a pointer to the rectangle of pixels to update, or
                                      // NULL to update the entire texture
                               frame->data[0],      // the raw pixel data for the Y plane
                               frame->linesize[0],  // the number of bytes between rows of
                                                    // pixel data for the Y plane
                               frame->data[1],      // the raw pixel data for the U plane
                               frame->linesize[1],  // the number of bytes between rows of
                                                    // pixel data for the U plane
                               frame->data[2],      // the raw pixel data for the V plane
                               frame->linesize[2]  // the number of bytes between rows of pixel
                                                   // data for the V plane
          );
          SDL_RenderClear(sdlRenderer);
          SDL_RenderCopy(sdlRenderer, sdlTexture, NULL, NULL);
          SDL_RenderPresent(sdlRenderer);

        } else {
          cout << "WARNING, frame is nullptr." << endl;
        }
      } else {
        std::cout << "no more pkt, work finish." << std::endl;
        break;
      }
    }
  }

 public:
  Player(int w, int h) : width(h), height(h) {}
  ~Player() { cout << "deconstruct Player" << endl; }

  void start() {
    I_LOG("play started.");
    std::thread w(&Player::worker, this);
    w.detach();
  }

  void pushPacket(std::unique_ptr<AVPacket, void (*)(AVPacket*)> pkt) {
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

  // std::ofstream fout("rtp_received.h264", std::ios::binary);


  Player player(1280, 720);
  player.start();

  uint8_t* packetBuffer = new uint8_t[1920 * 1080]();
  int bufPos = 0;
  int h264PktLen = 0;

  uint8_t* payload;

  while (!done) {
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

          // fout.write((char*)payload, packet->GetPayloadLength());

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

          c += 1;

          payload = packet->GetPayloadData();
          auto len = packet->GetPayloadLength();
          totalPayLoadLength += len;

          if (packet->HasMarker()) {
            memcpy(&packetBuffer[bufPos], payload, len);
            int size = len + bufPos;
            bufPos = 0;
            // create AVPacket and send to decoder.


            std::unique_ptr<AVPacket, void (*)(AVPacket*)> pkt{
                av_packet_alloc(), [](AVPacket* p) { av_packet_free(&p); }};

            //av_parser_parse2();
            uint8_t* data = (uint8_t*)av_malloc(size);
            memcpy(data, packetBuffer, size);

#if 1
            av_packet_from_data(pkt.get(), data, size);  // solution 1
#else
            av_new_packet(pkt.get(), size);  // solution 2
            pkt->data = data;
#endif
            player.pushPacket(std::move(pkt));

          } else {
            memcpy(&packetBuffer[bufPos], payload, len);
            bufPos = bufPos + len;
          }


          session.DeletePacket(packet);
        }
      } while (session.GotoNextSource());
    }
    session.EndDataAccess();
    RTPTime::Wait(delay);

    RTPTime t = RTPTime::CurrentTime();
    t -= starttime;
    if (t > RTPTime(30.0)) done = true;
  }

  delete[] packetBuffer;
  // packetBuffer = nullptr;

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
  cout << "play out" << endl;
  return 0;
}
