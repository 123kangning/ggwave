const KroomsClient = require("../lib/binding_new.js");

console.log("=== KRooms Client Callback Debug Test ===");

let client = null;
let callbackCount = 0;

function cleanup() {
    if (client) {
        console.log("Cleaning up client...");
        try {
            client.stop();
        } catch (error) {
            console.error("Error stopping client:", error);
        }
        client = null;
    }
    
    setTimeout(() => {
        console.log(`Test completed. Total callbacks received: ${callbackCount}`);
        process.exit(0);
    }, 1000);
}

// 处理退出信号
process.on('SIGINT', () => {
    console.log('\nSIGINT received. Cleaning up...');
    cleanup();
});

try {
    console.log("Creating client instance...");
    client = new KroomsClient("debug-client");
    
    console.log("Setting up callback function...");
    const dataCallback = (receivedData) => {
        callbackCount++;
        const timestamp = new Date().toLocaleString();
        console.log(`[CALLBACK ${callbackCount}] [${timestamp}] Received: '${receivedData}'`);
        console.log(`Data length: ${receivedData.length}, Type: ${typeof receivedData}`);
    };
    
    console.log("Starting client with callback...");
    const result = client.start(dataCallback);
    console.log(`Client start result: ${result}`);
    
    console.log("Client is now listening for audio data...");
    console.log("You should see C++ debug messages in the console.");
    console.log("Send some audio data from a server to test the callback.");
    console.log("Press Ctrl+C to stop.");
    
    // 自动停止测试（30秒后）
    setTimeout(() => {
        console.log("\nAuto-stopping test after 30 seconds...");
        cleanup();
    }, 30000);
    
} catch (error) {
    console.error("Test failed:", error);
    cleanup();
}