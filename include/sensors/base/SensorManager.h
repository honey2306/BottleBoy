/**
 * @file SensorManager.h
 * @brief 传感器管理器
 */

#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include <vector>
#include "sensors/base/Sensor.h"
#include <ArduinoJson.h>

/**
 * @class SensorManager
 * @brief 管理所有传感器，提供统一的读取和访问接口
 */
class SensorManager {
public:
    /**
     * @brief 构造函数
     */
    SensorManager();

    /**
     * @brief 析构函数
     */
    ~SensorManager();

    /**
     * @brief 添加传感器
     * @param sensor 传感器指针
     * @return true 添加成功，false 失败
     */
    bool addSensor(Sensor* sensor);

    /**
     * @brief 初始化所有传感器
     * @return true 全部初始化成功，false 有失败
     */
    bool begin();

    /**
     * @brief 读取所有传感器数据
     */
    void readAll();

    /**
     * @brief 获取所有传感器数据的 JSON 格式
     * @param doc JSON 文档对象
     */
    void getAllJSON(JsonObject& doc);

    /**
     * @brief 根据名称获取传感器
     * @param name 传感器名称
     * @return 传感器指针，未找到返回 nullptr
     */
    Sensor* getSensor(const String& name);

    /**
     * @brief 获取传感器数量
     * @return 传感器数量
     */
    size_t getSensorCount() const { return _sensors.size(); }

private:
    std::vector<Sensor*> _sensors; ///< 传感器列表
};

#endif // SENSOR_MANAGER_H
