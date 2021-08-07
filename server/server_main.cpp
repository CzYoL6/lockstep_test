#include "server.h"

int main(int argc, char** argv) {
    Server server;
    server.Start(argv[1], 2333);
    while (server.IsRunning()) {
        server.Update();
        server.HandleData();
    }
    return 0;
}