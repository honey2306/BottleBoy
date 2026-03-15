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
 * - TFTDisplay: TFT 屏幕显示（时钟 + 温湿度）
 * - NTPTime: NTP 网络时间同步
 */

#include "actuators/base/ActuatorManager.h"
#include "automation/AutomationRules.h"
#include "core/Config.h"
#include "init/DeviceRegistry.h"
#include "init/NetworkInit.h"
#include "network/WebServer.h"
#include "network/WiFiManager.h"
#include "remote/RFManager.h"
#include "sensors/base/SensorManager.h"
#include "sensors/impl/DHT22Sensor.h"
#include "sensors/impl/LightSensor.h"
#include "actuators/impl/DualColorLED.h"
#include "tft/NTPTime.h"
#include "tft/TFTDisplay.h"
#include <Arduino.h>
#include <ArduinoOTA.h>

// ========== 全局管理器实例 ==========
WiFiManager    wifiManager;
SensorManager  sensorManager;
ActuatorManager actuatorManager;
AutomationRules automationRules(sensorManager, actuatorManager);
WebServerManager *webServer = nullptr; // 堆分配，ESP32重启前自动清理

// ========== TFT + NTP ==========
NTPTime    ntpTime(NTP_UTC_OFFSET);
TFTDisplay tftDisplay;

// ========== 433MHz RF ==========
RFManager  rfManager;

// ========== 时间控制变量 ==========
unsigned long lastSensorUpdate = 0;
unsigned long lastPIRCheck     = 0;
unsigned long lastButtonCheck  = 0;  ///< 自锁按钮轮询节流
unsigned long lastLightCheck   = 0;
unsigned long lastBroadcast    = 0;
unsigned long lastWsCleanup    = 0;
unsigned long lastTFTUpdate    = 0;
unsigned long lastNTPSync      = 0;
unsigned long lastRFBroadcast  = 0;  ///< RF 状态播报节流

/**
 * @brief 全局WebSocket广播函数（供AutomationRules回调使用）
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

  // ========== 2. 初始化 TFT 屏幕（尽早显示，不依赖 WiFi）==========
  tftDisplay.begin();
  tftDisplay.showWaiting();

  // ========== 3. 初始化WiFi ==========
  NetworkInit::initWiFi(wifiManager);

  // ========== 4. 初始化 NTP 时间（WiFi 连接后）==========
  ntpTime.begin();

  // ========== 5. 初始化设备（传感器+执行器） ==========
  DeviceRegistry::init(sensorManager, actuatorManager);

  // ========== 6. 注册自动化规则回调 ==========
  automationRules.registerCallbacks();

  // ========== 7. 初始化 RF 模块 ==========
  rfManager.begin();

  // ========== 8. 初始化Web服务器 ==========
  webServer = NetworkInit::initWebServer(sensorManager, actuatorManager, tftDisplay, rfManager);

  // ========== 9. 初始化 OTA 无线更新 ==========
  ArduinoOTA.setHostname(WIFI_HOSTNAME);
  ArduinoOTA.begin();
}

/**
 * @brief Arduino loop函数 - 主循环
 */
void loop() {
  // ========== 1. 获取当前时间 ==========
  unsigned long now = millis();

  // ========== 2. WiFi维护 + OTA ==========
  wifiManager.handle();
  ArduinoOTA.handle();

  // ========== 3. WebSocket客户端清理（每5秒一次）==========
  if (now - lastWsCleanup >= WS_CLEANUP_INTERVAL) {
    lastWsCleanup = now;
    if (webServer) {
      webServer->handle();
    }
  }

  // ========== 4. 轮询传感器（不同频率） ==========

  // 每200ms检查自锁按钮（和 PIR 相同节奏）
  if (now - lastButtonCheck >= PIR_CHECK_INTERVAL) {
    lastButtonCheck = now;
    Sensor *btnSensor = sensorManager.getSensor("LatchBtn");
    if (btnSensor) btnSensor->read();
  }

  // 每200ms检查PIR（快速响应人体感应）
  if (now - lastPIRCheck >= PIR_CHECK_INTERVAL) {
    lastPIRCheck = now;
    Sensor *pirSensor = sensorManager.getSensor("PIR");
    if (pirSensor) pirSensor->read();
  }

  // 每1000ms检查光敏（LED 自动模式期间跳过，避免 LED 光干扰传感器误判白天）
  if (now - lastLightCheck >= LIGHT_CHECK_INTERVAL) {
    lastLightCheck = now;
    DualColorLED *led = static_cast<DualColorLED*>(actuatorManager.getActuator("DualLED"));
    if (!(led && led->isAutoOn())) {
      Sensor *lightSensor = sensorManager.getSensor("Light");
      if (lightSensor) lightSensor->read();
    }
  }

  // ========== 5. 定时读取轮询型传感器（2秒间隔） ==========
  if (now - lastSensorUpdate >= SENSOR_UPDATE_INTERVAL) {
    lastSensorUpdate = now;
    for (size_t i = 0; i < sensorManager.getSensorCount(); i++) {
      Sensor *sensor = sensorManager.getSensor(i);
      if (sensor && sensor->getName() != "PIR" && sensor->getName() != "Light" && sensor->getName() != "LatchBtn") {
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

  // ========== 9. TFT 屏幕刷新（每秒一次）==========
  if (now - lastTFTUpdate >= TFT_UPDATE_INTERVAL) {
    lastTFTUpdate = now;

    // 更新 NTP 时间
    ntpTime.update();

    // 根据光敏传感器自动同步 TFT 日/夜模式（网页手动覆盖后不生效）
    // 夜灯自动模式期间跳过同步，避免 LED 发光导致光敏误判为白天
    LightSensor *light = static_cast<LightSensor*>(sensorManager.getSensor("Light"));
    DualColorLED *led  = static_cast<DualColorLED*>(actuatorManager.getActuator("DualLED"));
    if (light && !(led && led->isAutoOn())) {
      tftDisplay.setNightModeAuto(light->isNight());
    }

    if (ntpTime.isValid()) {
      // 从 DHT22 获取最新温湿度
      float temp  = NAN;
      float humid = NAN;
      DHT22Sensor *dht = static_cast<DHT22Sensor*>(sensorManager.getSensor("DHT22"));
      if (dht) {
        temp  = dht->getValue();       // 温度（摄氏度）
        humid = dht->getHumidity();    // 湿度（百分比）
      }
      tftDisplay.update(ntpTime, temp, humid);
    } else {
      tftDisplay.showWaiting();
    }
  }

  // ========== 10. NTP 定时强制同步（每小时一次）==========
  if (now - lastNTPSync >= NTP_SYNC_INTERVAL) {
    lastNTPSync = now;
    ntpTime.forceUpdate();
  }

  // ========== 11. RF 主循环（学习模式信号捕获）==========
  rfManager.loop();

  // ========== 12. RF 学习状态变化时广播更新 ==========
  RFManager::LearnState rfState = rfManager.getLearnState();
  if (rfState == RFManager::LearnState::Success || rfState == RFManager::LearnState::Timeout) {
    // 节流：同一结果只广播一次
    if (now - lastRFBroadcast >= 500) {
      lastRFBroadcast = now;
      if (webServer) webServer->broadcastData();  // 先广播（此时 state 还是 success/timeout）
      rfManager.resetLearnState();                // 广播完再重置为 idle
    }
  }
}
