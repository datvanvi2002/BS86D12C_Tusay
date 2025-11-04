#include "ioConfig.h" 
#include "tm1640.h"


// ===================== MAPPING =====================
// Chỉ SỬA 8 define dưới nếu ký tự hiển thị sai đoạn
// Quy ước: bit0..bit7 là SEG1..SEG8 của TM1640.
// Bạn nên chạy walk-test (hàm bên dưới) để biết A..G,DP đang là bit nào.
#define TM_SEG_BIT_FOR_A   0   // mặc định: SEG1 là 'a'
#define TM_SEG_BIT_FOR_B   1
#define TM_SEG_BIT_FOR_C   2
#define TM_SEG_BIT_FOR_D   3
#define TM_SEG_BIT_FOR_E   4
#define TM_SEG_BIT_FOR_F   5
#define TM_SEG_BIT_FOR_G   6
#define TM_SEG_BIT_FOR_DP  7   // SEG8 là DP

#define _BIT(n)            (1u << (n))
#define SEG_A  _BIT(TM_SEG_BIT_FOR_A)
#define SEG_B  _BIT(TM_SEG_BIT_FOR_B)
#define SEG_C  _BIT(TM_SEG_BIT_FOR_C)
#define SEG_D  _BIT(TM_SEG_BIT_FOR_D)
#define SEG_E  _BIT(TM_SEG_BIT_FOR_E)
#define SEG_F  _BIT(TM_SEG_BIT_FOR_F)
#define SEG_G  _BIT(TM_SEG_BIT_FOR_G)
#define SEG_DP _BIT(TM_SEG_BIT_FOR_DP)

// Số 0..9 (không DP). 1=ON (TM1640 active high)
static const uint8_t seven_seg_digits[10] = {
    /*0*/ SEG_A|SEG_B|SEG_C|SEG_D|SEG_E|SEG_F,
    /*1*/ SEG_B|SEG_C,
    /*2*/ SEG_A|SEG_B|SEG_D|SEG_E|SEG_G,
    /*3*/ SEG_A|SEG_B|SEG_C|SEG_D|SEG_G,
    /*4*/ SEG_F|SEG_G|SEG_B|SEG_C,
    /*5*/ SEG_A|SEG_F|SEG_G|SEG_C|SEG_D,
    /*6*/ SEG_A|SEG_F|SEG_G|SEG_C|SEG_D|SEG_E,
    /*7*/ SEG_A|SEG_B|SEG_C,
    /*8*/ SEG_A|SEG_B|SEG_C|SEG_D|SEG_E|SEG_F|SEG_G,
    /*9*/ SEG_A|SEG_B|SEG_C|SEG_D|SEG_F|SEG_G
};


static inline void tm_delay_short(void) {
    __asm__("nop"); __asm__("nop"); __asm__("nop"); __asm__("nop");
}

// ======= primitives =======
static inline void tm_clk_high(void){ CLK_HIGH(); tm_delay_short(); }
static inline void tm_clk_low (void){ CLK_LOW();  tm_delay_short(); }
static inline void tm_din_high(void){ DIN_HIGH(); tm_delay_short(); }
static inline void tm_din_low (void){ DIN_LOW();  tm_delay_short(); }

static void tm_start(void){
    tm_clk_high(); tm_din_high();
    tm_din_low();
    tm_delay_short();
}

static void tm_stop(void){
    tm_clk_low();
    tm_din_low();
    tm_clk_high();
    tm_din_high(); 
}


static void tm_write_byte(uint8_t b){
    uint8_t i=0;
    for(i = 0;i<8;i++){
        tm_clk_low();
        if (b & 0x01) tm_din_high(); else tm_din_low();
        tm_clk_high();        
        b >>= 1;
    }
    tm_clk_low();
    tm_din_low();
}


static inline void tm_cmd(uint8_t cmd){
    tm_start();
    tm_write_byte(cmd);
    tm_stop();
}

// ======= API  =======

