const KroomsClient = require("../lib/binding_new.js");
const assert = require("assert");

console.log("=== KRooms Client Node.js Test ===");

// 测试基本功能
function testBasic() {
    console.log("Testing basic functionality...");
    const client = new KroomsClient("test-client");
    
    // 测试 greet 方法
    assert(client.greet, "The greet method is not defined");
    console.log("✓ Greet method exists");
    
    // 测试 start 方法
    assert(client.start, "The start method is not defined");
    console.log("✓ Start method exists");
    
    // 测试 stop 方法
    assert(client.stop, "The stop method is not defined");
    console.log("✓ Stop method exists");
}

// 全局变量用于跟踪客户端实例
let globalClient = null;
let isRunning = false;

// 测试音频接收功能（模拟 main.cpp 的逻辑）
function testAudioReceiving() {
    console.log("\nTesting audio receiving functionality...");
    globalClient = new KroomsClient("audio-receiver");
    
    // 定义回调函数，类似 main.cpp 中的逻辑
    const dataCallback = (receivedData) => {
        const timestamp = new Date().toLocaleString();
        console.log(`[${timestamp}] Received: '${receivedData}'`);
    };
    
    console.log("Starting audio client...");
    console.log("Listening for audio data... (Press Ctrl+C to stop)");
    
    try {
        isRunning = true;
        
        // 启动客户端并注册回调函数
        const result = globalClient.start(dataCallback);
        console.log("Client start result:", result);
        
        // 清理函数
        const cleanup = () => {
            stopClient();
        };
        
        // 注册清理函数
        process.once('SIGINT', cleanup);
        process.once('SIGTERM', cleanup);
        process.once('exit', cleanup);
        
    } catch (error) {
        console.error("Error starting client:", error);
        stopClient();
    }
}

// 停止客户端的函数
function stopClient() {
    if (globalClient && isRunning) {
        console.log("Stopping client...");
        isRunning = false;
        try {
            globalClient.stop();
            console.log("Client stopped.");
        } catch (error) {
            console.error("Error stopping client:", error);
        }
        globalClient = null;
        
        // 强制退出
        setTimeout(() => {
            process.exit(0);
        }, 1000);
    }
}

// 处理 Ctrl+C 信号
process.on('SIGINT', () => {
    console.log('\nSIGINT signal received. Stopping client...');
    stopClient();
});

process.on('SIGTERM', () => {
    console.log('\nSIGTERM signal received. Stopping client...');
    stopClient();
});

// 运行测试
try {
    testBasic();
    console.log("✓ Basic tests passed");
    
    // 如果命令行参数包含 --listen，则启动音频接收测试
    if (process.argv.includes('--listen')) {
        testAudioReceiving();
    } else {
        console.log("\n✓ All tests passed!");
        console.log("Run with --listen flag to test audio receiving: node test_client_node.js --listen");
    }
} catch (error) {
    console.error("Test failed:", error);
    process.exit(1);
}