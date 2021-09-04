#pragma once
#include "game.h"
#include "message.pb.h"

class Game;
class Player {
  private:
    unsigned int id{0};
    std::string nickname;
    Game *game{nullptr};

  public:
    unsigned int GetId() { return id; }
    Game *GetGame() { return game; }
    Player(unsigned int _id, std::string _nickname, Game *_game);
    ~Player();

    void HandleFspMessage(Chat::ControlInfo_C_TO_S);
    void SendToClient(Chat::TYPE, char *buf);

    std::string GetNickname();
};
