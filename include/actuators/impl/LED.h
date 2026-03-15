/**
 * @file LED.h
 * @brief LED控制类
 * 
 * 这是一个示例类，演示如何控制LED灯
 * 支持：
 * - 开关控制
 * - PWM调光（亮度控制）
 */

#ifndef LED_H
#define LED_H

#include "actuators/base/Actuator.h"

/**
 * @class LED
 * @brief LED灯控制类
 */
class LED : public Actuator {
public:
    /**
     * @brief 构造函数
     * @param name LED名称（如"LED1"）
     * @param pin GPIO引脚号
     * @param invertLogic 是否反转逻辑（共阳极LED需要反转）
     */
    LED(const String& name, uint8_t pin, bool invertLogic = false) 
        : Actuator(name, "led"), 
          _pin(pin), 
          _invertLogic(invertLogic),
          _brightness(0),
          _state(false) {}

    /**
     * @brief 初始化LED
     * 
     * 设置GPIO为输出模式，初始状态为关闭
     * 
     * @return true 初始化成功
     */
    bool begin() override {
        // 设置引脚为输出模式
        pinMode(_pin, OUTPUT);
        
        // 初始状态：关闭
        setValue(0);
        
        return true;
    }

    /**
     * @brief 设置LED亮度
     * 
     * @param value 亮度值 0-255（0=关闭，255=最亮）
     * @return true 设置成功
     */
    bool setValue(float value) override {
        // 限制范围 0-255
        _brightness = constrain((int)value, 0, 255);
        
        // 判断开关状态
        _state = (_brightness > 0);
        
        // 写入PWM值
        int pwmValue = _brightness;
        
        // 如果是反转逻辑（共阳极LED）
        if (_invertLogic) {
            pwmValue = 255 - pwmValue;
        }
        
        // 输出到引脚
        analogWrite(_pin, pwmValue);
        
        return true;
    }

    /**
     * @brief 获取当前亮度
     * @return 亮度值 0-255
     */
    float getValue() const override {
        return _brightness;
    }

    /**
     * @brief 获取LED的JSON数据
     * 
     * JSON格式：
     * {
     *   "name": "LED1",
     *   "type": "led",
     *   "state": "on",         // on/off
     *   "brightness": 255,     // 0-255
     *   "pin": 2
     * }
     */
    void getJSON(JsonObject& doc) override {
        doc["name"] = _name;
        doc["type"] = _type;
        doc["state"] = _state ? "on" : "off";
        doc["brightness"] = _brightness;
        doc["pin"] = _pin;
    }

    /**
     * @brief 从JSON设置LED状态
     * 
     * 接受的JSON格式：
     * {
     *   "brightness": 128      // 设置亮度
     * }
     * 或
     * {
     *   "state": "on"          // 开（最大亮度）
     * }
     * 或
     * {
     *   "state": "off"         // 关
     * }
     */
    bool setFromJSON(JsonObject& doc) override {
        // 优先使用brightness
        if (doc["brightness"].is<int>()) {
            int brightness = doc["brightness"];
            return setValue(brightness);
        }
        
        // 其次使用state
        if (doc["state"].is<String>()) {
            String state = doc["state"].as<String>();
            if (state == "on") {
                return setValue(255);  // 开到最亮
            } else if (state == "off") {
                return setValue(0);    // 关闭
            }
        }
        
        return false;
    }

    /**
     * @brief 开灯（最大亮度）
     */
    void turnOn() {
        setValue(255);
    }

    /**
     * @brief 关灯
     */
    void turnOff() {
        setValue(0);
    }

    /**
     * @brief 切换状态
     */
    void toggle() {
        if (_state) {
            turnOff();
        } else {
            turnOn();
        }
    }

    /**
     * @brief 获取开关状态
     * @return true=开，false=关
     */
    bool getState() const {
        return _state;
    }

private:
    uint8_t _pin;           ///< GPIO引脚号
    bool _invertLogic;      ///< 是否反转逻辑
    int _brightness;        ///< 亮度值 0-255
    bool _state;            ///< 开关状态
};

#endif // LED_H
