#pragma once

#include <napi.h>
#include <thread>
#include "krooms_receive_core.h"

class KroomsReceiveWrapper : public Napi::ObjectWrap<KroomsReceiveWrapper> {
   public:
    KroomsReceiveWrapper(const Napi::CallbackInfo & info);
    Napi::Value Greet(const Napi::CallbackInfo &);
    Napi::Value Start(const Napi::CallbackInfo & info);
    Napi::Value Stop(const Napi::CallbackInfo & info);
    Napi::Value UpdateMessage(const Napi::CallbackInfo & info);

    static Napi::Function GetClass(Napi::Env);

   private:
    int check(const Napi::CallbackInfo & info); 
    std::string _greeterName;
    KRoomReceive client;
    Napi::ThreadSafeFunction tsfn;
    std::thread clientThread;
};