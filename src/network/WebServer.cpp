/**
 * @file WebServer.cpp
 * @brief Web服务器管理类实现
 * 
 * 这个类负责提供Web服务：
 * 1. HTTP服务器：提供网页和API接口
 * 2. WebSocket：实时推送传感器数据到网页
 * 3. REST API：让外部程序可以获取数据和控制设备
 */

#include "network/WebServer.h"
#include "web/WebPages.h"
#include "sensors/impl/LightSensor.h"
#include "sensors/impl/PIRSensor.h"
#include "tft/TFTDisplay.h"
#include "remote/RFManager.h"

/**
 * @brief 构造函数
 * 
 * 初始化Web服务器组件：
 * - HTTP服务器（监听80端口）
 * - WebSocket服务器（路径为/ws）
 * - 保存传感器和执行器管理器的引用
 * 
 * @param sensorManager 传感器管理器引用
 * @param actuatorManager 执行器管理器引用
 */
WebServerManager::WebServerManager(SensorManager& sensorManager, ActuatorManager& actuatorManager,
                                   TFTDisplay& tftDisplay, RFManager& rfManager)
    : _server(WEB_SERVER_PORT),
      _ws(WEBSOCKET_PATH),
      _sensorManager(sensorManager),
      _actuatorManager(actuatorManager),
      _tftDisplay(tftDisplay),
      _rfManager(rfManager) {
}

/**
 * @brief 析构函数
 */
WebServerManager::~WebServerManager() {
}

/**
 * @brief 初始化Web服务器
 * 
 * 设置WebSocket回调、路由规则，然后启动服务器
 * 
 * @return true表示启动成功
 */
bool WebServerManager::begin() {
    // ========== 1. 设置WebSocket事件处理函数 ==========
    // 使用Lambda表达式将成员函数绑定为回调函数
    _ws.onEvent([this](AsyncWebSocket* server, AsyncWebSocketClient* client, 
                       AwsEventType type, void* arg, uint8_t* data, size_t len) {
        // 调用自己的事件处理函数
        this->onWebSocketEvent(server, client, type, arg, data, len);
    });
    
    // ========== 2. 将WebSocket添加到HTTP服务器 ==========
    _server.addHandler(&_ws);
    
    // ========== 3. 设置HTTP路由 ==========
    setupRoutes();
    
    // ========== 4. 启动HTTP服务器 ==========
    _server.begin();
    
    return true;
}

/**
 * @brief 设置HTTP路由规则
 * 
 * 定义各个URL的处理逻辑：
 * - GET /               返回网页界面
 * - GET /api/data       返回所有数据（传感器+执行器）
 * - GET /api/sensors    返回传感器数据（JSON）
 * - GET /api/actuators  返回执行器数据（JSON）
 * - GET /api/system     返回系统信息（JSON）
 * - POST /api/actuator  控制执行器（JSON）
 */
void WebServerManager::setupRoutes() {
    // ========== 路由1: 主页 ==========
    _server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
        request->send(200, "text/html", HTML_INDEX);
    });

    // ========== 路由2: 获取所有数据API ==========
    // 访问 http://设备IP/api/data 返回传感器+执行器数据
    _server.on("/api/data", HTTP_GET, [this](AsyncWebServerRequest* request) {
        request->send(200, "application/json", getAllDataJSON());
    });

    // ========== 路由3: 获取传感器数据API ==========
    _server.on("/api/sensors", HTTP_GET, [this](AsyncWebServerRequest* request) {
        request->send(200, "application/json", getSensorDataJSON());
    });
    
    // ========== 路由4: 获取执行器数据API ==========
    _server.on("/api/actuators", HTTP_GET, [this](AsyncWebServerRequest* request) {
        request->send(200, "application/json", getActuatorDataJSON());
    });

    // ========== 路由5: 获取系统信息API ==========
    _server.on("/api/system", HTTP_GET, [this](AsyncWebServerRequest* request) {
        request->send(200, "application/json", getSystemInfoJSON());
    });

    // ========== 路由6: 控制执行器API ==========
    // POST到 http://设备IP/api/actuator 可以控制执行器
    // 请求体示例：{"name": "LED1", "value": 255}
    // 或：{"name": "LED1", "brightness": 128}
    // 或：{"name": "LED1", "state": "on"}
    _server.on("/api/actuator", HTTP_POST, 
        [](AsyncWebServerRequest* request) {},
        NULL,
        [this](AsyncWebServerRequest* request, uint8_t* data, size_t len, size_t index, size_t total) {
            // 解析JSON数据
            JsonDocument doc;
            DeserializationError error = deserializeJson(doc, (const char*)data, len);
            
            if (error) {
                request->send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
                return;
            }

            // 获取执行器名称
            String name = doc["name"] | "";
            
            if (name.isEmpty()) {
                request->send(400, "application/json", "{\"error\":\"Missing name\"}");
                return;
            }
            
            // 查找执行器
            Actuator* actuator = _actuatorManager.getActuator(name);
            if (!actuator) {
                request->send(404, "application/json", "{\"error\":\"Actuator not found\"}");
                return;
            }
            
            // 从JSON设置执行器状态
            JsonObject obj = doc.as<JsonObject>();
            if (actuator->setFromJSON(obj)) {
                // 广播更新后的数据
                broadcastData();
                request->send(200, "application/json", "{\"status\":\"ok\"}");
            } else {
                request->send(400, "application/json", "{\"error\":\"Failed to set value\"}");
            }
    });

    // ========== 404处理 ==========
    _server.onNotFound([](AsyncWebServerRequest* request) {
        request->send(404, "text/plain", "Not found");
    });
}

