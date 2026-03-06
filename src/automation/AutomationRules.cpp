/**
 * @file AutomationRules.cpp
 * @brief 自动化规则管理实现
 */

#include "automation/AutomationRules.h"
#include <Arduino.h>

/**
 * @brief 构造函数
 */
AutomationRules::AutomationRules(SensorManager& sensorManager, ActuatorManager& actuatorManager)
    : _sensorManager(sensorManager), _actuatorManager(actuatorManager) {
}

/**
 * @brief 处理所有自动化规则
 * 
 * 依次检查并执行所有规则
 * 可以根据需要启用/禁用某些规则
 */
void AutomationRules::process() {
    // 执行各个规则
    rulePIRToLED();           // 人体感应控制LED
    // ruleTemperatureToFan();   // 温度控制风扇（暂时注释）
    // ruleLightToLED();         // 光线控制LED（暂时注释）
    
    // TODO: 添加更多规则...
}

// ========== 具体规则实现 ==========

/**
 * @brief 规则1: 人体感应自动开灯
 * 
 * 实现逻辑：
 * 1. 获取人体感应传感器（PIRSensor）
 * 2. 检查是否检测到人（value > 0）
 * 3. 获取LED执行器（LED1）
 * 4. 检测到人 → 开灯（255），否则 → 关灯（0）
 */
void AutomationRules::rulePIRToLED() {
    // ========== 1. 获取人体感应传感器 ==========
    Sensor* pirSensor = _sensorManager.getSensor("PIRSensor");
    if (!pirSensor || !pirSensor->isEnabled()) {
        return;  // 传感器不存在或已禁用
    }

    // ========== 2. 获取LED执行器 ==========
    Actuator* led = _actuatorManager.getActuator("LED1");
    if (!led || !led->isEnabled()) {
        return;  // 执行器不存在或已禁用
    }

    // ========== 3. 读取传感器数值 ==========
    float pirValue = pirSensor->getValue();

    // ========== 4. 执行控制逻辑 ==========
    if (pirValue > 0) {
        // 检测到人，开灯
        led->setValue(255);  // 最大亮度
        // Serial.println("[Automation] PIR detected motion, LED ON");
    } else {
        // 没检测到人，关灯
        led->setValue(0);    // 关闭
        // Serial.println("[Automation] PIR no motion, LED OFF");
    }
}

/**
 * @brief 规则2: 温度控制风扇（示例）
 * 
 * 这是一个示例规则，演示如何根据温度控制风扇
 * 需要添加相应的传感器和执行器才能使用
 */
void AutomationRules::ruleTemperatureToFan() {
    // 获取温度传感器
    Sensor* tempSensor = _sensorManager.getSensor("Temperature");
    if (!tempSensor || !tempSensor->isEnabled()) {
        return;
    }

    // 获取风扇执行器（假设名为"Fan1"）
    Actuator* fan = _actuatorManager.getActuator("Fan1");
    if (!fan || !fan->isEnabled()) {
        return;
    }

    // 读取温度
    float temperature = tempSensor->getValue();

    // 控制逻辑（带滞后，避免频繁开关）
    if (temperature > 30.0) {
        // 温度高，开风扇
        fan->setValue(255);
    } else if (temperature < 25.0) {
        // 温度低，关风扇
        fan->setValue(0);
    }
    // 25-30度之间保持当前状态（滞后）
}

/**
 * @brief 规则3: 光线传感器自动开灯（示例）
 * 
 * 根据光线强度自动控制LED亮度
 */
void AutomationRules::ruleLightToLED() {
    // 获取光线传感器
    Sensor* lightSensor = _sensorManager.getSensor("LightSensor");
    if (!lightSensor || !lightSensor->isEnabled()) {
        return;
    }

    // 获取LED
    Actuator* led = _actuatorManager.getActuator("LED2");
    if (!led || !led->isEnabled()) {
        return;
    }

    // 读取光线强度
    float lightLevel = lightSensor->getValue();

    // 控制逻辑
    if (lightLevel < 300) {
        // 光线暗，开灯
        led->setValue(255);
    } else if (lightLevel > 500) {
        // 光线亮，关灯
        led->setValue(0);
    }
    // 300-500之间保持当前状态
}
