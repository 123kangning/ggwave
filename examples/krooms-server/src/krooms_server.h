#pragma once

#include <napi.h>
#include <thread>
#include <memory>
#include "krooms_server_core.h"

using namespace Napi;


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
    std::unique_ptr<std::thread> serverThread;
};