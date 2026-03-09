/**
 * @file DHT22Sensor.h
 * @brief DHT22温湿度传感器
 * 
 * DHT22传感器可以同时测量温度和湿度
 * 
 * 硬件连接：
 * - VCC → 3.3V 或 5V
 * - GND → GND
 * - DATA → GPIO引脚（需要10K上拉电阻）
 * 
 * 使用示例：
 * ```cpp
 * sensorManager.addSensor(new DHT22Sensor("DHT22", 4));  // GPIO4
 * ```
 */

#ifndef DHT22_SENSOR_H
#define DHT22_SENSOR_H

#include "sensors/base/Sensor.h"
#include <DHT.h>

/**
 * @class DHT22Sensor
 * @brief DHT22温湿度传感器
 * 
 * 这个传感器同时返回温度和湿度两个数据
 * - getValue() 返回温度值（主要值）
 * - getJSON() 返回温度和湿度完整数据
 * 
 * 特点：
 * - 读取失败时保留上次有效值
 * - 避免频繁读取（最小2秒间隔）
 */
class DHT22Sensor : public Sensor {
public:
    /**
     * @brief 构造函数
     * @param name 传感器名称
     * @param pin GPIO引脚号
     */
    DHT22Sensor(const String& name, uint8_t pin) 
        : Sensor(name, "dht22"), 
          _pin(pin),
          _dht(pin, DHT22),
          _temperature(NAN), 
          _humidity(NAN),
          _lastReadTime(0),
          _readFailCount(0) {}

    /**
     * @brief 初始化传感器
     */
    bool begin() override {
        _dht.begin();
        return true;
    }

    /**
     * @brief 读取温湿度数据
     * 
     * DHT22规范要求至少2秒读取间隔
     * 读取失败时保留上次有效值
     */
    bool read() override {
        unsigned long now = millis();
        
        // 确保至少2秒间隔（溢出安全）
        if (now - _lastReadTime < 2000) {
            return true;
        }
        
        _lastReadTime = now;
        
        // 读取温度（摄氏度）
        float temp = _dht.readTemperature();
        
        // 读取湿度（百分比）
        float hum = _dht.readHumidity();
        
        // 检查是否读取成功
        if (isnan(temp) || isnan(hum)) {
            _readFailCount++;
            return false;
        }
        
        // 读取成功，更新值
        if (_readFailCount > 0) {
            _readFailCount = 0;
        }
        
        _temperature = temp;
        _humidity = hum;
        
        // 触发回调（使用温度作为主值）
        notifyValueChanged(_temperature);
        
        return true;
    }

    /**
     * @brief 获取温度值（主要值）
     * @return 温度（摄氏度）
     */
    float getValue() const override {
        return _temperature;
    }

    /**
     * @brief 获取湿度值
     * @return 湿度（百分比）
     */
    float getHumidity() const {
        return _humidity;
    }

    /**
     * @brief 获取JSON数据（包含温度和湿度）
     * 
     * JSON格式：
     * {\n     *   \"name\": \"DHT22\",\n     *   \"type\": \"dht22\",\n     *   \"temperature\": 25.6,\n     *   \"humidity\": 65.2,\n     *   \"tempUnit\": \"°C\",\n     *   \"humUnit\": \"%\"\n     * }\n     */
    void getJSON(JsonObject& doc) override {
        doc["name"] = _name;
        doc["type"] = _type;
        doc["temperature"] = _temperature;
        doc["humidity"] = _humidity;
        doc["tempUnit"] = "°C";
        doc["humUnit"] = "%";
        
        if (_readFailCount > 0) {
            doc["warning"] = "读取异常";
            doc["failCount"] = _readFailCount;
        }
    }

private:
    uint8_t _pin;
    DHT _dht;
    float _temperature;
    float _humidity;
    unsigned long _lastReadTime;
    uint16_t _readFailCount;
};

#endif // DHT22_SENSOR_H
