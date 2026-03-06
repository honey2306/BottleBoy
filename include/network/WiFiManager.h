/**
 * @file WiFiManager.h
 * @brief WiFi 连接管理类
 */

#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <WiFi.h>
#include "core/Config.h"

/**
 * @class WiFiManager
 * @brief 管理 WiFi 连接，支持 STA 和 AP 模式
 */
class WiFiManager {
public:
    /**
     * @brief 构造函数
     */
    WiFiManager();

    /**
     * @brief 初始化 WiFi
     * @return true 初始化成功，false 失败
     */
    bool begin();

    /**
     * @brief 连接到 WiFi（STA模式）
     * @param ssid WiFi SSID
     * @param password WiFi 密码
     * @param timeout 超时时间（毫秒）
     * @return true 连接成功，false 失败
     */
    bool connectWiFi(const char* ssid, const char* password, uint32_t timeout = WIFI_CONNECT_TIMEOUT);

    /**
     * @brief 启动 AP 模式
     * @param ssid AP SSID
     * @param password AP 密码
     * @return true 启动成功，false 失败
     */
    bool startAP(const char* ssid, const char* password);

    /**
     * @brief 检查是否已连接
     * @return true 已连接，false 未连接
     */
    bool isConnected();

    /**
     * @brief 获取 IP 地址
     * @return IP 地址字符串
     */
    String getIPAddress();

    /**
     * @brief 获取 MAC 地址
     * @return MAC 地址字符串
     */
    String getMACAddress();

    /**
     * @brief 获取信号强度（RSSI）
     * @return 信号强度值
     */
    int getRSSI();

    /**
     * @brief 处理 WiFi 事件（在主循环中调用）
     */
    void handle();

private:
    bool _isAPMode;
    unsigned long _lastReconnectAttempt;
    static const uint32_t RECONNECT_INTERVAL = 30000; // 30秒重连间隔
};

#endif // WIFI_MANAGER_H
