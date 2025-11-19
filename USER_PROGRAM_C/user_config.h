#ifndef USERCONFIG_H
#define USERCONFIG_H
#include <stdint.h>

// step press in key : +1 unit with STEP_PRESS_TIME (ms)
#define STEP_PRESS_TIME 10

#define TEMP_STARTUP_LOCK 90      // ≥90°C khóa chạy
#define TEMP_STARTUP_OK_C 60      // <60°C cho phép
#define TEMP_DANGER_C 140         // >140°C nguy hiểm
#define TEMP_SAFE_EXIT_C 70       // <70°C cho phép thoát Danger
#define STARTUP_BUZZ_MS 3000U     // còi 3s khi khởi động
#define VENT_ON_STARTUP_MS 300000 // B=1 xả áp 5 phút nếu lock

// 4 chế độ tiệt trùng được định nghĩa
typedef enum
{
    STERILIZATION_NULL = 0,
    STERILIZATION_1,
    STERILIZATION_2,
    STERILIZATION_3,
    STERILIZATION_4,
    QUICK_MODE,
} sterilization_mode_t;
typedef enum
{
    SUP_BOOT = 0,
    SUP_IDLE,
    SUP_LOCKED,
    SUP_DANGER
} SupState;

/// @brief Time system config
typedef struct
{
    uint8_t temp_setting;             // TA: temperature setting (degC) 121 or 134
    uint8_t last_mode;                // mode to save EEPROM
    uint16_t time_steri;              // T1: sterilation setting time(m)
    uint16_t time_pressure_release_1; // T21: pressure release setting time 1 (m)
    uint16_t time_pressure_release_2; // T22: pressure release setting time 2 (m)
    uint16_t time_water_release;      // T3: water release time (m)
    uint16_t time_drying;             // T4: drying time (m)
    uint16_t crc;                     // checksum
} system_init_t;                      // time in minutes
float g_temperature = 125;            /*test*/
float g_pressure = 0;
#define UI_REPEAT_MS_SLOW 100
#define UI_REPEAT_MS_FAST 50
#define UI_ACCEL_MS 2000
typedef enum
{
    UI_IDLE = 0,
    UI_SET_T1,
    UI_SET_T21,
    UI_SET_T22,
    UI_SET_T3,
    UI_SET_T4
} UiField;

typedef enum
{
    UI_TEMP_NOW = 0,
    UI_TEMP_SETTING,
    UI_ERR_BL,
} led1_mode_t;

bit steri1_running = 0;
bit steri2_running = 0;
bit steri3_running = 0;
bit steri4_running = 0;
bit quick_mode_running = 0;
bit steri_running = 0;
/*Biến dùng chung cho các mode*/
static uint32_t steri_tick = 0;
uint16_t remain_count = 0;

typedef enum
{
    STERI_WAITTING_1 = 0,
    START_STERI_1,
    HEATING_1, // chờ gia nhiệt
    STERING_1, // sau khi gia nhiệt xong, chờ tiệt trùng xong
    END_STERI_1,
    STOP_STERI_1,
} steri_1_status_t;
static steri_1_status_t steri1_stt = STERI_WAITTING_1;

typedef enum
{
    STERI_WAITTING_2 = 0,
    START_STERI_2,
    HEATING_21, // chờ gia nhiệt
    RELEASE_21, // sau khi gia nhiệt xong, chờ tiệt trùng xong
    HEATING_22,
    STERING_2,
    RELEASE_22,
    END_STERI_2,
    STOP_STERI_2,
} steri_2_status_t;
static steri_2_status_t steri2_stt = STERI_WAITTING_2;

typedef enum
{
    STERI_WAITTING_3 = 0,
    START_STERI_3,
    HEATING_31, // A=1, chờ đạt Ta
    RELEASE_3,
    HEATING_32,
    VENT_AND_STER3, // B=1 trong T21 & đếm T1 song song; khi T21=0 -> B=0, tiếp tục T1
    REALSE_WATER_3,
    FINISH_3, // T1=0 -> A=0, C=1, kêu 60s; quản lý ngưỡng 40/70°C
    STOP_STERI_3,
} steri_3_status_t;
static steri_3_status_t steri3_stt = STERI_WAITTING_3;

typedef enum
{
    STERI_WAITTING_4 = 0,
    START_STERI_4,
    HEATING_41, // A=1, chờ đạt Ta
    VENT_4,     // B=1 trong T21
    STERI_42,
    DRAIN_4,    // Sau T1=0: C=1, đếm T22; hết T22 -> C=0, D=1
    DRY_HEAT_4, // D=1, gia nhiệt tới Tb
    DRY_HOLD_4, // Duy trì Tb trong T4
    END_STERI_4,
    STOP_STERI_4,
} steri_4_status_t;
static steri_4_status_t steri4_stt = STERI_WAITTING_4;

typedef enum
{
    START_QUICK = 0,
    HEATING,
    END_QUICK,
    STOP_QUICK,
} quick_mode_t;
quick_mode_t quick_mode = STOP_QUICK;

void sterilization_start(sterilization_mode_t mode);
void sterilization_stop();
/// @brief beep with ms = time beep, blink_cnt: number blink
/// @param ms millisecond
/// @param blink_cnt number of beep
static inline void buzzer_start_ms(unsigned int ms);
static inline void buzzer_start_blink_ms(uint16_t ms, uint8_t blink_cnt);
void eeprom_write_system(const system_init_t *sys);
uint8_t eeprom_read_system(system_init_t *sys);
#endif // USERCONFIG_H