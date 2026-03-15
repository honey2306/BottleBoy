/**
 * @file LatchButtonSensor.h
 * @brief 自锁按钮传感器 - 轮询模式
 *
 * 自锁按钮（Latching Button）按下后保持状态，再次按下恢复。
 * 与 PIRSensor 相同的轮询 + notifyValueChanged 模式。
 *
 * 接线：
 *   - 按钮一端 → GPIOxx
 *   - 按钮另一端 → GND
 *
 * 状态映射（INPUT_PULLUP）：
 *   - 按钮锁住（闭合） → GPIO = LOW  → value = 1.0（激活）
 *   - 按钮弹起（断开） → GPIO = HIGH → value = 0.0（未激活）
 */

#ifndef LATCH_BUTTON_SENSOR_H
#define LATCH_BUTTON_SENSOR_H

#include "sensors/base/Sensor.h"
#include <Arduino.h>

/**
 * @class LatchButtonSensor
 * @brief 自锁按钮传感器 - 轮询模式
 *
 * 值含义：
 *   - 1.0 = 按钮锁住（手动模式激活）
 *   - 0.0 = 按钮弹起（自动模式）
 */
class LatchButtonSensor : public Sensor {
public:
    /**
     * @brief 构造函数
     * @param name 传感器名称
     * @param pin  GPIO 引脚号
     */
    LatchButtonSensor(const String& name, uint8_t pin)
        : Sensor(name, "latch_button"), _pin(pin), _latched(false) {}

    /**
     * @brief 初始化引脚（INPUT_PULLUP，无需外部电阻）
     */
    bool begin() override {
        pinMode(_pin, INPUT_PULLUP);
        _latched = (digitalRead(_pin) == LOW);
        return true;
    }

    /**
     * @brief 轮询按钮状态，状态变化时触发 onValueChanged 回调
     */
    bool read() override {
        bool current = (digitalRead(_pin) == LOW);

        if (current && !_latched) {
            _latched = true;
            notifyValueChanged(1.0f);
        } else if (!current && _latched) {
            _latched = false;
            notifyValueChanged(0.0f);
        }

        return true;
    }

    /**
     * @brief 返回当前值（1.0=锁住，0.0=弹起）
     */
    float getValue() const override {
        return _latched ? 1.0f : 0.0f;
    }

    /**
     * @brief 返回按钮是否处于锁住状态
     * @return true=锁住（手动模式），false=弹起（自动模式）
     */
    bool isLatched() const {
        return _latched;
    }

    /**
     * @brief 获取 JSON 数据
     */
    void getJSON(JsonObject& doc) override {
        doc["name"]    = _name;
        doc["type"]    = _type;
        doc["latched"] = _latched;
        doc["value"]   = getValue();
    }

private:
    uint8_t _pin;      ///< GPIO 引脚号
    bool    _latched;  ///< 当前锁住状态
};

#endif // LATCH_BUTTON_SENSOR_H
