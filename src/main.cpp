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

#include "actuators/base/ActuatorManager.h"
#include "automation/AutomationRules.h"
#include "core/Config.h"
#include "init/DeviceRegistry.h"
#include "init/NetworkInit.h"
#include "network/WebServer.h"
#include "network/WiFiManager.h"
#include "sensors/base/SensorManager.h"
#include <Arduino.h>

// ========== 全局管理器实例 ==========
WiFiManager wifiManager;
SensorManager sensorManager;
ActuatorManager actuatorManager;
AutomationRules automationRules(sensorManager, actuatorManager);
WebServerManager *webServer = nullptr; // 堆分配，ESP32重启前自动清理

// ========== 时间控制变量 ==========
unsigned long lastSensorUpdate = 0;
unsigned long lastPIRCheck = 0;
unsigned long lastLightCheck = 0;
unsigned long lastBroadcast = 0;
unsigned long lastWsCleanup = 0;

/**
 * @brief 全局WebSocket广播函数（供DeviceRegistry回调使用）
 */
void broadcastSensorData() {
  if (webServer) {
    webServer->broadcastData();
  }
}

/**
 * @brief Arduino setup函数 - 系统初始化
 */
void setup() {
  // ========== 1. 初始化串口 ==========
  Serial.begin(DEBUG_SERIAL_BAUD);
  delay(100);

  // ========== 2. 初始化WiFi ==========
  NetworkInit::initWiFi(wifiManager);

  // ========== 3. 初始化设备（传感器+执行器） ==========
  DeviceRegistry::init(sensorManager, actuatorManager);

  // ========== 4. 注册自动化规则回调 ==========
  automationRules.registerCallbacks();

  // ========== 5. 初始化Web服务器 ==========
  webServer = NetworkInit::initWebServer(sensorManager, actuatorManager);
}

/**
 * @brief Arduino loop函数 - 主循环
 */
void loop() {
  // ========== 1. 获取当前时间 ==========
  unsigned long now = millis();

  // ========== 2. WiFi维护 ==========
  wifiManager.handle();

  // ========== 3. WebSocket客户端清理（每5秒一次）==========
  if (now - lastWsCleanup >= WS_CLEANUP_INTERVAL) {
    lastWsCleanup = now;
    if (webServer) {
      webServer->handle();
    }
  }

  // ========== 4. 轮询传感器（不同频率） ==========

  // 每200ms检查PIR（快速响应人体感应）
  if (now - lastPIRCheck >= PIR_CHECK_INTERVAL) {
    lastPIRCheck = now;
    Sensor *pirSensor = sensorManager.getSensor("PIR");
    if (pirSensor) pirSensor->read();
  }

  // 每1000ms检查光敏（有5秒去抖，1秒足够）
  if (now - lastLightCheck >= LIGHT_CHECK_INTERVAL) {
    lastLightCheck = now;
    Sensor *lightSensor = sensorManager.getSensor("Light");
    if (lightSensor) lightSensor->read();
  }

  // ========== 5. 定时读取轮询型传感器（2秒间隔） ==========
  // DHT22等传感器：定时轮询读取
  if (now - lastSensorUpdate >= SENSOR_UPDATE_INTERVAL) {
    lastSensorUpdate = now;

    // 读取除PIR和Light外的所有传感器（它们已在步骤4中单独快速读取）
    for (size_t i = 0; i < sensorManager.getSensorCount(); i++) {
      Sensor *sensor = sensorManager.getSensor(i);
      if (sensor && sensor->getName() != "PIR" && sensor->getName() != "Light") {
        sensor->read();
      }
    }
  }

  // ========== 6. 定时广播数据 ==========
  if (now - lastBroadcast >= BROADCAST_INTERVAL) {
    lastBroadcast = now;
    if (webServer) {
      webServer->broadcastData();
    }
  }

  // ========== 7. 执行器定时逻辑（夜灯自动关闭等）==========
  actuatorManager.update();

  // ========== 8. 自动化规则持续状态检查（夜灯续期等）==========
  automationRules.update();
}
