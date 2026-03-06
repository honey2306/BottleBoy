/**
 * @file NetworkInit.h
 * @brief 网络初始化模块
 * 
 * 负责WiFi和Web服务器的初始化
 */

#ifndef NETWORK_INIT_H
#define NETWORK_INIT_H

#include "network/WiFiManager.h"
#include "network/WebServer.h"
#include "sensors/base/SensorManager.h"
#include "actuators/base/ActuatorManager.h"

/**
 * @namespace NetworkInit
 * @brief 网络初始化命名空间
 */
namespace NetworkInit {
    /**
     * @brief 初始化WiFi连接
     * 
     * 尝试连接到配置的WiFi，失败则启动AP模式
     * 
     * @param wifiManager WiFi管理器引用
     */
    void initWiFi(WiFiManager& wifiManager);
    
    /**
     * @brief 初始化Web服务器
     * 
     * 创建并启动Web服务器
     * 
     * @param sensorManager 传感器管理器引用
     * @param actuatorManager 执行器管理器引用
     * @return Web服务器指针
     */
    WebServerManager* initWebServer(SensorManager& sensorManager, ActuatorManager& actuatorManager);
}

#endif // NETWORK_INIT_H
