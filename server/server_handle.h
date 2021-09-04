#pragma once
#include "game.h"
#include "message.pb.h"
#include "packet.h"
#include "server.h"
#include "server_client.h"
#include "server_send.h"
#include <assert.h>

namespace SERVER_HANDLE {
void SetNickName(char *buf, int sze, ServerClient *cli);
void GetControlInfo(char *buf, int sze, ServerClient *cli);
}; // namespace SERVER_HANDLE
