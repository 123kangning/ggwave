#pragma once

#include <napi.h>
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

class KroomsClientWrapper : public Napi::ObjectWrap<KroomsClientWrapper> {
   public:
    // KRoomServerWrapper();
    KroomsClientWrapper(const Napi::CallbackInfo & info);
    Napi::Value Greet(const Napi::CallbackInfo &);
    Napi::Value Start(const Napi::CallbackInfo & info);
    Napi::Value Stop(const Napi::CallbackInfo & info);
    Napi::Value UpdateMessage(const Napi::CallbackInfo & info);

    static Napi::Function GetClass(Napi::Env);

   private:
    int check(const Napi::CallbackInfo & info); 
    std::string _greeterName;
    KRoomClient client;
};