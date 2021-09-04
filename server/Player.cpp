#include "Player.h"

Player::Player(unsigned int _id, std::string _nickname, Game *_game)
    : id(_id), nickname(_nickname), game(_game) {}

Player::~Player() {
    //TODO
}

std::string Player::GetNickname() {
    return nickname;
}

void Player::SendToClient(Chat::TYPE, char *buf) {
    //TODO
}

void HandleFspMessage(Chat::ControlInfo_C_TO_S) {
    //TODO
}
