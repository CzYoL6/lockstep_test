#pragma once
#include "game.h"
#include "message.pb.h"
#include "packet.h"
#include "server.h"
#include <assert.h>

namespace SERVER_SEND {
void Welcome(ServerClient *cli);

void UpdateRoomInfo(ServerClient *cli, int playerCount);

void NotifyStartGame();

void UpdateInfo(Chat::UpdateInfo_S_TO_C updateInfo);
} // namespace SERVER_SEND