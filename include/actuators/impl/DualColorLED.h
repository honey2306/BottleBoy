/**
 * @file DualColorLED.h
 * @brief 双色调温LED灯
 * 
 * 双色LED可以通过调节白光和暖光的比例实现色温调节
 * 
 * 硬件连接：
 * - 白光引脚 → GPIO（支持PWM）
 * - 暖光引脚 → GPIO（支持PWM）
 * - GND → GND
 * 
 * 功能：
 * - 亮度调节（0-255）
 * - 色温调节（0-100，0=纯暖光，100=纯白光）
 * - 开关控制
 * 
 * 使用示例：
 * ```cpp
 * auto led = new DualColorLED("DualLED", 2, 15);  // 白光GPIO2，暖光GPIO15
 * led->setBrightness(200);           // 设置亮度
 * led->setColorTemperature(50);      // 设置色温为中性
 * led->turnOn();                     // 开灯
 * ```
 */

#ifndef DUAL_COLOR_LED_H
#define DUAL_COLOR_LED_H

#include "actuators/base/Actuator.h"

/**
 * @class DualColorLED
 * @brief 双色调温LED灯控制类
 */
class DualColorLED : public Actuator {
public:
    /**
     * @brief 构造函数
     * @param name 执行器名称
     * @param whitePinGPIO 白光引脚号
     * @param warmPin 暖光引脚号
     */
    DualColorLED(const String& name, uint8_t whitePin, uint8_t warmPin) 
        : Actuator(name, "dual_color_led"),
          _whitePin(whitePin),
          _warmPin(warmPin),
          _brightness(0),
          _colorTemp(50),
          _isOn(false),
          _lastBrightness(0),
          _lastColorTemp(50) {}

    /**
     * @brief 初始化LED
     */
    bool begin() override {
        // 设置引脚为输出模式
        pinMode(_whitePin, OUTPUT);
        pinMode(_warmPin, OUTPUT);
        
        // 初始状态：关闭
        turnOff();
        
        return true;
    }

    /**
     * @brief 设置亮度（0-255）
     * 
     * 这个方法由框架调用，将亮度值设置到LED
     * 同时保持当前的色温比例
     */
    bool setValue(float value) override {
        // 限制范围 0-255
        _brightness = constrain((int)value, 0, 255);
        
        // 更新状态
        _isOn = (_brightness > 0);
        
        // 应用到LED
        updateLED();
        
        return true;
    }

    /**
     * @brief 获取当前亮度
     * @return 亮度值（0-255）
     */
    float getValue() const override {
        return _brightness;
    }

    /**
     * @brief 设置亮度（0-255）
     * @param brightness 亮度值
     */
    void setBrightness(uint8_t brightness) {
        setValue(brightness);
    }

    /**
     * @brief 获取当前亮度
     * @return 亮度值（0-255）
     */
    uint8_t getBrightness() const {
        return _brightness;
    }

    /**
     * @brief 设置色温（0-100）
     * 
     * @param colorTemp 色温值
     *        - 0 = 纯暖光（最暖）
     *        - 50 = 中性白光
     *        - 100 = 纯白光（最冷）
     */
    void setColorTemperature(uint8_t colorTemp) {
        // 限制范围 0-100
        _colorTemp = constrain(colorTemp, 0, 100);
        
        // 如果灯是开着的，更新LED
        if (_isOn) {
            updateLED();
        }
    }

    /**
     * @brief 获取当前色温
     * @return 色温值（0-100）
     */
    uint8_t getColorTemperature() const {
        return _colorTemp;
    }

    /**
     * @brief 开灯
     * 
     * 恢复到上次关灯前的亮度和色温
     * 如果是第一次开灯（没有历史值），则使用默认值
     */
    void turnOn() {
        _isOn = true;
        
        // 如果从未设置过亮度，使用默认值
        if (_brightness == 0 && _lastBrightness == 0) {
            _brightness = 200;     // 默认亮度
            _colorTemp = 50;       // 默认中性色温
        } else if (_brightness == 0) {
            // 恢复上次的亮度和色温
            _brightness = _lastBrightness;
            _colorTemp = _lastColorTemp;
        }
        
        updateLED();
    }

    /**
     * @brief 关灯
     * 
     * 保存当前的亮度和色温，以便下次开灯时恢复
     */
    void turnOff() {
        // 保存当前状态
        if (_brightness > 0) {
            _lastBrightness = _brightness;
            _lastColorTemp = _colorTemp;
        }
        
        _isOn = false;
        analogWrite(_whitePin, 0);
        analogWrite(_warmPin, 0);
    }

