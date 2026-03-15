/**
 * @file TFTDisplay.h
 * @brief TFT 显示模块 - 极简居中时钟（仿 iOS 风格，支持日/夜模式）
 *
 * 屏幕：ILI9341 / 2.2寸 / 320×240 / 横屏
 *
 * 无缝区块布局（屏幕分三个不重叠、不留空的区块）：
 *
 *   Y=0   ┌──────────────────────────────┐
 *         │        TIME_ZONE             │  0 ~ 139
 *         │   Font7×size2  "12:45"       │
 * Y=140  ├──────────────────────────────┤
 *         │        DATE_ZONE             │  140 ~ 179
 *         │   Font4×size1  "2026-03-13"  │
 * Y=180  ├──────────────────────────────┤
 *         │        SNSR_ZONE             │  180 ~ 239
 *         │   Font6/Font4  "25.3*C  68%" │
 * Y=240  └──────────────────────────────┘
 *
 * 日间模式：白底深蓝字
 * 夜间模式：黑底琥珀橙字（护眼低亮度）
 */

#ifndef TFT_DISPLAY_H
#define TFT_DISPLAY_H

#include <TFT_eSPI.h>
#include <Arduino.h>
#include "tft/NTPTime.h"

// TFT 背光 GPIO（platformio.ini: -DTFT_BL=21）
#ifndef TFT_BL
  #define TFT_BL 21
#endif
#define TFT_BL_LEDC_CH   3      ///< LEDC 通道（避免与 LED 占用的通道冲突）
#define TFT_BL_LEDC_FREQ 5000   ///< PWM 频率 5 kHz
#define TFT_BL_LEDC_RES  8      ///< 8-bit 精度（0-255）

// ===== 日间配色（RGB565）=====
#define COLOR_DAY_BG        TFT_WHITE   ///< 白色背景
#define COLOR_DAY_PRIMARY   0x10A3      ///< 深蓝近黑（时间）
#define COLOR_DAY_SECONDARY 0x4228      ///< 深灰（日期）
#define COLOR_DAY_ACCENT    0x10A3      ///< 温湿度同主色

// ===== 夜间配色（RGB565）=====
#define COLOR_NIGHT_BG        TFT_BLACK  ///< 纯黑背景
#define COLOR_NIGHT_PRIMARY   0xFD20     ///< 琥珀橙（时间）  #FFA500
#define COLOR_NIGHT_SECONDARY 0xFC60     ///< 略淡琥珀（日期）#FF8800
#define COLOR_NIGHT_ACCENT    0xFD20     ///< 温湿度同主色

// ===== 屏幕尺寸 =====
#define DISP_W  320
#define DISP_H  240

// ===== 无缝区块边界 =====
#define TIME_ZONE_TOP     0
#define TIME_ZONE_END   140
#define DATE_ZONE_END   180
#define SNSR_ZONE_END   DISP_H

// ===== 各行文字中心 Y =====
#define ROW_TIME_CY    75
#define ROW_DATE_CY   160
#define ROW_SNSR_CY   210

/**
 * @class TFTDisplay
 * @brief 极简居中时钟 + 温湿度，支持日/夜双模式
 */
class TFTDisplay {
public:
    TFTDisplay() : _tft(),
                   _lastHour(-1), _lastMin(-1), _lastSec(-1),
                   _lastTemp(-999.f), _lastHumid(-999.f),
                   _initialized(false), _nightMode(false), _on(true), _brightness(255),
                   _manualOverride(false),
                   _bg(COLOR_DAY_BG),
                   _colorPrimary(COLOR_DAY_PRIMARY),
                   _colorSecondary(COLOR_DAY_SECONDARY),
                   _colorAccent(COLOR_DAY_ACCENT) {}

    /**
     * @brief 初始化屏幕（含背光 PWM）
     */
    void begin() {
        // 初始化背光 PWM
        ledcSetup(TFT_BL_LEDC_CH, TFT_BL_LEDC_FREQ, TFT_BL_LEDC_RES);
        ledcAttachPin(TFT_BL, TFT_BL_LEDC_CH);
        ledcWrite(TFT_BL_LEDC_CH, _brightness);

        _tft.init();
        _tft.setRotation(1);
        _tft.fillScreen(_bg);
        _initialized = true;
    }

