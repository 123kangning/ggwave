const KroomsServer = require("../lib/binding_new.js");
const assert = require("assert");

console.log("=== KRooms Server Node.js Test ===");

// 测试基本功能
function testBasic() {
    console.log("Testing basic functionality...");
    const server = new KroomsServer("test-server");
    
    // 测试 start 方法
    assert(server.start, "The start method is not defined");
    console.log("✓ Start method exists");
    
    // 测试 stop 方法
    assert(server.stop, "The stop method is not defined");
    console.log("✓ Stop method exists");
    
    // 测试 updateMessage 方法
    assert(server.updateMessage, "The updateMessage method is not defined");
    console.log("✓ UpdateMessage method exists");
}

// 全局变量用于跟踪服务器实例
let globalServer = null;
let isRunning = false;
let autoStopTimeout = null;

// 测试音频发送功能（模拟 main.cpp 的逻辑）
function testAudioSending() {
    console.log("\nTesting audio sending functionality...");
    globalServer = new KroomsServer("audio-sender");
    
    console.log("Starting audio server...");
    
    try {
        isRunning = true;
        
        // 启动服务器
        const startResult = globalServer.start("123456-507");
        console.log("Server start result:", startResult);
        
        // 启动定期更新（模拟 main.cpp 中的 updateInterval 线程）
        console.log("Starting periodic message updates every 5 seconds...");
        globalServer.startPeriodicUpdate(5);
        
        console.log("Server is running and sending audio data... (Press Ctrl+C to stop)");
        
        // 注册清理函数
        const cleanup = () => {
            stopServer();
        };
        
        process.once('SIGINT', cleanup);
        process.once('SIGTERM', cleanup);
        process.once('exit', cleanup);
        
    } catch (error) {
        console.error("Error starting server:", error);
        stopServer();
    }
}

// 测试手动消息更新
function testManualUpdate() {
    console.log("\nTesting manual message update...");
    globalServer = new KroomsServer("manual-server");
    
    try {
        isRunning = true;
        
        // 启动服务器
        globalServer.start("000000-507");
        
        // 手动更新几次消息
        const messages = ["111111-507", "222222-507", "333333-507"];
        let index = 0;
        
        const updateInterval = setInterval(() => {
            if (!isRunning) {
                clearInterval(updateInterval);
                return;
            }
            
            if (index < messages.length) {
                globalServer.updateMessage(messages[index]);
                index++;
            } else {
                clearInterval(updateInterval);
                console.log("Manual update test completed.");
                stopServer();
            }
        }, 2000);
        
        // 注册清理函数
        const cleanup = () => {
            clearInterval(updateInterval);
            stopServer();
        };
        
        process.once('SIGINT', cleanup);
        process.once('SIGTERM', cleanup);
        process.once('exit', cleanup);
        
    } catch (error) {
        console.error("Error in manual update test:", error);
        stopServer();
    }
}

// 停止服务器的函数
function stopServer() {
    if (globalServer && isRunning) {
        console.log("Stopping server...");
        isRunning = false;
        try {
            globalServer.stop();
            console.log("Server stopped.");
        } catch (error) {
            console.error("Error stopping server:", error);
        }
        globalServer = null;
        
        // 强制退出
        setTimeout(() => {
            process.exit(0);
        }, 1000);
    }
}

// 处理 Ctrl+C 信号
process.on('SIGINT', () => {
    console.log('\nSIGINT signal received. Stopping server...');
    stopServer();
});

process.on('SIGTERM', () => {
    console.log('\nSIGTERM signal received. Stopping server...');
    stopServer();
});

// 运行测试
try {
    testBasic();
    console.log("✓ Basic tests passed");
    
    // 根据命令行参数选择测试模式
    if (process.argv.includes('--send')) {
        testAudioSending();
    } else if (process.argv.includes('--manual')) {
        testManualUpdate();
    } else {
        console.log("\n✓ All tests passed!");
        console.log("Available test modes:");
        console.log("  --send   : Test audio sending with periodic updates");
        console.log("  --manual : Test manual message updates");
        console.log("\nExample: node test_server_node.js --send");
    }
} catch (error) {
    console.error("Test failed:", error);
    process.exit(1);
}