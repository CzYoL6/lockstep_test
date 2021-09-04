#include "server_handle.h"

void SERVER_HANDLE::SetNickName(char *buf, int sze, ServerClient *cli) {
    Chat::SetNickName_C_TO_S newMsg;
    if (!newMsg.ParseFromArray(buf, sze))
        return;
    std::cout << "new member nickname set to: " << newMsg.nickname() << std::endl;
    cli->SetNickname(newMsg.nickname());

    Game::GetInstance()->AddPlayer(cli->GetPlayerID(), newMsg.nickname());
    SERVER_SEND::UpdateRoomInfo(cli, Game::GetInstance()->GetPlayerCount());

    if (Game::GetInstance()->GetPlayerCount() >= 2) {
        //TODO

        Game::GetInstance()->NotifyStartGame();
    }
}

void SERVER_HANDLE::GetControlInfo(char *buf, int sze, ServerClient *cli) {
    Chat::ControlInfo_C_TO_S newMsg;
    if (!newMsg.ParseFromArray(buf, sze))
        return;
    std::cout << "recv control info from player: " << newMsg.playerid() << std::endl;
    std::cout << "WASD:  " << newMsg.w() << newMsg.a() << newMsg.s() << newMsg.d() << std::endl;

    Game::GetInstance()->AddCmdToFrame(newMsg);
}