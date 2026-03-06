/**
 * @file DeviceRegistry.h
 * @brief 设备注册器 - 负责注册所有传感器和执行器
 * 
 * 这个模块负责：
 * 1. 注册所有传感器到SensorManager
 * 2. 注册所有执行器到ActuatorManager
 * 3. 初始化所有设备
 * 
 * 修改设备配置时，只需要修改这个文件即可
 */

#ifndef DEVICE_REGISTRY_H
#define DEVICE_REGISTRY_H

#include "sensors/base/SensorManager.h"
#include "actuators/base/ActuatorManager.h"

/**
 * @namespace DeviceRegistry
 * @brief 设备注册命名空间
 */
namespace DeviceRegistry {
    /**
     * @brief 初始化并注册所有设备
     * 
     * 依次执行：
     * 1. 注册所有传感器
     * 2. 初始化传感器
     * 3. 注册所有执行器
     * 4. 初始化执行器
     * 
     * @param sensorManager 传感器管理器引用
     * @param actuatorManager 执行器管理器引用
     */
    void init(SensorManager& sensorManager, ActuatorManager& actuatorManager);
}

#endif // DEVICE_REGISTRY_H
