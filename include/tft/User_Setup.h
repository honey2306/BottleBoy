/**
 * @file User_Setup.h
 * @brief TFT_eSPI 引脚与驱动配置（ILI9341 / 2.2寸 SPI / ESP32）
 * 
 * 接线方式（硬件 VSPI）：
 *   屏幕引脚   →  ESP32 GPIO
 *   CS         →  GPIO 15
 *   RESET      →  GPIO 0
 *   DC/RS      →  GPIO 2
 *   SDI(MOSI)  →  GPIO 23  (VSPI MOSI)
 *   SCK        →  GPIO 18  (VSPI SCK)
 *   LED(背光)  →  3.3V 直接接（常亮）或 GPIO 21（可控亮度）
 *   VCC        →  3.3V
 *   GND        →  GND
 * 
 * 注意：MISO(SDO) 不需要接，仅写屏不读屏。
 */

// ---- 驱动芯片 ----
#define ILI9341_DRIVER

// ---- SPI 速率（ILI9341 最高 80MHz，建议 40MHz 保稳定）----
#define SPI_FREQUENCY        40000000
#define SPI_READ_FREQUENCY    20000000

// ---- 引脚定义 ----
#define TFT_CS    15   // 片选
#define TFT_DC     2   // 数据/命令
#define TFT_RST    22   // 复位（避开 GPIO0 BOOT 引脚）
#define TFT_MOSI  23   // VSPI MOSI
#define TFT_SCLK  18   // VSPI SCK
#define TFT_MISO  -1   // 不使用
#define TFT_BL    21   // 背光控制（-1 表示直接接 3.3V 常亮）

// ---- 色彩顺序（多数 ILI9341 模块为 BGR，若颜色不对换成 BGR）----
// 如果颜色反了（红蓝互换），取消注释下面这行：
// #define TFT_RGB_ORDER TFT_BGR

// ---- 字体内置（减少 flash 占用，只打开用到的）----
#define LOAD_GLCD    // 系统默认小字体
#define LOAD_FONT2   // 小号字体（用于副标题）
#define LOAD_FONT4   // 中号字体（用于日期/湿度）
#define LOAD_FONT6   // 大号数字字体（7段数码管风格，适合温度）
#define LOAD_FONT7   // 超大 7段数码管风格数字（用于时钟）
#define LOAD_FONT8   // 最大尺寸（备用）
#define LOAD_GFXFF   // GFX Free Fonts（Adafruit 兼容字体）
#define SMOOTH_FONT  // 平滑字体支持（抗锯齿）
