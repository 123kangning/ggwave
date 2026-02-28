#include "krooms_send_core.h"

KRoomSend sender;
bool g_running = true;
void signalHandler(int signum) {
    printf("Interrupt signal (%d) received.\n", signum);
    g_running = false;
    sender.stop();
    printf("Sender stopped.\n");
}

void updateInterval(int interval) {
    sender.updateMessage("123456-507");  // 设置初始消息
    while (g_running) {
        // 模拟每10秒投屏码更新
        std::this_thread::sleep_for(std::chrono::seconds(interval));

        // 随机生成6位数字 + '-507'
        int random1 = rand() % 10;
        int random2 = rand() % 10;
        int random3 = rand() % 10;
        int random4 = rand() % 10;
        int random5 = rand() % 10;
        int random6 = rand() % 10;
        std::string newMessage = std::to_string(random1) + std::to_string(random2) + std::to_string(random3) + std::to_string(random4) + std::to_string(random5) + std::to_string(random6) + "-507";
        printf("Updating message to: %s\n", newMessage.c_str());
        sender.updateMessage(newMessage);
    }
}

int main() {
    signal(SIGINT, signalHandler);

    // 启动更新线程
    std::thread updateThread(updateInterval, 10);
    // 启动发送器
    sender.start("123456-507");
    updateThread.join();
    return 0;
}