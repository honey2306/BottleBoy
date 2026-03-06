# BottleBoy 🍼

基于ESP32-S3的物联网智能控制框架

## 📖 项目简介

BottleBoy是一个模块化的IoT框架，运行在ESP32-S3平台上，提供传感器数据采集、执行器控制、Web界面管理和自动化规则引擎。

### ✨ 主要特性

- 🔌 **模块化架构** - 传感器和执行器采用插件式设计，易于扩展
- 🌐 **Web控制界面** - 美观的Web界面，支持实时数据显示和设备控制
- 📡 **双模式通信** - 支持REST API和WebSocket实时推送
- 🤖 **自动化规则** - 内置规则引擎，支持自定义自动化逻辑
- 🔔 **事件驱动** - 支持传感器值变化回调，实时响应
- 📱 **响应式设计** - 支持手机、平板、桌面端访问

## 🛠️ 硬件支持

### 开发板
- ESP32-S3 (4D Systems ESP32-S3 Gen4 R8N16)

### 传感器
- **DHT22** - 温湿度传感器
- **PIR** - 人体红外感应传感器
- **Light Sensor** - 光敏传感器（支持白天/晚上判断）

### 执行器
- **DualColorLED** - 双色调温LED（白光+暖光）
- **LED** - 普通LED（可扩展）

## 📋 引脚配置

| 设备 | 引脚 | 说明 |
|------|------|------|
| DHT22 | GPIO4 | 温湿度传感器数据线 |
| PIR | GPIO5 | 人体感应传感器输出 |
| Light Sensor | GPIO34 | 光敏传感器模拟输入（ADC1） |
| DualLED 白光 | GPIO2 | PWM控制白光亮度 |
| DualLED 暖光 | GPIO15 | PWM控制暖光亮度 |

## 🚀 快速开始

### 1. 环境准备

#### 安装PlatformIO
```bash
# 通过VSCode安装PlatformIO扩展
# 或使用命令行安装
pip install platformio
```

### 2. 克隆项目
```bash
git clone https://github.com/honey2306/BottleBoy.git
cd BottleBoy
```

### 3. 配置WiFi

编辑 `include/core/Config.h`，修改WiFi配置：
```cpp
#define WIFI_SSID "你的WiFi名称"
#define WIFI_PASSWORD "你的WiFi密码"
```

### 4. 编译上传
```bash
# 编译
pio run

# 上传到设备
pio run --target upload

# 查看串口输出
pio device monitor
```

### 5. 访问Web界面

设备连接WiFi后，串口会显示IP地址：
```
WiFi connected! IP: 192.168.1.100
```

在浏览器中访问：`http://192.168.1.100`

## 📁 项目结构

```
BottleBoy/
├── include/
│   ├── actuators/          # 执行器
│   │   ├── base/           # 执行器基类
│   │   └── impl/           # 具体实现
│   ├── sensors/            # 传感器
│   │   ├── base/           # 传感器基类
│   │   └── impl/           # 具体实现
│   ├── network/            # 网络模块
│   ├── web/                # Web页面
│   ├── automation/         # 自动化规则
│   ├── init/               # 初始化模块
│   └── core/               # 核心配置
├── src/                    # 源代码实现
├── platformio.ini          # PlatformIO配置
└── README.md
```

## 🎮 API接口

### REST API

#### 获取所有传感器数据
```http
GET /api/sensors
```

#### 获取单个传感器数据
```http
GET /api/sensors/{name}
```

#### 获取所有执行器状态
```http
GET /api/actuators
```

#### 控制执行器
```http
POST /api/actuators/{name}
Content-Type: application/json

{
  "brightness": 200,      // 设置亮度
  "colorTemp": 50,        // 设置色温
  "state": "on"           // 开关状态
}
```

#### 系统信息
```http
GET /api/system
```

### WebSocket

连接地址：`ws://{设备IP}/ws`

实时接收传感器和执行器数据更新。

## 🔧 添加新设备

### 添加传感器

1. 在 `include/sensors/impl/` 创建传感器类：
```cpp
class MySensor : public Sensor {
    // 实现 begin(), read(), getValue(), getJSON()
};
```

2. 在 `src/init/DeviceRegistry.cpp` 注册：
```cpp
sensorManager.addSensor(new MySensor("MySensor", PIN));
```

### 添加执行器

1. 在 `include/actuators/impl/` 创建执行器类：
```cpp
class MyActuator : public Actuator {
    // 实现 begin(), setValue(), getValue(), getJSON(), setFromJSON()
};
```

2. 在 `src/init/DeviceRegistry.cpp` 注册：
```cpp
actuatorManager.addActuator(new MyActuator("MyActuator", PIN));
```

## 🤖 自动化规则

在 `src/automation/AutomationRules.cpp` 中添加自定义规则：

```cpp
void AutomationRules::processRules() {
    // 示例：晚上自动开灯
    LightSensor* light = (LightSensor*)_sensorManager.getSensor("Light");
    DualColorLED* led = (DualColorLED*)_actuatorManager.getActuator("DualLED");
    
    if (light && led) {
        if (light->isNightTime()) {
            led->setBrightness(200);
            led->setColorTemperature(20);  // 暖光
            led->turnOn();
        } else {
            led->turnOff();
        }
    }
}
```

## 📊 技术栈

- **平台**: ESP32-S3 / Arduino Framework
- **构建工具**: PlatformIO
- **Web服务器**: ESPAsyncWebServer
- **数据格式**: ArduinoJson
- **传感器库**: DHT sensor library (Adafruit)
- **前端**: 原生HTML/CSS/JavaScript
- **通信**: REST API + WebSocket

## 🎨 Web界面预览

### 传感器监控
- 实时显示温度、湿度
- 人体感应状态
- 光线强度和昼夜判断

### 执行器控制
- 亮度滑块（0-255）
- 色温调节（暖光🔥 ↔️ 冷光❄️）
- 开关控制

## 🔐 配置说明

编辑 `include/core/Config.h` 可修改：
- WiFi SSID和密码
- AP模式配置
- Web服务器端口
- 传感器读取间隔
- WebSocket推送间隔

## 📝 待办事项

- [ ] 添加MQTT支持
- [ ] 添加OTA更新功能
- [ ] 支持更多传感器类型
- [ ] 添加数据记录和图表
- [ ] 移动App支持

## 🤝 贡献

欢迎提交Issue和Pull Request！

## 📄 许可证

MIT License

## 👨‍💻 作者

honey2306

## 🙏 致谢

感谢所有开源库的作者和贡献者！
