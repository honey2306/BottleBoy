/**
 * @file DeviceRegistry.cpp
 * @brief 设备注册器实现 - 只负责创建并注册传感器和执行器
 */

#include "init/DeviceRegistry.h"
#include "sensors/impl/DHT22Sensor.h"
#include "sensors/impl/PIRSensor.h"
#include "sensors/impl/LightSensor.h"
#include "sensors/impl/LatchButtonSensor.h"
#include "actuators/impl/DualColorLED.h"
#include "core/Config.h"
#include <Arduino.h>

namespace DeviceRegistry {

/**
 * @brief 初始化并注册所有设备
 */
void init(SensorManager& sensorManager, ActuatorManager& actuatorManager) {
    // ========== 注册传感器 ==========
    // ESP32 引脚分配：
    //   GPIO4   → DHT22 数据引脚
    //   GPIO5   → PIR 信号引脚
    //   GPIO34  → 光敏传感器 AO（ADC1_CH6，仅输入）
    //   GPIO16  → LED 白光 PWM
    //   GPIO17  → LED 暖光 PWM
    // 注意：GPIO34/35/36/39 仅输入，不能做输出

    // ===== DHT22 温湿度传感器 =====
    const uint8_t DHT_PIN = 4;
    sensorManager.addSensor(new DHT22Sensor("DHT22", DHT_PIN));

    // ===== PIR 人体感应传感器 =====
    const uint8_t PIR_PIN = 5;
    sensorManager.addSensor(new PIRSensor("PIR", PIR_PIN, PIR_HOLD_TIME));

    // ===== 光敏传感器（模拟模式）=====
    // 连接说明：
    // - AO引脚 → GPIO34（ADC1_CH6，仅输入）；VCC → 3.3V；GND → GND；DO → 不连接
    // - 阈值由 Config.h 中 LIGHT_NIGHT_THRESHOLD / LIGHT_DAY_THRESHOLD 控制
    const uint8_t LIGHT_AO_PIN = 34;
    sensorManager.addSensor(new LightSensor("Light", LIGHT_AO_PIN, LIGHT_NIGHT_THRESHOLD, LIGHT_DAY_THRESHOLD, true));

    // ===== 自锁按钮 =====
    sensorManager.addSensor(new LatchButtonSensor("LatchBtn", LATCH_BUTTON_PIN));

    // 初始化所有传感器
    sensorManager.begin();

    // ========== 注册执行器 ==========

    // ===== 双色调温 LED =====
    const uint8_t LED_WHITE_PIN = 16;
    const uint8_t LED_WARM_PIN  = 17;
    actuatorManager.addActuator(new DualColorLED("DualLED", LED_WHITE_PIN, LED_WARM_PIN));

    // TODO: 添加其他执行器
    // actuatorManager.addActuator(new Relay("Relay1", 26));

    // 初始化所有执行器
    actuatorManager.begin();
}

} // namespace DeviceRegistry
