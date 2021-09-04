#pragma
#include "game.h"
#include "server_send.h"

Game *Game::instance = nullptr;

Game::Game() {}

Game::~Game() {
    if (instance != nullptr)
        delete instance;
    for (auto iter = playerMap.begin(); iter != playerMap.end(); iter++)
        if (iter->second != nullptr)
            delete iter->second;
}

void Game::EnterFrame() {

    IUINT32 current = iclock();

    const IUINT32 INTERVAL = 66;
    if (current - last_ticks >= INTERVAL) {

        lockedFrame.set_frameid(++currentFrame);
        SERVER_SEND::UpdateInfo(lockedFrame);

        lockedFrame.Clear();

        //std::cout << "ticking " << currentFrame << std::endl;
        //std::cout << "sending commands to all players..." << std::endl;
        // if (currentFrame >= 120)
        //     exit(0);
        last_ticks = current;
    }
}

void Game::AddCmdToFrame(Chat::ControlInfo_C_TO_S msg) {
    auto info = lockedFrame.add_controlinfos();
    (*info) = msg;
}

void Game::AddPlayer(int playerID, std::string nickname) {
    Player *player = new Player(playerID, nickname, this);
    playerMap[playerID] = player;
}

void Game::NotifyStartGame() {
    StartGame();
    SERVER_SEND::NotifyStartGame();
}

Game *Game::GetInstance() {
    if (instance == nullptr)
        instance = new Game;
    return instance;
}

int Game::GetPlayerCount() {
    return playerMap.size();
}

std::map<int, Player *> Game::GetPlayerMap() {
    return playerMap;
}

void Game::StartGame() {
    hasStarted = true;
}

bool Game::HasStarted() {
    return hasStarted;
}