/**
 * @file LightSensor.h
 * @brief 光敏传感器 - 模拟模式，双阈值滞后
 * 
 * 硬件连接：
 * - VCC → 3.3V
 * - GND → GND
 * - AO (Analog Out) → ADC 模拟引脚
 * 
 * 工作机制（双阈值滞后）：
 * - ADC < nightThreshold → 切换为黑夜
 * - ADC > dayThreshold   → 切换为白天
 * - 两者之间             → 保持当前状态，避免临界值频繁切换
 */

#ifndef LIGHT_SENSOR_H
#define LIGHT_SENSOR_H

#include "sensors/base/Sensor.h"

/**
 * @class LightSensor
 * @brief 光敏传感器 - 模拟双阈值模式
 */
class LightSensor : public Sensor {
public:
    /**
     * @brief 构造函数
     * @param name           传感器名称
     * @param analogPin      AO 模拟引脚号
     * @param nightThreshold 低于此 ADC 值判定为黑夜
     * @param dayThreshold   高于此 ADC 值判定为白天（须大于 nightThreshold）
     */
    LightSensor(const String& name, uint8_t analogPin,
                uint16_t nightThreshold, uint16_t dayThreshold)
        : Sensor(name, "light"),
          _analogPin(analogPin),
          _nightThreshold(nightThreshold),
          _dayThreshold(dayThreshold),
          _analogValue(0),
          _isNight(false) {}

    /**
     * @brief 初始化传感器，读取初始状态
     */
    bool begin() override {
        pinMode(_analogPin, INPUT);
        _analogValue = analogRead(_analogPin);
        if (_analogValue < _nightThreshold) {
            _isNight = true;
        } else if (_analogValue > _dayThreshold) {
            _isNight = false;
        }
        // 处于滞后区间时默认为白天
        return true;
    }

    /**
     * @brief 读取 ADC 值并更新黑夜/白天状态
     */
    bool read() override {
        const bool oldIsNight = _isNight;
        _analogValue = analogRead(_analogPin);

        if (_analogValue < _nightThreshold) {
            _isNight = true;
        } else if (_analogValue > _dayThreshold) {
            _isNight = false;
        }
        // 滞后区间内不改变状态

        if (_isNight != oldIsNight) {
            notifyValueChanged(_isNight ? 1.0f : 0.0f);
        }

        return true;
    }

    /**
     * @brief 获取光照强度百分比（ADC 越小光越暗）
     * @return 0.0（全暗）~ 100.0（全亮）
     */
    float getValue() const override {
        return (_analogValue / 4095.0f) * 100.0f;
    }

    /**
     * @brief 判断当前是否是黑夜
     * @return true=黑夜，false=白天
     */
    bool isNight() const { return _isNight; }

    /**
     * @brief 判断当前是否是白天
     * @return true=白天，false=黑夜
     */
    bool isDay() const { return !_isNight; }

    /**
     * @brief 获取原始 ADC 值
     * @return ADC 值（0-4095）
     */
    uint16_t getAnalogValue() const { return _analogValue; }

    /**
     * @brief 获取 JSON 数据
     */
    void getJSON(JsonObject& doc) override {
        doc["name"]           = _name;
        doc["type"]           = "light";
        doc["isNight"]        = _isNight;
        doc["state"]          = _isNight ? "黑夜" : "白天";
        doc["analogValue"]    = _analogValue;
        doc["lightIntensity"] = getValue();
        doc["nightThreshold"] = _nightThreshold;
        doc["dayThreshold"]   = _dayThreshold;

        char displayStr[50];
        sprintf(displayStr, "%s (ADC:%d)", _isNight ? "🌙 黑夜" : "☀️ 白天", _analogValue);
        doc["displayValue"] = displayStr;
    }

private:
    uint8_t  _analogPin;        ///< AO 模拟引脚号
    uint16_t _nightThreshold;   ///< 低于此值判定为黑夜
    uint16_t _dayThreshold;     ///< 高于此值判定为白天
    uint16_t _analogValue;      ///< 最新 ADC 读数（0-4095）
    bool     _isNight;          ///< 当前是否是黑夜
};

#endif // LIGHT_SENSOR_H
