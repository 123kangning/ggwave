#pragma once

#include <SDL.h>

#include <atomic>
#include <chrono>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>

#include "ggwave-common-sdl2.h"
#include "ggwave-common.h"
#include "ggwave/ggwave.h"

class KRoomSend {
    bool running{true};
    std::string currentMessage = "xxx";
    std::thread senderThread;
    std::mutex mutex;
    bool sendEnabled = true;

    int senderLoop();
    public:
    KRoomSend();
    int start(const std::string & msg);
    void stop();
    void disableSend();
    void enableSend();
    void updateMessage(const std::string & msg);
};