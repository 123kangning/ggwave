#pragma once

#include <napi.h>
#include <thread>
#include "krooms_client_core.h"

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
    Napi::ThreadSafeFunction tsfn;
    std::thread clientThread;
};