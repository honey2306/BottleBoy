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
#include "core/Config.h"
#include <Arduino.h>

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
          _brightness(NIGHT_LED_BRIGHTNESS),
          _colorTemp(NIGHT_LED_COLOR_TEMP),
          _isOn(false),
          _isAutoOn(false),
          _autoOnTime(0) {}

    /**
     * @brief 初始化LED
     */
    bool begin() override {
        // 设置引脚为输出模式（ESP32 支持 analogWrite）
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
        _brightness = constrain((int)value, 0, 255);
        if (_isOn && !_isAutoOn) {
            updateLED();
        }
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
     * 使用当前 _brightness 和 _colorTemp（构造时已初始化为默认值 200 / 50）
     */
    void turnOn() {
        _isOn = true;
        updateLED();
    }

    /**
     * @brief 以夜间模式开灯（固定亮度和色温，不修改 _brightness / _colorTemp）
     * 
     * 使用 Config.h 中定义的 NIGHT_LED_BRIGHTNESS 和 NIGHT_LED_COLOR_TEMP。
     * 开启自动模式，经过 NIGHT_LED_AUTO_OFF_TIME 毫秒后由 update() 自动关灯。
     * 若已处于自动模式则重置倒计时。
     */
    void turnOnNightMode() {
        // 先直接写 PWM 到夜间目标值（避免 LEDC 从 0 切换时的毛刺瞬态）
        updateLED(NIGHT_LED_BRIGHTNESS, NIGHT_LED_COLOR_TEMP);
        _isOn       = true;
        _isAutoOn   = true;
        _autoOnTime = millis();
    }

    /**
     * @brief 查询夜灯是否处于自动开启状态
     * @return true = 自动模式中，false = 非自动模式
     */
    bool isAutoOn() const { return _isAutoOn; }

    /**
     * @brief 主循环更新 - 检查自动关灯倒计时
     * 
     * 由 ActuatorManager::update() 每帧调用。
     * 处于自动模式且超过 NIGHT_LED_AUTO_OFF_TIME 时自动关灯。
     */
    void update() override {
        if (!_isAutoOn) return;
        if (millis() - _autoOnTime >= NIGHT_LED_AUTO_OFF_TIME) {
            turnOff();
            _isAutoOn = false;
        }
    }

    /**
     * @brief 关灯
     * 
     * 仅将 _isOn 置 false 并关掉 PWM 输出，
     * _brightness 和 _colorTemp 保持不变，下次 turnOn() 可直接恢复。
     */
    void turnOff() {
        _isOn = false;
        analogWrite(_whitePin, 0);
        analogWrite(_warmPin,  0);
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
        // 自动模式下汇报夜间实际值，保持 web UI 与硬件一致
        const uint8_t reportBrightness = _isAutoOn ? NIGHT_LED_BRIGHTNESS : _brightness;
        const uint8_t reportColorTemp  = _isAutoOn ? NIGHT_LED_COLOR_TEMP  : _colorTemp;
        const float   whiteRatio       = reportColorTemp / 100.0f;

        doc["name"]         = _name;
        doc["type"]         = _type;
        doc["state"]        = _isOn ? "on" : "off";
        doc["brightness"]   = reportBrightness;
        doc["colorTemp"]    = reportColorTemp;
        doc["autoMode"]     = _isAutoOn;
        
        // 色温描述
        String colorDesc;
        if (reportColorTemp < 20) {
            colorDesc = "暖光";
        } else if (reportColorTemp < 40) {
            colorDesc = "暖白";
        } else if (reportColorTemp < 60) {
            colorDesc = "中性白";
        } else if (reportColorTemp < 80) {
            colorDesc = "冷白";
        } else {
            colorDesc = "纯白光";
        }
        doc["colorTempDesc"] = colorDesc;
        
        doc["whiteValue"] = _isOn ? (uint8_t)(reportBrightness * whiteRatio)          : 0;
        doc["warmValue"]  = _isOn ? (uint8_t)(reportBrightness * (1.0f - whiteRatio)) : 0;
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
        
        // 先处理亮度（不改变开关状态，自动模式时不刷新 PWM）
        if (doc["brightness"].is<int>()) {
            _brightness = constrain((int)doc["brightness"], 0, 255);
            if (_isOn && !_isAutoOn) updateLED();
            changed = true;
        }
        
        // 再处理色温（不改变开关状态，自动模式时不刷新 PWM）
        if (doc["colorTemp"].is<int>()) {
            _colorTemp = constrain((int)doc["colorTemp"], 0, 100);
            if (_isOn && !_isAutoOn) updateLED();
            changed = true;
        }
        
        // 最后处理开关（自动模式时忽略 on 指令，避免覆盖夜间值）
        if (doc["state"].is<String>()) {
            String state = doc["state"].as<String>();
            if (state == "on" && !_isAutoOn) {
                turnOn();
                changed = true;
            } else if (state == "off") {
                turnOff();
                _isAutoOn = false;   // 手动关灯退出自动模式
                changed = true;
            }
        }
        
        return changed;
    }

private:
    uint8_t       _whitePin;     ///< 白光引脚号
    uint8_t       _warmPin;      ///< 暖光引脚号
    uint8_t       _brightness;   ///< 当前亮度（0-255）
    uint8_t       _colorTemp;    ///< 当前色温（0-100）
    bool          _isOn;         ///< 开关状态
    bool          _isAutoOn;     ///< 是否处于夜间自动模式
    unsigned long _autoOnTime;   ///< 自动模式开启的时间戳（ms）

    /**
     * @brief 用当前成员变量更新 LED 输出
     */
    void updateLED() {
        updateLED(_brightness, _colorTemp);
    }

    /**
     * @brief 用指定参数直接输出 PWM（不检查成员变量）
     * 
     * @param brightness 亮度（0-255）
     * @param colorTemp  色温（0-100，0=纯暖光，100=纯白光）
     */
    void updateLED(uint8_t brightness, uint8_t colorTemp) {
        const float whiteRatio = colorTemp / 100.0f;
        // 先写暖光再写白光，避免两次 analogWrite 间隙出现白色闪烁
        analogWrite(_warmPin,  (uint8_t)(brightness * (1.0f - whiteRatio)));
        analogWrite(_whitePin, (uint8_t)(brightness * whiteRatio));
    }
};

#endif // DUAL_COLOR_LED_H
