#pragma once
#include "Player.h"
#include "message.pb.h"
#include "time_util.h"
#include <map>
class Player;
class Game {
  private:
    unsigned int gameId{0};
    IUINT32 last_ticks{0};
    Chat::UpdateInfo_S_TO_C lockedFrame;
    int currentFrame{0};
    std::map<int, Player *> playerMap; //[conv(playerID), Player]
    static Game *instance;
    bool hasStarted{false};

  public:
    Game(/* args */);
    ~Game();
    void EnterFrame();
    void AddPlayer(int playerID, std::string nickname);
    void NotifyStartGame();
    void AddCmdToFrame(Chat::ControlInfo_C_TO_S msg);
    static Game *GetInstance();
    int GetPlayerCount();
    std::map<int, Player *> GetPlayerMap();
    void StartGame();
    bool HasStarted();
};