    /**
     * @brief 设置屏幕开关
     * @param on true=开，false=关（背光熄灭）
     */
    void setOn(bool on) {
        if (_on == on) return;
        _on = on;
        ledcWrite(TFT_BL_LEDC_CH, on ? _brightness : 0);
    }

    /**
     * @brief 返回屏幕是否开启
     */
    bool isOn() const { return _on; }

    /**
     * @brief 设置背光亮度（0-255），不影响开关状态
     * @param brightness 亮度值 0-255
     */
    void setBrightness(uint8_t brightness) {
        _brightness = brightness;
        if (_on) ledcWrite(TFT_BL_LEDC_CH, brightness);
    }

    /**
     * @brief 返回当前亮度值（0-255）
     */
    uint8_t getBrightness() const { return _brightness; }

    /**
     * @brief 网页手动切换日/夜模式，设置后不再被光敏自动覆盖
     * @param night true=夜间模式，false=日间模式
     */
    void setNightMode(bool night) {
        _manualOverride = true;  // 标记为手动覆盖，阻止光敏自动覆盖
        _applyNightMode(night);
    }

    /**
     * @brief 光敏传感器自动调用（手动覆盖时不生效）
     * @param night true=夜间模式，false=日间模式
     */
    void setNightModeAuto(bool night) {
        if (_manualOverride) return;  // 手动覆盖期间忽略光敏指令
        _applyNightMode(night);
    }

    /**
     * @brief 恢复自动模式（网页可调用此接口解除手动覆盖）
     */
    void setNightModeAutomatic() { _manualOverride = false; }

    /**
     * @brief 返回当前是否处于夜间模式
     */
    bool isNightMode() const { return _nightMode; }

    /**
     * @brief 每秒调用，差量刷新
     * @param ntp       NTPTime 对象
     * @param tempC     摄氏温度
     * @param humidPct  湿度百分比
     */
    void update(const NTPTime& ntp, float tempC, float humidPct) {
        if (!_initialized) return;

        const int h = ntp.getHour();
        const int m = ntp.getMinute();
        const int s = ntp.getSecond();

        if (h != _lastHour || m != _lastMin) {
            _lastHour = h;
            _lastMin  = m;
            _drawTime(h, m);
            _drawDate(ntp.getDateString(), ntp.getWeekdayCN());
        }

        _lastSec = s;

        if (isnan(tempC) != isnan(_lastTemp) ||
            (!isnan(tempC) && fabsf(tempC - _lastTemp) > 0.05f)) {
            _lastTemp = isnan(tempC) ? 0.f : tempC;
            _drawSensors(isnan(tempC) ? 0.f : tempC,
                         isnan(_lastHumid) ? 0.f : _lastHumid);
        }
        if (isnan(humidPct) != isnan(_lastHumid) ||
            (!isnan(humidPct) && fabsf(humidPct - _lastHumid) > 0.5f)) {
            _lastHumid = isnan(humidPct) ? 0.f : humidPct;
            _drawSensors(isnan(_lastTemp) ? 0.f : _lastTemp,
                         isnan(humidPct) ? 0.f : humidPct);
        }
    }

    /**
     * @brief NTP 同步中的等待提示
     */
    void showWaiting() {
        if (!_initialized) return;
        _tft.fillScreen(_bg);
        _tft.setTextDatum(MC_DATUM);
        _tft.setTextColor(_colorSecondary, _bg);
        _tft.setTextFont(4);
        _tft.setTextSize(1);
        _tft.drawString("Syncing time...", DISP_W / 2, DISP_H / 2);
    }

private:
    TFT_eSPI _tft;
    int      _lastHour, _lastMin, _lastSec;
    float    _lastTemp, _lastHumid;
    bool     _initialized;
    bool     _nightMode;
    bool     _on;
    uint8_t  _brightness;
    bool     _manualOverride;   ///< true=网页手动覆盖，阻止光敏自动覆盖夜间模式

    // 运行时颜色（随模式切换）
    uint16_t _bg;
    uint16_t _colorPrimary;
    uint16_t _colorSecondary;
    uint16_t _colorAccent;

    // ───── 内部工具方法 ─────

