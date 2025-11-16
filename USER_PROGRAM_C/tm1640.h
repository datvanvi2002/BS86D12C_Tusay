#ifndef TM1640_H
#define TM1640_H
#include "bs86d12c.h"
#include <stdint.h>

void tm1640_init(uint8_t brightness_0_to_7);     // 0..7
void tm1640_clear_all(void);                     // xóa 16 grid
void tm1640_write_fixed(uint8_t addr, uint8_t seg_bits); // ghi 1 byte vào addr (0..0x0F)
void tm1640_write_auto(uint8_t start_addr, const uint8_t *data, uint8_t len);

/// @brief Ghi 1 chữ số (0..9) vào địa chỉ cố định
/// @param addr Grid address (0..15)
/// @param digit Digit (0..9)
void tm1640_write_digit(uint8_t addr, uint8_t digit, _Bool with_dp);
void tm1640_write_led(uint8_t led_number, float number);

void tm1640_show_1_to_9_on_grid1_to_9(void);     

void tm1640_walk_segments(uint8_t grid, uint16_t on_ms);
void tm1640_walk_grids_a(uint16_t hold_ms);
extern uint8_t TM1640_grid_order[16];

void tm1640_keyring_only(uint8_t key_number);
void tm1640_keyring_clear_all(void);
void tm1640_keyring_clear(uint8_t key_number);
void tm1640_keyring_add(uint8_t key_number);

/// @brief Show ERR in led_number 
/// @param led_number 1 or 2 or 3
void tm1640_write_err(uint8_t led_number);
void tm1640_write_end(uint8_t led_number);
void tm1640_clear_led(uint8_t led_number);
#endif // TM1640_H