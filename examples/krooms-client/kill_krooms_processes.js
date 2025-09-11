#!/usr/bin/env node

/**
 * 强制终止 KRooms 相关的 Node.js 进程
 * 当 Ctrl+C 无法正常终止程序时使用
 */

const { exec } = require('child_process');
const os = require('os');

console.log("=== KRooms Process Killer ===");

function killProcesses() {
    const platform = os.platform();
    
    if (platform === 'darwin' || platform === 'linux') {
        // macOS 和 Linux
        console.log("Searching for KRooms Node.js processes...");
        
        exec('ps aux | grep -E "(krooms|test_.*node)" | grep -v grep', (error, stdout, stderr) => {
            if (error) {
                console.log("No KRooms processes found or error occurred:", error.message);
                return;
            }
            
            if (stdout.trim()) {
                console.log("Found processes:");
                console.log(stdout);
                
                // 提取进程 ID 并终止
                const lines = stdout.trim().split('\n');
                const pids = lines.map(line => {
                    const parts = line.trim().split(/\s+/);
                    return parts[1]; // PID 通常在第二列
                }).filter(pid => pid && !isNaN(pid));
                
                if (pids.length > 0) {
                    console.log(`\nKilling processes: ${pids.join(', ')}`);
                    pids.forEach(pid => {
                        exec(`kill -9 ${pid}`, (killError) => {
                            if (killError) {
                                console.error(`Failed to kill process ${pid}:`, killError.message);
                            } else {
                                console.log(`✓ Killed process ${pid}`);
                            }
                        });
                    });
                } else {
                    console.log("No valid PIDs found.");
                }
            } else {
                console.log("No KRooms processes found.");
            }
        });
        
    } else if (platform === 'win32') {
        // Windows
        console.log("Searching for KRooms Node.js processes on Windows...");
        
        exec('tasklist /FI "IMAGENAME eq node.exe" /FO CSV', (error, stdout, stderr) => {
            if (error) {
                console.log("No Node.js processes found or error occurred:", error.message);
                return;
            }
            
            console.log("Found Node.js processes:");
            console.log(stdout);
            
            // 在 Windows 上，你可能需要手动识别和终止进程
            console.log("\nOn Windows, please manually identify and terminate KRooms processes using Task Manager.");
            console.log("Look for node.exe processes running KRooms test scripts.");
        });
        
    } else {
        console.log(`Unsupported platform: ${platform}`);
    }
}

function showUsage() {
    console.log("\nUsage:");
    console.log("  node kill_krooms_processes.js [--kill]");
    console.log("");
    console.log("Options:");
    console.log("  --kill   Actually kill the processes (default: just list them)");
    console.log("");
    console.log("This script helps you terminate KRooms Node.js processes that");
    console.log("may be stuck and not responding to Ctrl+C.");
}

// 主程序
if (process.argv.includes('--kill')) {
    killProcesses();
} else if (process.argv.includes('--help') || process.argv.includes('-h')) {
    showUsage();
} else {
    console.log("Listing KRooms processes (use --kill to terminate them):");
    killProcesses();
}