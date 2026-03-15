/**
 * @file RFManager.h
 * @brief 433MHz RF 遥控学习/发射管理器
 *
 * 功能：
 * 1. 学习模式：监听接收模块信号，捕获 RF 码并存储到 Preferences
 * 2. 发射模式：按名称查找设备并通过发射模块发出信号
 * 3. 设备管理：增删查，最多 RF_MAX_DEVICES 条
 *
 * 依赖：RCSwitch（sui77/rc-switch）、ESP32 Preferences
 */

#ifndef RF_MANAGER_H
#define RF_MANAGER_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <Preferences.h>
#include <RCSwitch.h>
#include "core/Config.h"

// ──────────────────────────────────────────────────────────────────
// 数据结构
// ──────────────────────────────────────────────────────────────────

/**
 * @brief 单个 RF 遥控设备的数据结构
 */
struct RFDevice {
    char     name[32];     ///< 设备名称（UTF-8，限 31 字节）
    uint32_t value;        ///< 接收到的 RF 码值
    uint32_t pulse;        ///< 脉冲宽度（微秒）
    int      protocol;     ///< 协议号（1~12，RCSwitch 协议）
    int      bitLength;    ///< 数据位数（通常 24 或 32）
};

// ──────────────────────────────────────────────────────────────────
// RFManager
// ──────────────────────────────────────────────────────────────────

/**
 * @class RFManager
 * @brief 433MHz RF 学习/发射/存储管理器
 *
 * 使用示例：
 * ```cpp
 * RFManager rf;
 * rf.begin();
 * rf.startLearn("灯1");      // 开始学习，等待信号
 * rf.loop();                 // 在主循环中调用
 * rf.transmit("灯1");        // 发射已学习的码
 * ```
 */
class RFManager {
public:
    // ──────────────────────────────────────────────────────────────
    // 学习状态枚举
    // ──────────────────────────────────────────────────────────────

    /**
     * @brief 学习状态
     */
    enum class LearnState {
        Idle,       ///< 空闲
        Waiting,    ///< 等待信号（学习模式激活）
        Success,    ///< 本次学习成功（短暂停留，供外部读取结果）
        Timeout     ///< 学习超时（无信号）
    };

    // ──────────────────────────────────────────────────────────────
    // 构造/析构
    // ──────────────────────────────────────────────────────────────

    /**
     * @brief 构造函数
     * @param rxPin 接收模块数据引脚
     * @param txPin 发射模块数据引脚
     */
    explicit RFManager(uint8_t rxPin = RF_RX_PIN, uint8_t txPin = RF_TX_PIN)
        : _rxPin(rxPin), _txPin(txPin),
          _learnState(LearnState::Idle),
          _learnStartMs(0),
          _deviceCount(0),
          _rxEnabled(false) {}

    // ──────────────────────────────────────────────────────────────
    // 生命周期
    // ──────────────────────────────────────────────────────────────

    /**
     * @brief 初始化发射器，并从 Preferences 加载已存设备
     *
     * 接收器不在此处启动，仅在 startLearn() 时按需开启，
     * 避免常开中断产生噪声干扰。
     *
     * @return true 初始化成功
     */
    bool begin() {
        _tx.enableTransmit(_txPin);
        _tx.setRepeatTransmit(5);   // 每次发射重复 5 次，提高可靠性
        _loadDevices();
        return true;
    }

    /**
     * @brief 主循环处理（需在 loop() 中调用，无节流，每帧都调）
     *
     * 负责：
     * - 学习模式下持续检测 RF 信号（跳过 value==0 的噪声）
     * - 学习超时检测
     * - 捕获成功后停止接收并切换到 Success 状态
     */
    void loop() {
        if (_learnState != LearnState::Waiting) return;

        unsigned long now = millis();

        // ── 超时检测 ──
        if (now - _learnStartMs >= RF_LEARN_TIMEOUT_MS) {
            _stopReceive();
            _learnState = LearnState::Timeout;
            return;
        }

        // ── 收到信号 ──
        if (!_rx.available()) return;

        uint32_t val      = (uint32_t)_rx.getReceivedValue();
        int      bits     = _rx.getReceivedBitlength();
        int      proto    = _rx.getReceivedProtocol();
        uint32_t pulse    = _rx.getReceivedDelay();

        _rx.resetAvailable();   // 必须先 reset，再判断有效性

        if (val == 0) {
            // 噪声，继续等待
            return;
        }

        // ── 停止接收 ──
        _stopReceive();

        // ── 保存设备 ──
        RFDevice dev{};
        strncpy(dev.name, _pendingName.c_str(), sizeof(dev.name) - 1);
        dev.value     = val;
        dev.bitLength = bits;
        dev.protocol  = proto;
        dev.pulse     = pulse;

        bool replaced = false;
        for (int i = 0; i < _deviceCount; i++) {
            if (String(_devices[i].name) == String(dev.name)) {
                _devices[i] = dev;
                replaced = true;
                break;
            }
        }
        if (!replaced && _deviceCount < RF_MAX_DEVICES) {
            _devices[_deviceCount++] = dev;
        }

        _saveDevices();
        _learnState = LearnState::Success;
    }

