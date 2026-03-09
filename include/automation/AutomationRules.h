/**
 * @file AutomationRules.h
 * @brief 自动化规则管理 - 统一管理传感器事件驱动的联动规则
 * 
 * 职责：
 * 1. 通过 registerCallbacks() 绑定所有传感器的事件回调
 * 2. 在回调中实现传感器 → 执行器的联动逻辑
 * 
 * 扩展方式：
 * - 新增规则：在 registerCallbacks() 里绑定新传感器回调，实现对应的私有处理函数
 */

#ifndef AUTOMATION_RULES_H
#define AUTOMATION_RULES_H

#include "sensors/base/SensorManager.h"
#include "actuators/base/ActuatorManager.h"
#include "actuators/impl/DualColorLED.h"
#include "sensors/impl/PIRSensor.h"
#include "sensors/impl/LightSensor.h"

/**
 * @class AutomationRules
 * @brief 自动化规则管理器 - 事件驱动模式
 */
class AutomationRules {
public:
    /**
     * @brief 构造函数
     * @param sensorManager 传感器管理器引用
     * @param actuatorManager 执行器管理器引用
     */
    AutomationRules(SensorManager& sensorManager, ActuatorManager& actuatorManager);

    /**
     * @brief 注册所有传感器事件回调
     * 
     * 在设备初始化完成后调用，将规则逻辑绑定到传感器状态变化事件。
     * 每条规则对应一个或多个传感器的 onValueChanged 回调。
     */
    void registerCallbacks();

    /**
     * @brief 主循环更新
     * 
     * 在 loop() 每帧调用。处理事件回调无法覆盖的「持续状态」规则：
     * - 夜间 + 人一直在场 + LED 处于自动模式 → 持续重置倒计时
     */
    void update();

private:
    SensorManager& _sensorManager;       ///< 传感器管理器引用
    ActuatorManager& _actuatorManager;   ///< 执行器管理器引用

    // ========== 缓存的设备指针（registerCallbacks 时赋值）==========
    PIRSensor*    _pirSensor    = nullptr;  ///< PIR 传感器
    LightSensor*  _lightSensor  = nullptr;  ///< 光敏传感器
    DualColorLED* _ledActuator  = nullptr;  ///< 双色 LED 执行器



    // ========== 规则回调处理函数 ==========

    /**
     * @brief 规则1: PIR 状态变化 → 夜间联动 LED
     */
    void onPIRChanged(Sensor* sensor, float newValue, float oldValue);

    // ========== 持续状态规则处理函数 ==========

    /**
     * @brief 夜间有人持续时，重置 LED 倒计时
     */
    void updateNightLED();

    // TODO: 新增规则在此处添加对应的私有处理函数
    // void onTemperatureChanged(Sensor* sensor, float newValue, float oldValue);
};

#endif // AUTOMATION_RULES_H