    /**
     * @brief 切换开关状态
     */
    void toggle() {
        if (_isOn) {
            turnOff();
        } else {
            turnOn();
        }
    }

    /**
     * @brief 检查是否开启
     * @return true=开，false=关
     */
    bool isOn() const {
        return _isOn;
    }

    /**
     * @brief 获取JSON数据
     * 
     * JSON格式：
     * {
     *   "name": "DualLED",
     *   "type": "dual_color_led",
     *   "state": "on",
     *   "brightness": 200,
     *   "colorTemp": 50,
     *   "colorTempDesc": "中性白",
     *   "whiteValue": 100,
     *   "warmValue": 100
     * }
     */
    void getJSON(JsonObject& doc) override {
        doc["name"] = _name;
        doc["type"] = _type;
        doc["state"] = _isOn ? "on" : "off";
        doc["brightness"] = _brightness;
        doc["colorTemp"] = _colorTemp;
        
        // 色温描述
        String colorDesc;
        if (_colorTemp < 20) {
            colorDesc = "暖光";
        } else if (_colorTemp < 40) {
            colorDesc = "暖白";
        } else if (_colorTemp < 60) {
            colorDesc = "中性白";
        } else if (_colorTemp < 80) {
            colorDesc = "冷白";
        } else {
            colorDesc = "纯白光";
        }
        doc["colorTempDesc"] = colorDesc;
        
        // 当前白光和暖光的实际PWM值
        uint8_t white, warm;
        calculatePWM(white, warm);
        doc["whiteValue"] = white;
        doc["warmValue"] = warm;
    }

    /**
     * @brief 从JSON设置状态
     * 
     * 支持的JSON格式：
     * {"brightness": 200}                    // 设置亮度
     * {"colorTemp": 50}                      // 设置色温
     * {"brightness": 200, "colorTemp": 50}   // 同时设置
     * {"state": "on"}                        // 开灯
     * {"state": "off"}                       // 关灯
     */
    bool setFromJSON(JsonObject& doc) override {
        bool changed = false;
        
        // 处理state
        if (doc.containsKey("state")) {
            String state = doc["state"].as<String>();
            if (state == "on") {
                turnOn();
                changed = true;
            } else if (state == "off") {
                turnOff();
                changed = true;
            }
        }
        
        // 处理亮度
        if (doc.containsKey("brightness")) {
            setBrightness(doc["brightness"]);
            changed = true;
        }
        
        // 处理色温
        if (doc.containsKey("colorTemp")) {
            setColorTemperature(doc["colorTemp"]);
            changed = true;
        }
        
        return changed;
    }

private:
    uint8_t _whitePin;          ///< 白光引脚号
    uint8_t _warmPin;           ///< 暖光引脚号
    uint8_t _brightness;        ///< 当前亮度（0-255）
    uint8_t _colorTemp;         ///< 当前色温（0-100）
    bool _isOn;                 ///< 开关状态
    uint8_t _lastBrightness;    ///< 上次关灯前的亮度
    uint8_t _lastColorTemp;     ///< 上次关灯前的色温

    /**
     * @brief 计算白光和暖光的PWM值
     * 
     * 根据亮度和色温计算两个引脚的PWM值
     * 
     * @param white 输出白光PWM值
     * @param warm 输出暖光PWM值
     */
    void calculatePWM(uint8_t& white, uint8_t& warm) const {
        if (!_isOn || _brightness == 0) {
            white = 0;
            warm = 0;
            return;
        }
        
        // 根据色温计算白光和暖光的比例
        // colorTemp: 0=纯暖光, 100=纯白光
        float whiteRatio = _colorTemp / 100.0;      // 白光比例 0-1
        float warmRatio = 1.0 - whiteRatio;         // 暖光比例 1-0
        
        // 应用亮度
        white = (uint8_t)(_brightness * whiteRatio);
        warm = (uint8_t)(_brightness * warmRatio);
    }

    /**
     * @brief 更新LED输出
     * 
     * 根据当前亮度和色温计算并输出PWM值
     */
    void updateLED() {
        uint8_t white, warm;
        calculatePWM(white, warm);
        
        // 输出到引脚
        analogWrite(_whitePin, white);
        analogWrite(_warmPin, warm);
    }
};

#endif // DUAL_COLOR_LED_H
