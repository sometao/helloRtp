#include <iostream>
#include <list>
#include <memory>
#include <mutex>
#include <chrono>
#include <condition_variable>
#include <thread>
#include <atomic>



extern "C" {
#include "SDL/SDL.h"
};


#ifdef _WIN32
// Windows
extern "C" {
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
};
#else
// Linux...
#ifdef __cplusplus
extern "C" {
#endif
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <SDL/SDL.h>
#ifdef __cplusplus
};
#endif
#endif



namespace {

using std::cout;
using std::endl;
using std::list;
using std::string;
using std::unique_ptr;

list<unique_ptr<AVPacket>> pktList{};
std::mutex mx{};
std::condition_variable cv{};


// auto pktDeleter = [](AVPacket* p) { av_packet_free(&p); };

AVFormatContext* formatCtx = nullptr;


std::atomic<int> videoIndex{-1};

int audioIndex = -1;

void pktGrabber(const std::string& url) {
  // get pkt from avformat and put it to pktList.


  formatCtx = avformat_alloc_context();
  if (avformat_open_input(&formatCtx, url.c_str(), NULL, NULL) != 0) {
    string errorMsg = "Can not open input file:";
    errorMsg += url;
    cout << errorMsg << endl;
    throw std::runtime_error(errorMsg);
  }

  if (avformat_find_stream_info(formatCtx, NULL) < 0) {
    string errorMsg = "Can not find stream information in input file:";
    errorMsg += url;
    cout << errorMsg << endl;
    throw std::runtime_error(errorMsg);
  }

  for (int i = 0; i < formatCtx->nb_streams; i++) {
    if (formatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO &&
        videoIndex.load() == -1) {
      videoIndex.store(i);
      cout << "video stream index = : [" << i << "]" << endl;
    }

    if (formatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO && audioIndex == -1) {
      audioIndex = i;
      cout << "audio stream index = : [" << i << "]" << endl;
    }
  }

  while (true) {
    unique_ptr<AVPacket> pkt(av_packet_alloc());
    if (av_read_frame(formatCtx, pkt.get()) >= 0) {
      cout << "get a pkt." << endl;
      pkt->stream_index;
      {
        std::lock_guard<std::mutex> lg{mx};
        pktList.push_back(std::move(pkt));
      }
      cv.notify_one();
    } else {
      cout << "no more pkt" << endl;
      auto p = pkt.release();
      av_packet_free(&p);
      pktList.push_back(nullptr);
      break;
    }
  }
  videoIndex.store(INT_MAX);

  cout << "pktGrabber finish" << endl;

}


void decodeAndShow(int streamIndex) {
  // get pkt from pktList and decode, then show it.

  if(streamIndex == INT_MAX) {
    return;
  }


  int width = 640;
  int height = 480;

  AVCodec* codec = avcodec_find_decoder(formatCtx->streams[streamIndex]->codecpar->codec_id);

  if (codec == nullptr) {
    string errorMsg = "Could not find codec: ";
    errorMsg += formatCtx->streams[streamIndex]->codecpar->codec_id;
    cout << errorMsg << endl;
    throw std::runtime_error(errorMsg);
  }

  AVCodecContext* codecCtx = avcodec_alloc_context3(codec);

  if (avcodec_parameters_to_context(codecCtx, formatCtx->streams[streamIndex]->codecpar) !=
      0) {
    string errorMsg = "Could not copy codec context: ";
    errorMsg += codec->name;
    cout << errorMsg << endl;
    throw std::runtime_error(errorMsg);
  }

  if (avcodec_open2(codecCtx, codec, nullptr) < 0) {
    string errorMsg = "Could not open codec: ";
    errorMsg += codec->name;
    cout << errorMsg << endl;
    throw std::runtime_error(errorMsg);
  }

  cout << " [" << codecCtx->codec->name << "] codec context initialize success." << endl;
  AVFrame* frame = av_frame_alloc();



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



  while (true) {
    std::unique_ptr<AVPacket> pkt{nullptr};
    AVPacket* targetPkt = nullptr;

    {
      std::unique_lock<std::mutex> lk{mx};
      cv.wait(lk, [&]() { return !pktList.empty(); });
      auto pkt = std::move(pktList.front());
      pktList.pop_front();
      if (pkt != nullptr) {
        targetPkt = pkt.release();

        int ret = -1;
        ret = avcodec_send_packet(codecCtx, pkt.release());

        ret = avcodec_receive_frame(codecCtx, frame);
        if (ret == 0) {
          // show pic

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
        }

      } else {
        cout << "nullptr pkt" << endl;
        break;
      }
    }
  }

  cout << "decodeAndShow finish" << endl;

}



}  // namespace



void ffplay() {

  std::cout << "ffplay..." << std::endl;

  //const std::string url = "rtp://127.0.0.1:8000";
  const std::string url = "D:/data/video/rtptest/rtp_h264a.sdp";

  std::thread t1(pktGrabber, std::ref(url));
  std::cout << "pktGrabber start." << std::endl;

  while (videoIndex.load() < 0 && videoIndex.load() != INT_MAX) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::cout << "sleep:" << videoIndex.load() << std::endl;
  }

  std::cout << "sleep finish." << std::endl;

  std::thread t2(decodeAndShow, videoIndex.load() );
  std::cout << "decodeAndShow start." << std::endl;


  t1.join();

  t2.join();


  std::cout << "DONE." << std::endl;

}