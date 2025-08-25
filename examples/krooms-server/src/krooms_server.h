#pragma once

#include <SDL.h>
#include <napi.h>

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

using namespace Napi;


class KRoomServer {
    bool running{true};
    std::string currentMessage = "xxx";
    std::thread senderThread;
    std::mutex mutex;
    bool sendEnabled = true;

    int senderLoop();
    public:
    KRoomServer();
    int start(const std::string & msg);
    void stop();
    void disableSend();
    void enableSend();
    void updateMessage(const std::string & msg);
};


class KRoomServerWrapper : public Napi::ObjectWrap<KRoomServerWrapper> {
   public:
    // KRoomServerWrapper();
    KRoomServerWrapper(const Napi::CallbackInfo & info);
    Napi::Value Greet(const Napi::CallbackInfo &);
    Napi::Value Start(const Napi::CallbackInfo & info);
    Napi::Value Stop(const Napi::CallbackInfo & info);
    Napi::Value UpdateMessage(const Napi::CallbackInfo & info);

    static Napi::Function GetClass(Napi::Env);

   private:
    int check(const Napi::CallbackInfo & info); 
    std::string _greeterName;
    KRoomServer server;
};