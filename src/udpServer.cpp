#include <iostream>
#include <string>
#include "seeker/socketUtil.h"
#include "seeker/loggerApi.h"
#include <cstdio>

using std::cout;
using std::endl;
using std::sprintf;
using std::string;


void startServer(int port) {
  using seeker::SocketUtil;


  if (SocketUtil::startupWSA() == ERR) {
    cout << "WSAStartup error." << endl;
    throw std::runtime_error("WSAStartup error.");
  }

  SOCKET serverSocket = socket(AF_INET, SOCK_DGRAM, 0);
  I_LOG("serverSocket get success.");

  sockaddr_in serverAddr = {0};
  SocketUtil::setSocketAddr(&serverAddr, nullptr);
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(port);

  if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(sockaddr_in)) == SOCKET_ERROR) {
    SocketUtil::closeSocket(serverSocket);
    SocketUtil::cleanWSA();
    cout << "bind error." << endl;
    throw std::runtime_error("bind error.");
  }

  I_LOG("serverSocket bind success.");

  char recvBuf[100];
  char tempBuf[100];



  SOCKADDR_IN addrClient;
  int len = sizeof(SOCKADDR_IN);  

  int recv_num = recvfrom(serverSocket, recvBuf, 100, 0, (sockaddr*)&addrClient, (socklen_t *)&len);
  
  if(recv_num < 0 ) {
    auto msg  = fmt::format("error: recv_num={}", recv_num);
    E_LOG(msg);
    throw std::runtime_error("msg");
  }
  
  I_LOG("recv_num={}", recv_num);

  recvBuf[recv_num] = '\0';  

  I_LOG("serverSocket recvfrom success: {}", recvBuf);

  I_LOG("client ip: {}", inet_ntoa(addrClient.sin_addr));


  I_LOG("Server finish.");
  SocketUtil::closeSocket(serverSocket);
  SocketUtil::cleanWSA();
}