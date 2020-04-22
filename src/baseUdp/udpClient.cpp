#include <string>
#include <iostream>
#include "seeker/socketUtil.h"
#include "seeker/loggerApi.h"



using std::cout;
using std::endl;
using std::string;



static int resolvehelper(const char* hostname, int family, const char* service,
                         sockaddr_storage* pAddr) {
  int result;
  addrinfo* result_list = NULL;
  addrinfo hints = {};
  hints.ai_family = family;
  hints.ai_socktype = SOCK_DGRAM;  // without this flag, getaddrinfo will return 3x the number
                                   // of addresses (one for each socket type).
  result = getaddrinfo(hostname, service, &hints, &result_list);
  if (result == 0) {
    // ASSERT(result_list->ai_addrlen <= sizeof(sockaddr_in));
    memcpy(pAddr, result_list->ai_addr, result_list->ai_addrlen);
    freeaddrinfo(result_list);
  }

  return result;
}



static SOCKET initSocket() {}


void startClient(const string& targetIp, int port) {
  using seeker::SocketUtil;


  // Get socket ready
  if (SocketUtil::startupWSA() == ERR) {
    E_LOG("WSAStartup error.");
    throw std::runtime_error("WSAStartup error.");
  }
  SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);


  // init target address
  sockaddr_in targetAddr = {0};
  targetAddr.sin_family = AF_INET;
  SocketUtil::setSocketAddr(&targetAddr, targetIp.c_str());
  targetAddr.sin_port = htons(port);


  // get data ready.

  auto len = sizeof(SOCKADDR);

  int c = 0;
  int n = 10;
  while (n > 0) {
    string data = "hello world." + std::to_string(c);

    sendto(sock, data.c_str(), data.size(), 0, (sockaddr*)&targetAddr, len);
    I_LOG("send data:[{}]", data);

    // receive response.
    char recvBuf[100];
    int recvNum = recvfrom(sock, recvBuf, 100, 0, (sockaddr*)&targetAddr, (socklen_t*)&len);
    if (recvNum < 0) {
      auto msg = fmt::format("error: recv_num={}", recvNum);
      E_LOG(msg);
      throw std::runtime_error(msg);
    }
    recvBuf[recvNum] = '\0';
    I_LOG("[\t{}\t]: recv [{} byte] from [{}]: [{}]", c, recvNum,
          inet_ntoa(targetAddr.sin_addr), recvBuf);

    c += 1;
    n -= 1;
  }



  I_LOG("client finish.");
  SocketUtil::closeSocket(sock);
  SocketUtil::cleanWSA();
}
