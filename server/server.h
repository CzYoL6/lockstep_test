#pragma once

#include "kcpserver.h"
#include "server_client.h"
#include "time_util.h"
#include <arpa/inet.h>
#include <assert.h>
#include <chrono>
#include <cstring>
#include <ctime>
#include <iostream>
#include <map>
#include <memory>
#include <sys/epoll.h>
#include <sys/errno.h>
#include <sys/fcntl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

typedef void (*OnNewTcpConnection)(ServerClient *cli);

class Server {
  public:
    Server();
    ~Server();
    void Start(std::string ip, int port);
    bool Update();
    void HandleData();
    bool IsRunning() const { return isRunning; }
    ServerClient *GetClient(int fd);
    sockaddr *GetServerAddr() {
        return reinterpret_cast<sockaddr *>(&server_addr);
    }
    OnNewTcpConnection new_tcp_connection_cb;

    static Server *GetInstance();

    std::shared_ptr<std::map<int, ServerClient *>> GetClients();

  private:
    int epfd{-1};
    epoll_event events[1024];
    bool isRunning{false};
    std::shared_ptr<std::map<int, ServerClient *>> clients_tcp;
    int server_tcp_sock{-1};

    sockaddr_in server_addr;
    int SetNonBlocking(int fd);
    void SetReusable(int fd);
    int Bind(int sock, std::string ip, int port);

    static Server *instance;
};
