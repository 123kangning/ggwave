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
    serverThread = nullptr;
}

// 包装start方法 - 在后台线程中启动，避免阻塞 JavaScript
Napi::Value KRoomServerWrapper::Start(const Napi::CallbackInfo & info) {
    if (check(info) < 0) {
        return Napi::Number::New(info.Env(), -1);
    }
    
    std::string message = info[0].ToString();
    printf("Starting server in background thread with message: %s\n", message.c_str());
    
    // 在新线程中启动服务器，避免阻塞 JavaScript 主线程
    serverThread = std::make_unique<std::thread>([this, message]() {
        printf("Server thread started\n");
        int result = server.start(message);
        printf("Server thread finished with result: %d\n", result);
    });
    
    // 给服务器一点时间初始化
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    return Napi::Number::New(info.Env(), 0);
}

// 包装stop方法
Napi::Value KRoomServerWrapper::Stop(const Napi::CallbackInfo & info) {
    printf("Stopping server...\n");
    server.stop();
    
    // 等待服务器线程结束
    if (serverThread && serverThread->joinable()) {
        printf("Waiting for server thread to finish...\n");
        serverThread->join();
        serverThread.reset();
        printf("Server thread joined successfully\n");
    }
    
    return Napi::Number::New(info.Env(), 0);
}

// 包装updateMessage - 在主线程中执行，不会被阻塞
Napi::Value KRoomServerWrapper::UpdateMessage(const Napi::CallbackInfo & info) {
    if (check(info) < 0) {
        return Napi::Number::New(info.Env(), -1);
    }
    std::string msg = info[0].ToString();
    printf("Updating message to: %s\n", msg.c_str());
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