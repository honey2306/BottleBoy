/**
 * @file LightSensor.h
 * @brief 光敏传感器（光线强度传感器）
 * 
 * 光敏传感器用于检测环境光线强度
 * 使用光敏电阻或光敏二极管，输出模拟电压
 * 
 * 硬件连接：
 * - VCC → 3.3V
 * - GND → GND
 * - AO (Analog Out) → ADC引脚（GPIO 32-39）
 * 
 * 注意：ESP32的ADC引脚
 * - ADC1: GPIO 32, 33, 34, 35, 36, 37, 38, 39
 * - ADC2: GPIO 0, 2, 4, 12-15, 25-27（WiFi使用时不可用）
 * - 建议使用ADC1的引脚
 * 
 * 使用示例：
 * ```cpp
 * sensorManager.addSensor(new LightSensor("Light", 34));  // GPIO34
 * 
 * // 可选：注册回调
 * lightSensor->onValueChanged([](Sensor* s, float newVal, float oldVal) {
 *     if (newVal < 500) {
 *         Serial.println("光线暗，需要开灯");
 *     }
 * });
 * ```
 */

#ifndef LIGHT_SENSOR_H
#define LIGHT_SENSOR_H

#include "sensors/base/Sensor.h"

/**
 * @class LightSensor
 * @brief 光敏传感器
 * 
 * 返回值范围：0-4095
 * - 0 = 完全黑暗
 * - 4095 = 非常明亮
 * 
 * 注意：实际值取决于硬件电路设计
 * 有些模块是光线越亮值越大，有些相反
 */
class LightSensor : public Sensor {
public:
    /**
     * @brief 构造函数
     * @param name 传感器名称
     * @param pin ADC引脚号（GPIO 32-39）
     * @param inverted 是否反转（true=光线越亮值越小）
     * @param dayNightThreshold 白天/晚上的阈值（默认1500）
     */
    LightSensor(const String& name, uint8_t pin, bool inverted = false, int dayNightThreshold = 1500) 
        : Sensor(name, "light"), 
          _pin(pin),
          _inverted(inverted),
          _lightLevel(0),
          _dayNightThreshold(dayNightThreshold) {}

    /**
     * @brief 初始化传感器
     */
    bool begin() override {
        pinMode(_pin, INPUT);
        
        // 设置ADC分辨率为12位（0-4095）
        analogReadResolution(12);
        
        // 设置ADC衰减为11dB（0-3.3V）
        analogSetAttenuation(ADC_11db);
        
        return true;
    }

    /**
     * @brief 读取光线强度
     */
    bool read() override {
        // 读取ADC值（0-4095）
        int rawValue = analogRead(_pin);
        
        // 如果需要反转
        if (_inverted) {
            rawValue = 4095 - rawValue;
        }
        
        _lightLevel = rawValue;
        
        // 触发回调
        notifyValueChanged(_lightLevel);
        
        return true;
    }

    /**
     * @brief 获取光线强度值
     * @return 光线强度（0-4095）
     */
    float getValue() const override {
        return _lightLevel;
    }

    /**
     * @brief 获取光线亮度百分比
     * @return 亮度百分比（0-100）
     */
    float getPercentage() const {
        return (_lightLevel / 4095.0) * 100.0;
    }

    /**
     * @brief 判断光线是否暗
     * @param threshold 阈值（默认800）
     * @return true=暗，false=亮
     */
    bool isDark(int threshold = 800) const {
        return _lightLevel < threshold;
    }

    /**
     * @brief 判断光线是否亮
     * @param threshold 阈值（默认2000）
     * @return true=亮，false=暗
     */
    bool isBright(int threshold = 2000) const {
        return _lightLevel > threshold;
    }

    /**
     * @brief 判断是否是白天
     * @return true=白天，false=晚上
     */
    bool isDayTime() const {
        return _lightLevel >= _dayNightThreshold;
    }

    /**
     * @brief 判断是否是晚上
     * @return true=晚上，false=白天
     */
    bool isNightTime() const {
        return _lightLevel < _dayNightThreshold;
    }

    /**
     * @brief 设置白天/晚上的阈值
     * @param threshold 阈值（光线值大于等于此值为白天）
     */
    void setDayNightThreshold(int threshold) {
        _dayNightThreshold = threshold;
    }

    /**
     * @brief 获取白天/晚上的阈值
     * @return 当前阈值
     */
    int getDayNightThreshold() const {
        return _dayNightThreshold;
    }

    /**
     * @brief 获取JSON数据
     * 
     * JSON格式：
     * {
     *   "name": "Light",
     *   "type": "light",
     *   "value": 1250,
     *   "percentage": 30.5,
     *   "level": "暗",
     *   "dayNight": "晚上",
     *   "isDayTime": false,
     *   "unit": "lux"
     * }
     */
    void getJSON(JsonObject& doc) override {
        doc["name"] = _name;
        doc["type"] = _type;
        doc["value"] = _lightLevel;
        doc["percentage"] = getPercentage();
        
        // 判断亮度等级
        String level;
        if (_lightLevel < 500) {
            level = "很暗";
        } else if (_lightLevel < 1500) {
            level = "暗";
        } else if (_lightLevel < 2500) {
            level = "正常";
        } else if (_lightLevel < 3500) {
            level = "亮";
        } else {
            level = "很亮";
        }
        doc["level"] = level;
        
        // 判断白天还是晚上
        doc["dayNight"] = isDayTime() ? "白天" : "晚上";
        doc["isDayTime"] = isDayTime();
        
        doc["unit"] = "lux";
    }

private:
    uint8_t _pin;            ///< ADC引脚号
    bool _inverted;          ///< 是否反转
    float _lightLevel;       ///< 当前光线强度
    int _dayNightThreshold;  ///< 白天/晚上的阈值
};

#endif // LIGHT_SENSOR_H