// Xóa toàn bộ 16 địa chỉ (GRID1..GRID16)
void tm1640_clear_all(void){
    tm_start();
    tm_write_byte(0x40);             // Data command: auto-increment
    tm_stop();

    tm_start();
    tm_write_byte(0xC0 | 0x00);      // Address = 0
    uint8_t i=0;
    for(i=0;i<16;i++) tm_write_byte(0x00);
    tm_stop();
}

// Khởi tạo: cấu hình chân, bật hiển thị, đặt độ sáng (0..7)
void tm1640_init(uint8_t brightness_0_to_7){
    if (brightness_0_to_7 > 7) brightness_0_to_7 = 7;

    // IO mức nhàn rỗi: CLK=HIGH, DIN=HIGH
    DIN_OUTPUT(); CLK_OUTPUT();
    tm_clk_high(); tm_din_high();

    // Xóa và bật hiển thị với độ sáng
    tm1640_clear_all();
    tm_cmd(0x88 | brightness_0_to_7);   // Display ON + brightness
}

// Ghi 1 byte theo địa chỉ cố định (địa chỉ 0..0x0F)
void tm1640_write_fixed(uint8_t addr, uint8_t seg_bits){
    addr &= 0x0F;

    tm_cmd(0x44);                       // Data command: fixed address
    tm_start();
    tm_write_byte(0xC0 | addr);         // Address command
    tm_write_byte(seg_bits);            // B0..B7 -> SEG1..SEG8
    tm_stop();
}

// Ghi nhiều byte ở chế độ auto-increment từ start_addr
void tm1640_write_auto(uint8_t start_addr, const uint8_t *data, uint8_t len){
    start_addr &= 0x0F;

    tm_cmd(0x40);                       // auto-increment
    tm_start();
    tm_write_byte(0xC0 | start_addr);
    uint8_t i=0;
    for(i=0;i<len;i++) tm_write_byte(data[i]);
    tm_stop();
}

void tm1640_write_digit(uint8_t addr, uint8_t digit, _Bool with_dp){
    if (digit > 9) digit = 0; // chỉ hiển thị 0..9
    if (with_dp) {
        tm1640_write_fixed(addr, seven_seg_digits[digit] | SEG_DP);
    } else {
        tm1640_write_fixed(addr, seven_seg_digits[digit]);
    }
}

/// @brief 
/// Temperature show LED 1: Grid 1 Grid 2 and Grid 3 ///
/// Times show LED 2: Grid 4 Grid 5 and Grid 6 ///
/// Pressure show LED 3: Grid 7 Grid 8 and Grid 9 ///
/// @param  led_number  1 show Temperature; 2 show Times; 3 show Pressure
/// @param  number 0,00 .. 999 (float: 2 digits after the decimal point)
void tm1640_write_led(uint8_t led_number, float number)
{
    if (number < 0.0f) number = 0.0f;
    if (number > 999.0f) number = 999.0f;

    _Bool with_dp_1 = 0; // dp for hundreds digit
    _Bool with_dp_2 = 0; // dp for tens digit

    if(number < 10.0f)
    {
        with_dp_1 = 1;
        with_dp_2 = 0;
    }
    else if(number < 100.0f)
    {
        with_dp_1 = 0;
        with_dp_2 = 1;
    }
    else
    {
        with_dp_1 = 0;
        with_dp_2 = 0;
    }
    
    if(number < 10.0f)
    {
        number = number * 100.0f;
    } else if(number < 100)
    {
        number = number * 10.0f;
    } else
    {
        number = number * 1.0f;
    }
    uint8_t digit_1 = ((uint16_t)number / 100) % 10; // hundreds
    uint8_t digit_2 = ((uint16_t)number / 10) % 10;  // tens
    uint8_t digit_3 = ((uint16_t)number / 1) % 10;   // units
  
    switch (led_number)
    {
        case 1: // Temperature show LED
            tm1640_write_digit(0, digit_1, with_dp_1); // Grid 1
            tm1640_write_digit(1, digit_2, with_dp_2); // Grid 2
            tm1640_write_digit(2, digit_3, 0);         // Grid 3
            break;
        case 2: // Times show LED
            tm1640_write_digit(3, digit_1, with_dp_1); // Grid 4
            tm1640_write_digit(4, digit_2, with_dp_2); // Grid 5
            tm1640_write_digit(5, digit_3, 0);         // Grid 6
            break;
        case 3: // Pressure show LED
            tm1640_write_digit(6, digit_1, with_dp_1); // Grid 7
            tm1640_write_digit(7, digit_2, with_dp_2); // Grid 8
            tm1640_write_digit(8, digit_3, 0);         // Grid 9
            break;
        default:
            // Invalid led_number; do nothing or handle error as needed
            break;
    }
    // uint8_t hundreds = number / 100;
    // uint8_t tens = (number / 10) % 10;
    // uint8_t units = number % 10;

    // switch (led_number)
    // {
    //     case 1: // Temperature show LED
    //         tm1640_write_digit(0, hundreds, 0); // Grid 1
    //         tm1640_write_digit(1, tens, 1);     // Grid 2 with DP
    //         tm1640_write_digit(2, units, 0);    // Grid 3
    //         break;
    //     case 2: // Times show LED
    //         tm1640_write_digit(3, hundreds, 0); // Grid 4
    //         tm1640_write_digit(4, tens, 1);     // Grid 5 with DP
    //         tm1640_write_digit(5, units, 0);    // Grid 6
    //         break;
    //     case 3: // Pressure show LED
    //         tm1640_write_digit(6, hundreds, 0); // Grid 7
    //         tm1640_write_digit(7, tens, 1);     // Grid 8 with DP
    //         tm1640_write_digit(8, units, 0);    // Grid 9
    //         break;
    //     default:
    //         // Invalid led_number; do nothing or handle error as needed
    //         break;
    // }
}