    // ──────────────────────────────────────────────────────────────
    // 学习接口
    // ──────────────────────────────────────────────────────────────

    /**
     * @brief 开始学习指定名称的设备
     *
     * 此时才开启接收中断，避免常开时噪声积累。
     *
     * @param name 设备名称（不超过 31 字节）
     */
    void startLearn(const String& name) {
        _pendingName  = name;
        _learnStartMs = millis();
        _learnState   = LearnState::Waiting;
        // 先停止旧中断（如果有），再重新开启，清空内部缓冲
        _stopReceive();
        _rx.enableReceive(_rxPin);
        _rxEnabled = true;
    }

    /**
     * @brief 取消当前学习
     */
    void cancelLearn() {
        _stopReceive();
        _learnState = LearnState::Idle;
    }

    /**
     * @brief 获取当前学习状态
     * @return LearnState 枚举值
     */
    LearnState getLearnState() const { return _learnState; }

    /**
     * @brief 将学习状态重置为 Idle（外部读取 Success/Timeout 后调用）
     */
    void resetLearnState() { _learnState = LearnState::Idle; }

    /**
     * @brief 返回正在学习的设备名称
     */
    const String& getPendingName() const { return _pendingName; }

    // ──────────────────────────────────────────────────────────────
    // 发射接口
    // ──────────────────────────────────────────────────────────────

    /**
     * @brief 按名称发射 RF 信号
     * @param name 设备名称
     * @return true 找到设备并发送，false 未找到
     */
    bool transmit(const String& name) {
        for (int i = 0; i < _deviceCount; i++) {
            if (String(_devices[i].name) == name) {
                _tx.setProtocol(_devices[i].protocol);
                _tx.setPulseLength(_devices[i].pulse);
                _tx.send(_devices[i].value, _devices[i].bitLength);
                return true;
            }
        }
        return false;
    }

    // ──────────────────────────────────────────────────────────────
    // 设备管理接口
    // ──────────────────────────────────────────────────────────────

    /**
     * @brief 删除指定名称的设备
     * @param name 设备名称
     * @return true 删除成功，false 未找到
     */
    bool removeDevice(const String& name) {
        for (int i = 0; i < _deviceCount; i++) {
            if (String(_devices[i].name) == name) {
                // 后移覆盖
                for (int j = i; j < _deviceCount - 1; j++) {
                    _devices[j] = _devices[j + 1];
                }
                _deviceCount--;
                _saveDevices();
                return true;
            }
        }
        return false;
    }

    /**
     * @brief 获取已存储设备数量
     */
    int getDeviceCount() const { return _deviceCount; }

    /**
     * @brief 按索引获取设备（只读）
     * @param index 设备索引（0 ~ getDeviceCount()-1）
     * @return 设备指针，越界返回 nullptr
     */
    const RFDevice* getDevice(int index) const {
        if (index < 0 || index >= _deviceCount) return nullptr;
        return &_devices[index];
    }

    /**
     * @brief 按名称查找设备（只读）
     * @param name 设备名称
     * @return 设备指针，未找到返回 nullptr
     */
    const RFDevice* findDevice(const String& name) const {
        for (int i = 0; i < _deviceCount; i++) {
            if (String(_devices[i].name) == name) return &_devices[i];
        }
        return nullptr;
    }

    // ──────────────────────────────────────────────────────────────
    // JSON 序列化
    // ──────────────────────────────────────────────────────────────

    /**
     * @brief 将所有设备列表序列化到 JSON 数组
     * @param arr 目标 JSON 数组
     */
    void getDevicesJSON(JsonArray arr) const {
        for (int i = 0; i < _deviceCount; i++) {
            JsonObject obj = arr.add<JsonObject>();
            obj["name"]      = _devices[i].name;
            obj["value"]     = _devices[i].value;
            obj["protocol"]  = _devices[i].protocol;
            obj["bitLength"] = _devices[i].bitLength;
            obj["pulse"]     = _devices[i].pulse;
        }
    }

