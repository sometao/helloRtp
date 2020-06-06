/**
@project seeker
@author Tao Zhang
@since 2020/5/11
@version 0.0.1-SNAPSHOT 2020/5/13
*/
#include <iostream>
#include "seeker/loggerApi.h"

extern "C" {
#include "libavutil/mem.h"
}


//TODO throw exception instead of return negative value.
class RtpToH264Parser {
 private:
  uint8_t *buff;
  uint8_t start_seq[4];
  size_t pos;
  const size_t size;
  const uint8_t start_code_len = 4;

  void pushData(const uint8_t *data, int len) {
    if (pos + len > size) {
      auto msg = fmt::format(
          "RtpToH264Parser pushData failed: pos[{}] + len[{}] > size[{}]", pos, len, size);
      E_LOG(msg);
      throw std::runtime_error(msg);
    }
    memcpy(buff + pos, data, len);
    pos += len;
  }

  size_t pullData(uint8_t **outData) {
    //*outData = new uint8_t[pos];
    *outData = (uint8_t *)av_malloc(pos);
    memcpy(*outData, buff, pos);
    size_t len = pos;
    pos = 0;
    return len;
  }

  size_t handleSTAPa(const uint8_t *data, int len, uint8_t **outData) {
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
        throw std::runtime_error(fmt::format("nal size exceeds length: {} {}", nal_size, src_len));
      }
      // eat what we handled
      src += nal_size;
      src_len -= nal_size;
    }
    return pullData(outData);
  }


  size_t handleFUa(const uint8_t *data, int len, uint8_t **outData) {
    uint8_t fu_indicator, fu_header, start_bit, end_bit, nal_type, nal_header;

    if (len < 3) {
      throw std::runtime_error(fmt::format("Too short data for FU-A H.264 RTP packet"));
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
      return pullData(outData);
    } else {
      pushData(data, len);
      return 0;
    }
  }


 public:
  RtpToH264Parser(size_t buffSize = 1024 * 1024) : size(buffSize), pos(0) {
    buff = new uint8_t[buffSize];


    start_seq[0] = 0;
    start_seq[1] = 0;
    start_seq[2] = 0;
    start_seq[3] = 1;

    std::cout << "RtpToH264Parser created." << std::endl;


    D_LOG("start_seq size: {}", sizeof(start_seq));
    D_LOG("start_seq[0]: {}", start_seq[0]);
    D_LOG("start_seq[1]: {}", start_seq[1]);
    D_LOG("start_seq[2]: {}", start_seq[2]);
    D_LOG("start_seq[3]: {}", start_seq[3]);
  }

  ~RtpToH264Parser() {
    if (buff != nullptr) {
      delete buff;
    }
  }

  /*
  return:   0 a part of packet, not finish
            > 0 read packet finish, packet length returned.
  throw runtime_error, when parse failed.
  */
  size_t parse(const uint8_t *data, int len, uint8_t **outData) {
    uint8_t naluHeader;
    uint8_t naluType;
    size_t outLen = 0;

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
        pos = 0;
        break;
      case 24:  // STAP-A (one packet, multiple nals)
        // consume the STAP-A NAL
        data++;
        len--;
        outLen = handleSTAPa(data, len, outData);
        break;
      case 25:  // STAP-B
      case 26:  // MTAP-16
      case 27:  // MTAP-24
      case 29:  // FU-B
        throw std::runtime_error(fmt::format("Unsupported NALU. RTP H.264 NAL unit type: {}", naluType));
        break;
      case 28:  // FU-A (fragmented nal)
        outLen = handleFUa(data, len, outData);
        break;
      case 30:  // undefined
      case 31:  // undefined
      default:
        throw std::runtime_error(fmt::format("NALU. Undefined type : {}", naluType));
        break;
    }

    return outLen;
  }
};
