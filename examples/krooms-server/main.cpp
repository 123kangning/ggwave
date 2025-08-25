#include <SDL.h>

#include <atomic>
#include <chrono>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>

#include "ggwave-common-sdl2.h"
#include "ggwave-common.h"
#include "ggwave/ggwave.h"

class KRoomServer {
    std::atomic<bool> running{false};
    std::string currentMessage = "xxx";
    std::thread senderThread;
    std::mutex mutex;
    bool sendEnabled = true;

    int senderLoop() {
        // GGWave_init(0, 0, -1, 0.0f, true);
        auto ggWave = GGWave_instance();
        int protocalId = GGWAVE_PROTOCOL_AUDIBLE_FAST;
        // 协议设置
        auto & protocols = GGWave::Protocols::kDefault();

        printf("Using protocol: %s\n", protocols[protocalId].name);

        while (running) {
            // 初始化编码
            {
                // std::lock_guard<std::mutex> lock(mutex);
                if (sendEnabled) {
                    std::time_t timestamp = std::time(nullptr);
                    std::string tstr = std::asctime(std::localtime(&timestamp));
                    tstr.back() = 0;
                    printf("[%s]Sending message: %s\n", tstr.c_str(), currentMessage.c_str());
                    ggWave->init(currentMessage.c_str(), GGWave::TxProtocolId(protocalId), 10);
                }
            }

            // 等待下次发送
            printf("Waiting 3 seconds before next transmission...\n\n");
            std::this_thread::sleep_for(std::chrono::seconds(3));
        }
        return 0;
    }

   public:
    int start(const std::string & msg) {
        running.store(true);
        currentMessage = msg;
        // 初始化音频系统
        if (GGWave_init(0, 0, -1, 0.0f, true) == false) {
            fprintf(stderr, "Failed to initialize GGWave\n");
            return -1;
        }
        senderThread = std::thread(&KRoomServer::senderLoop, this);
        while (running) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            {
                // std::lock_guard<std::mutex> lock(mutex);
                GGWave_mainLoop();
            }
        }
        return 0;
    }

    void disableSend() {
        sendEnabled = false;
    }
    void enableSend() {
        sendEnabled = true;
    }
    void stop() {
        running.store(false);
        GGWave_deinit();
        if (senderThread.joinable())
            senderThread.join();
    }

    void updateMessage(const std::string & msg) {
        currentMessage = msg;
    }
};

std::atomic<bool> g_running(true);
KRoomServer server;

void signalHandler(int signum) {
    printf("Interrupt signal (%d) received.\n", signum);
    g_running = false;
    server.stop();
    printf("Server stopped.\n");
}

void updateInterval(int interval) {
    server.updateMessage("123456-507");  // 设置初始消息
    while (g_running) {
        // 模拟每10秒投屏码更新
        std::this_thread::sleep_for(std::chrono::seconds(interval));

        // 随机生成6位数字 + '-507'
        int random = rand() % 1000000;
        std::string newMessage = std::to_string(random) + "-507";
        printf("Updating message to: %s\n", newMessage.c_str());
        server.updateMessage(newMessage);
    }
}

int main() {
    signal(SIGINT, signalHandler);

    // 启动更新线程
    std::thread updateThread(updateInterval, 10);
    // 启动服务器
    server.start("123456-507");
    updateThread.join();
    return 0;
}