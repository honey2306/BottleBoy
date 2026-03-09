/**
 * @file AutomationRules.cpp
 * @brief 自动化规则管理实现
 */

#include "automation/AutomationRules.h"
#include "core/Config.h"
#include <Arduino.h>

// 外部 WebSocket 广播函数（在 main.cpp 中定义）
extern void broadcastSensorData();

/**
 * @brief 构造函数
 */
AutomationRules::AutomationRules(SensorManager& sensorManager, ActuatorManager& actuatorManager)
    : _sensorManager(sensorManager), _actuatorManager(actuatorManager) {
}

/**
 * @brief 注册所有传感器事件回调
 */
void AutomationRules::registerCallbacks() {
    // 缓存传感器指针
    _pirSensor   = static_cast<PIRSensor*>(_sensorManager.getSensor("PIR"));
    _lightSensor = static_cast<LightSensor*>(_sensorManager.getSensor("Light"));
    _ledActuator = static_cast<DualColorLED*>(_actuatorManager.getActuator("DualLED"));

    // ===== 规则1: PIR → 夜间联动 LED =====
    if (_pirSensor) {
        _pirSensor->onValueChanged([this](Sensor* s, float newVal, float oldVal) {
            onPIRChanged(s, newVal, oldVal);
        });
    }

}

// ========== 规则回调实现 ==========

/**
 * @brief 规则1: PIR 状态变化 → 夜间联动 LED
 */
void AutomationRules::onPIRChanged(Sensor* sensor, float newValue, float oldValue) {
    bool isNight    = _lightSensor ? _lightSensor->isNight() : false;
    bool isDetected = _pirSensor ? _pirSensor->isDetected() : false;

    if (isNight && isDetected) {
        if (_ledActuator) _ledActuator->turnOnNightMode();
    }

    broadcastSensorData();
}

/**
 * @brief 主循环更新：依次调用各持续状态规则
 */
void AutomationRules::update() {
    updateNightLED();
}

/**
 * @brief 持续状态规则：夜间有人持续时重置 LED 倒计时
 */
void AutomationRules::updateNightLED() {
    if (!_ledActuator || !_ledActuator->isAutoOn()) return;

    bool isNight    = _lightSensor ? _lightSensor->isNight()  : false;
    bool isDetected = _pirSensor   ? _pirSensor->isDetected() : false;

    if (isNight && isDetected) {
        _ledActuator->turnOnNightMode();  // 重置 _autoOnTime，倒计时从头开始
    }
}


