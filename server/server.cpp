
#include "server.h"

#include "message.pb.h"
#include "packet.h"
#include "server_handle.h"
#include "server_send.h"

#include "netinet/in.h"

#define EPOLL_SIZE 50
#define BUF_SIZE 1024 * 10

Server *Server::instance = nullptr;

Server *Server::GetInstance() {
    if (instance == nullptr) {
        instance = new Server;
    }
    return instance;
}

int Server::SetNonBlocking(int fd) {
    int flags = fcntl(fd, F_GETFL);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    return flags;
}

void Server::SetReusable(int fd) {
    int option = true;
    socklen_t optlen = sizeof(option);
    setsockopt(
        fd, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<void *>(&option), optlen);
}

int Server::Bind(int sock, std::string ip, int port) {
    socklen_t addr_len;
    addr_len = sizeof(server_addr);
    int ret = bind(sock, reinterpret_cast<sockaddr *>(&server_addr), addr_len);
    return ret;
}

void Server::Start(std::string ip, int port) {
    clients_tcp.reset(new std::map<int, ServerClient *>);

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    //server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    inet_pton(AF_INET, ip.c_str(), &server_addr.sin_addr.s_addr);
    server_addr.sin_port = htons(port);

    char str[INET_ADDRSTRLEN];
    memset(str, 0, sizeof(str));
    inet_ntop(AF_INET, &server_addr.sin_addr, str, sizeof(str));
    std::cout << "server tcp addr: " << str << std::endl;

    server_tcp_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    assert(server_tcp_sock >= 0);
    SetNonBlocking(server_tcp_sock);
    SetReusable(server_tcp_sock);

    int tcp_bind_ret = Bind(server_tcp_sock, ip, port);
    assert(tcp_bind_ret >= 0);
    std::cout << "tcp bind() succeeded..." << std::endl;

    int listen_ret = listen(server_tcp_sock, 5);
    assert(listen_ret >= 0);
    std::cout << "tcp listen() succeeded..." << std::endl;

    epfd = epoll_create(EPOLL_SIZE);
    {
        epoll_event ev;
        ev.data.fd = server_tcp_sock;
        ev.events = EPOLLIN;
        epoll_ctl(epfd, EPOLL_CTL_ADD, server_tcp_sock, &ev);
    }

    epoll_event events[EPOLL_SIZE];

    std::cout << "epoll created successfully..." << std::endl;

    isRunning = true;
}

