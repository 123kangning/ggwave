#include "krooms_server.h"

using namespace Napi;

static Napi::FunctionReference constructor;
// 实现静态函数 GetClass
Napi::Function KRoomServerWrapper::GetClass(Napi::Env env) {
    Napi::Function func = DefineClass(
        env, "KRoomServerWrapper",
        {KRoomServerWrapper::InstanceMethod("start", &KRoomServerWrapper::Start), KRoomServerWrapper::InstanceMethod("stop", &KRoomServerWrapper::Stop),
         KRoomServerWrapper::InstanceMethod("updateMessage", &KRoomServerWrapper::UpdateMessage)});
    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();
    return func;
}
KRoomServerWrapper::KRoomServerWrapper(const Napi::CallbackInfo & info) : ObjectWrap(info) {
}

// 包装start方法
Napi::Value KRoomServerWrapper::Start(const Napi::CallbackInfo & info) {
    if (check(info) < 0) {
        return Napi::Number::New(info.Env(), -1);
    }
    return Napi::Number::New(info.Env(), server.start(info[0].ToString()));
}

// 包装stop方法
Napi::Value KRoomServerWrapper::Stop(const Napi::CallbackInfo & info) {
    server.stop();
    return Napi::Number::New(info.Env(), 0);
}

// 包装updateMessage
Napi::Value KRoomServerWrapper::UpdateMessage(const Napi::CallbackInfo & info) {
    if (check(info) < 0) {
        return Napi::Number::New(info.Env(), -1);
    }
    std::string msg = info[0].ToString();
    server.updateMessage(msg);
    return Napi::Number::New(info.Env(), 0);
}

int KRoomServerWrapper::check(const Napi::CallbackInfo & info) {
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

// 初始化导出模块
Napi::Object Init(Napi::Env env, Napi::Object exports) {
    exports.Set("KRoomServerWrapper", KRoomServerWrapper::GetClass(env));
    return exports;
}

NODE_API_MODULE(addon, Init)