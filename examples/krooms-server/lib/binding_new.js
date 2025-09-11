// 加载从 build/lib 目录下的 .node 文件
const path = require('path');
const addon = require(path.join(__dirname, '../../../build/lib/krooms-server-lib.node'));

class KroomsServer {
    constructor(name = 'default-server') {
        this._addonInstance = new addon.KRoomServerWrapper(name);
        this._running = false;
        this._updateInterval = null;
    }

    start(message = "123456-507") {
        console.log(`Starting server with message: ${message}`);
        this._running = true;
        
        // 现在 start 方法是非阻塞的，在后台线程中运行
        const result = this._addonInstance.start(message);
        
        // 给服务器一点时间初始化
        setTimeout(() => {
            console.log("Server should be running in background thread now");
        }, 200);
        
        return result;
    }

    stop() {
        console.log("Stopping server...");
        this._running = false;
        
        // 清理定时器
        if (this._updateInterval) {
            console.log("Clearing periodic update interval...");
            clearInterval(this._updateInterval);
            this._updateInterval = null;
        }
        
        try {
            const result = this._addonInstance.stop();
            console.log("Server stopped successfully");
            return result;
        } catch (error) {
            console.error("Error stopping server:", error);
            throw error;
        }
    }

    updateMessage(message) {
        console.log(`Updating message to: ${message}`);
        return this._addonInstance.updateMessage(message);
    }

    // 模拟 main.cpp 中的定期更新逻辑
    startPeriodicUpdate(intervalSeconds = 10) {
        console.log(`Starting periodic updates every ${intervalSeconds} seconds...`);
        
        // 清除之前的定时器（如果存在）
        if (this._updateInterval) {
            console.log("Clearing existing update interval...");
            clearInterval(this._updateInterval);
            this._updateInterval = null;
        }
        
        // 设置初始消息
        console.log("Setting initial message...");
        this.updateMessage("123456-507");
        
        let updateCount = 0;
        
        this._updateInterval = setInterval(() => {
            updateCount++;
            console.log(`[UPDATE ${updateCount}] Periodic update triggered (running: ${this._running})`);
            
            if (!this._running) {
                console.log("Server not running, stopping periodic updates...");
                clearInterval(this._updateInterval);
                this._updateInterval = null;
                return;
            }
            
            // 随机生成6位数字 + '-507'
            const random1 = Math.floor(Math.random() * 10);
            const random2 = Math.floor(Math.random() * 10);
            const random3 = Math.floor(Math.random() * 10);
            const random4 = Math.floor(Math.random() * 10);
            const random5 = Math.floor(Math.random() * 10);
            const random6 = Math.floor(Math.random() * 10);
            
            const newMessage = `${random1}${random2}${random3}${random4}${random5}${random6}-507`;
            console.log(`[UPDATE ${updateCount}] Generated new message: ${newMessage}`);
            
            try {
                this.updateMessage(newMessage);
                console.log(`[UPDATE ${updateCount}] Message update sent successfully`);
            } catch (error) {
                console.error(`[UPDATE ${updateCount}] Error updating message:`, error);
            }
        }, intervalSeconds * 1000);
        
        console.log(`Periodic update timer started with interval: ${intervalSeconds}s`);
    }

    isRunning() {
        return this._running;
    }
}

module.exports = KroomsServer;