// Nếu grid thứ tự vật lý bị đảo, chỉnh mảng này (0..15 là địa chỉ TM1640)
uint8_t TM1640_grid_order[16] = {
    0,1,2, 3,4,5, 6,7,8, 9,10,11, 12,13,14,15
};

// DEMO: hiển thị 1..9 ở GRID1..GRID9
void tm1640_show_1_to_9_on_grid1_to_9(void){
    uint8_t buf[9];
    uint8_t i=0;
    for(i=0;i<9;i++){
        buf[i] = seven_seg_digits[i+1]; // số 1..9
    }
    for(i=0;i<9;i++) tm1640_write_fixed(TM1640_grid_order[i], buf[i]);
    //tm1640_write_auto(0x00, buf, 9);    // địa chỉ 0..8 -> GRID1..9
}

void tm1640_walk_segments(uint8_t grid, uint16_t on_ms){
	uint8_t b=0;
	volatile uint16_t i,d;
    grid &= 0x0F;


    tm1640_clear_all();               // đảm bảo mọi thứ tắt sạch
    tm1640_write_fixed(grid, 0x00);   // blank tại grid này
    // nhỏ: chèn 1 nhịp trễ để thấy rõ "tắt hẳn" rồi mới bật A
    for(d=0; d<8000; d++) __asm__("nop");

    for(b=0;b<8;b++){
        tm1640_write_fixed(grid, (1u<<b));   // bật đúng 1 SEG (bit0..7 = SEG1..8)
        // giữ on_ms (thô @8MHz)
        uint16_t ms = on_ms;
        while(ms--){
            __asm__("clr wdt");
            for(i=0;i<500;i++) __asm__("nop");
        }
        tm1640_write_fixed(grid, 0x00);      // tắt SEG trước khi sang SEG kế tiếp
    }
}

// Bật duy nhất SEG A và lần lượt chạy qua addr 0..15
void tm1640_walk_grids_a(uint16_t hold_ms){
    tm1640_clear_all();
    uint8_t addr=0;
    volatile uint16_t i=0;
    for(addr=0; addr<16; addr++){
        tm1640_write_fixed(addr, (1u<<TM_SEG_BIT_FOR_A)); // chỉ SEG A
        // giữ hold_ms rồi tắt để chuyển sang grid tiếp theo
        uint16_t ms = hold_ms;
        while(ms--){
            __asm__("clr wdt");
            for(i=0;i<500;i++) __asm__("nop");
        }
        tm1640_write_fixed(addr, 0x00);
    }
}
