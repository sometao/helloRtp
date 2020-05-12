#include <iostream>
#include "seeker/loggerApi.h"

extern "C" {
#include "libavutil/mem.h"
}

class H264NALUFromRtpPayloadParser {
 private:
  uint8_t *buff;
  uint8_t start_seq[4];
  size_t pos;
  const size_t size;
  const uint8_t start_code_len = 4;


  void pushData(const uint8_t *data, int len) {
    memcpy(buff + pos, data, len);
    pos += len;
  }

  size_t pullData(uint8_t **outData) {
    //*outData = new uint8_t[pos];
    *outData = (uint8_t*)av_malloc(pos);
    memcpy(*outData, buff, pos);
    size_t len = pos;
    pos = 0;
    return len;
  }

  int handleSTAPa(const uint8_t *data, int len, uint8_t **outData, size_t &outLen) {
    const uint8_t *src = data;
    int src_len = len;
    while (src_len > 2) {
      uint16_t nal_size = (uint16_t)src[0] << 8 | src[1];
      // consume the length of the aggregate
      src += 2;
      src_len -= 2;
      if (nal_size <= src_len) {
        pushData(start_seq, start_code_len);
        pushData(src, nal_size);
      } else {
        W_LOG("nal size exceeds length: {} {}", nal_size, src_len);
        return -1;
      }
      // eat what we handled
      src += nal_size;
      src_len -= nal_size;
    }

    outLen = pullData(outData);
    return outLen;
  }


  int handleFUa(const uint8_t *data, int len, uint8_t **outData, size_t &outLen) {
    uint8_t fu_indicator, fu_header, start_bit, end_bit, nal_type, nal_header;

    if (len < 3) {
      E_LOG("Too short data for FU-A H.264 RTP packet");
      return -1;
    }
    /*
       
      FU indicator: data[0]
      +---------------+
      |0|1|2|3|4|5|6|7|
      +-+-+-+-+-+-+-+-+
      |F|NRI|  Type   |
      +---------------+


      fu_header  : data[1]
      +---------------+
      |0|1|2|3|4|5|6|7|
      +-+-+-+-+-+-+-+-+
      |S|E|R|  Type   |
      +---------------+
    */

    fu_indicator = data[0];
    fu_header = data[1];
    start_bit = fu_header & 0x80;
    end_bit = fu_header & 0x40;
    nal_type = fu_header & 0x1f;
    nal_header = fu_indicator & 0xe0 | nal_type;

    // skip the fu_indicator and fu_header
    data += 2;
    len -= 2;

    if (start_bit) {
      int pos = 0;
      pushData(start_seq, start_code_len);
      pushData(&nal_header, 1);
      pushData(data, len);
      return 0;
    } else if (end_bit) {
      pushData(data, len);
      outLen = pullData(outData);
      return outLen;
    } else {
      pushData(data, len);
      return 0;
    }

  }


 public:
  H264NALUFromRtpPayloadParser(size_t buffSize = 1024 * 1024) : size(buffSize), pos(0) {
    buff = new uint8_t[buffSize];


    start_seq[0] = 0;
    start_seq[1] = 0;
    start_seq[2] = 0;
    start_seq[3] = 1;

    std::cout << "H264NALUFromRtpPayloadParser created." << std::endl;


    D_LOG("start_seq size: {}", sizeof(start_seq));
    D_LOG("start_seq[0]: {}", start_seq[0]);
    D_LOG("start_seq[1]: {}", start_seq[1]);
    D_LOG("start_seq[2]: {}", start_seq[2]);
    D_LOG("start_seq[3]: {}", start_seq[3]);
  }

  ~H264NALUFromRtpPayloadParser() {
    if (buff != nullptr) {
      delete buff;
    }
  }

  /*
  return:   0 a part of packet, not finish
            < 0 some error
            > 0 read packet finish.
  */
  int parse(const uint8_t *data, int len, uint8_t **outData, size_t &outLen) {
    uint8_t naluHeader;
    uint8_t naluType;
    int result = 0;

    naluHeader = data[0];
    naluType = naluHeader & 0x1f;
    if (naluType >= 1 && naluType <= 23) naluType = 1;
    switch (naluType) {
      case 0:
      case 1:
        pos = 0;
        pushData(start_seq, start_code_len);
        pushData(data, len);
        outLen = pullData(outData);
        result = outLen;
        pos = 0;
        break;
      case 24:  // STAP-A (one packet, multiple nals)
        // consume the STAP-A NAL
        data++;
        len--;
        result = handleSTAPa(data, len, outData, outLen);
        pos = 0;
        break;
      case 25:  // STAP-B
      case 26:  // MTAP-16
      case 27:  // MTAP-24
      case 29:  // FU-B
        W_LOG("Unsupported NALU. RTP H.264 NAL unit type: {}", naluType);
        result = -2;
        break;
      case 28:  // FU-A (fragmented nal)
        result = handleFUa(data, len, outData, outLen);
        break;
      case 30:  // undefined
      case 31:  // undefined
      default:
        W_LOG("NALU. Undefined type : {}", naluType);
        result = -1;
        break;
    }


    return result;
  }
};
