/**
 * @file WiFiManager.cpp
 * @brief WiFi 连接管理类实现
 * 
 * 这个类负责管理ESP32的WiFi连接，支持两种模式：
 * 1. STA模式（Station）：作为客户端连接到现有的WiFi路由器
 * 2. AP模式（Access Point）：作为热点，让其他设备连接到ESP32
 */

#include "network/WiFiManager.h"

/**
 * @brief 构造函数
 * 
 * 初始化成员变量：
 * - _isAPMode: 是否处于AP模式（默认false）
 * - _lastReconnectAttempt: 上次尝试重连的时间（默认0）
 */
WiFiManager::WiFiManager() 
    : _isAPMode(false), _lastReconnectAttempt(0) {
}

/**
 * @brief 初始化WiFi模块
 * 
 * 设置WiFi工作模式为STA（客户端模式），并设置设备的主机名
 * @return 总是返回true（初始化不会失败）
 */
bool WiFiManager::begin() {
    // 设置WiFi为STA模式（客户端模式，用于连接路由器）
    WiFi.mode(WIFI_STA);
    
    // 设置设备的主机名，在路由器上可以看到这个名字
    WiFi.setHostname(WIFI_HOSTNAME);
    
    return true;
}

/**
 * @brief 连接到WiFi网络（STA模式）
 * 
 * 尝试连接到指定的WiFi网络，如果当前是AP模式，会先切换到STA模式
 * 
 * @param ssid WiFi网络名称（路由器名称）
 * @param password WiFi密码
 * @param timeout 超时时间（毫秒），超过这个时间还没连上就放弃
 * @return true表示连接成功，false表示连接失败
 */
bool WiFiManager::connectWiFi(const char* ssid, const char* password, uint32_t timeout) {
    // 如果当前是AP模式，先切换到STA模式
    if (_isAPMode) {
        WiFi.mode(WIFI_STA);
        _isAPMode = false;
    }

    // 开始连接WiFi
    WiFi.begin(ssid, password);
    
    // 记录开始时间
    unsigned long startTime = millis();
    
    // 循环等待连接成功或超时
    // WL_CONNECTED表示已连接
    while (WiFi.status() != WL_CONNECTED && (millis() - startTime) < timeout) {
        delay(500);  // 每500ms检查一次
    }

    // 检查是否连接成功
    if (WiFi.status() == WL_CONNECTED) {
        return true;  // 连接成功
    }
    
    return false;  // 连接失败（超时）
}

/**
 * @brief 启动AP模式（热点模式）
 * 
 * 让ESP32变成一个WiFi热点，其他设备（如手机）可以连接到它
 * 适用于无法连接到路由器的情况
 * 
 * @param ssid 热点名称（会在手机WiFi列表中显示）
 * @param password 热点密码（至少8位）
 * @return true表示启动成功，false表示启动失败
 */
bool WiFiManager::startAP(const char* ssid, const char* password) {
    // 设置WiFi为AP模式（热点模式）
    WiFi.mode(WIFI_AP);
    _isAPMode = true;
    
    // 启动热点
    bool success = WiFi.softAP(ssid, password);
    
    if (success) {
        delay(100);  // 等待热点稳定
    }
    
    return success;
}

/**
 * @brief 检查是否已连接
 * 
 * 根据当前模式检查连接状态：
 * - STA模式：检查是否连接到路由器
 * - AP模式：检查是否有设备连接到热点
 * 
 * @return true表示已连接，false表示未连接
 */
bool WiFiManager::isConnected() {
    if (_isAPMode) {
        // AP模式：检查是否有设备连接到热点
        // softAPgetStationNum()返回连接到热点的设备数量
        return WiFi.softAPgetStationNum() > 0;
    }
    // STA模式：检查是否连接到路由器
    return WiFi.status() == WL_CONNECTED;
}

/**
 * @brief 获取IP地址
 * 
 * 根据当前模式返回相应的IP地址：
 * - STA模式：返回路由器分配的IP地址
 * - AP模式：返回热点的IP地址（通常是192.168.4.1）
 * 
 * @return IP地址字符串（如"192.168.1.100"）
 */
String WiFiManager::getIPAddress() {
    if (_isAPMode) {
        // AP模式：返回热点的IP地址
        return WiFi.softAPIP().toString();
    }
    // STA模式：返回分配的IP地址
    return WiFi.localIP().toString();
}

/**
 * @brief 获取MAC地址
 * 
 * MAC地址是设备的唯一硬件地址，类似于设备的"身份证号"
 * 
 * @return MAC地址字符串（如"AA:BB:CC:DD:EE:FF"）
 */
String WiFiManager::getMACAddress() {
    return WiFi.macAddress();
}

/**
 * @brief 获取WiFi信号强度（RSSI）
 * 
 * RSSI (Received Signal Strength Indicator) 表示WiFi信号强度
 * 数值越大越好（负数，越接近0越好）
 * 
 * @return 信号强度值（dBm）
 *         - AP模式返回0（没有意义）
 *         - STA模式返回实际信号强度（-30最强，-90最弱）
 */
int WiFiManager::getRSSI() {
    if (_isAPMode) {
        return 0;  // AP模式没有信号强度的概念
    }
    return WiFi.RSSI();
}

/**
 * @brief 处理WiFi事件（需要在主循环中定期调用）
 * 
 * 这个函数负责：
 * 1. 检测WiFi断线
 * 2. 自动尝试重连（每30秒尝试一次）
 * 
 * 必须在loop()函数中定期调用才能实现自动重连功能
 */
void WiFiManager::handle() {
    // AP模式不需要重连
    if (_isAPMode) {
        return;
    }

    // 检查是否断开连接
    if (WiFi.status() != WL_CONNECTED) {
        unsigned long now = millis();
        
        // 每隔RECONNECT_INTERVAL（30秒）尝试重连一次
        // 避免频繁重连导致资源浪费
        if (now - _lastReconnectAttempt >= RECONNECT_INTERVAL) {
            _lastReconnectAttempt = now;
            WiFi.reconnect();  // 尝试重新连接
        }
    }
}
