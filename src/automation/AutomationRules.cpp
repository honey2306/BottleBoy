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
    _latchBtn    = static_cast<LatchButtonSensor*>(_sensorManager.getSensor("LatchBtn"));

    // ===== 规则1: PIR → 夜间联动 LED =====
    if (_pirSensor) {
        _pirSensor->onValueChanged([this](Sensor* s, float newVal, float oldVal) {
            onPIRChanged(s, newVal, oldVal);
        });
    }

    // ===== 规则2: 自锁按钮 → 手动/自动模式切换 =====
    if (_latchBtn) {
        _latchBtn->onValueChanged([this](Sensor* s, float newVal, float oldVal) {
            onLatchButtonChanged(s, newVal, oldVal);
        });
    }

}

// ========== 规则回调实现 ==========

/**
 * @brief 规则1: PIR 状态变化 → 夜间联动 LED
 * 自锁按钮锁住期间（手动模式）不响应 PIR
 */
void AutomationRules::onPIRChanged(Sensor* sensor, float newValue, float oldValue) {
    // 手动模式或感应模式关闭时跳过 PIR 自动逻辑
    if (!_pirEnabled || (_latchBtn && _latchBtn->isLatched())) {
        broadcastSensorData();
        return;
    }

    // 冷却期内不响应 PIR（自动关灯后一段时间内，避免立刻被重新触发）
    if (_ledOffTime > 0 && millis() - _ledOffTime < NIGHT_LED_COOLDOWN_TIME) {
        broadcastSensorData();
        return;
    }

    bool isNight    = _lightSensor ? _lightSensor->isNight() : false;
    bool isDetected = _pirSensor   ? _pirSensor->isDetected() : false;
    bool isLedOn    = _ledActuator ? _ledActuator->isOn()     : false;

    // 只在灯是关着时才开灯，不重置已在计时的倒计时
    if (isNight && isDetected && !isLedOn) {
        if (_ledActuator) _ledActuator->turnOnNightMode();
    }

    broadcastSensorData();
}

/**
 * @brief 主循环更新：每帧调用持续状态规则
 */
void AutomationRules::update() {
    updateNightLED();
}

/**
 * @brief 规则2: 自锁按钮状态变化 → 手动/自动模式切换
 *
 * - 锁住（newValue=1.0）：直接开灯，屏蔽 PIR 自动模式
 * - 弹起（newValue=0.0）：关灯，恢复 PIR 自动模式
 */
void AutomationRules::onLatchButtonChanged(Sensor* sensor, float newValue, float oldValue) {
    if (!_ledActuator) return;

    if (newValue == 1.0f) {
        // 按钮锁住：直接开灯，_isAutoOn 保持 false，PIR 自动逻辑被屏蔽
        _ledActuator->turnOn();
    } else {
        // 按钮弹起：关灯，恢复由 PIR 自动控制
        _ledActuator->turnOff();
    }

    broadcastSensorData();
}
/**
 * @brief 持续状态规则：追踪 LED 自动关灯时机，记录冷却起始时间
 */
void AutomationRules::updateNightLED() {
    if (!_ledActuator) return;

    // 追踪 isAutoOn 从 true → false 的瞬间（即 DualColorLED::update() 触发了自动关灯）
    bool isAutoOn = _ledActuator->isAutoOn();
    if (_wasAutoOn && !isAutoOn) {
        _ledOffTime = millis();
    }
    _wasAutoOn = isAutoOn;
}