/**
 * @brief 处理WebSocket事件
 * 
 * 当WebSocket有事件发生时（连接、断开、收到消息等），这个函数会被调用
 * 
 * @param server WebSocket服务器指针
 * @param client WebSocket客户端指针
 * @param type 事件类型（连接、断开、数据等）
 * @param arg 参数指针
 * @param data 数据指针
 * @param len 数据长度
 */
void WebServerManager::onWebSocketEvent(AsyncWebSocket* server, AsyncWebSocketClient* client,
                                       AwsEventType type, void* arg, uint8_t* data, size_t len) {
    switch (type) {
        // ===== 客户端连接事件 =====
        case WS_EVT_CONNECT:
            client->text(getAllDataJSON());
            break;
            
        // ===== 客户端断开事件 =====
        case WS_EVT_DISCONNECT:
            // 客户端断开连接，不需要特殊处理
            break;
            
        // ===== 收到数据事件 =====
        case WS_EVT_DATA:
            {
                AwsFrameInfo* info = (AwsFrameInfo*)arg;
                if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
                    // 解析JSON命令
                    data[len] = 0;  // 确保字符串终止

                    JsonDocument doc;
                    DeserializationError error = deserializeJson(doc, (const char*)data, len);

                    if (!error && doc["cmd"].is<String>()) {
                        String cmd = doc["cmd"].as<String>();

                        if (cmd == "control" && doc["name"].is<String>()) {
                            String name = doc["name"].as<String>();
                            Actuator* actuator = _actuatorManager.getActuator(name);

                            if (actuator) {
                                JsonObject obj = doc.as<JsonObject>();
                                actuator->setFromJSON(obj);
                                broadcastData();
                            }
                        }
                        else if (cmd == "tft") {
                            // autoMode: 解除手动覆盖，恢复光敏自动
                            if (!doc["autoMode"].isNull() && doc["autoMode"].as<bool>()) {
                                _tftDisplay.setNightModeAutomatic();
                            }
                            if (!doc["state"].isNull()) {
                                bool on = doc["state"].as<String>() == "on";
                                _tftDisplay.setOn(on);
                            }
                            if (!doc["brightness"].isNull()) {
                                int bri = doc["brightness"].as<int>();
                                _tftDisplay.setBrightness((uint8_t)bri);
                            }
                            if (!doc["nightMode"].isNull()) {
                                bool night = doc["nightMode"].as<bool>();
                                _tftDisplay.setNightMode(night);
                            }
                            // 广播最新状态
                            broadcastData();
                        }
                        // ─── RF 相关命令 ───
                        else if (cmd == "rfLearn") {
                            // 开始学习：{"cmd":"rfLearn", "name":"设备名"}
                            if (doc["name"].is<String>()) {
                                _rfManager.startLearn(doc["name"].as<String>());
                            }
                            broadcastData();
                        }
                        else if (cmd == "rfCancel") {
                            // 取消学习：{"cmd":"rfCancel"}
                            _rfManager.cancelLearn();
                            broadcastData();
                        }
                        else if (cmd == "rfSend") {
                            // 发射：{"cmd":"rfSend", "name":"设备名"}
                            if (doc["name"].is<String>()) {
                                _rfManager.transmit(doc["name"].as<String>());
                            }
                        }
                        else if (cmd == "rfDelete") {
                            // 删除设备：{"cmd":"rfDelete", "name":"设备名"}
                            if (doc["name"].is<String>()) {
                                _rfManager.removeDevice(doc["name"].as<String>());
                            }
                            broadcastData();
                        }
                        else if (cmd == "getData") {
                            client->text(getAllDataJSON());
                        }
                    }
                }
            }
            break;
            
        // ===== PONG事件 =====
        case WS_EVT_PONG:
            // 心跳包响应，不需要处理
            break;
            
        // ===== 错误事件 =====
        case WS_EVT_ERROR:
            // WebSocket错误，不需要处理
            break;
    }
}

/**
 * @brief 处理WebSocket维护任务（需要在主循环中调用）
 * 
 * 清理断开的WebSocket客户端连接，释放资源
 */
