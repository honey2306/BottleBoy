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
#define WIFI_SSID "ChinaNet-NZct"          // 修改为你的WiFi名称
#define WIFI_PASSWORD "yxvu96dn"  // 修改为你的WiFi密码
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
#define SENSOR_UPDATE_INTERVAL 2000         // 传感器读取间隔（毫秒），每2秒读取一次（DHT22要求最小2s间隔）
#define PIR_CHECK_INTERVAL 200              // PIR检查间隔（毫秒），200ms足够快
#define LIGHT_CHECK_INTERVAL 1000           // 光敏检查间隔（毫秒），1秒足够
#define BROADCAST_INTERVAL 2000             // WebSocket广播间隔（毫秒），心跳兜底推送
#define WS_CLEANUP_INTERVAL 5000            // WebSocket断开客户端清理间隔（毫秒）

// ========== PIR 传感器参数 ==========
#define PIR_HOLD_TIME 5000                  // PIR 检测到人后的保持时间（毫秒）

// ========== 光敏传感器参数（模拟模式）==========
#define LIGHT_NIGHT_THRESHOLD 800           // 低于此 ADC 值判定为黑夜（进入黑夜阈值）
#define LIGHT_DAY_THRESHOLD   1000          // 高于此 ADC 值判定为白天（进入白天阈值）
// 说明：800~1000 之间为滞后区间，保持当前状态不变，避免临界值时频繁切换

// ========== 夜间自动亮灯参数 ==========
#define NIGHT_LED_BRIGHTNESS    180         // 夜间自动开灯亮度（0-255）
#define NIGHT_LED_COLOR_TEMP     20         // 夜间自动开灯色温（0-100，0=纯暖光，20=暖光）
#define NIGHT_LED_AUTO_OFF_TIME 10000       // 夜间自动开灯后的自动关闭时间（毫秒），默认30秒

// ========== 调试配置 ==========
#define DEBUG_SERIAL_ENABLE                 // 启用串口调试输出（注释此行可关闭所有调试输出）
#define DEBUG_SERIAL_BAUD 115200            // 串口波特率（常用115200）

#endif // CONFIG_H
