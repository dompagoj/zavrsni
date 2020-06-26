#pragma once

#include <netinet/in.h>
#include <zconf.h>

class UdpClient
{
public:
  sockaddr_in ServerAddr{};
  sockaddr* ServerAddrPtr;
  socklen_t ServerAddrSize;
  int ServerFD;

  explicit UdpClient(in_port_t Port)
  {
    ServerAddr.sin_port = htons(Port);
    ServerAddr.sin_family = AF_INET;
    ServerAddr.sin_addr.s_addr = INADDR_ANY;

    ServerFD = socket(AF_INET, SOCK_DGRAM, 0);
    ServerAddrPtr = (sockaddr*) &ServerAddr;
    ServerAddrSize = sizeof(ServerAddr);
  }

  ~UdpClient() { close(ServerFD); }

  void Send(const void* Payload, size_t PayloadSize) const
  {
    sendto(ServerFD, Payload, PayloadSize, MSG_DONTWAIT, ServerAddrPtr, ServerAddrSize);
  }
};

class TcpClient
{
public:
  sockaddr_in ServerAddr{};
  sockaddr* ServerAddrPtr;
  socklen_t ServerAddrSize;
  int ServerFD;

  explicit TcpClient(in_port_t Port)
  {
    ServerAddr.sin_port = htons(Port);
    ServerAddr.sin_family = AF_INET;
    ServerAddr.sin_addr.s_addr = INADDR_ANY;

    ServerFD = socket(AF_INET, SOCK_STREAM, 0);
    ServerAddrPtr = (sockaddr*) &ServerAddr;
    ServerAddrSize = sizeof(ServerAddr);
  }

  ~TcpClient() { close(ServerFD); }

  void Send(const void* Payload, size_t PayloadSize) const
  {
    sendto(ServerFD, Payload, PayloadSize, MSG_DONTWAIT, ServerAddrPtr, ServerAddrSize);
  }
};

