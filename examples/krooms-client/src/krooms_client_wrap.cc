#include "krooms_client.h"

using namespace Napi;

KroomsClientWrapper::KroomsClientWrapper(const Napi::CallbackInfo & info) : ObjectWrap(info) {
}

Napi::Value KroomsClientWrapper::Greet(const Napi::CallbackInfo & info) {
    Napi::Env env = info.Env();

    if (info.Length() < 1) {
        Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[0].IsString()) {
        Napi::TypeError::New(env, "You need to introduce yourself to greet").ThrowAsJavaScriptException();
        return env.Null();
    }

    Napi::String name = info[0].As<Napi::String>();

    printf("Hello %s\n", name.Utf8Value().c_str());
    printf("I am %s\n", this->_greeterName.c_str());

    return Napi::String::New(env, this->_greeterName);
}
Napi::Value KroomsClientWrapper::Start(const Napi::CallbackInfo & info) {
    Napi::Env env = info.Env();

    if (info.Length() < 1) {
        Napi::TypeError::New(env, "Expected a callback function").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[0].IsFunction()) {
        Napi::TypeError::New(env, "First argument must be a function").ThrowAsJavaScriptException();
        return env.Null();
    }

    Napi::Function jsCallback = info[0].As<Napi::Function>();

    // 保存回调函数和线程安全的引用
    auto callback = [env = Napi::Env(info.Env()),
                     callbackRef = Napi::ThreadSafeFunction::New(env, jsCallback, "KRoomClientCallback", 0, 1)](const std::string & data) {
        callbackRef.BlockingCall([data](Napi::Env env, Napi::Function jsCallback) { jsCallback.Call({Napi::String::New(env, data)}); });
    };

    int ret = client.start(callback);
    return Napi::Number::New(env, ret);
}
Napi::Value KroomsClientWrapper::Stop(const Napi::CallbackInfo & info) {
    Napi::Env env = info.Env();
    client.stop();
    return Napi::Number::New(env, ret);
}

Napi::Function KroomsClientWrapper::GetClass(Napi::Env env) {
    return DefineClass(env, "KroomsClientWrapper",
                       {
                           KroomsClientWrapper::InstanceMethod("greet", &KroomsClientWrapper::Greet),
                           KroomsClientWrapper::InstanceMethod("start", &KroomsClientWrapper::Start),
                           KroomsClientWrapper::InstanceMethod("stop", &KroomsClientWrapper::Stop),
                       });
}

int KroomsClientWrapper::check(const Napi::CallbackInfo & info) {
    Napi::Env env = info.Env();

    if (info.Length() < 1) {
        Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
        return -1;
    }

    if (!info[0].IsString()) {
        Napi::TypeError::New(env, "You need to name yourself").ThrowAsJavaScriptException();
        return -1;
    }
    return 0;
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    Napi::String name = Napi::String::New(env, "KroomsClientWrapper");
    exports.Set(name, KroomsClientWrapper::GetClass(env));
    return exports;
}

NODE_API_MODULE(addon, Init)