bool Server::Update() {
    for (auto iter = clients_tcp->begin(); iter != clients_tcp->end(); iter++) {

        if (iter->second->HasDataToSend()) {
            std::cout << iter->first << " has data to send..." << std::endl;
            epoll_event ev;
            ev.data.fd = iter->first;
            ev.events = EPOLLIN | EPOLLOUT;
            epoll_ctl(epfd, EPOLL_CTL_MOD, iter->first, &ev);
            std::cout << "modify its fd (EPOLLOUT) " << std::endl;
        }
    }

    int cnt = epoll_wait(epfd, events, EPOLL_SIZE, 0);
    if (cnt < 0) {
        printf("epoll failure!\n");
        return false;
    }

    // std::cout << "epoll_wait cnt: " << cnt << std::endl;

    for (int i = 0; i < cnt; i++) {
        int fd = events[i].data.fd;
        int eve = events[i].events;
        if (fd == server_tcp_sock) {
            //有新连接
            std::cout << "new connection..." << std::endl;
            sockaddr_in new_cli_addr;
            memset(&new_cli_addr, 0, sizeof(new_cli_addr));
            socklen_t addr_len;
            addr_len = sizeof(new_cli_addr);
            int new_cli_sock = accept(server_tcp_sock, reinterpret_cast<sockaddr *>(&new_cli_addr), &addr_len);

            ServerClient *new_cli = new ServerClient(new_cli_sock, reinterpret_cast<sockaddr *>(&new_cli_addr));
            clients_tcp->insert(std::make_pair(new_cli_sock, new_cli));
            epoll_event ev;
            ev.data.fd = new_cli_sock;
            ev.events = EPOLLIN;
            epoll_ctl(epfd, EPOLL_CTL_ADD, new_cli_sock, &ev);
            std::cout << inet_ntoa(new_cli_addr.sin_addr) << " connected..." << std::endl;
            // std::cout << "fd: " << new_cli_sock << std::endl;
            //SERVER_SEND::Welcome(new_cli);
            new_tcp_connection_cb(new_cli);

        } else {
            if (eve & EPOLLIN) {
                char buf[BUF_SIZE];

                //有消息来
                memset(buf, 0, sizeof(buf));
                int cnt = recv(fd, buf, BUF_SIZE, 0);
                auto iter = clients_tcp->find(fd);
                if (cnt < 0) {
                    std::cout << "something went wrong.." << std::endl;
                    close(fd);
                    delete iter->second;
                    clients_tcp->erase(iter);
                    std::cout << "and has been removed from the server.."
                              << std::endl;
                    break;
                } else if (cnt == 0) {
                    //对端关闭
                    std::cout << "the other side is closed.." << std::endl;
                    close(fd);
                    delete iter->second;
                    clients_tcp->erase(iter);
                    std::cout << "and has been removed from the server.."
                              << std::endl;
                    break;
                } else {
                    //收到消息
                    std::cout << "recv() succeeded.."
                              << "cnt: " << cnt << std::endl;

                    auto cli = iter->second;

                    cli->WriteIntoRecvBuffer(buf, cnt);
                }
            }
        }
        if (eve & EPOLLOUT) {
            char tmp[1024];
            memset(tmp, 0, sizeof(tmp));
            int size = (*clients_tcp)[fd]->GetSendBuffer()->GetLength();
            (*clients_tcp)[fd]->ReadFromSendBuffer(tmp, size, false);
            send(fd, tmp, size, 0);

            epoll_event ev;
            ev.data.fd = fd;
            ev.events = EPOLLIN;
            epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &ev);
        }
    }
    return true;
}

void Server::HandleData() {

    for (auto iter = clients_tcp->begin(); iter != clients_tcp->end(); iter++) {
        auto cli = iter->second;

        //包结构： sze(4) | TYPE(4) | msg(*)
        if (cli->GetRecvBuffer()->GetLength() >= sizeof(unsigned int)) {
            char tmp[1024];
            memset(tmp, 0, sizeof(tmp));
            int read_res = cli->GetRecvBuffer()->Read(tmp, sizeof(unsigned int), true);
            unsigned int p_tmp_res = *(reinterpret_cast<unsigned int *>(tmp));
            std::cout << "read_res: " << read_res << " "
                      << " tmp_res: " << p_tmp_res << std::endl;

            if (cli->GetRecvBuffer()->GetLength() >= sizeof(unsigned int) + p_tmp_res) {
                memset(tmp, 0, sizeof(tmp));
                cli->GetRecvBuffer()->Read(tmp, sizeof(unsigned int));

                memset(tmp, 0, sizeof(tmp));
                cli->GetRecvBuffer()->Read(tmp, sizeof(Chat::TYPE));
                Chat::TYPE type = *(reinterpret_cast<Chat::TYPE *>(tmp));

                switch (type) {
                case Chat::TYPE::setNickName_C_TO_S: {
                    memset(tmp, 0, sizeof(tmp));
                    cli->GetRecvBuffer()->Read(tmp, p_tmp_res - sizeof(Chat::TYPE));
                    SERVER_HANDLE::SetNickName(tmp, p_tmp_res - sizeof(Chat::TYPE), cli);

                    break;
                }
                case Chat::TYPE::controlInfo_C_TO_S: {
                    memset(tmp, 0, sizeof(tmp));
                    cli->GetRecvBuffer()->Read(tmp, p_tmp_res - sizeof(Chat::TYPE));
                    SERVER_HANDLE::GetControlInfo(tmp, p_tmp_res - sizeof(Chat::TYPE), cli);

                    break;
                }
                }

            } else {
                std::cout << "parse type failed." << std::endl;
            }
        }
    }
}

Server::Server() {}

Server::~Server() {
    if (instance != nullptr)
        delete instance;
}

ServerClient *Server::GetClient(int fd) {
    return (*clients_tcp)[fd];
}

std::shared_ptr<std::map<int, ServerClient *>> Server::GetClients() {
    return clients_tcp;
}