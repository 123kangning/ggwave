#include "krooms_client.h"

using namespace Napi;

KroomsClientWrapper::KroomsClientWrapper(const Napi::CallbackInfo & info) : ObjectWrap(info) {
    // 初始化 ThreadSafeFunction 为空
    this->tsfn = nullptr;
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

    // 创建 ThreadSafeFunction 并保存为成员变量
    this->tsfn = Napi::ThreadSafeFunction::New(
        env,
        jsCallback,
        "KRoomClientCallback",
        0,  // 无限制队列大小
        1   // 只有一个线程会调用
    );

    // 创建回调函数
    auto callback = [this](const std::string & data) {
        if (this->tsfn) {
            printf("C++ callback called with data: %s\n", data.c_str()); // 调试输出
            
            auto status = this->tsfn.BlockingCall([data](Napi::Env env, Napi::Function jsCallback) {
                printf("Calling JavaScript callback with data: %s\n", data.c_str()); // 调试输出
                jsCallback.Call({Napi::String::New(env, data)});
            });
            
            if (status != napi_ok) {
                printf("ThreadSafeFunction call failed with status: %d\n", status);
            }
        } else {
            printf("ThreadSafeFunction is null!\n");
        }
    };

    // 在新线程中启动客户端，避免阻塞主线程
    this->clientThread = std::thread([this, callback]() {
        printf("Starting client in background thread...\n");
        this->client.start(callback);
    });

    return Napi::Number::New(env, 0);
}
Napi::Value KroomsClientWrapper::Stop(const Napi::CallbackInfo & info) {
    Napi::Env env = info.Env();
    
    printf("Stopping client...\n");
    client.stop();
    
    // 等待线程结束
    if (this->clientThread.joinable()) {
        printf("Waiting for client thread to finish...\n");
        this->clientThread.join();
    }
    
    // 清理 ThreadSafeFunction
    if (this->tsfn) {
        printf("Releasing ThreadSafeFunction...\n");
        this->tsfn.Release();
        this->tsfn = nullptr;
    }
    
    return Napi::Number::New(env, 0);
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
