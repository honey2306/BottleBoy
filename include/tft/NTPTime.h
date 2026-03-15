/**
 * @file NTPTime.h
 * @brief NTP 网络时间同步模块（UTC+8，中文星期）
 *
 * 依赖：arduino-libraries/NTPClient
 * WiFi 连接成功后调用 begin()，之后每秒调用 update() 即可。
 */

#ifndef NTP_TIME_H
#define NTP_TIME_H

#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Arduino.h>

/**
 * @class NTPTime
 * @brief 封装 NTPClient，提供 24 小时制时间与中文星期
 */
class NTPTime {
public:
    /**
     * @brief 构造函数
     * @param utcOffsetSeconds UTC 偏移秒数，默认 +8 时区（28800）
     */
    explicit NTPTime(long utcOffsetSeconds = 28800)
        : _udp(), _client(_udp, "pool.ntp.org", utcOffsetSeconds, 60000) {}

    /**
     * @brief 初始化并首次同步（需在 WiFi 连接后调用）
     */
    void begin() {
        _client.begin();
        _client.update();
    }

    /**
     * @brief 更新时间（建议每秒调用一次）
     * @return true = 已有有效时间
     */
    bool update() {
        _client.update();
        return isValid();
    }

    /**
     * @brief 强制立即向 NTP 服务器同步
     */
    void forceUpdate() {
        _client.forceUpdate();
    }

    /**
     * @brief 是否已获取到有效时间（epoch > 0）
     */
    bool isValid() const {
        return _client.getEpochTime() > 0;
    }

    /** @brief 小时（0-23，24小时制） */
    int getHour()   const { return _client.getHours(); }

    /** @brief 分钟（0-59） */
    int getMinute() const { return _client.getMinutes(); }

    /** @brief 秒钟（0-59） */
    int getSecond() const { return _client.getSeconds(); }

    /**
     * @brief 获取格式化时间字符串（HH:MM）
     * @return 例如 "09:05"
     */
    String getTimeHHMM() const {
        char buf[6];
        snprintf(buf, sizeof(buf), "%02d:%02d", getHour(), getMinute());
        return String(buf);
    }

    /**
     * @brief 获取格式化时间字符串（HH:MM:SS）
     * @return 例如 "09:05:30"
     */
    String getTimeHHMMSS() const {
        char buf[9];
        snprintf(buf, sizeof(buf), "%02d:%02d:%02d",
                 getHour(), getMinute(), getSecond());
        return String(buf);
    }

    /**
     * @brief 获取日期字符串（YYYY-MM-DD）
     * @return 例如 "2026-03-13"
     */
    String getDateString() const {
        time_t epoch = static_cast<time_t>(_client.getEpochTime());
        struct tm* t = localtime(&epoch);
        char buf[11];
        snprintf(buf, sizeof(buf), "%04d-%02d-%02d",
                 t->tm_year + 1900, t->tm_mon + 1, t->tm_mday);
        return String(buf);
    }

    /**
     * @brief 获取英文星期缩写（Font2/Font4 内置字体仅支持 ASCII）
     * @return 例如 "Fri"
     */
    String getWeekdayCN() const {
        static const char* const days[] = {
            "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
        };
        return days[_client.getDay()];
    }

private:
    WiFiUDP   _udp;
    NTPClient _client;
};

#endif // NTP_TIME_H
