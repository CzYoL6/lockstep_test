#include "server_send.h"

void SERVER_SEND::Welcome(ServerClient *cli) {
    std::cout << "sending welcome msg..." << std::endl;
    Packet *packet = new Packet(Chat::TYPE::welcome_S_TO_C);
    std::cout << "packet length1: " << packet->GetAllLength() << std::endl;

    Chat::Welcome_S_TO_C msg;
    msg.set_playerid(cli->GetPlayerID());
    msg.set_kcp_conv(cli->GetPlayerID());
    msg.set_udp_address(KCPServer::GetInstance()->GetOption().ip);
    msg.set_udp_port(KCPServer::GetInstance()->GetOption().port);

    char tmp[1024];
    memset(tmp, 0, sizeof(tmp));
    msg.SerializeToArray(tmp, msg.ByteSizeLong());

    packet->AddVal(tmp, msg.ByteSizeLong());
    std::cout << "packet length2: " << packet->GetAllLength() << std::endl;

    packet->InsertLengthInFront();

    std::cout << "packet length3: " << packet->GetAllLength() << std::endl;

    cli->WriteIntoSendBuffer(packet->GetCircleBuffer()->GetBuffer(),
                             packet->GetAllLength());
    delete packet;
}
void SERVER_SEND::UpdateRoomInfo(ServerClient *cli, int playerCount) {
    std::cout << "sending roominfo update msg..." << std::endl;
    Packet *packet = new Packet(Chat::TYPE::updateRoomInfo_S_TO_C);
    std::cout << "packet length1: " << packet->GetAllLength() << std::endl;

    Chat::UpdateRoomInfo_S_TO_C protoMsg;
    protoMsg.set_playercount(playerCount);
    //std::cout << "protoMsg.playerCount: " << protoMsg.playercount() << std::endl;
    char tmp[1024];
    memset(tmp, 0, sizeof(tmp));
    protoMsg.SerializeToArray(tmp, protoMsg.ByteSizeLong());
    packet->AddVal(tmp, protoMsg.ByteSizeLong());
    std::cout << "packet length2: " << protoMsg.ByteSizeLong() << " " << packet->GetAllLength() << std::endl;

    packet->InsertLengthInFront();

    std::cout << "packet length3: " << packet->GetAllLength() << std::endl;

    cli->WriteIntoSendBuffer(packet->GetCircleBuffer()->GetBuffer(),
                             packet->GetAllLength());

    delete packet;
}

void SERVER_SEND::NotifyStartGame() {
    std::cout << "sending notify start game msg..." << std::endl;
    Packet *packet = new Packet(Chat::TYPE::notifyStartGame_S_TO_C);
    std::cout << "packet length1: " << packet->GetAllLength() << std::endl;

    Chat::NotifyStartGame msg;
    std::map<int, Player *> playerMap = Game::GetInstance()->GetPlayerMap();
    for (auto iter = playerMap.begin(); iter != playerMap.end(); iter++) {
        Player *player = iter->second;
        Chat::PlayerInfo *playerInfo = msg.add_playerinfos();
        playerInfo->set_playerid(player->GetId());
        playerInfo->set_nickname(player->GetNickname());
    }

    char tmp[1024];
    memset(tmp, 0, sizeof(tmp));
    msg.SerializeToArray(tmp, msg.ByteSizeLong());
    packet->AddVal(tmp, msg.ByteSizeLong());
    std::cout << "packet length2: " << msg.ByteSizeLong() << " " << packet->GetAllLength() << std::endl;

    packet->InsertLengthInFront();
    std::cout << "packet length3: " << packet->GetAllLength() << std::endl;

    auto clients = Server::GetInstance()->GetClients();
    for (auto iter = clients->begin(); iter != clients->end(); iter++) {
        auto cli = iter->second;
        cli->WriteIntoSendBuffer(packet->GetCircleBuffer()->GetBuffer(),
                                 packet->GetAllLength());
    }

    delete packet;
}

//will be changed to kcp
void SERVER_SEND::UpdateInfo(Chat::UpdateInfo_S_TO_C updateInfo) {
    std::cout << "sending update msg..." << std::endl;
    Packet *packet = new Packet(Chat::TYPE::updateInfo_S_TO_C);
    std::cout << "packet length1: " << packet->GetAllLength() << std::endl;

    char tmp[1024];
    memset(tmp, 0, sizeof(tmp));
    updateInfo.SerializeToArray(tmp, updateInfo.ByteSizeLong());
    packet->AddVal(tmp, updateInfo.ByteSizeLong());
    std::cout << "packet length2: " << updateInfo.ByteSizeLong() << " " << packet->GetAllLength() << std::endl;

    packet->InsertLengthInFront();
    std::cout << "packet length3: " << packet->GetAllLength() << std::endl;

    auto clients = Server::GetInstance()->GetClients();
    for (auto iter = clients->begin(); iter != clients->end(); iter++) {
        auto cli = iter->second;
        cli->WriteIntoSendBuffer(packet->GetCircleBuffer()->GetBuffer(),
                                 packet->GetAllLength());
    }

    delete packet;
}