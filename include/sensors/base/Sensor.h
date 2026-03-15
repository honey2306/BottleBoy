/**
 * @file Sensor.h
 * @brief 传感器抽象基类
 * 
 * 框架支持两种工作模式：
 * 1. 轮询模式：定期调用read()读取数据（默认）
 * 2. 事件模式：数据变化时通过回调通知（可选）
 */

#ifndef SENSOR_H
#define SENSOR_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <functional>

// 前置声明
class Sensor;

/**
 * @brief 传感器值变化回调函数类型
 *
 * 使用 std::function 支持 lambda（含捕获）、函数指针、成员函数绑定等任意 callable。
 *
 * @param sensor   触发回调的传感器指针
 * @param newValue 新值
 * @param oldValue 旧值
 */
using SensorValueChangedCallback = std::function<void(Sensor*, float, float)>;

/**
 * @class Sensor
 * @brief 传感器基类，定义传感器通用接口
 * 
 * 混合模式设计：
 * - 所有传感器都支持轮询模式（实现read()方法）
 * - 传感器可以选择性支持事件模式（调用notifyValueChanged()）
 * - 用户可以选择注册回调来实时响应数据变化
 */
class Sensor {
public:
    /**
     * @brief 构造函数
     * @param name 传感器名称
     * @param type 传感器类型
     */
    Sensor(const String& name, const String& type) 
        : _name(name), _type(type), _enabled(true), 
          _callback(nullptr), _lastValue(0.0f), _hasLastValue(false) {}

    /**
     * @brief 虚析构函数
     */
    virtual ~Sensor() {}

    /**
     * @brief 初始化传感器
     * @return true 初始化成功，false 失败
     */
    virtual bool begin() = 0;

    /**
     * @brief 读取传感器数据（轮询模式）
     * 
     * 子类必须实现这个方法
     * 如果支持事件模式，可以在这个方法中检测数据变化并调用notifyValueChanged()
     * 
     * @return true 读取成功，false 失败
     */
    virtual bool read() = 0;

    /**
     * @brief 获取当前传感器值
     * 
     * 子类必须实现这个方法
     * 
     * @return 当前传感器值
     */
    virtual float getValue() const = 0;

    /**
     * @brief 获取 JSON 格式的传感器数据
     * @param doc JSON 文档对象
     */
    virtual void getJSON(JsonObject& doc) = 0;

    /**
     * @brief 设置值变化回调（事件模式）
     * 
     * 注册回调后，当传感器值变化时会自动调用回调函数
     * 适用于需要实时响应的场景（如人体感应、按钮等）
     * 
     * @param callback 回调函数指针，传nullptr取消回调
     * 
     * 使用示例：
     * ```cpp
     * sensor->onValueChanged([](Sensor* s, float newVal, float oldVal) {
     *     Serial.printf("传感器 %s: %f -> %f\n", 
     *                   s->getName().c_str(), oldVal, newVal);
     * });
     * ```
     */
    void onValueChanged(SensorValueChangedCallback callback) {
        _callback = callback;
    }

    /**
     * @brief 获取传感器名称
     * @return 传感器名称
     */
    String getName() const { return _name; }

    /**
     * @brief 获取传感器类型
     * @return 传感器类型
     */
    String getType() const { return _type; }

    /**
     * @brief 启用传感器
     */
    void enable() { _enabled = true; }

    /**
     * @brief 禁用传感器
     */
    void disable() { _enabled = false; }

    /**
     * @brief 检查传感器是否启用
     * @return true 已启用，false 已禁用
     */
    bool isEnabled() const { return _enabled; }

protected:
    String _name;    ///< 传感器名称
    String _type;    ///< 传感器类型
    bool _enabled;   ///< 是否启用

    /**
     * @brief 通知值已变化（供子类调用）
     * 
     * 子类在检测到值变化时调用此方法，框架会自动触发回调
     * 只有在值真正变化时才会触发回调
     * 
     * @param newValue 新的传感器值
     * 
     * 使用示例（在子类的read()方法中）：
     * ```cpp
     * bool MyS ensor::read() {
     *     float value = readFromHardware();
     *     notifyValueChanged(value);  // 框架会自动处理回调
     *     return true;
     * }
     * ```
     */
    void notifyValueChanged(float newValue) {
        // 如果有回调且值发生变化
        if (_callback != nullptr) {
            // 第一次读取，或值发生变化
            if (!_hasLastValue || _lastValue != newValue) {
                float oldValue = _hasLastValue ? _lastValue : newValue;
                _callback(this, newValue, oldValue);
            }
        }
        _lastValue = newValue;
        _hasLastValue = true;
    }

private:
    SensorValueChangedCallback _callback;  ///< 值变化回调函数
    float _lastValue;                      ///< 上次的值（用于检测变化）
    bool _hasLastValue;                    ///< 是否有上次的值
};

#endif // SENSOR_H