    /**
     * @brief 实际应用日/夜模式（内部共用）
     */
    void _applyNightMode(bool night) {
        if (_nightMode == night) return;
        _nightMode = night;

        if (night) {
            _bg             = COLOR_NIGHT_BG;
            _colorPrimary   = COLOR_NIGHT_PRIMARY;
            _colorSecondary = COLOR_NIGHT_SECONDARY;
            _colorAccent    = COLOR_NIGHT_ACCENT;
        } else {
            _bg             = COLOR_DAY_BG;
            _colorPrimary   = COLOR_DAY_PRIMARY;
            _colorSecondary = COLOR_DAY_SECONDARY;
            _colorAccent    = COLOR_DAY_ACCENT;
        }
        _tft.fillScreen(_bg);
        _lastHour  = -1;
        _lastMin   = -1;
        _lastTemp  = -999.f;
        _lastHumid = -999.f;
    }

    // ───── 时间区（0 ~ TIME_ZONE_END）─────

    /**
     * @brief 绘制 HH:MM（Font7 × size2，7段灯管风格）
     */
    void _drawTime(int h, int m) {
        char buf[6];
        snprintf(buf, sizeof(buf), "%02d:%02d", h, m);

        _tft.fillRect(0, TIME_ZONE_TOP, DISP_W, TIME_ZONE_END - TIME_ZONE_TOP, _bg);

        int strW  = _tft.textWidth(buf, 7);  // 先查 Font7 宽度（size1）
        // size2 时实际宽 = strW * 2
        int startX = (DISP_W - strW * 2) / 2;
        _tft.setTextColor(_colorPrimary, _bg);
        _tft.setTextFont(7);
        _tft.setTextSize(2);
        _tft.setTextDatum(TL_DATUM);
        _tft.drawString(buf, startX, ROW_TIME_CY - 48);
        _tft.setTextSize(1);
    }

    // ───── 日期区（TIME_ZONE_END ~ DATE_ZONE_END）─────

    /**
     * @brief 绘制日期和星期（Font4 × size1，居中）
     * 格式：2026-03-13  Fri
     */
    void _drawDate(const String& dateStr, const String& weekday) {
        _tft.fillRect(0, TIME_ZONE_END, DISP_W, DATE_ZONE_END - TIME_ZONE_END, _bg);

        String full = dateStr + "  " + weekday;
        _tft.setTextDatum(MC_DATUM);
        _tft.setTextColor(_colorSecondary, _bg);
        _tft.setTextFont(4);
        _tft.setTextSize(1);
        _tft.drawString(full, DISP_W / 2, ROW_DATE_CY);
    }

    // ───── 温湿度区（DATE_ZONE_END ~ SNSR_ZONE_END）─────

    /**
     * @brief 绘制温度和湿度
     *
     * 数字部分用 Font6（48px 7段数码管），单位部分用 Font4，
     * 实现"数字大、单位小"的视觉层次。
     * 摄氏度用 "*C"（Font4 不含 ° 字符）。
     */
    void _drawSensors(float tempC, float humidPct) {
        _tft.fillRect(0, DATE_ZONE_END, DISP_W, SNSR_ZONE_END - DATE_ZONE_END, _bg);

        _tft.setTextColor(_colorAccent, _bg);

        // ---- 温度 ----
        char numBuf[6];
        char unitBuf[4];
        snprintf(numBuf,  sizeof(numBuf),  "%.1f", tempC);
        snprintf(unitBuf, sizeof(unitBuf), "*C");

        int tempMidX = DISP_W * 3 / 8;
        _tft.setTextFont(6);
        _tft.setTextSize(1);
        _tft.setTextDatum(MR_DATUM);
        _tft.drawString(numBuf, tempMidX, ROW_SNSR_CY);

        _tft.setTextFont(4);
        _tft.setTextDatum(ML_DATUM);
        _tft.drawString(unitBuf, tempMidX + 2, ROW_SNSR_CY);

        // ---- 湿度 ----
        char humNumBuf[5];
        snprintf(humNumBuf, sizeof(humNumBuf), "%.0f", humidPct);

        int humMidX = DISP_W * 7 / 8;
        _tft.setTextFont(6);
        _tft.setTextDatum(MR_DATUM);
        _tft.drawString(humNumBuf, humMidX, ROW_SNSR_CY);

        _tft.setTextFont(4);
        _tft.setTextDatum(ML_DATUM);
        _tft.drawString("%", humMidX + 2, ROW_SNSR_CY);
    }
};

#endif // TFT_DISPLAY_H
