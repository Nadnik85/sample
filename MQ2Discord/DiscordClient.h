#pragma once

#include "SafeQueue.h"
#include <string>
#include <thread>
#include <atomic>

// Queue for messages to be sent to discord channel
extern SafeQueue<std::string> toDiscord;

// Queue for messages to be displayed in chat
extern SafeQueue<std::string> fromDiscord;

extern std::thread * g_pThread;
extern std::atomic_bool bRun;
extern std::atomic_bool bFinished;

void DiscordThread(std::string token, std::string channelId, std::string controlUserId, std::string connectMsg);
