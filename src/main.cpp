/**
 * @file main.cpp
 * @brief BottleBoy 主程序入口
 * 
 * 这个文件保持简洁，只负责：
 * 1. 初始化串口
 * 2. 调用各模块的初始化函数
 * 3. 在主循环中调用各模块的处理函数
 * 
 * 各模块职责：
 * - NetworkInit: WiFi和Web服务器初始化
 * - DeviceRegistry: 传感器和执行器注册
 * - AutomationRules: 传感器和执行器联动规则
 */

#include <Arduino.h>
#include "core/Config.h"
#include "network/WiFiManager.h"
#include "sensors/base/SensorManager.h"
#include "actuators/base/ActuatorManager.h"
#include "network/WebServer.h"
#include "init/NetworkInit.h"
#include "init/DeviceRegistry.h"
#include "automation/AutomationRules.h"

// ========== 全局管理器实例 ==========
WiFiManager wifiManager;
SensorManager sensorManager;
ActuatorManager actuatorManager;
WebServerManager* webServer = nullptr;
AutomationRules* automationRules = nullptr;

// ========== 时间控制变量 ==========
unsigned long lastSensorUpdate = 0;
unsigned long lastBroadcast = 0;
unsigned long lastAutomation = 0;

/**
 * @brief Arduino setup函数 - 系统初始化
 */
void setup() {
    // ========== 1. 初始化串口 ==========
    Serial.begin(DEBUG_SERIAL_BAUD);
    delay(1000);
    Serial.println("\n\n=== BottleBoy Starting ===");
    
    // ========== 2. 初始化WiFi ==========
    NetworkInit::initWiFi(wifiManager);
    
    // ========== 3. 初始化设备（传感器+执行器） ==========
    DeviceRegistry::init(sensorManager, actuatorManager);
    
    // ========== 4. 初始化Web服务器 ==========
    webServer = NetworkInit::initWebServer(sensorManager, actuatorManager);
    
    // ========== 5. 初始化自动化规则 ==========
    Serial.println("Initializing automation rules...");
    automationRules = new AutomationRules(sensorManager, actuatorManager);
    Serial.println("Automation rules initialized");
    
    // ========== 6. 启动完成 ==========
    Serial.println("\n=== BottleBoy Ready ===");
    Serial.print("Visit: http://");
    Serial.println(wifiManager.getIPAddress());
    Serial.println("========================\n");
}

/**
 * @brief Arduino loop函数 - 主循环
 */
void loop() {
    // ========== 1. WiFi维护 ==========
    wifiManager.handle();
    
    // ========== 2. Web服务器维护 ==========
    if (webServer) {
        webServer->handle();
    }
    
    // ========== 3. 获取当前时间 ==========
    unsigned long now = millis();
    
    // ========== 4. 定时读取传感器 ==========
    if (now - lastSensorUpdate >= SENSOR_UPDATE_INTERVAL) {
        lastSensorUpdate = now;
        sensorManager.readAll();
    }
    
    // ========== 5. 定时执行自动化规则 ==========
    // 每100ms执行一次自动化规则（可以调整频率）
    if (now - lastAutomation >= 100) {
        lastAutomation = now;
        if (automationRules) {
            automationRules->process();
        }
    }
    
    // ========== 6. 定时广播数据 ==========
    if (now - lastBroadcast >= SENSOR_UPDATE_INTERVAL) {
        lastBroadcast = now;
        if (webServer) {
            webServer->broadcastData();
        }
    }
    
    // ========== 7. 短暂延时 ==========
    delay(10);
}

