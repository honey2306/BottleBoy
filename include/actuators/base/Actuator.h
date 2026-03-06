/**
 * @file Actuator.h
 * @brief 执行器（可控制设备）抽象基类
 * 
 * 执行器是指可以被控制的设备，如：
 * - LED灯（开关、亮度、颜色）
 * - 继电器（开关）
 * - 舵机（角度）
 * - 电机（速度）
 * 等等
 */

#ifndef ACTUATOR_H
#define ACTUATOR_H

#include <Arduino.h>
#include <ArduinoJson.h>

/**
 * @class Actuator
 * @brief 执行器基类，定义可控制设备的通用接口
 */
class Actuator {
public:
    /**
     * @brief 构造函数
     * @param name 执行器名称（如"LED1"）
     * @param type 执行器类型（如"led"、"relay"等）
     */
    Actuator(const String& name, const String& type) 
        : _name(name), _type(type), _enabled(true) {}

    /**
     * @brief 虚析构函数
     */
    virtual ~Actuator() {}

    /**
     * @brief 初始化执行器
     * @return true 初始化成功，false 失败
     */
    virtual bool begin() = 0;

    /**
     * @brief 设置执行器状态（纯虚函数，子类必须实现）
     * @param value 控制值（具体含义由子类定义）
     * @return true 设置成功，false 失败
     */
    virtual bool setValue(float value) = 0;

    /**
     * @brief 获取执行器当前状态
     * @return 当前状态值
     */
    virtual float getValue() const = 0;

    /**
     * @brief 获取执行器的JSON数据
     * @param doc JSON对象引用
     */
    virtual void getJSON(JsonObject& doc) = 0;

    /**
     * @brief 从JSON设置执行器状态
     * @param doc JSON对象引用
     * @return true 设置成功，false 失败
     */
    virtual bool setFromJSON(JsonObject& doc) = 0;

    /**
     * @brief 获取执行器名称
     * @return 执行器名称
     */
    String getName() const { return _name; }

    /**
     * @brief 获取执行器类型
     * @return 执行器类型
     */
    String getType() const { return _type; }

    /**
     * @brief 启用执行器
     */
    void enable() { _enabled = true; }

    /**
     * @brief 禁用执行器
     */
    void disable() { _enabled = false; }

    /**
     * @brief 检查执行器是否启用
     * @return true 已启用，false 已禁用
     */
    bool isEnabled() const { return _enabled; }

protected:
    String _name;    ///< 执行器名称
    String _type;    ///< 执行器类型
    bool _enabled;   ///< 是否启用
};

#endif // ACTUATOR_H
