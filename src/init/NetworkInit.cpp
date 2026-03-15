/**
 * @file NetworkInit.cpp
 * @brief 网络初始化模块实现
 */

#include "init/NetworkInit.h"
#include "core/Config.h"
#include "tft/TFTDisplay.h"
#include "remote/RFManager.h"
#include <Arduino.h>

namespace NetworkInit {

/**
 * @brief 初始化WiFi连接
 */
void initWiFi(WiFiManager& wifiManager) {
    // 初始化WiFi模块
    wifiManager.begin();
    
    // 尝试连接到配置的WiFi网络
    if (!wifiManager.connectWiFi(WIFI_SSID, WIFI_PASSWORD)) {
        // 连接失败，启动AP模式
        wifiManager.startAP(AP_SSID, AP_PASSWORD);
    }
}

/**
 * @brief 初始化Web服务器
 */
WebServerManager* initWebServer(SensorManager& sensorManager, ActuatorManager& actuatorManager,
                                TFTDisplay& tftDisplay, RFManager& rfManager) {
    WebServerManager* webServer = new WebServerManager(sensorManager, actuatorManager, tftDisplay, rfManager);
    webServer->begin();
    return webServer;
}

} // namespace NetworkInit