    /**
     * @brief 将学习状态序列化到 JSON 对象
     * @param obj 目标 JSON 对象
     */
    void getLearnStatusJSON(JsonObject obj) const {
        switch (_learnState) {
            case LearnState::Idle:    obj["state"] = "idle";    break;
            case LearnState::Waiting: obj["state"] = "waiting"; break;
            case LearnState::Success: obj["state"] = "success"; break;
            case LearnState::Timeout: obj["state"] = "timeout"; break;
        }
        obj["name"] = _pendingName;
    }

private:
    // ──────────────────────────────────────────────────────────────
    // 私有成员
    // ──────────────────────────────────────────────────────────────

    uint8_t     _rxPin;                       ///< 接收引脚
    uint8_t     _txPin;                       ///< 发射引脚
    RCSwitch    _rx;                          ///< 接收器实例
    RCSwitch    _tx;                          ///< 发射器实例
    Preferences _prefs;                       ///< Preferences 存储

    LearnState  _learnState;                  ///< 当前学习状态
    unsigned long _learnStartMs;              ///< 学习开始时间
    String      _pendingName;                 ///< 当前正在学习的设备名称

    RFDevice    _devices[RF_MAX_DEVICES];     ///< 设备列表
    int         _deviceCount;                 ///< 已存设备数量
    bool        _rxEnabled;                   ///< 接收中断是否已开启

    // ──────────────────────────────────────────────────────────────
    // Preferences 持久化
    // ──────────────────────────────────────────────────────────────

    /**
     * @brief 从 Preferences 加载所有设备
     *
     * 存储格式：
     * - 键 "cnt"        → uint32_t 设备数量
     * - 键 "d{i}n"      → String   设备名称
     * - 键 "d{i}v"      → uint32_t RF 码值
     * - 键 "d{i}p"      → uint32_t 脉冲宽度
     * - 键 "d{i}pt"     → int32_t  协议号
     * - 键 "d{i}b"      → int32_t  数据位数
     */
    /**
     * @brief 安全停止接收中断（仅在已开启时才调用，避免 GPIO 报错）
     */
    void _stopReceive() {
        if (_rxEnabled) {
            _rx.disableReceive();
            _rxEnabled = false;
        }
    }

    void _loadDevices() {
        _prefs.begin(RF_PREFS_NAMESPACE, true);  // 只读
        _deviceCount = (int)_prefs.getUInt("cnt", 0);
        if (_deviceCount > RF_MAX_DEVICES) _deviceCount = RF_MAX_DEVICES;

        for (int i = 0; i < _deviceCount; i++) {
            char kn[8], kv[8], kp[8], kpt[8], kb[8];
            snprintf(kn,  sizeof(kn),  "d%dn",  i);
            snprintf(kv,  sizeof(kv),  "d%dv",  i);
            snprintf(kp,  sizeof(kp),  "d%dp",  i);
            snprintf(kpt, sizeof(kpt), "d%dpt", i);
            snprintf(kb,  sizeof(kb),  "d%db",  i);

            String name = _prefs.getString(kn, "");
            strncpy(_devices[i].name, name.c_str(), sizeof(_devices[i].name) - 1);
            _devices[i].name[sizeof(_devices[i].name) - 1] = '\0';
            _devices[i].value     = _prefs.getUInt(kv,  0);
            _devices[i].pulse     = _prefs.getUInt(kp,  350);
            _devices[i].protocol  = _prefs.getInt(kpt,  1);
            _devices[i].bitLength = _prefs.getInt(kb,   24);
        }
        _prefs.end();
    }

    /**
     * @brief 将所有设备保存到 Preferences
     */
    void _saveDevices() {
        _prefs.begin(RF_PREFS_NAMESPACE, false);  // 读写
        _prefs.putUInt("cnt", (uint32_t)_deviceCount);

        for (int i = 0; i < _deviceCount; i++) {
            char kn[8], kv[8], kp[8], kpt[8], kb[8];
            snprintf(kn,  sizeof(kn),  "d%dn",  i);
            snprintf(kv,  sizeof(kv),  "d%dv",  i);
            snprintf(kp,  sizeof(kp),  "d%dp",  i);
            snprintf(kpt, sizeof(kpt), "d%dpt", i);
            snprintf(kb,  sizeof(kb),  "d%db",  i);

            _prefs.putString(kn,  _devices[i].name);
            _prefs.putUInt(kv,    _devices[i].value);
            _prefs.putUInt(kp,    _devices[i].pulse);
            _prefs.putInt(kpt,    _devices[i].protocol);
            _prefs.putInt(kb,     _devices[i].bitLength);
        }
        _prefs.end();
    }
};

#endif // RF_MANAGER_H
