# KRooms Node.js 测试指南

本文档说明如何使用 JavaScript 测试 KRooms 的 Node.js 绑定。

## 前置条件

1. 确保已经构建了 .node 文件：
   ```bash
   # 在项目根目录执行
   make
   ```

2. 确认以下文件存在：
   - `build/lib/krooms-client-lib.node`
   - `build/lib/krooms-server-lib.node`

3. 安装 Node.js 依赖：
   ```bash
   cd examples/krooms-client && npm install
   cd examples/krooms-server && npm install
   ```

## 测试文件说明

### KRooms Client 测试

**文件位置**: `examples/krooms-client/test/test_client_node.js`

**功能**: 测试音频接收功能，模拟 `main.cpp` 中的客户端逻辑

**运行方式**:
```bash
cd examples/krooms-client
# 基本功能测试
node test/test_client_node.js

# 音频监听测试（会持续监听10秒）
node test/test_client_node.js --listen
```

### KRooms Server 测试

**文件位置**: `examples/krooms-server/test/test_server_node.js`

**功能**: 测试音频发送功能，模拟 `main.cpp` 中的服务器逻辑

**运行方式**:
```bash
cd examples/krooms-server
# 基本功能测试
node test/test_server_node.js

# 音频发送测试（会发送30秒，每5秒更新消息）
node test/test_server_node.js --send

# 手动消息更新测试
node test/test_server_node.js --manual

# 测试定期更新功能（专门测试）
node test/test_periodic_update.js

# 简单的定期更新测试（不涉及音频）
node test/test_simple_periodic.js

# 测试非阻塞行为
node test/test_non_blocking.js
```

### 集成测试

**文件位置**: `examples/test_krooms_integration.js`

**功能**: 同时测试服务器和客户端的通信

**运行方式**:
```bash
cd examples
# 显示使用说明
node test_krooms_integration.js

# 运行集成测试
node test_krooms_integration.js --run
```

## 测试逻辑说明

### Client 测试逻辑（对应 krooms-client/src/main.cpp）

1. **创建客户端实例**
2. **注册回调函数**: 接收到音频数据时打印时间戳和内容
3. **启动音频监听**: 持续监听音频信号
4. **信号处理**: 支持 Ctrl+C 优雅退出

JavaScript 实现：
```javascript
const client = new KroomsClient("audio-receiver");

const dataCallback = (receivedData) => {
    const timestamp = new Date().toLocaleString();
    console.log(`[${timestamp}] Received: '${receivedData}'`);
};

client.start(dataCallback);
```

### Server 测试逻辑（对应 krooms-server/src/main.cpp）

1. **创建服务器实例**
2. **启动服务器**: 在后台线程中开始发送音频信号（非阻塞）
3. **定期更新消息**: 在主线程中每隔指定时间生成新的6位随机数字 + "-507"
4. **信号处理**: 支持 Ctrl+C 优雅退出

JavaScript 实现：
```javascript
const server = new KroomsServer("audio-sender");

// 启动服务器（非阻塞，在后台线程运行）
server.start("123456-507");

// 定期更新（在主线程中执行，不会被阻塞）
server.startPeriodicUpdate(10); // 每10秒更新
```

**重要改进**: 现在 `server.start()` 是非阻塞的，它会在后台线程中运行音频发送循环，而 `updateMessage()` 在主线程中执行，确保消息更新不会被阻塞。

## API 接口说明

### KroomsClient

- `constructor(name)`: 创建客户端实例
- `greet(str)`: 测试方法
- `start(callback)`: 开始监听音频，callback 接收数据
- `stop()`: 停止监听

### KroomsServer

- `constructor(name)`: 创建服务器实例
- `start(message)`: 开始发送指定消息
- `stop()`: 停止发送
- `updateMessage(message)`: 更新要发送的消息
- `startPeriodicUpdate(intervalSeconds)`: 开始定期随机更新消息

## 注意事项

1. **音频设备**: 确保系统有可用的音频输入/输出设备
2. **环境噪音**: 测试时保持安静的环境以获得最佳效果
3. **权限**: 可能需要授予 Node.js 访问麦克风的权限
4. **同时运行**: 可以在不同终端窗口同时运行服务器和客户端测试
5. **调试**: 如果遇到问题，检查 .node 文件是否正确生成在 `build/lib/` 目录下

## 故障排除

### 常见问题

1. **.node 文件找不到**
   - 确保 .node 文件存在且可访问
   - 检查路径是否正确

2. **Node.js 版本兼容性**
   - 建议使用 Node.js 14+ 版本
   - 确保 node-addon-api 版本兼容

3. **音频设备问题**
   - 检查系统音频设备是否正常工作
   - 确保没有其他程序占用音频设备
   - 在 macOS 上可能需要授予麦克风权限

### Ctrl+C 无法终止程序

如果遇到 Ctrl+C 无法终止 JavaScript 程序的问题：

**方法1：使用强制终止脚本**
```bash
cd examples
node kill_krooms_processes.js --kill
```

**方法2：手动查找并终止进程**
```bash
# 查找相关进程
ps aux | grep -E "(krooms|test_.*node)" | grep -v grep

# 终止特定进程（替换 PID）
kill -9 <PID>
```

**方法3：在新终端中终止**
```bash
# 查找 Node.js 进程
pgrep -f "node.*test"

# 终止所有匹配的进程
pkill -f "node.*test"
```

### 程序卡住的原因

程序可能卡住的原因：
1. C++ 扩展中的阻塞操作（如音频处理循环）
2. 信号处理器没有正确注册
3. 异步操作没有正确清理

### 预防措施

1. **设置自动超时**：所有测试都有自动超时机制
2. **使用 --timeout 参数**：
   ```bash
   timeout 30s node test/test_client_node.js --listen
   ```
3. **在独立终端运行**：避免影响其他工作