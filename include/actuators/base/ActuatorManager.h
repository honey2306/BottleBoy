/**
 * @file ActuatorManager.h
 * @brief 执行器管理器
 */

#ifndef ACTUATOR_MANAGER_H
#define ACTUATOR_MANAGER_H

#include <vector>
#include "actuators/base/Actuator.h"
#include <ArduinoJson.h>

/**
 * @class ActuatorManager
 * @brief 管理所有执行器（可控制设备），提供统一的访问和控制接口
 */
class ActuatorManager {
public:
    /**
     * @brief 构造函数
     */
    ActuatorManager();

    /**
     * @brief 析构函数
     */
    ~ActuatorManager();

    /**
     * @brief 添加执行器
     * @param actuator 执行器指针
     * @return true 添加成功，false 失败
     */
    bool addActuator(Actuator* actuator);

    /**
     * @brief 初始化所有执行器
     * @return true 全部初始化成功，false 有失败
     */
    bool begin();

    /**
     * @brief 获取所有执行器数据的JSON格式
     * @param doc JSON文档对象
     */
    void getAllJSON(JsonObject& doc);

    /**
     * @brief 根据名称获取执行器
     * @param name 执行器名称
     * @return 执行器指针，未找到返回nullptr
     */
    Actuator* getActuator(const String& name);

    /**
     * @brief 控制执行器
     * @param name 执行器名称
     * @param value 控制值
     * @return true 控制成功，false 失败
     */
    bool controlActuator(const String& name, float value);

    /**
     * @brief 获取执行器数量
     * @return 执行器数量
     */
    size_t getActuatorCount() const { return _actuators.size(); }

private:
    std::vector<Actuator*> _actuators; ///< 执行器列表
};

#endif // ACTUATOR_MANAGER_H
