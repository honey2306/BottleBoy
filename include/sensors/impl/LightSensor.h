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
 * 
 * 注意：若传感器为反向型（暗=ADC高），构造时传 inverted=true，
 * 内部会对原始值取反（4095 - raw），再与阈值比较，阈值无需修改。
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
     * @param inverted       反向型传感器：暗时 ADC 高，读值时自动取反（4095 - raw）
     */
    LightSensor(const String& name, uint8_t analogPin,
                uint16_t nightThreshold, uint16_t dayThreshold,
                bool inverted = false)
        : Sensor(name, "light"),
          _analogPin(analogPin),
          _nightThreshold(nightThreshold),
          _dayThreshold(dayThreshold),
          _analogValue(0),
          _isNight(false),
          _inverted(inverted) {}

    /**
     * @brief 初始化传感器，读取初始状态
     */
    bool begin() override {
        pinMode(_analogPin, INPUT);
        _analogValue = _read();
        _updateState();
        return true;
    }

    /**
     * @brief 读取 ADC 值并更新黑夜/白天状态
     */
    bool read() override {
        const bool oldIsNight = _isNight;
        _analogValue = _read();
        _updateState();

        if (_isNight != oldIsNight) {
            notifyValueChanged(_isNight ? 1.0f : 0.0f);
        }

        return true;
    }

    /**
     * @brief 获取光照强度百分比（0=全暗，100=全亮）
     * @return 0.0 ~ 100.0
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
     * @brief 获取（取反后的）ADC 值
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
        // 亮度百分比（0=全暗，100=全亮）
        doc["percent"]        = (uint8_t)constrain((_analogValue * 100) / 4095, 0, 100);

        char displayStr[50];
        sprintf(displayStr, "%s (ADC:%d)", _isNight ? "🌙 黑夜" : "☀️ 白天", _analogValue);
        doc["displayValue"] = displayStr;
    }

private:
    uint8_t  _analogPin;        ///< AO 模拟引脚号
    uint16_t _nightThreshold;   ///< 低于此值判定为黑夜
    uint16_t _dayThreshold;     ///< 高于此值判定为白天
    uint16_t _analogValue;      ///< 取反后的 ADC 读数（0-4095）
    bool     _isNight;          ///< 当前是否是黑夜
    bool     _inverted;         ///< 反向型：读值时执行 4095 - raw

    /**
     * @brief 读取 ADC，反向型自动取反
     * @return 处理后的 ADC 值（0-4095）
     */
    uint16_t _read() const {
        const uint16_t raw = analogRead(_analogPin);
        return _inverted ? (4095 - raw) : raw;
    }

    /**
     * @brief 根据当前 ADC 值更新 _isNight（双阈值滞后）
     */
    void _updateState() {
        if (_analogValue < _nightThreshold)      _isNight = true;
        else if (_analogValue > _dayThreshold)   _isNight = false;
        // 滞后区间内保持当前状态不变
    }
};

#endif // LIGHT_SENSOR_H
