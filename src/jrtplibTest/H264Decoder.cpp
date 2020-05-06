/****************************************************************************
filename:           H264Decoder.cpp
Author:             linlongqing
Date:               2017/9/12
Discription:
url:https://github.com/linshufei/UbuntuReceivePicture/blob/master/ReceiveSocket/H264Decoder.cpp
****************************************************************************/

#include "H264Decoder.h"
#include <iostream>


CH264Decoder::CH264Decoder() {
  avcodec_register_all();
  pFormatCtx = avformat_alloc_context();
  packet = (AVPacket*)av_malloc(sizeof(AVPacket));

  pCodec = avcodec_find_decoder(AV_CODEC_ID_H264);
  if (pCodec == NULL) {
    printf("Fail to get decoder !\n");
  }

  pCodecCtx = avcodec_alloc_context3(pCodec);
  if (pCodecCtx == NULL) {
    printf("Fail to get decoder context !\n");
  }

  pCodecCtx->pix_fmt = AV_PIX_FMT_YUV420P;
  pCodecCtx->codec_type = AVMEDIA_TYPE_VIDEO;

  if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
    printf("Fail to open decoder !\n");
  }

  pFrame = av_frame_alloc();

  frameCount = 0;
}

CH264Decoder::~CH264Decoder() {
  //std::cout << "de CH264Decoder begin" << std::endl;
  avcodec_close(pCodecCtx);
  //std::cout << "de CH264Decoder 1" << std::endl;
  av_free(pCodecCtx);
  //std::cout << "de CH264Decoder 2" << std::endl;
  av_frame_free(&pFrame);
  //std::cout << "de CH264Decoder 3" << std::endl;

  if (packet != NULL) {
  //std::cout << "de CH264Decoder 4" << std::endl;
    //delete packet;
    av_packet_free(&packet);
  //std::cout << "de CH264Decoder 5" << std::endl;
  }
  //std::cout << "de CH264Decoder end" << std::endl;
}

int CH264Decoder::Decode(uint8_t* pDataIn, int nInSize, uint8_t* pDataOut) {
  packet->size = nInSize;
  packet->data = pDataIn;

  int gotPicture;
  int ret = avcodec_decode_video2(pCodecCtx, pFrame, &gotPicture, packet);
  if (ret < 0) {
    printf("Decode Error.\n");
  }
  if (gotPicture) {
    return 0;
  }

  return -1;
}

AVFrame* CH264Decoder::decodePacket(AVPacket* pkt) {
  int gotPicture;
  int ret = avcodec_decode_video2(pCodecCtx, pFrame, &gotPicture, pkt);
  if (ret < 0) {
    printf("Decode Error.\n");
  }
  if (gotPicture) {
    return pFrame;
  }
  return nullptr;
}




int CH264Decoder::GetSize(int& width, int& height) {
  width = pFrame->width;
  height = pFrame->height;

  return 0;
}

int CH264Decoder::GetData(uint8_t* pData) {
  memcpy(pData, pFrame->data[0], pFrame->width * pFrame->height * sizeof(uint8_t));
  return 0;
}