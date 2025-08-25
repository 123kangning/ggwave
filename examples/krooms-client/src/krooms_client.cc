#include "krooms_client.h"

using namespace Napi;
KRoomClient::KRoomClient() {
    this->running = true;
    this->dataCallback = nullptr;
}

int KRoomClient::start(std::function<void(const std::string &)> callback) {
    this->dataCallback = callback;
    this->running = true;
    // 初始化音频设备
    if (GGWave_init(0, 0, -1, 0.0f, false) == false) {
        printf("初始化失败\n");
        return -1;
    }
    auto ggWave = GGWave_instance();

    for (int i = 0; i < GGWAVE_PROTOCOL_COUNT; ++i) {
        ggwave_rxToggleProtocol((ggwave_ProtocolId) i, false);
    }
    ggwave_rxToggleProtocol(GGWAVE_PROTOCOL_AUDIBLE_FAST, true);

    // ggWave->init(0, nullptr, GGWAVE_PROTOCOL_AUDIBLE_FAST, 10);
    printf("开始监听音频...\n");
    while (this->running) {
        GGWave_mainLoop();  // 持续处理音频

        // 检查是否有接收到的数据
        GGWave::TxRxData rxData;
        int n = ggWave->rxTakeData(rxData);
        if (n > 0 && this->dataCallback) {
            // 使用正确的构造函数创建字符串
            std::string receivedData(reinterpret_cast<const char *>(rxData.data()), rxData.size());
            // 调用回调函数
            this->dataCallback(receivedData);
        }
    }

    return 0;
}
// void receiveLoop() {

// }

void KRoomClient::stop() {
    this->running = false;
    GGWave_deinit();
}
