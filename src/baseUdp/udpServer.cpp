#include <iostream>
#include <string>
#include "seeker/socketUtil.h"
#include "seeker/loggerApi.h"
#include <cstdio>

using std::cout;
using std::endl;
using std::sprintf;
using std::string;

using seeker::SocketUtil;



static SOCKET initSocket(const string& host, int port) {
  if (SocketUtil::startupWSA() == ERR) {
    cout << "WSAStartup error." << endl;
    throw std::runtime_error("WSAStartup error.");
  }

  SOCKET serverSocket = socket(AF_INET, SOCK_DGRAM, 0);
  I_LOG("serverSocket get success.");

  sockaddr_in serverAddr = {0};
  SocketUtil::setSocketAddr(&serverAddr, host.c_str());
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(port);

  if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(sockaddr_in)) == SOCKET_ERROR) {
    SocketUtil::closeSocket(serverSocket);
    SocketUtil::cleanWSA();
    cout << "bind error." << endl;
    throw std::runtime_error("bind error.");
  }

  return serverSocket;
}


void startServer(int port) {
  I_LOG("serverSocket bind success.");

  char recvBuf[100];
  char tempBuf[100];

  SOCKET sock = initSocket("0.0.0.0", port);

  SOCKADDR_IN peerAddr;
  size_t addrLen = sizeof(SOCKADDR_IN);
  int c = 0;
  while (true) {
    int recvNum =
      recvfrom(sock, recvBuf, 100, 0, (sockaddr*)&peerAddr, (socklen_t*)&addrLen);
    if (recvNum < 0) {
      auto msg = fmt::format("error: recv_num={}", recvNum);
      E_LOG(msg);
      throw std::runtime_error(msg);
    }
    c += 1;

    recvBuf[recvNum] = '\0';
    I_LOG("[\t{}\t]: recv [{} byte] from [{}]: [{}]", c, recvNum, inet_ntoa(peerAddr.sin_addr), recvBuf);
    sendto(sock, recvBuf, recvNum, 0, (sockaddr*)&peerAddr, addrLen);
  }





  I_LOG("Server finish.");
  SocketUtil::closeSocket(sock);
  SocketUtil::cleanWSA();
}