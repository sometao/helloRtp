#include <string>
#include <iostream>
#include "seeker/socketUtil.h"
#include "seeker/loggerApi.h"



using std::cout;
using std::endl;
using std::string;


namespace {


int resolvehelper(const char* hostname, int family, const char* service,
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

}  // namespace


void startClient(const string& targetIp, int port) {
  using seeker::SocketUtil;


  //Get socket ready
  if (SocketUtil::startupWSA() == ERR) {
    E_LOG("WSAStartup error.");
    throw std::runtime_error("WSAStartup error.");
  }
  SOCKET clientSocket = socket(AF_INET, SOCK_DGRAM, 0);


  //init target address
  sockaddr_in targetAddr = {0};
  targetAddr.sin_family = AF_INET;
  SocketUtil::setSocketAddr(&targetAddr, targetIp.c_str());
  targetAddr.sin_port = htons(port);


  //get data ready.
  const char* data = "hello world.";

  constexpr auto len = sizeof(SOCKADDR);
  sendto(clientSocket, data, strlen(data), 0, (sockaddr*)&targetAddr, len);


  I_LOG("client send data finish.");
  SocketUtil::closeSocket(clientSocket);
  SocketUtil::cleanWSA();


}
