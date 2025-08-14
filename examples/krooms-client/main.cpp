#include <SDL.h>

#include "ggwave-common-sdl2.h"
#include "ggwave/ggwave.h"


std::atomic<bool> g_running(true);

void signalHandler(int signum) {
    printf("Interrupt signal (%d) received.\n", signum);
    g_running = false;
    printf("Client stopped.\n");
}

int main() {
    signal(SIGINT, signalHandler);

    // 初始化音频设备
    if (GGWave_init(0, 0, -1, 0.0f, false) == false) {
        printf("初始化失败\n");
        return -1;
    }

    auto ggWave = GGWave_instance();
    ggWave->init(0, nullptr, GGWAVE_PROTOCOL_ULTRASOUND_FASTEST, 10);

    printf("开始监听音频...\n");
    while (g_running) {
        GGWave_mainLoop();  // 持续处理音频

        // 打印接收到的消息

        GGWave::TxRxData rxData;
        int n = ggWave->rxTakeData(rxData);
        if (n > 0) {
            std::time_t timestamp = std::time(nullptr);
            std::string tstr = std::asctime(std::localtime(&timestamp));
            tstr.back() = 0;
            printf("[%s] Received: '%s'\n", tstr.c_str(), rxData.data());
        }
    }

    GGWave_deinit();
    return 0;
}