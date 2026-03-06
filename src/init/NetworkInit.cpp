/**
 * @file NetworkInit.cpp
 * @brief 网络初始化模块实现
 */

#include "init/NetworkInit.h"
#include "core/Config.h"
#include <Arduino.h>

namespace NetworkInit {

/**
 * @brief 初始化WiFi连接
 */
void initWiFi(WiFiManager& wifiManager) {
    Serial.println("Initializing WiFi...");
    
    // 初始化WiFi模块
    wifiManager.begin();
    
    // 尝试连接到配置的WiFi网络
    if (wifiManager.connectWiFi(WIFI_SSID, WIFI_PASSWORD)) {
        // 连接成功
        Serial.println("WiFi connected!");
        Serial.print("IP Address: ");
        Serial.println(wifiManager.getIPAddress());
    } else {
        // 连接失败，启动AP模式
        Serial.println("WiFi connection failed, starting AP mode...");
        wifiManager.startAP(AP_SSID, AP_PASSWORD);
        Serial.print("AP IP Address: ");
        Serial.println(wifiManager.getIPAddress());
    }
}

/**
 * @brief 初始化Web服务器
 */
WebServerManager* initWebServer(SensorManager& sensorManager, ActuatorManager& actuatorManager) {
    Serial.println("Starting Web Server...");
    
    // 创建Web服务器实例
    WebServerManager* webServer = new WebServerManager(sensorManager, actuatorManager);
    
    // 启动服务器
    webServer->begin();
    
    Serial.println("Web Server started on port 80");
    
    return webServer;
}

} // namespace NetworkInit
