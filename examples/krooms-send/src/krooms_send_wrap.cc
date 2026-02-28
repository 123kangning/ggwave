#include "krooms_send.h"

using namespace Napi;

static Napi::FunctionReference constructor;
// 实现静态函数 GetClass
Napi::Function KRoomSendWrapper::GetClass(Napi::Env env) {
    Napi::Function func = DefineClass(
        env, "KRoomSendWrapper",
        {KRoomSendWrapper::InstanceMethod("start", &KRoomSendWrapper::Start), KRoomSendWrapper::InstanceMethod("stop", &KRoomSendWrapper::Stop),
         KRoomSendWrapper::InstanceMethod("updateMessage", &KRoomSendWrapper::UpdateMessage)});
    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();
    return func;
}
KRoomSendWrapper::KRoomSendWrapper(const Napi::CallbackInfo & info) : ObjectWrap(info) {
    senderThread = nullptr;
}

// 包装start方法 - 在后台线程中启动，避免阻塞 JavaScript
Napi::Value KRoomSendWrapper::Start(const Napi::CallbackInfo & info) {
    if (check(info) < 0) {
        return Napi::Number::New(info.Env(), -1);
    }
    
    std::string message = info[0].ToString();
    printf("Starting sender in background thread with message: %s\n", message.c_str());
    
    // 在新线程中启动发送器，避免阻塞 JavaScript 主线程
    senderThread = std::make_unique<std::thread>([this, message]() {
        printf("Sender thread started\n");
        int result = sender.start(message);
        printf("Sender thread finished with result: %d\n", result);
    });
    
    // 给服务器一点时间初始化
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    return Napi::Number::New(info.Env(), 0);
}

// 包装stop方法
Napi::Value KRoomSendWrapper::Stop(const Napi::CallbackInfo & info) {
    printf("Stopping sender...\n");
    sender.stop();
    
    // 等待发送器线程结束
    if (senderThread && senderThread->joinable()) {
        printf("Waiting for sender thread to finish...\n");
        senderThread->join();
        senderThread.reset();
        printf("Sender thread joined successfully\n");
    }
    
    return Napi::Number::New(info.Env(), 0);
}

// 包装updateMessage - 在主线程中执行，不会被阻塞
Napi::Value KRoomSendWrapper::UpdateMessage(const Napi::CallbackInfo & info) {
    if (check(info) < 0) {
        return Napi::Number::New(info.Env(), -1);
    }
    std::string msg = info[0].ToString();
    printf("Updating message to: %s\n", msg.c_str());
    sender.updateMessage(msg);
    return Napi::Number::New(info.Env(), 0);
}

int KRoomSendWrapper::check(const Napi::CallbackInfo & info) {
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
    exports.Set("KRoomSendWrapper", KRoomSendWrapper::GetClass(env));
    return exports;
}

NODE_API_MODULE(addon, Init)