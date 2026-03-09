/**
 * @file DeviceRegistry.cpp
 * @brief 设备注册器实现 - 只负责创建并注册传感器和执行器
 */

#include "init/DeviceRegistry.h"
#include "sensors/impl/DHT22Sensor.h"
#include "sensors/impl/PIRSensor.h"
#include "sensors/impl/LightSensor.h"
#include "actuators/impl/DualColorLED.h"
#include "core/Config.h"
#include <Arduino.h>

namespace DeviceRegistry {

/**
 * @brief 初始化并注册所有设备
 */
void init(SensorManager& sensorManager, ActuatorManager& actuatorManager) {
    // ========== 注册传感器 ==========

    // ===== DHT22 温湿度传感器 =====
    const uint8_t DHT_PIN = 4;
    sensorManager.addSensor(new DHT22Sensor("DHT22", DHT_PIN));

    // ===== PIR 人体感应传感器 =====
    const uint8_t PIR_PIN = 5;
    sensorManager.addSensor(new PIRSensor("PIR", PIR_PIN, PIR_HOLD_TIME));

    // ===== 光敏传感器（模拟模式）=====
    // 连接说明：
    // - AO引脚 → ADC GPIO6；VCC → 3.3V；GND → GND；DO → 不连接
    // - 阈值由 Config.h 中 LIGHT_NIGHT_THRESHOLD / LIGHT_DAY_THRESHOLD 控制
    const uint8_t LIGHT_AO_PIN = 6;
    sensorManager.addSensor(new LightSensor("Light", LIGHT_AO_PIN, LIGHT_NIGHT_THRESHOLD, LIGHT_DAY_THRESHOLD));

    // 初始化所有传感器
    sensorManager.begin();

    // ========== 注册执行器 ==========

    // ===== 双色调温 LED =====
    const uint8_t LED_WHITE_PIN = 1;
    const uint8_t LED_WARM_PIN  = 2;
    actuatorManager.addActuator(new DualColorLED("DualLED", LED_WHITE_PIN, LED_WARM_PIN));

    // TODO: 添加其他执行器
    // actuatorManager.addActuator(new Relay("Relay1", 17));

    // 初始化所有执行器
    actuatorManager.begin();
}

} // namespace DeviceRegistry
