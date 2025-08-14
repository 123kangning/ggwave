#include <SDL.h>

#include <atomic>
#include <csignal>
#include <ctime>
#include <functional>

#include "ggwave-common-sdl2.h"
#include "ggwave-common.h"
#include "ggwave/ggwave.h"

class KRoomClient {
   public:
    // 注册回调函数
    void registerDataCallback(std::function<void(const std::string &)> callback) {
        dataCallback = callback;
    }

    int start(std::function<void(const std::string &)> callback) {
        dataCallback = callback;
        running.store(true);
        // 初始化音频设备
        if (GGWave_init(0, 0, -1, 0.0f, true) == false) {
            printf("初始化失败\n");
            return -1;
        }
        auto ggWave = GGWave_instance();

        for (int i = 0; i < GGWAVE_PROTOCOL_COUNT; ++i) {
            ggwave_rxToggleProtocol((ggwave_ProtocolId) i, false);
        }
        ggwave_rxToggleProtocol(GGWAVE_PROTOCOL_AUDIBLE_FAST, true);

        ggWave->init(0, nullptr, GGWAVE_PROTOCOL_ULTRASOUND_FASTEST, 10);
        printf("开始监听音频...\n");
        while (running.load()) {
            GGWave_mainLoop();  // 持续处理音频

            // 检查是否有接收到的数据
            GGWave::TxRxData rxData;
            int n = ggWave->rxTakeData(rxData);
            if (n > 0 && dataCallback) {
                // 使用正确的构造函数创建字符串
                std::string receivedData(reinterpret_cast<const char *>(rxData.data()), rxData.size());
                // 调用回调函数
                dataCallback(receivedData);
            }
        }

        return 0;
    }

    void stop() {
        running.store(false);
        GGWave_deinit();
    }

   private:
    std::atomic<bool> running;
    std::function<void(const std::string &)> dataCallback;
};

KRoomClient client;

void signalHandler(int signum) {
    printf("SIGINT %d signal received. Stopping client...\n", signum);
    client.stop();
}

int main() {
    signal(SIGINT, signalHandler);
    std::function<void(const std::string &)> call = [](const std::string & data) {
        std::time_t timestamp = std::time(nullptr);
        std::string tstr = std::asctime(std::localtime(&timestamp));
        tstr.back() = 0;
        printf("[%s] Received: '%s'\n", tstr.c_str(), data.c_str());
    };
    // 注册回调函数
    client.registerDataCallback(call);

    client.start(call);
    return 0;
}