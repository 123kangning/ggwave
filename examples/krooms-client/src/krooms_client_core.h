#pragma once

#include <SDL.h>

#include <atomic>
#include <csignal>
#include <ctime>
#include <functional>

#include "ggwave-common-sdl2.h"
#include "ggwave-common.h"
#include "ggwave/ggwave.h"

class KRoomClient
{
public:
    KRoomClient();
    int start(std::function<void(const std::string &)> callback);
    void stop();

private:
    bool running;
    std::function<void(const std::string &)> dataCallback;
};