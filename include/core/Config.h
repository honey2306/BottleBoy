/**
 * @file Config.h
 * @brief 项目全局配置文件
 * 
 * 这个文件集中管理所有的配置参数，方便统一修改
 * 使用前需要修改WiFi的SSID和密码
 */

#ifndef CONFIG_H
#define CONFIG_H

// ========== WiFi STA模式配置 ==========
// STA模式：ESP32作为客户端连接到路由器
#define WIFI_SSID "your_wifi_ssid"          // 修改为你的WiFi名称
#define WIFI_PASSWORD "your_wifi_password"  // 修改为你的WiFi密码
#define WIFI_HOSTNAME "BottleBoy"           // 设备在路由器上显示的名称
#define WIFI_CONNECT_TIMEOUT 10000          // 连接超时时间（毫秒），10秒

// ========== WiFi AP模式配置 ==========
// AP模式：ESP32创建热点，手机直接连接
// 当无法连接到路由器时，会自动启动AP模式
#define AP_SSID "BottleBoy-AP"              // 热点名称（会在手机WiFi列表中显示）
#define AP_PASSWORD "12345678"              // 热点密码（至少8位）

// ========== Web服务器配置 ==========
#define WEB_SERVER_PORT 80                  // HTTP服务器端口（80是标准HTTP端口）
#define WEBSOCKET_PATH "/ws"                // WebSocket路径（网页通过这个路径连接）

// ========== 传感器配置 ==========
#define SENSOR_UPDATE_INTERVAL 2000         // 传感器读取间隔（毫秒），每2秒读取一次

// ========== 调试配置 ==========
#define DEBUG_SERIAL_ENABLE true            // 是否启用串口调试输出
#define DEBUG_SERIAL_BAUD 115200            // 串口波特率（常用115200）

#endif // CONFIG_H
