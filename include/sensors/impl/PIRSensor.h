/**
 * @file PIRSensor.h
 * @brief 人体感应传感器（PIR - Passive Infrared Sensor）
 * 
 * PIR传感器用于检测人体移动
 * 当检测到人体红外辐射时输出高电平
 * 
 * 硬件连接：
 * - VCC → 5V
 * - GND → GND
 * - OUT → GPIO引脚
 * 
 * 使用示例：
 * ```cpp
 * sensorManager.addSensor(new PIRSensor("PIR", 5));  // GPIO5
 * 
 * // 可选：注册回调，实时响应
 * pirSensor->onValueChanged([](Sensor* s, float newVal, float oldVal) {
 *     if (newVal > 0) {
 *         Serial.println("检测到人！");
 *     } else {
 *         Serial.println("无人");
 *     }
 * });
 * ```
 */

#ifndef PIR_SENSOR_H
#define PIR_SENSOR_H

#include "sensors/base/Sensor.h"

/**
 * @class PIRSensor
 * @brief 人体感应传感器
 * 
 * 返回值：
 * - 1.0 = 检测到人
 * - 0.0 = 无人
 */
class PIRSensor : public Sensor {
public:
    /**
     * @brief 构造函数
     * @param name 传感器名称
     * @param pin GPIO引脚号
     */
    PIRSensor(const String& name, uint8_t pin) 
        : Sensor(name, "pir"), 
          _pin(pin),
          _detected(false) {}

    /**
     * @brief 初始化传感器
     */
    bool begin() override {
        pinMode(_pin, INPUT);
        return true;
    }

    /**
     * @brief 读取传感器数据
     */
    bool read() override {
        // 读取GPIO状态
        bool detected = digitalRead(_pin) == HIGH;
        
        // 更新状态
        _detected = detected;
        
        // 触发回调（如果状态变化）
        notifyValueChanged(_detected ? 1.0 : 0.0);
        
        return true;
    }

    /**
     * @brief 获取传感器值
     * @return 1.0=检测到人，0.0=无人
     */
    float getValue() const override {
        return _detected ? 1.0 : 0.0;
    }

    /**
     * @brief 检查是否检测到人
     * @return true=检测到人，false=无人
     */
    bool isDetected() const {
        return _detected;
    }

    /**
     * @brief 获取JSON数据
     * 
     * JSON格式：
     * {
     *   "name": "PIR",
     *   "type": "pir",
     *   "detected": true,
     *   "value": 1.0,
     *   "state": "检测到人"
     * }
     */
    void getJSON(JsonObject& doc) override {
        doc["name"] = _name;
        doc["type"] = _type;
        doc["detected"] = _detected;
        doc["value"] = _detected ? 1.0 : 0.0;
        doc["state"] = _detected ? "检测到人" : "无人";
    }

private:
    uint8_t _pin;     ///< GPIO引脚号
    bool _detected;   ///< 是否检测到人
};

#endif // PIR_SENSOR_H
