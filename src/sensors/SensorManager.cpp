/**
 * @file SensorManager.cpp
 * @brief 传感器管理器实现
 * 
 * 这个类像一个"传感器管家"，负责管理所有的传感器：
 * - 添加/删除传感器
 * - 统一初始化所有传感器
 * - 统一读取所有传感器数据
 * - 将所有传感器数据转换成JSON格式
 */

#include "sensors/base/SensorManager.h"

/**
 * @brief 构造函数
 * 
 * 创建一个空的传感器管理器
 */
SensorManager::SensorManager() {
}

/**
 * @brief 析构函数
 * 
 * 当SensorManager对象被销毁时，自动释放所有传感器的内存
 * 这样可以避免内存泄漏
 */
SensorManager::~SensorManager() {
    // 遍历所有传感器，逐个删除
    for (auto sensor : _sensors) {
        delete sensor;  // 释放传感器对象的内存
    }
    // 清空传感器列表
    _sensors.clear();
}

/**
 * @brief 添加传感器到管理器
 * 
 * 将一个传感器对象添加到管理列表中
 * 
 * @param sensor 传感器指针（不能为空）
 * @return true表示添加成功，false表示添加失败（传感器为空）
 */
bool SensorManager::addSensor(Sensor* sensor) {
    // 检查传感器指针是否有效
    if (sensor == nullptr) {
        return false;  // 空指针，添加失败
    }
    
    // 将传感器添加到列表末尾
    _sensors.push_back(sensor);
    return true;
}

/**
 * @brief 初始化所有传感器
 * 
 * 遍历所有传感器，调用它们的begin()方法进行初始化
 * 只初始化已启用的传感器（isEnabled() == true）
 * 
 * @return true表示所有传感器都初始化成功，false表示有传感器初始化失败
 */
bool SensorManager::begin() {
    bool allSuccess = true;  // 记录是否全部成功
    
    // 遍历所有传感器
    for (auto sensor : _sensors) {
        // 只初始化启用的传感器
        if (sensor->isEnabled()) {
            // 调用传感器的begin()方法进行初始化
            if (!sensor->begin()) {
                allSuccess = false;  // 有传感器初始化失败
            }
        }
    }
    
    return allSuccess;
}

/**
 * @brief 读取所有传感器的数据
 * 
 * 遍历所有传感器，调用它们的read()方法读取最新数据
 * 只读取已启用的传感器
 * 
 * 这个函数应该定期调用（如每2秒调用一次）
 */
void SensorManager::readAll() {
    // 遍历所有传感器
    for (auto sensor : _sensors) {
        // 只读取启用的传感器
        if (sensor->isEnabled()) {
            sensor->read();  // 读取传感器数据
        }
    }
}

/**
 * @brief 获取所有传感器数据的JSON格式
 * 
 * 将所有传感器的数据组织成JSON格式，方便通过API返回给网页
 * 
 * JSON格式示例：
 * {
 *   "sensors": [
 *     {"name": "Temperature", "type": "dummy", "value": 25.6, "unit": "°C"},
 *     {"name": "Humidity", "type": "dummy", "value": 65.2, "unit": "%"}
 *   ]
 * }
 * 
 * @param doc JSON文档对象（传入引用，函数会修改这个对象）
 */
void SensorManager::getAllJSON(JsonObject& doc) {
    JsonArray sensorsArray = doc["sensors"].to<JsonArray>();
    
    Sensor* lightSensor = getSensor("Light");
    bool isNight = false;
    if (lightSensor && lightSensor->isEnabled()) {
        isNight = (lightSensor->getValue() == 1.0);
    }
    
    for (auto sensor : _sensors) {
        if (sensor->isEnabled()) {
            JsonObject sensorObj = sensorsArray.add<JsonObject>();
            sensor->getJSON(sensorObj);
            
            if (sensor->getType() == "pir") {
                if (!isNight) {
                    sensorObj["disabled"] = true;
                    sensorObj["disabledReason"] = "白天模式";
                } else {
                    sensorObj["disabled"] = false;
                }
            } else if (sensor->getType() == "light") {
                sensorObj["displayValue"] = isNight ? "黑夜" : "白天";
            }
        }
    }
}

/**
 * @brief 根据名称查找传感器
 * 
 * 在传感器列表中查找指定名称的传感器
 * 
 * @param name 传感器名称（如"Temperature"）
 * @return 找到返回传感器指针，找不到返回nullptr
 */
Sensor* SensorManager::getSensor(const String& name) {
    // 遍历所有传感器
    for (auto sensor : _sensors) {
        // 比较传感器名称
        if (sensor->getName() == name) {
            return sensor;  // 找到了，返回指针
        }
    }
    
    return nullptr;  // 没找到，返回空指针
}

/**
 * @brief 根据索引获取传感器
 * 
 * @param index 传感器索引（0到getSensorCount()-1）
 * @return 传感器指针，索引越界返回nullptr
 */
Sensor* SensorManager::getSensor(size_t index) {
    if (index >= _sensors.size()) {
        return nullptr;
    }
    return _sensors[index];
}
