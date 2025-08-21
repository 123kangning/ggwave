#include "krooms_server.h"

KRoomServer::KRoomServer() {
    this->running = true;
}
int KRoomServer::senderLoop() {
    // GGWave_init(0, 0, -1, 0.0f, true);
    auto ggWave = GGWave_instance();
    int protocalId = GGWAVE_PROTOCOL_AUDIBLE_FAST;
    // 协议设置
    auto & protocols = GGWave::Protocols::kDefault();

    printf("Using protocol: %s\n", protocols[protocalId].name);

    while (this->running) {
        // 初始化编码
        {
            // std::lock_guard<std::mutex> lock(mutex);
            if (sendEnabled) {
                std::time_t timestamp = std::time(nullptr);
                std::string tstr = std::asctime(std::localtime(&timestamp));
                tstr.back() = 0;
                printf("[%s]Sending message: %s\n", tstr.c_str(), currentMessage.c_str());
                ggWave->init(currentMessage.c_str(), GGWave::TxProtocolId(protocalId), 50);
            }
        }

        // 等待下次发送
        printf("Waiting 3 seconds before next transmission...\n\n");
        std::this_thread::sleep_for(std::chrono::seconds(3));
    }
    return 0;
}
int KRoomServer::start(const std::string & msg) {
    this->running = true;
    currentMessage = msg;
    // 初始化音频系统
    if (GGWave_init(0, 0, -1, 0.0f, false) == false) {
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

void KRoomServer::disableSend() {
    sendEnabled = false;
}
void KRoomServer::enableSend() {
    sendEnabled = true;
}
void KRoomServer::stop() {
    this->running = false;
    GGWave_deinit();
    if (senderThread.joinable())
        senderThread.join();
}

void KRoomServer::updateMessage(const std::string & msg) {
    currentMessage = msg;
}