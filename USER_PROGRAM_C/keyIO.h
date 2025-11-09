#ifndef KEYIO
#define KEYIO

#include <stdint.h>

#define KEYBIT(n)           (1u << ((n) - 1))

#define SETTING_TEMP_KEY    1
#define TEMP_UP_KEY         3
#define TEMP_DOWN_KEY       2
#define SETTING_TIME_KEY    4
#define TIME_UP_KEY         6
#define TIME_DOWN_KEY       5
#define QUICK_KEY          7
#define START_KEY           8


typedef struct {
    uint8_t down;         // level: đang nhấn
    uint8_t idle;         // level: đang rảnh (= ~down & 0xFF)
    uint8_t pressed;      // edge: rảnh -> nhấn
    uint8_t released;     // edge: nhấn -> rảnh
    uint8_t hold;         // level: đang trong trạng thái giữ
    uint8_t hold_start;   // edge: vừa chuyển sang giữ
} KeyEvents;
KeyEvents key_update(uint8_t raw);
#endif 