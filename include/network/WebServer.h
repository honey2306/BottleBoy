/**
 * @file WebServer.h
 * @brief Web服务器管理类
 */

#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include "sensors/base/SensorManager.h"
#include "actuators/base/ActuatorManager.h"
#include "core/Config.h"

/**
 * @class WebServerManager
 * @brief 管理 HTTP 服务器、REST API 和 WebSocket
 */
class WebServerManager {
public:
    /**
     * @brief 构造函数
     * @param sensorManager 传感器管理器引用
     * @param actuatorManager 执行器管理器引用
     */
    WebServerManager(SensorManager& sensorManager, ActuatorManager& actuatorManager);

    /**
     * @brief 析构函数
     */
    ~WebServerManager();

    /**
     * @brief 初始化 Web 服务器
     * @return true 初始化成功，false 失败
     */
    bool begin();

    /**
     * @brief 处理 WebSocket 事件（在主循环中调用）
     */
    void handle();

    /**
     * @brief 广播所有数据（传感器+执行器）到所有 WebSocket 客户端
     */
    void broadcastData();

private:
    AsyncWebServer _server;
    AsyncWebSocket _ws;
    SensorManager& _sensorManager;
    ActuatorManager& _actuatorManager;

    /**
     * @brief 设置路由
     */
    void setupRoutes();

    /**
     * @brief 处理 WebSocket 事件
     */
    void onWebSocketEvent(AsyncWebSocket* server, AsyncWebSocketClient* client, 
                          AwsEventType type, void* arg, uint8_t* data, size_t len);

    /**
     * @brief 获取系统信息 JSON
     */
    String getSystemInfoJSON();

    /**
     * @brief 获取传感器数据 JSON
     */
    String getSensorDataJSON();
    
    /**
     * @brief 获取执行器数据 JSON
     */
    String getActuatorDataJSON();
    
    /**
     * @brief 获取所有数据 JSON（传感器+执行器）
     */
    String getAllDataJSON();
};

#endif // WEB_SERVER_H