void WebServerManager::handle() {
    _ws.cleanupClients();  // 清理已断开的客户端
}

/**
 * @brief 广播所有数据到所有WebSocket客户端
 * 
 * 获取最新的传感器和执行器数据，并发送给所有连接的网页客户端
 * 这样网页上的数据就能实时更新
 */
void WebServerManager::broadcastData() {
    // 向所有连接的客户端发送所有数据（JSON格式）
    _ws.textAll(getAllDataJSON());
}

/**
 * @brief 获取系统信息的JSON字符串
 * 
 * 收集ESP32的系统信息，转换成JSON格式
 * 
 * JSON格式示例：
 * {
 *   "uptime": 3600,           // 运行时间（秒）
 *   "freeHeap": 245760,       // 可用内存（字节）
 *   "chipModel": "ESP32-S3",  // 芯片型号
 *   "cpuFreq": 240            // CPU频率（MHz）
 * }
 * 
 * @return JSON格式的系统信息字符串
 */
String WebServerManager::getSystemInfoJSON() {
    JsonDocument doc;
    JsonObject root = doc.to<JsonObject>();
    
    root["uptime"] = millis() / 1000;
    root["freeHeap"] = ESP.getFreeHeap();
    root["chipModel"] = ESP.getChipModel();
    root["cpuFreq"] = ESP.getCpuFreqMHz();
    
    // RF 状态
    JsonObject rf = root["rf"].to<JsonObject>();
    _rfManager.getLearnStatusJSON(rf);
    JsonArray rfDevices = rf["devices"].to<JsonArray>();
    _rfManager.getDevicesJSON(rfDevices);

    String output;
    serializeJson(root, output);
    return output;
}

/**
 * @brief 获取传感器数据的JSON字符串
 */
String WebServerManager::getSensorDataJSON() {
    JsonDocument doc;
    JsonObject root = doc.to<JsonObject>();
    
    root["timestamp"] = millis();
    _sensorManager.getAllJSON(root);
    
    String output;
    serializeJson(root, output);
    return output;
}

/**
 * @brief 获取执行器数据的JSON字符串
 * 
 * JSON格式示例：
 * {
 *   "timestamp": 123456,
 *   "actuators": [
 *     {"name": "LED1", "type": "led", "state": "on", "brightness": 255}
 *   ]
 * }
 */
String WebServerManager::getActuatorDataJSON() {
    JsonDocument doc;
    JsonObject root = doc.to<JsonObject>();
    
    root["timestamp"] = millis();
    _actuatorManager.getAllJSON(root);
    
    String output;
    serializeJson(root, output);
    return output;
}

/**
 * @brief 获取所有数据的JSON字符串（传感器+执行器）
 * 
 * JSON格式示例：
 * {
 *   "timestamp": 123456,
 *   "sensors": [...],
 *   "actuators": [...]
 * }
 */
String WebServerManager::getAllDataJSON() {
    JsonDocument doc;
    JsonObject root = doc.to<JsonObject>();

    root["timestamp"] = millis();
    root["uptime"]    = millis() / 1000;
    root["freeHeap"]  = ESP.getFreeHeap();
    root["chipModel"] = ESP.getChipModel();
    root["cpuFreq"]   = ESP.getCpuFreqMHz();

    _sensorManager.getAllJSON(root);
    _actuatorManager.getAllJSON(root);

    // TFT 屏幕状态
    JsonObject tft = root["tft"].to<JsonObject>();
    tft["on"]         = _tftDisplay.isOn();
    tft["brightness"] = _tftDisplay.getBrightness();
    tft["nightMode"]  = _tftDisplay.isNightMode();

    // 自动化规则状态（使用语义接口，不依赖原始 getValue()）
    LightSensor* lightSensor = static_cast<LightSensor*>(_sensorManager.getSensor("Light"));
    PIRSensor*   pirSensor   = static_cast<PIRSensor*>(_sensorManager.getSensor("PIR"));

    bool isNight    = lightSensor ? lightSensor->isNight()    : false;
    bool isDetected = pirSensor   ? pirSensor->isDetected()   : false;

    JsonObject automation = root["automation"].to<JsonObject>();
    automation["mode"]         = isNight ? "night" : "day";
    automation["modeText"]     = isNight ? "黑夜模式" : "白天模式";
    automation["pirDetected"]  = isDetected;

    if (isNight) {
        automation["action"] = isDetected ? "检测到人 → 应开灯" : "无人 → 应关灯";
    } else {
        automation["action"] = "白天 → 自动关灯";
    }

    // RF 状态和设备列表
    JsonObject rf = root["rf"].to<JsonObject>();
    _rfManager.getLearnStatusJSON(rf);
    JsonArray rfDevices = rf["devices"].to<JsonArray>();
    _rfManager.getDevicesJSON(rfDevices);

    String output;
    serializeJson(root, output);
    return output;
}
