#pragma once
#include "circle_buffer.hpp"
#include "ikcp.h"
#include "sys/socket.h"
#include <sys/socket.h>

class ServerClient {
  public:
    ServerClient(int fd, sockaddr *addr);

    ~ServerClient();

    int GetSockFd() const { return m_nSockFd; }

    bool HasDataToSend() const;

    bool HasRecvData() const;

    // buffer operations
    int ReadFromSendBuffer(char *buf, int size, bool peek) const;

    int WriteIntoSendBuffer(char *buf, int size) const;

    int ReadFromRecvBuffer(char *buf, int size, bool peek) const;

    int WriteIntoRecvBuffer(char *buf, int size) const;

    CircleBuffer<char> *GetSendBuffer() { return m_sendBuffer; }

    CircleBuffer<char> *GetRecvBuffer() { return m_recvBuffer; }

    void SetNickname(std::string name);

    std::string GetNickname();

    int GetPlayerID() { return playerID; }

  private:
    int m_nSockFd{-1};
    sockaddr m_sAddr;
    CircleBuffer<char> *m_sendBuffer{nullptr};
    CircleBuffer<char> *m_recvBuffer{nullptr};
    std::string nickname;
    int playerID{-1};
    static int playerIdCounter;
};
