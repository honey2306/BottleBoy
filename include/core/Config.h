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
#define PIR_HOLD_TIME 2000                  // PIR 检测到人后的保持时间（毫秒）

// ========== 光敏传感器参数（模拟模式）==========
#define LIGHT_NIGHT_THRESHOLD 800           // 低于此 ADC 值判定为黑夜（取反后的值）
#define LIGHT_DAY_THRESHOLD   1000          // 高于此 ADC 值判定为白天（取反后的值）
// 说明：800~1000 之间为滞后区间，保持当前状态不变，避免临界值时频繁切换

// ========== 夜间自动亮灯参数 ==========
#define NIGHT_LED_BRIGHTNESS    180         // 夜间自动开灯亮度（0-255）
#define NIGHT_LED_COLOR_TEMP     20         // 夜间自动开灯色温（0-100，0=纯暖光，20=暖光）
#define NIGHT_LED_AUTO_OFF_TIME 10000       // 夜间自动开灯后的自动关闭时间（毫秒）
#define NIGHT_LED_COOLDOWN_TIME  5000       // 自动关灯后的冷却时间（毫秒），需大于 PIR 硬件脉冲周期

// ========== TFT 显示屏配置 ==========
// ILI9341 / 2.2寸 / 320×240 / 横屏
// 接线：CS→GPIO15  DC→GPIO2  RST→GPIO22  MOSI→GPIO23  SCK→GPIO18  LED→GPIO21（背光PWM控制）
#define TFT_UPDATE_INTERVAL  1000           // 屏幕刷新间隔（毫秒），每秒刷新一次

// ========== NTP 时间配置 ==========
#define NTP_UTC_OFFSET  28800               // UTC+8（中国标准时间，28800 = 8*3600 秒）
#define NTP_SYNC_INTERVAL  3600000          // NTP 强制重新同步间隔（毫秒），默认1小时

// ========== 自锁按钮配置 ==========
// 接线：按钮一端 → GPIO26，按钮另一端 → GND（使用内部上拉，无需外部电阻）
#define LATCH_BUTTON_PIN        26              ///< 自锁按钮 GPIO 引脚

// ========== 433MHz RF 配置 ==========
// 接线：接收模块 DATA → GPIO_RF_RX；发射模块 DATA → GPIO_RF_TX
// ⚠️ 注意：GPIO19 是 ESP32 VSPI MISO，与 TFT SPI 总线冲突，不可用
#define RF_RX_PIN           13              ///< 433MHz 接收模块数据引脚（GPIO13，远离 SPI/I2C）
#define RF_TX_PIN           25              ///< 433MHz 发射模块数据引脚
#define RF_LEARN_TIMEOUT_MS 15000           ///< 学习模式超时（毫秒），15秒无信号则退出
#define RF_MAX_DEVICES      20              ///< 最多存储的 RF 设备数量
#define RF_PREFS_NAMESPACE  "rfdev"         ///< Preferences 命名空间

// ========== 调试配置 ==========
#define DEBUG_SERIAL_ENABLE                 // 启用串口调试输出（注释此行可关闭所有调试输出）
#define DEBUG_SERIAL_BAUD 115200            // 串口波特率（常用115200）

#endif // CONFIG_H
