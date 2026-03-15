/**
 * @file PIRSensor.h
 * @brief 人体感应传感器（PIR）- 轮询模式 + 智能保持
 * 
 * PIR传感器检测红外辐射变化
 * 智能保持机制：检测到人后保持一段时间，避免频繁切换
 */

#ifndef PIR_SENSOR_H
#define PIR_SENSOR_H

#include "sensors/base/Sensor.h"

/**
 * @class PIRSensor
 * @brief 人体感应传感器 - 智能保持模式（轮询）
 * 
 * 工作机制：
 * 1. 检测到人（GPIO HIGH） → 立即响应
 * 2. 保持"有人"状态一段时间（默认5秒）
 * 3. 超时后才变为"无人"
 */
class PIRSensor : public Sensor {
public:
    /**
     * @brief 构造函数
     * @param name 传感器名称
     * @param pin GPIO引脚号
     * @param holdTime 软件保持时间（毫秒），默认5秒
     */
    PIRSensor(const String& name, uint8_t pin, unsigned long holdTime = 5000) 
        : Sensor(name, "pir"), 
          _pin(pin),
          _holdTime(holdTime),
          _detected(false),
          _lastTriggerTime(0) {
    }

    bool begin() override {
        pinMode(_pin, INPUT);
        
        _detected = false;
        _lastTriggerTime = 0;
        
        return true;
    }

    bool read() override {
        // 直接读取GPIO状态
        bool currentGPIO = digitalRead(_pin);
        unsigned long now = millis();
        
        // 检测到人（GPIO HIGH）
        if (currentGPIO == HIGH) {
            _lastTriggerTime = now;
            
            if (!_detected) {
                _detected = true;
                notifyValueChanged(1.0);
            }
        } 
        else {
            if (_detected && (now - _lastTriggerTime > _holdTime)) {
                _detected = false;
                notifyValueChanged(0.0);
            }
        }
        
        return true;
    }

    float getValue() const override {
        return _detected ? 1.0f : 0.0f;
    }

    /**
     * @brief 判断当前是否检测到人
     * @return true=检测到人，false=无人
     */
    bool isDetected() const {
        return _detected;
    }

    void getJSON(JsonObject& doc) override {
        doc["name"] = _name;
        doc["type"] = "pir";
        doc["value"] = getValue();
        doc["detected"] = _detected;
        doc["state"] = _detected ? "检测到人" : "无人";
        doc["mode"] = "polling";
    }

private:
    uint8_t _pin;                    ///< GPIO引脚号
    unsigned long _holdTime;         ///< 保持时间（毫秒）
    bool _detected;                  ///< 当前是否检测到人
    unsigned long _lastTriggerTime;  ///< 最后触发时间
};

#endif // PIR_SENSOR_H
