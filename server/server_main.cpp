#include "kcpserver.h"
#include "server.h"
#include "server_send.h"

void on_kcp_revc(int conv, const char *data, int len) {
    assert(len >= 4);
    char buffer[1024];
    memcpy(buffer, data, len);
    buffer[len] = '\0';
    printf("[RECV] conv=%d data=%s len(%d)\n", conv, &buffer[4], len - 4);
}

void on_session_kick(int conv) {
    printf("conv:%d kicked\n", conv);
}

void on_error_report(const char *data) {
    printf("kcp error:%s\n", data);
}

void on_new_tcp_connection(ServerClient *cli) {
    SERVER_SEND::Welcome(cli);
}

int main(int argc, char **argv) {
    Server *server = Server::GetInstance();
    server->new_tcp_connection_cb = on_new_tcp_connection;
    server->Start(argv[1], atoi(argv[2]));

    // KCPOptions options;
    // options.recv_cb = on_kcp_revc;
    // options.kick_cb = on_session_kick;
    // options.error_reporter = on_error_report;
    // options.ip = argv[3];
    // options.port = atoi(argv[4]);
    // KCPServer *kcp_server = KCPServer::GetInstance();
    // kcp_server->SetOption(options);
    // if (!kcp_server->Start()) {
    //     printf("server start error");
    //     exit(0);
    // }

    while (server->IsRunning()) {
        server->Update();
        server->HandleData();
        if (Game::GetInstance()->HasStarted()) {
            Game::GetInstance()->EnterFrame();
        }
        //kcp_server->Update();
    }

    return 0;
}
