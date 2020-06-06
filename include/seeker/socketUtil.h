/**
@project seeker
@author Tao Zhang
@since 2020/3/1
@version 0.0.1-SNAPSHOT 2020/5/13
*/
#pragma once

#ifdef _WIN32
#include <WinSock2.h>
#include <WS2tcpip.h>
#else
#include <unistd.h>
#include <cstring>
#include <cstdint>
#include <arpa/inet.h>
#include <sys/socket.h>
#define SOCKET int
#define INVALID_SOCKET (SOCKET)(~0)
#define SOCKET_ERROR (-1)
#endif

#define OK 0
#define ERR -1
#define SOCKET_VERSION MAKEWORD(2, 2)


namespace seeker {

class SocketUtil {
 public:
  static void cleanWSA() {
#ifdef _WIN32
    WSACleanup();
#else
    // do nothing.
#endif
  }


  static void closeSocket(SOCKET s) {
#ifdef _WIN32
    closesocket(s);
#else
    close(s);
#endif
  }


  static void setSocketAddr(sockaddr_in* addr, const char* ip) {
    if (ip == nullptr) {
#ifdef _WIN32
      (*addr).sin_addr.S_un.S_addr = htonl(INADDR_ANY);
#else
      (*addr).sin_addr.s_addr = htonl(INADDR_ANY);
#endif
    } else {
#ifdef _WIN32
      (*addr).sin_addr.S_un.S_addr = inet_addr(ip);
#else
      (*addr).sin_addr.s_addr = inet_addr(ip);
#endif
    }
  }


  static int startupWSA() {
#ifdef _WIN32
    WSADATA wsaData = {};
    if (WSAStartup(SOCKET_VERSION, &wsaData) != OK) {
      return ERR;
    } else {
      return OK;
    }
#else
    return OK;
#endif
  }
};



}  // namespace seeker
