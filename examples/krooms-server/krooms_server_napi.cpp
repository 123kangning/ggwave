#include <napi.h>

class KRoomServerWrapper : public Napi::ObjectWrap<KRoomServerWrapper> {
public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    KRoomServerWrapper(const Napi::CallbackInfo& info);

private:
    static Napi::FunctionReference constructor;
    KRoomServer server;

    // 包装start方法
    Napi::Value Start(const Napi::CallbackInfo& info) {
        return Napi::Number::New(info.Env(), server.start());
    }

    // 包装stop方法
    void Stop(const Napi::CallbackInfo& info) {
        server.stop();
    }

    // 包装updateMessage
    void UpdateMessage(const Napi::CallbackInfo& info) {
        std::string msg = info[0].ToString();
        server.updateMessage(msg);
    }
};

// 初始化导出模块
Napi::Object Init(Napi::Env env, Napi::Object exports) {
    exports.Set("KRoomServer", KRoomServerWrapper::GetClass(env));
    return exports;
}

NODE_API_MODULE(krooms_server, Init)
