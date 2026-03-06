/**
 * @file ActuatorManager.cpp
 * @brief 执行器管理器实现
 * 
 * 这个类管理所有的执行器（可控制设备）：
 * - 添加/删除执行器
 * - 统一初始化
 * - 提供控制接口
 * - JSON数据转换
 */

#include "actuators/base/ActuatorManager.h"

/**
 * @brief 构造函数
 */
ActuatorManager::ActuatorManager() {
}

/**
 * @brief 析构函数
 * 
 * 释放所有执行器的内存，避免内存泄漏
 */
ActuatorManager::~ActuatorManager() {
    // 遍历所有执行器，逐个删除
    for (auto actuator : _actuators) {
        delete actuator;
    }
    // 清空列表
    _actuators.clear();
}

/**
 * @brief 添加执行器到管理器
 * 
 * @param actuator 执行器指针（不能为空）
 * @return true 添加成功，false 添加失败
 */
bool ActuatorManager::addActuator(Actuator* actuator) {
    // 检查指针是否有效
    if (actuator == nullptr) {
        return false;
    }
    
    // 添加到列表
    _actuators.push_back(actuator);
    return true;
}

/**
 * @brief 初始化所有执行器
 * 
 * 调用每个执行器的begin()方法进行初始化
 * 
 * @return true 全部成功，false 有失败
 */
bool ActuatorManager::begin() {
    bool allSuccess = true;
    
    // 遍历所有执行器
    for (auto actuator : _actuators) {
        // 只初始化启用的执行器
        if (actuator->isEnabled()) {
            if (!actuator->begin()) {
                allSuccess = false;
            }
        }
    }
    
    return allSuccess;
}

/**
 * @brief 获取所有执行器的JSON数据
 * 
 * JSON格式示例：
 * {
 *   "actuators": [
 *     {"name": "LED1", "type": "led", "state": "on", "brightness": 255},
 *     {"name": "Relay1", "type": "relay", "state": "off"}
 *   ]
 * }
 * 
 * @param doc JSON对象引用
 */
void ActuatorManager::getAllJSON(JsonObject& doc) {
    // 创建执行器数组
    JsonArray actuatorsArray = doc["actuators"].to<JsonArray>();
    
    // 遍历所有执行器
    for (auto actuator : _actuators) {
        // 只添加启用的执行器
        if (actuator->isEnabled()) {
            JsonObject actuatorObj = actuatorsArray.add<JsonObject>();
            // 让执行器自己填充数据
            actuator->getJSON(actuatorObj);
        }
    }
}

/**
 * @brief 根据名称查找执行器
 * 
 * @param name 执行器名称
 * @return 找到返回指针，找不到返回nullptr
 */
Actuator* ActuatorManager::getActuator(const String& name) {
    // 遍历查找
    for (auto actuator : _actuators) {
        if (actuator->getName() == name) {
            return actuator;
        }
    }
    
    return nullptr;
}

/**
 * @brief 控制执行器
 * 
 * 根据名称找到执行器，然后设置其值
 * 
 * @param name 执行器名称
 * @param value 控制值（具体含义由执行器类型决定）
 * @return true 控制成功，false 失败（执行器不存在或设置失败）
 */
bool ActuatorManager::controlActuator(const String& name, float value) {
    // 查找执行器
    Actuator* actuator = getActuator(name);
    
    if (actuator == nullptr) {
        return false;  // 执行器不存在
    }
    
    if (!actuator->isEnabled()) {
        return false;  // 执行器已禁用
    }
    
    // 设置值
    return actuator->setValue(value);
}
