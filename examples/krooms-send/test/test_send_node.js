const KroomsSend = require("../lib/binding_new.js");
const assert = require("assert");

console.log("=== KRooms Send Node.js Test ===");

// 全局变量用于跟踪发送器实例
let globalSender = null;
let isRunning = false;

// 测试音频发送功能（模拟 main.cpp 的逻辑）
function testAudioSending() {
    console.log("\nTesting audio sending functionality...");
    globalSender = new KroomsSend("audio-sender");
    
    console.log("Starting audio sender...");
    
    try {
        isRunning = true;
        
        // 启动发送器
        const startResult = globalSender.start("123456-507");
        console.log("Sender start result:", startResult);
        
        // 启动定期更新（模拟 main.cpp 中的 updateInterval 线程）
        console.log("Starting periodic message updates every 5 seconds...");
        globalSender.startPeriodicUpdate(5);
        
        console.log("Sender is running and sending audio data... (Press Ctrl+C to stop)");
        
        // 注册清理函数
        const cleanup = () => {
            stopSender();
        };
        
        process.once('SIGINT', cleanup);
        process.once('SIGTERM', cleanup);
        process.once('exit', cleanup);
        
    } catch (error) {
        console.error("Error starting sender:", error);
        stopSender();
    }
}

// 停止发送器的函数
function stopSender() {
    if (globalSender && isRunning) {
        console.log("Stopping sender...");
        isRunning = false;
        try {
            globalSender.stop();
            console.log("Sender stopped.");
        } catch (error) {
            console.error("Error stopping sender:", error);
        }
        globalSender = null;
        
        // 强制退出
        setTimeout(() => {
            process.exit(0);
        }, 1000);
    }
}

// 处理 Ctrl+C 信号
process.on('SIGINT', () => {
    console.log('\nSIGINT signal received. Stopping sender...');
    stopSender();
});

process.on('SIGTERM', () => {
    console.log('\nSIGTERM signal received. Stopping sender...');                stopSender();
});

// 运行测试
try {
    testAudioSending();
} catch (error) {
    console.error("Test failed:", error);
    process.exit(1);
}