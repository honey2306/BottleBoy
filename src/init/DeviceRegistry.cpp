/**
 * @file DeviceRegistry.cpp
 * @brief 设备注册器实现
 */

#include "init/DeviceRegistry.h"
#include "sensors/impl/DHT22Sensor.h"
#include "sensors/impl/PIRSensor.h"
#include "sensors/impl/LightSensor.h"
#include "actuators/impl/LED.h"
#include "actuators/impl/DualColorLED.h"
#include <Arduino.h>

namespace DeviceRegistry {

/**
 * @brief 初始化并注册所有设备
 */
void init(SensorManager& sensorManager, ActuatorManager& actuatorManager) {
    // ========== 注册传感器 ==========
    Serial.println("Registering sensors...");
    
    // ===== DHT22温湿度传感器 =====
    const uint8_t DHT_PIN = 4;  // DHT22连接的GPIO引脚
    sensorManager.addSensor(new DHT22Sensor("DHT22", DHT_PIN));
    
    // ===== 人体感应传感器 =====
    const uint8_t PIR_PIN = 5;  // PIR连接的GPIO引脚
    sensorManager.addSensor(new PIRSensor("PIR", PIR_PIN));
    
    // ===== 光敏传感器 =====
    const uint8_t LIGHT_PIN = 34;  // 光敏传感器连接的ADC引脚（GPIO34）
    sensorManager.addSensor(new LightSensor("Light", LIGHT_PIN));
    
    // TODO: 添加更多传感器
    // 示例：
    // #include "sensors/impl/BMP280Sensor.h"
    // sensorManager.addSensor(new BMP280Sensor("Pressure"));
    
    // 初始化所有传感器
    if (sensorManager.begin()) {
        Serial.println("All sensors initialized successfully");
    } else {
        Serial.println("Some sensors failed to initialize");
    }
    
    // ========== 注册执行器 ==========
    Serial.println("Registering actuators...");
    
    // ===== 双色调温LED =====
    const uint8_t LED_WHITE_PIN = 2;   // 白光引脚
    const uint8_t LED_WARM_PIN = 15;   // 暖光引脚
    actuatorManager.addActuator(new DualColorLED("DualLED", LED_WHITE_PIN, LED_WARM_PIN));
    
    // ===== 普通LED =====
    // actuatorManager.addActuator(new LED("LED1", 16));  // GPIO16
    
    // TODO: 添加其他执行器
    // 示例：
    // #include "actuators/impl/Relay.h"
    // actuatorManager.addActuator(new Relay("Relay1", 17));  // GPIO17
    
    // 初始化所有执行器
    if (actuatorManager.begin()) {
        Serial.println("All actuators initialized successfully");
    } else {
        Serial.println("Some actuators failed to initialize");
    }
}

} // namespace DeviceRegistry
