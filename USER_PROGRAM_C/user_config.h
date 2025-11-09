#ifndef USERCONFIG_H
#define USERCONFIG_H
#include <stdint.h>


#define TEMP_STARTUP_LOCK 90    // ≥90°C khóa chạy
#define TEMP_STARTUP_OK_C 60      // <60°C cho phép
#define TEMP_DANGER_C 140         // >140°C nguy hiểm
#define TEMP_SAFE_EXIT_C 70       // <70°C cho phép thoát Danger
#define STARTUP_BUZZ_MS 3000      // còi 3s khi khởi động
#define VENT_ON_STARTUP_MS 300000 // B=1 xả áp 5 phút nếu lock

//4 chế độ tiệt trùng được định nghĩa
typedef enum 
{
    STERILIZATION_NULL = 0,
    STERILIZATION_1,
    STERILIZATION_2,
    STERILIZATION_3,
    STERILIZATION_4,
    QUICK_MODE,
}sterilization_mode_t;
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
    uint8_t temp_setting;               // TA: temperature setting (degC) 121 or 134
    uint8_t last_mode;                  // mode to save EEPROM
    uint16_t time_steri;                 // T1: sterilation setting time(m)
    uint16_t time_pressure_release_1;    // T21: pressure release setting time 1 (m)
    uint16_t time_pressure_release_2;    // T22: pressure release setting time 2 (m)
    uint16_t time_water_release;         // T3: water release time (m)
    uint16_t time_drying;                // T4: drying time (m)
    uint16_t crc;                       // checksum
} system_init_t; // time in minutes


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

typedef enum{
    UI_TEMP_NOW,
    UI_TEMP_SETTING
}led1_mode_t;

#endif //USERCONFIG_H