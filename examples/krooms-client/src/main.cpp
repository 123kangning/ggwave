#include "krooms_client.h"

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
    // 启动并注册回调函数
    client.start(call);
    return 0;
}