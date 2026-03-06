/**
 * @file AutomationRules.h
 * @brief 自动化规则管理 - 处理传感器和执行器之间的联动逻辑
 * 
 * 这个模块负责：
 * 1. 定义传感器和执行器的联动规则
 * 2. 根据传感器数据自动控制执行器
 * 3. 实现各种自动化场景
 * 
 * 使用场景：
 * - 人体感应 → 开灯
 * - 温度过高 → 开风扇
 * - 光线暗 → 开灯
 * - 湿度低 → 开加湿器
 * 等等...
 */

#ifndef AUTOMATION_RULES_H
#define AUTOMATION_RULES_H

#include "sensors/base/SensorManager.h"
#include "actuators/base/ActuatorManager.h"

/**
 * @class AutomationRules
 * @brief 自动化规则管理器
 */
class AutomationRules {
public:
    /**
     * @brief 构造函数
     * @param sensorManager 传感器管理器引用
     * @param actuatorManager 执行器管理器引用
     */
    AutomationRules(SensorManager& sensorManager, ActuatorManager& actuatorManager);

    /**
     * @brief 处理所有自动化规则
     * 
     * 在主循环中定期调用，检查所有规则并执行相应动作
     * 建议调用频率：每秒1-10次
     */
    void process();

private:
    SensorManager& _sensorManager;       ///< 传感器管理器引用
    ActuatorManager& _actuatorManager;   ///< 执行器管理器引用

    // ========== 具体规则实现 ==========
    
    /**
     * @brief 规则1: 人体感应自动开灯
     * 
     * 逻辑：
     * - 检测到人 → LED开到最亮
     * - 未检测到人 → LED关闭
     */
    void rulePIRToLED();

    /**
     * @brief 规则2: 温度控制风扇（示例）
     * 
     * 逻辑：
     * - 温度 > 30°C → 打开风扇
     * - 温度 < 25°C → 关闭风扇
     */
    void ruleTemperatureToFan();

    /**
     * @brief 规则3: 光线传感器自动开灯（示例）
     * 
     * 逻辑：
     * - 光线 < 300 → 开灯
     * - 光线 > 500 → 关灯
     */
    void ruleLightToLED();

    // TODO: 在这里添加更多规则...
};

#endif // AUTOMATION_RULES_H
