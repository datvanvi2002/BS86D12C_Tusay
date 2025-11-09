#include "keyIO.h"
#include <stdint.h>

#define SCAN_PERIOD_MS       10      // chu kỳ gọi trong SCAN_CYCLEF
#define KEY_DEBOUNCE_MS       0      // 0 = bỏ qua 
#define KEY_HOLD_MS         500      // >= 500ms -> giữ (hold)
static uint8_t  s_prev_raw = 0;         
static uint16_t s_stable_ms[8] = {0};   
static uint16_t s_hold_ms[8]   = {0};
static uint8_t  s_hold_mask    = 0; 

// ===== Hàm cập nhật: gọi mỗi SCAN_CYCLEF =====
KeyEvents key_update(uint8_t raw)
{
    KeyEvents event_key = {0};
    uint8_t changed  = raw ^ s_prev_raw;
    uint8_t pressed  = changed &  raw;
    uint8_t released = changed & ~raw;
    s_prev_raw = raw;

	uint8_t i = 0;
    for (i = 0; i < 8; i++) {
        uint8_t m = (1u << i);
        uint8_t down_now = (raw & m) ? 1u : 0u;

        // Debounce (nếu KEY_DEBOUNCE_MS > 0)
        if (KEY_DEBOUNCE_MS > 0) {
            if (((changed & m) == 0)) {
                // không đổi mức, cộng dồn nếu đang down
                if (down_now) s_stable_ms[i] += SCAN_PERIOD_MS;
                else          s_stable_ms[i]  = 0;
            } else {
                // mới đổi mức -> reset bộ đếm ổn định
                s_stable_ms[i] = down_now ? SCAN_PERIOD_MS : 0;
            }
        } else {
            // không dùng debounce phụ
            s_stable_ms[i] = down_now ? 0xFFFF : 0;
        }

        // Xử lý pressed/released (one-shot) & giữ thời gian để nhận biết hold
        if (pressed & m) {
            event_key.pressed |= m;
            s_hold_ms[i] = 0;
        }
        if (released & m) {
            event_key.released |= m;
            s_hold_ms[i] = 0;
            s_hold_mask &= (uint8_t)~m; // thoát hold nếu đang hold
        }

        // Hold: nếu đang nhấn đủ lâu
        if (down_now) {
            // chỉ tính khi đã ổn định (debounce đạt)
            if (s_stable_ms[i] >= KEY_DEBOUNCE_MS) {
                // tích lũy thời gian giữ
                if (s_hold_ms[i] < 0xFFFF - SCAN_PERIOD_MS)
                    s_hold_ms[i] += SCAN_PERIOD_MS;

                if (!(s_hold_mask & m) && s_hold_ms[i] >= KEY_HOLD_MS) {
                    s_hold_mask |= m;     // vào trạng thái hold (levent_keyel)
                    event_key.hold_start |= m;   // one-shot
                }
            }
        }
    }

    // 3) Gán level outputs
    event_key.down = raw;
    event_key.idle = (uint8_t)(~raw) & 0xFF;
    event_key.hold = s_hold_mask;

    return event_key;
}