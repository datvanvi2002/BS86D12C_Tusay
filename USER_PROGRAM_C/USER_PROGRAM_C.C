/////******************************************Disclaimer****************************************************//**
/////*The material offered by Holtek Semiconductor Inc. (including its subsidiaries, hereinafter
/////*collectively referred to as “HOLTEK”), including but not limited to technical documentation and
/////*code, is provided “as is”, only for your reference, and may be superseded by updates. HOLTEK
/////*reserves the right to revise the offered material at any time without prior notice. You shall use the
/////*offered material at your own risk. HOLTEK disclaims any expressed, implied, or statutory warranties,
/////*including but not limited to accuracy, suitability for commercialization, satisfactory quality,
/////*specifications, characteristics, functions, fitness for a particular purpose, and non-infringement of
/////*any third-party’s rights. HOLTEK disclaims all liability arising from the offered material and its
/////*application. In addition, HOLTEK does not recommend the use of HOLTEK’s products where there is
/////*a risk of personal hazard due to malfunction or other reasons. HOLTEK hereby declares that it does
/////*not authorize the use of these products in life-saving, life-sustaining, or safety-critical components.
/////*Any use of HOLTEK’s products in life-saving, sustaining, or safety applications is entirely at your risk,
/////*and you agree to defend, indemnify, and hold HOLTEK harmless from any damages, claims, suits, or
/////*expenses resulting from such use.
/////************************************************************************************************************/

//////***********************************Intellectual Property*************************************************//**
/////*The offered material, including but not limited to the content, data, examples, materials, graphs,
/////*and trademarks, is the intellectual property of HOLTEK (and its licensors, where applicable) and is
/////*protected by copyright law and other intellectual property laws.
/////************************************************************************************************************/

#include "USER_PROGRAM_C.INC"
#include "adc.h"
#include "tm1640.h"
#include "uart.h"
#include "ioConfig.h"
#include <string.h>
#include "BS86D12C.h"
#include "keyIO.h"
#include "user_config.h"

static SupState prg_state = SUP_BOOT;
system_init_t system_config = {0};
sterilization_mode_t sterilization_mode = STERILIZATION_NULL;
static UiField ui_led2 = UI_IDLE;

uint32_t g_time_tik_10ms = 0;
void delay_ms(unsigned int ms)
{
    unsigned long i;
    while (ms--)
    {
        for (i = 250; i > 0; i--)
        {
            GCC_NOP();
        }
        __asm__("clr wdt");
    }
}
static inline void buzzer_start_ms(uint16_t ms);
static inline void buzzer_start_blink_ms(uint16_t ms, uint8_t blink_cnt);
void SystemInit(void)
{
    // //watchdog timer setup
    // _wdtc = (_wdtc & ~0x07) | 0x07;

    // IO init
    io_init_outputs(); // PC0..PC3, PD5, PA4 -> output & OFF
    adc_init_vdd_ref();
    uart0_init(); // TX debug init

    // init TM1640
    tm1640_init(7); // brightness 0..7

    system_config.temp_setting = 121;
    system_config.time_steri = 0;
    system_config.time_pressure_release_1 = 0;
    system_config.time_pressure_release_2 = 0;
    system_config.time_water_release = 0;
    system_config.time_drying = 0;
}
////==============================================
////**********************************************
////==============================================
// void __attribute((interrupt(0x04))) Interrupt_Extemal(void)
//{
//	//Insert your code here
// }

//==============================================
//**********************************************
//==============================================
void USER_PROGRAM_C_INITIAL()
{
    // Executes the USER_PROGRAM_C initialization function once
    SystemInit();
    // buzzer_start_ms(STARTUP_BUZZ_MS);
    //  tm1640_walk_grids_a(2);
    int i = 0;
    for (i = 0; i < 11; i++)
    {
        tm1640_walk_segments(i, 1);
    }

    tm1640_show_1_to_9_on_grid1_to_9();
    tm1640_write_end(2);
    tm1640_write_led(1, 321);
    tm1640_write_err(3);
    uart0_send_byte('\n');
    uart0_send_string("HELLO From BS86D12C\n");
    buzzer_start_blink_ms(1000, 3);
}

//==============================================
//**********************************************

//==============================================

//==============================================
static volatile uint8_t buzzer_ticks = 0;
static inline void buzzer_start_ms(uint16_t ms)
{
    uint8_t t = (ms + 9) / 10;
    if (t == 0)
        t = 1;
    buzzer_ticks = t;
    BUZZ_ON();
}
// g_time_tik_10ms
static uint16_t t_blink = 0;
static uint16_t buzzer_blink = 0;
static inline void buzzer_start_blink_ms(uint16_t ms, uint8_t blink_cnt)
{
    t_blink = (int)(ms / 2);
    if (t_blink < 10)
        t_blink = 10; // min 10ms
    buzzer_blink = blink_cnt * 2;
}
uint8_t buzze_blink = 0;
static inline void buzzer_service(void)
{
    static uint32_t tik_blink = 0;
    if (buzzer_ticks && !buzzer_blink)
    {
        if (--buzzer_ticks == 0)
            BUZZ_OFF();
        return;
    }
    else if (!buzzer_blink)
        BUZZ_OFF();

    if (buzzer_blink)
    {

        if (g_time_tik_10ms - tik_blink > (int)(t_blink / 10))
        {
            buzzer_blink--;
            uart0_send_number(buzzer_blink);
            tik_blink = g_time_tik_10ms;
            buzze_blink++;
            buzze_blink = (buzze_blink > 254) ? 0 : buzze_blink;
            if (buzze_blink % 2)
            {
                BUZZ_ON();
            }
            else
            {
                BUZZ_OFF();
            }
        }
    }
}

//==============================================

//==============================================
led1_mode_t ui_led1 = UI_TEMP_NOW;
static uint8_t key23_state = 0;
static uint8_t key56_state = 0;
static uint16_t key_led_timer = 0; // *63ms

static inline void key_off_time()
{
    if (key_led_timer)
    {
        key_led_timer--;
    }
    else
    {
        key23_state = 0; // hết thời gian thì tắt LED
        key56_state = 0;
    }
}

uint16_t _limit_data(uint16_t v, uint16_t lo, uint16_t hi)
{
    if (v < lo)
        return lo;
    if (v > hi)
        return hi;
    return v;
}
void blink_err(uint32_t g_millis, uint8_t led_num)
{
    static bit err_blink = 0;
    if (g_millis % 100 == 0)
    {
        if (err_blink)
        {
            tm1640_clear_led(led_num);
            err_blink = 1;
            BUZZ_ON();
        }
        else
        {
            tm1640_write_err(led_num);
            err_blink = 0;
            BUZZ_OFF();
        }
    }
}
void caculate_time_setup(int delta)
{
    switch (ui_led2)
    {
    case UI_SET_T1:
        if (system_config.time_steri == 0 && delta < 0)
            break;
        system_config.time_steri = _limit_data(system_config.time_steri + delta, 0, 999);
        break;
    case UI_SET_T21:
        if (system_config.time_pressure_release_1 == 0 && delta < 0)
            break;
        system_config.time_pressure_release_1 = _limit_data(system_config.time_pressure_release_1 + delta, 0, 999);
        break;
    case UI_SET_T22:
        if (system_config.time_pressure_release_2 == 0 && delta < 0)
            break;
        system_config.time_pressure_release_2 = _limit_data(system_config.time_pressure_release_2 + delta, 0, 999);
        break;
    case UI_SET_T3:
        if (system_config.time_water_release == 0 && delta < 0)
            break;
        system_config.time_water_release = _limit_data(system_config.time_water_release + delta, 0, 999);
        break;
    case UI_SET_T4:
        if (system_config.time_drying == 0 && delta < 0)
            break;
        system_config.time_drying = _limit_data(system_config.time_drying + delta, 0, 999);
        break;
    default:
        break;
    }
}

sterilization_mode_t check_mode_process(system_init_t system)
{
    if (!system.time_steri)
        return STERILIZATION_NULL;
    // T4 T3 T21 T22
    if (system.time_drying && system.time_water_release && system.time_pressure_release_1 && system.time_pressure_release_2)
        return STERILIZATION_4;
    // T21
    if (!system.time_drying && !system.time_water_release && system.time_pressure_release_1 && !system.time_pressure_release_2)
        return STERILIZATION_3;
    // T22 T21
    if (!system.time_drying && !system.time_water_release && system.time_pressure_release_1 && system.time_pressure_release_2)
        return STERILIZATION_2;

    if (!system.time_drying && !system.time_water_release && !system.time_pressure_release_1 && !system.time_pressure_release_2)
        return STERILIZATION_1;
    // other
    return STERILIZATION_NULL;
}
bit danger_temp = 0; // 0 OKE, any Danger
bit quick_mode = 0;
#define STEP_PRESS_TIME 10 // ms
void key_handle_service(KeyEvents ev)
{
    /*
    Key 1: Vào chế độ cài nhiệt độ
    Key 2: cài nhiệt độ tiệt trung là 121
    Key 2: cài nhiệt độ tiệt trùng là 134
    */
    if (ev.pressed)
    {
        buzzer_start_ms(30);
        key_led_timer = 80; // 50*63 == 5s
    }
    if (ev.hold)
    {
        buzzer_start_ms(30);
        key_led_timer = 1;
    }

    //==================Handle Led 1: KEY 1  2  3============================
    if (ev.pressed & KEYBIT(SETTING_TEMP_KEY))
    {
        /*
        Nhấn 1 lần: Vào chế độ cài đặt nhiệt độ
        Đèn key1 luôn sáng
        Nhấn lần nữa sẽ tắt đèn key 1 và thoat chế độ cài đặt nhiệt độ
        */
        if (ui_led1 == UI_TEMP_SETTING)
        {
            ui_led1 = UI_TEMP_NOW;
        }
        else
        {
            ui_led1 = UI_TEMP_SETTING;
        }
        key23_state = 0;
    }

    if (ui_led1 == UI_TEMP_SETTING)
    {
        /*
        Nếu đang chế độ cài đặt nhiệt độ thì mới đổi nhiệt độ
        KEY 3 = 134 KEY 2 = 121 deg C
        Khi ấn key nào thì key đó sáng
        */
        if (ev.pressed & KEYBIT(TEMP_UP_KEY))
        {
            system_config.temp_setting++;
            system_config.temp_setting = system_config.temp_setting > 134 ? 134 : system_config.temp_setting;
            key23_state |= (1 << 1);
            key23_state &= ~(1 << 0);
        }
        else if (ev.pressed & KEYBIT(TEMP_DOWN_KEY))
        {
            system_config.temp_setting--;
            system_config.temp_setting = system_config.temp_setting < 121 ? 121 : system_config.temp_setting;
            key23_state |= (1 << 0);
            key23_state &= ~(1 << 1);
        }
        else if (ev.hold & KEYBIT(TEMP_DOWN_KEY) && ev.hold | KEYBIT(TEMP_UP_KEY))
        {
            key23_state |= (1 << 0);
            key23_state &= ~(1 << 1);
            if (g_time_tik_10ms % STEP_PRESS_TIME == 0) // 50ms + 1
            {
                system_config.temp_setting--;
                system_config.temp_setting = system_config.temp_setting < 121 ? 121 : system_config.temp_setting;
            }
        }
        else if (ev.hold & KEYBIT(TEMP_UP_KEY) && ev.hold | KEYBIT(TEMP_DOWN_KEY))
        {
            key23_state |= (1 << 1);
            key23_state &= ~(1 << 0);
            if (g_time_tik_10ms % STEP_PRESS_TIME == 0) // 50ms + 1
            {
                system_config.temp_setting++;
                system_config.temp_setting = system_config.temp_setting > 134 ? 134 : system_config.temp_setting;
            }
        }
    }

    //==================Handle Led 2: KEY 4  5  6============================
    if (ev.pressed & KEYBIT(SETTING_TIME_KEY))
    {
        key56_state = 0;
        switch (ui_led2)
        {
        case UI_SET_T1:
            ui_led2 = UI_SET_T21;
            buzzer_start_blink_ms(150, 2);
            break;
        case UI_SET_T21:
            ui_led2 = UI_SET_T22;
            buzzer_start_blink_ms(150, 3);
            break;
        case UI_SET_T22:
            ui_led2 = UI_SET_T3;
            buzzer_start_blink_ms(150, 4);
            break;
        case UI_SET_T3:
            ui_led2 = UI_SET_T4;
            buzzer_start_blink_ms(150, 5);
            break;
        case UI_SET_T4:
            ui_led2 = UI_IDLE;
            break;
        default:
            ui_led2 = UI_SET_T1;
            buzzer_start_blink_ms(150, 1);
            break; // từ Idle/khác => T1
        }
    }

    if (ui_led2 != UI_IDLE)
    {
        if (ev.pressed & KEYBIT(TIME_UP_KEY))
        {
            caculate_time_setup(+1);
            key56_state |= (1 << 1);
            key56_state &= ~(1 << 0);
        }
        else if (ev.pressed & KEYBIT(TIME_DOWN_KEY))
        {
            caculate_time_setup(-1);
            key56_state |= (1 << 0);
            key56_state &= ~(1 << 1);
        }
        else if (ev.hold & KEYBIT(TIME_UP_KEY) && ev.hold | KEYBIT(TIME_DOWN_KEY))
        {
            key56_state |= (1 << 1);
            key56_state &= ~(1 << 0);
            if (g_time_tik_10ms % STEP_PRESS_TIME == 0) // 50ms + 1
            {
                caculate_time_setup(+1);
            }
        }
        else if (ev.hold & KEYBIT(TIME_DOWN_KEY) && ev.hold | KEYBIT(TIME_UP_KEY))
        {
            if (g_time_tik_10ms % STEP_PRESS_TIME == 0)
            {
                caculate_time_setup(-1);
                key56_state |= (1 << 0);
                key56_state &= ~(1 << 1);
            }
        }
    }

    //==================Handle KEY 7: START_KEY============================
    if (ev.pressed & KEYBIT(QUICK_KEY))
    {
        static uint16_t count = 0;
        count++;
        if (count % 3 == 0)
        {
            sterilization_mode = STERILIZATION_NULL;
        }
        sterilization_mode = QUICK_MODE;
        system_config.temp_setting = (system_config.temp_setting == 121) ? 134 : 121;
    }

    //==================Handle KEY 8: START_KEY============================
    if (ev.pressed & KEYBIT(START_KEY))
    {
        if (danger_temp)
        {
            danger_temp = 0; // clear danger_temp;
            return;
        }

        if (sterilization_mode == QUICK_MODE)
        {
            // start quick mode
            sterilization_mode = STERILIZATION_NULL;
        }
        else
        {
            sterilization_mode = check_mode_process(system_config);
            uart0_send_number(system_config.time_steri);
            uart0_send_number(system_config.time_pressure_release_1);
            uart0_send_number(system_config.time_pressure_release_2);
            uart0_send_number(system_config.time_water_release);
            uart0_send_number(system_config.time_drying);

            uart0_send_number(sterilization_mode);
            uart0_send_byte('\n');
            if (!sterilization_mode)
            {
                // uart0_send_number(sterilization_mode);
                tm1640_write_err(2);
            }
        }
    }
}

void main_handle_servie()
{
    uint8_t Ta = (int)((adc_read_channel(1) * 150) / 4098);

    static uint8_t boot_started = 0;
    static uint32_t tik_sup = 0;

    switch (prg_state)
    {
    case SUP_BOOT:
        /* code */

        if (!boot_started)
        {
            boot_started = 1;
            // A=B=C=D=0
            RELAYA_OFF();
            TRIACB_OFF();
            TRIACC_OFF();
            RELAYD_OFF();
            // E=1 trong 3s
            buzzer_start_ms(STARTUP_BUZZ_MS);
            tik_sup = g_time_tik_10ms;
        }
        if (g_time_tik_10ms - tik_sup > STARTUP_BUZZ_MS / 10)
        {
            if (Ta > TEMP_STARTUP_LOCK)
            {
                prg_state = SUP_LOCKED;
                TRIACB_ON();
                tik_sup = g_time_tik_10ms;
                tm1640_write_err(1);
            }
            else
            {
                prg_state = SUP_IDLE;
            }
        }
        break;
    case SUP_LOCKED:
        /* code */
        if (g_time_tik_10ms - tik_sup > VENT_ON_STARTUP_MS / 10)
        {
            if (Ta < TEMP_STARTUP_LOCK)
            {
                tm1640_clear_led(1);
                BUZZ_OFF();
                prg_state = SUP_IDLE;
            }
        }

        // blink 1s err and beep
        blink_err(g_time_tik_10ms, 1);

        break;

    case SUP_IDLE:
        /* code */
        if (Ta > TEMP_DANGER_C)
        {
            prg_state = SUP_DANGER;
            RELAYA_OFF();
            RELAYD_OFF();
            TRIACB_ON();
            danger_temp = 1;
        }
        break;
    case SUP_DANGER:
        /* code */
        tm1640_write_led(1, Ta);
        blink_err(g_time_tik_10ms, 2);

        if (Ta < TEMP_SAFE_EXIT_C || !danger_temp)
        {
            prg_state = SUP_IDLE;
        }
        break;
    default:
        break;
    }

    if (prg_state == SUP_IDLE)
    {
        switch (sterilization_mode)
        {
        case STERILIZATION_1:
            /* code */
            break;
        case STERILIZATION_2:
            /* code */
            break;
        case STERILIZATION_3:
            /* code */
            break;
        case STERILIZATION_4:
            /* code */
            break;
        case STERILIZATION_4:
            /* code */
            break;
        case STERILIZATION_NULL:
            /* code */
            break;
        case QUICK_MODE:
            /* code */
            break;
        default:
            break;
        }
    }
}

void led_handle()
{
}
/*
"🔹Tiệt trùng 1: ( A =1, B = C = D =E = 0 )  không xả áp, không xả nước, không sấy
Thiết bị sử dụng: Relay A (điện trở):
- Quy trình:
Cài đặt nhiệt độ tiệt trùng Ta (121°C hoặc 134°C) và thời gian tiệt trùng T1 .
Nhấn nút START để bắt đầu.
Relay A bật → gia nhiệt đến nhiệt độ cài đặt.
Khi đạt nhiệt độ →  bắt đầu đếm thời lùi gian tiệt trùng ( Thời gian này được cài đặt ).
Kết thúc thời gian tiệt trùng T1 = 0 ( vì đếm ngược ) → relay A tắt ( A = 0 ) - Bật chuông báo ( E = 1 trong 60s rồi E về 0, Khi nhiệt độ dưới 70 độ C )  → Khi kết thúc chương trình hiển thị End."

*/
typedef enum
{
    START_STERI = 0,
    HEATING_1, // chờ gia nhiệt
    STERING_1, // sau khi gia nhiệt xong, chờ tiệt trùng xong
    END_STERI_1,
    STOP_STERI_1,
} steri_1_status_t;

static steri_1_status_t steri1_stt = STOP_STERI_1;
static uint16_t steri1_T1_left_s = 0;
static uint32_t steri1_tick_1s = 0;
static uint8_t steri1_running = 0;

void sterilization_1_handle(void)
{

    switch (steri1_stt)
    {
    case START_STERI:      
        RELAYA_ON();
        steri1_stt = HEATING_1;
        steri1_T1_left_s = system_config.time_steri;
        steri1_tick_1s = g_time_tik_10ms;
        steri1_running = 1;
        /* code */
        break;
    case HEATING_1:
        /* code */
        uint8_t temp = temperature_sensor_read();
        if (temp >= system_config.temp_setting)
        {
            steri1_stt = STERING_1;
        }
        start_steri_1 = g_time_tik_10ms;

        break;
    case STERING_1:
        /* COUNTDOWN 1 phut */
        if (g_time_tik_10ms - start_steri_1 >= 6000)
        {
            start_steri_1 = g_time_tik_10ms;
            steri_time--;
            /* show time steri led 2 task*/
        }
        if (!steri_time)
        {
            steri1_stt = END_STERI_1;
        }
        break;
    case END_STERI_1:
        /* code */
        uint8_t temp = temperature_sensor_read();
        if (temp <= 70)
        {
            RELAYA_OFF();
            buzzer_start_ms(60 * 1000);
            // show end led 2 task
            steri1_stt = STOP_STERI_1;
        }
        break;

    case STOP_STERI_1:
        steri1_running = 0;
        RELAYA_OFF();
        break;
    default:
        break;
    }
}

/*
"🔹Tiệt trùng 2 ( A =  1, B = C = D = E = 0) có xả áp, không xả nước, không sấy
Thiết bị sử dụng: Relay A (điện trở), Relay B (van xả khí)

Quy trình:
Cài đặt nhiệt độ tiệt trùng Ta (121°C hoặc 134°C),  thời gian tiệt trùng T1, thời gian xả áp lần T21.T22
Nhấn nút START để bắt đầu.
Relay A bật → gia nhiệt đến nhiệt độ cài đặt .
Khi đạt nhiệt độ → relay B mở van xả khí ( B = 1)  trong thời gian cài đặt.
Sau khi xả khí ( Hết thời gian cài đặt xả )→ đóng relay B ( B = 0)
 → tiếp tục gia nhiệt đến nhiệt độ đã cài đặt và bắt đầu đếm thời gian tiệt trùng.
Kết thúc thời gian tiệt trùng (T1 đếm về 0) → relay A tắt ( A = 0)
  → relay B mở lần 2 ( B = 1)  để xả áp đến khi nhiệt độ xuống dưới 70°C, T22  đếm ngược, T22 không đếm khi Temp = 40 độ C.
  Bật chuông báo trong 60 giây (E = 1) hết thời gian 60s tắt chuông báo ( E = 0) → Khi kết thúc chương trình hiển thị End."

*/
typedef enum
{
    START_STERI_2 = 0,
    HEATING_21, // chờ gia nhiệt
    RELEASE_21, // sau khi gia nhiệt xong, chờ tiệt trùng xong
    HEATING_22,
    STERING_2,
    RELEASE_22,
    END_STERI_2,
    STOP_STERI_2,
} steri_2_status_t;

static steri_2_status_t steri2_stt = STOP_STERI_2;
static uint16_t steri2_T1_left_s = 0;
static uint16_t steri2_T21_left_s = 0;
static uint16_t steri2_T22_left_s = 0;
static uint32_t steri2_tick_1s = 0;
static uint8_t steri2_running = 0;

void sterilization_2_handle(void)
{
    if (!steri2_running)
    {
        steri2_stt = STOP_STERI_2;
    }

    switch (steri2_stt)
    {
    case START_STERI_2:
        RELAYA_ON();  // A = 1
        TRIACB_OFF(); // B = 0
        steri2_stt = HEATING_21;

        steri2_T1_left_s = system_config.time_steri;
        steri2_T21_left_s = system_config.time_pressure_release_1;
        steri2_T22_left_s = system_config.time_pressure_release_2; // có thể =0 nếu chỉ dùng để hiển thị
        steri2_tick_1s = g_time_tik_10ms;
        steri2_running = 1;
        steri2_stt = START_STERI_2;
        break;

    case HEATING_21:
    {
        uint8_t temp = temperature_sensor_read();
        if (temp >= system_config.temp_setting)
        {
            // Đạt nhiệt độ cài đặt -> mở B xả lần 1 trong T21
            steri2_tick_1s = g_time_tik_10ms;
            TRIACB_ON(); // B = 1
            steri2_stt = RELEASE_21;
        }
    }
    break;

    case RELEASE_21:
        if (g_time_tik_10ms - steri2_tick_1s >= 100)
        {
            steri2_tick_1s = g_time_tik_10ms;
            if (steri2_T21_left_s > 0)
            {
                steri2_T21_left_s--;

                /* Show time count down*/
            }
        }
        if (steri2_T21_left_s == 0)
        {
            TRIACB_OFF(); // B = 0
            steri2_stt = HEATING_22;
        }
        break;

    case HEATING_22:
    {
        // Sau khi đóng B, tiếp tục gia nhiệt lại tới nhiệt độ cài đặt rồi bắt đầu T1
        uint8_t temp = temperature_sensor_read();
        if (temp >= system_config.temp_setting)
        {
            steri2_tick_1s = g_time_tik_10ms;
            steri2_stt = STERING_2;
        }
    }
    break;

    case STERING_2:
        // Đếm T1
        if (g_time_tik_10ms - steri2_tick_1s >= 100)
        {
            steri2_tick_1s = g_time_tik_10ms;
            if (steri2_T1_left_s > 0)
            {
                steri2_T1_left_s--;
                /* Show time count down*/
            }
        }
        if (steri2_T1_left_s == 0)
        {
            // Hết T1: A = 0, B = 1; bật còi 60s
            RELAYA_OFF(); // A = 0
            TRIACB_ON();  // B = 1 (xả lần 2)

            steri2_tick_1s = g_time_tik_10ms;
            steri2_stt = RELEASE_22;
        }
        break;

    case RELEASE_22:
    {
        uint8_t temp = temperature_sensor_read();

        if (temp > 70)
        {
            break;
        }
        // T22 đếm ngược (chỉ để hiển thị/giới hạn), KHÔNG đếm khi Temp <= 40°C
        if (g_time_tik_10ms - steri2_tick_1s >= 100)
        {
            steri2_tick_1s = g_time_tik_10ms;

            if ((temp < 40 || steri2_T22_left_s == 0))
            {
                steri2_stt = END_STERI_2;
                TRIACB_OFF();
            }
            else
            {
                steri2_T22_left_s--;
            }
            /* Show time count down*/
        }
    }
    break;

    case END_STERI_2:
        steri2_running = 0;
        buzzer_start_ms(60 * 1000);
        steri2_stt = STOP_STERI_2;
        break;

    case STOP_STERI_2:
        steri2_running = 0;
        RELAYA_OFF();
        TRIACB_OFF();
        break;
    default:
        RELAYA_OFF();
        TRIACB_OFF();
        break;
    }
}

typedef enum {
    START_STERI_3 = 0,
    HEATING_3,      // A=1, chờ đạt Ta
    VENT_AND_STER3, // B=1 trong T21 & đếm T1 song song; khi T21=0 -> B=0, tiếp tục T1
    FINISH_3,       // T1=0 -> A=0, C=1, kêu 60s; quản lý ngưỡng 40/70°C
    STOP_STERI_3,
} steri_3_status_t;

static steri_3_status_t steri3_stt = STOP_STERI_3;
static uint16_t steri3_T1_left_s  = 0;
static uint16_t steri3_T21_left_s = 0;
static uint32_t steri3_tick_1s    = 0;
static uint8_t  steri3_running    = 0;

void sterilization_3_start(void)
{
    if (steri3_stt == STOP_STERI_3) {
        steri3_T1_left_s  = system_config.time_steri;     // T1
        steri3_T21_left_s = system_config.time_pressure_release_1;  // T21
        steri3_tick_1s    = g_time_tik_10ms;
        steri3_running    = 1;
        steri3_stt        = START_STERI_3;
        ui_show_running();
    }
}

void sterilization_3_stop(void)
{
    steri3_running = 0;
    steri3_stt     = STOP_STERI_3;
    RELAYA_OFF();
    TRIACB_OFF();
    TRIACC_OFF();
}

void sterilization_3_handle(void)
{
    if (!steri3_running) { steri3_stt = STOP_STERI_3; }

    switch (steri3_stt)
    {
    case START_STERI_3:
        RELAYA_ON();       // A=1
        TRIACB_OFF();      // B=0
        TRIACC_OFF();      // C=0
        steri3_stt = HEATING_3;
        break;

    case HEATING_3: {
        uint8_t temp = temperature_sensor_read();
        ui_show_mode3_Temp(temp);
        if (temp >= system_config.temp_setting) {
            // Bắt đầu đếm T1; mở B và bắt đầu đếm T21 song song
            TRIACB_ON();                       // B=1 (xả áp)
            steri3_tick_1s = g_time_tik_10ms;
            steri3_stt     = VENT_AND_STER3;
        }
        } break;

    case VENT_AND_STER3: {
        uint8_t temp = temperature_sensor_read();
        ui_show_mode3_Temp(temp);

        if (g_time_tik_10ms - steri3_tick_1s >= 100) {
            steri3_tick_1s = g_time_tik_10ms;

            // Đếm T1
            if (steri3_T1_left_s > 0) {
                steri3_T1_left_s--;
                ui_show_mode3_T1(steri3_T1_left_s);
            }

            // Đếm T21 cho B
            if (steri3_T21_left_s > 0) {
                steri3_T21_left_s--;
                ui_show_mode3_T21(steri3_T21_left_s);
                if (steri3_T21_left_s == 0) {
                    TRIACB_OFF(); // hết xả áp lần 1
                }
            }
        }

        if (steri3_T1_left_s == 0) {
            // Kết thúc tiệt trùng
            RELAYA_OFF();           // A=0
            TRIACC_ON();            // C=1 (xả nước không giới hạn)
            buzzer_start_ms(60*1000);
            steri3_stt = FINISH_3;
        }
        } break;

    case FINISH_3: {
        uint8_t temp = temperature_sensor_read();
        ui_show_mode3_Temp(temp);

        // C đóng khi ≤ 40°C
        if (temp <= 40) {
            TRIACC_OFF();
        }
        // Kết thúc chu trình khi < 70°C
        if (temp < 70) {
            ui_show_end();
            steri3_running = 0;
            steri3_stt     = STOP_STERI_3;
        }
        } break;

    case STOP_STERI_3:
    default:
        RELAYA_OFF();
        TRIACB_OFF();
        // C có thể vẫn ON nếu người dùng chưa về 40°C và chưa tắt nguồn; nhưng ở STOP ta đảm bảo OFF:
        // Nếu bạn muốn giữ nguyên hành vi "C không tắt đến khi tắt nguồn", hãy bỏ dòng sau.
        TRIACC_OFF();
        break;
    }
}


typedef enum {
    START_STERI_4 = 0,
    HEATING_41,     // A=1, chờ đạt Ta
    VENT_AND_STER4, // B=1 trong T21 & đếm T1 song song
    DRAIN_4,        // Sau T1=0: C=1, đếm T22; hết T22 -> C=0, D=1
    DRY_HEAT_4,     // D=1, gia nhiệt tới Tb
    DRY_HOLD_4,     // Duy trì Tb trong T4
    END_STERI_4,
    STOP_STERI_4,
} steri_4_status_t;

static steri_4_status_t steri4_stt = STOP_STERI_4;
static uint16_t steri4_T1_left_s  = 0;
static uint16_t steri4_T21_left_s = 0;
static uint16_t steri4_T22_left_s = 0;  // dùng để giữ C=1
static uint16_t steri4_T3_left_s  = 0;  // nếu muốn dùng T3 cho xả nước, thay thế cho T22
static uint16_t steri4_T4_left_s  = 0;
static uint32_t steri4_tick_1s    = 0;
static uint8_t  steri4_running    = 0;

void sterilization_4_start(void)
{
    if (steri4_stt == STOP_STERI_4) {
        steri4_T1_left_s  = system_config.time_steri;     // T1
        steri4_T21_left_s = system_config.time_pressure_release_1;  // T21
        steri4_T22_left_s = system_config.time_pressure_release_2;  // T22 (điều kiện đóng C & bắt đầu sấy)
        steri4_T3_left_s  = system_config.time_water_release;     // T3 (tùy chọn, nếu muốn)
        steri4_T4_left_s  = system_config.time_drying;       // T4
        steri4_tick_1s    = g_time_tik_10ms;
        steri4_running    = 1;
        steri4_stt        = START_STERI_4;
        ui_show_running();
    }
}

void sterilization_4_stop(void)
{
    steri4_running = 0;
    steri4_stt     = STOP_STERI_4;
    RELAYA_OFF();
    TRIACB_OFF();
    TRIACC_OFF();
    RELAYD_OFF();
}

void sterilization_4_handle(void)
{
    if (!steri4_running) { steri4_stt = STOP_STERI_4; }

    switch (steri4_stt)
    {
    case START_STERI_4:
        RELAYA_ON();  // A=1
        TRIACB_OFF();
        TRIACC_OFF();
        RELAYD_OFF();
        steri4_stt = HEATING_41;
        break;

    case HEATING_41: {
        uint8_t temp = temperature_sensor_read();
        ui_show_mode4_Temp(temp);
        if (temp >= system_config.temp_setting) {
            // Đạt Ta: mở B; bắt đầu T1 & T21 song song
            TRIACB_ON(); // B=1
            steri4_tick_1s = g_time_tik_10ms;
            steri4_stt     = VENT_AND_STER4;
        }
        } break;

    case VENT_AND_STER4: {
        uint8_t temp = temperature_sensor_read();
        ui_show_mode4_Temp(temp);

        if (g_time_tik_10ms - steri4_tick_1s >= 100) {
            steri4_tick_1s = g_time_tik_10ms;

            // Đếm T1
            if (steri4_T1_left_s > 0) {
                steri4_T1_left_s--;
                ui_show_mode4_T1(steri4_T1_left_s);
            }

            // Đếm T21 cho B
            if (steri4_T21_left_s > 0) {
                steri4_T21_left_s--;
                ui_show_mode4_T21(steri4_T21_left_s);
                if (steri4_T21_left_s == 0) {
                    TRIACB_OFF(); // hết xả áp lần 1
                }
            }
        }

        if (steri4_T1_left_s == 0) {
            // Chuyển sang xả nước: C=1; A có thể tắt ở đây (chu trình tiệt trùng đã xong)
            RELAYA_OFF();  // A=0, chuyển công đoạn xả nước
            TRIACC_ON();   // C=1
            // Theo mô tả: "T22 = 0 thì C = 0 và D = 1"
            // -> giữ C mở trong T22
            steri4_tick_1s = g_time_tik_10ms;
            steri4_stt     = DRAIN_4;
        }
        } break;

    case DRAIN_4: {
        uint8_t temp = temperature_sensor_read();
        ui_show_mode4_Temp(temp);

        if (g_time_tik_10ms - steri4_tick_1s >= 100) {
            steri4_tick_1s = g_time_tik_10ms;

            // === Chọn một trong hai logic cho C ===
            // 1) Theo mô tả: dùng T22 điều khiển C
            if (steri4_T22_left_s > 0) {
                steri4_T22_left_s--;
                ui_show_mode4_T22(steri4_T22_left_s);
            }
            // 2) Nếu muốn dùng T3 cho xả nước, thay block trên bằng:
            // if (steri4_T3_left_s > 0) { steri4_T3_left_s--; ui_show_mode4_T22(steri4_T3_left_s); }

            // Khi bộ đếm đã hết -> C=0, bắt đầu sấy (D=1)
            if (steri4_T22_left_s == 0 /*|| steri4_T3_left_s == 0*/) {
                TRIACC_OFF(); // đóng xả nước
                RELAYD_ON();  // D=1 (sấy)
                steri4_stt   = DRY_HEAT_4;
            }
        }
        } break;

    case DRY_HEAT_4: {
        // Gia nhiệt tới Tb
        uint8_t temp = temperature_sensor_read();
        ui_show_mode4_Temp(temp);
        if (temp >= system_config.temp_dry_setting /*Tb*/) {
            // Duy trì Tb trong T4
            steri4_tick_1s = g_time_tik_10ms;
            ui_show_mode4_T4(steri4_T4_left_s);
            steri4_stt = DRY_HOLD_4;
        }
        } break;

    case DRY_HOLD_4: {
        uint8_t temp = temperature_sensor_read();
        ui_show_mode4_Temp(temp);

        // Giữ D bật, bạn có thể PID/bật-tắt để duy trì Tb, ở đây giữ D=1 (tùy phần cứng)
        if (g_time_tik_10ms - steri4_tick_1s >= 100) {
            steri4_tick_1s = g_time_tik_10ms;
            if (steri4_T4_left_s > 0) {
                steri4_T4_left_s--;
                ui_show_mode4_T4(steri4_T4_left_s);
            }
        }

        if (steri4_T4_left_s == 0 && temp < 70) {
            RELAYD_OFF();                 // kết thúc sấy
            buzzer_start_ms(60*1000);     // E=1 trong 60s
            steri4_stt = END_STERI_4;
        }
        } break;

    case END_STERI_4:
        ui_show_end();
        steri4_running = 0;
        steri4_stt     = STOP_STERI_4;
        break;

    case STOP_STERI_4:
    default:
        RELAYA_OFF();
        TRIACB_OFF();
        TRIACC_OFF();
        RELAYD_OFF();
        break;
    }
}


void USER_PROGRAM_C(void)
{
    static unsigned char t63_cnt = 0;
    if (!TKS_ACTIVEF)
        return;

    // 1) Quét phím ở nhịp nhanh (~10ms)
    if (SCAN_CYCLEF)
    {
        g_time_tik_10ms++;
        GET_KEY_BITMAP();
        unsigned char key = DATA_BUF[0];
        KeyEvents evKey = key_update(key); // hàm chuẩn hoá pressed/idle/hold
        key_handle_service(evKey);         // hành vi UI + phản hồi (bíp/LED vòng)
        buzzer_service();
    }

    // 2) Nhiệm vụ chậm (63ms)
    if (TKS_63MSF)
    {

        static uint16_t adc_pressure, adc_temp = 0;

        if (++t63_cnt >= 8)
        { // ~0.5 s (chỉnh 16 => 1s nếu muốn)
            t63_cnt = 0;

            adc_pressure = adc_read_channel(0);
            adc_temp = adc_read_channel(1);

            // supervisor_run_1s(); // <<< gọi giám sát an toàn/khởi động
        }

        // LED 1: Select temperature show? temperature setting or now
        key_off_time();
        if (ui_led1 == UI_TEMP_NOW)
        {
            tm1640_write_led(1, (int)(adc_temp / 10));
            tm1640_keyring_clear(SETTING_TEMP_KEY);
            tm1640_keyring_clear(TEMP_UP_KEY);
            tm1640_keyring_clear(TEMP_DOWN_KEY);
        }
        else
        {
            tm1640_write_led(1, (int)system_config.temp_setting);
            tm1640_keyring_add(SETTING_TEMP_KEY);

            if (key23_state & (1 << 0))
                tm1640_keyring_add(TEMP_DOWN_KEY);
            else
                tm1640_keyring_clear(TEMP_DOWN_KEY);

            if (key23_state & (1 << 1))
                tm1640_keyring_add(TEMP_UP_KEY);
            else
                tm1640_keyring_clear(TEMP_UP_KEY);
        }
        // led 2
        uint16_t value_led_2 = system_config.time_steri;
        if (ui_led2 != UI_IDLE)
        {
            tm1640_keyring_add(SETTING_TIME_KEY);

            if (key56_state & (1 << 0))
            {
                tm1640_keyring_add(TIME_DOWN_KEY);
            }
            else
                tm1640_keyring_clear(TIME_DOWN_KEY);

            if (key56_state & (1 << 1))
                tm1640_keyring_add(TIME_UP_KEY);
            else
                tm1640_keyring_clear(TIME_UP_KEY);

            switch (ui_led2)
            {
            case UI_SET_T1:
                value_led_2 = system_config.time_steri;
                break;
            case UI_SET_T21:
                value_led_2 = system_config.time_pressure_release_1;
                break;
            case UI_SET_T22:
                value_led_2 = system_config.time_pressure_release_2;
                break;
            case UI_SET_T3:
                value_led_2 = system_config.time_water_release;
                break;
            case UI_SET_T4:
                value_led_2 = system_config.time_drying;
                break;
            default:
                break;
            }
        }
        else
        {
            tm1640_keyring_clear(SETTING_TIME_KEY);
            tm1640_keyring_clear(TIME_DOWN_KEY);
            tm1640_keyring_clear(TIME_UP_KEY);
        }
        tm1640_write_led(2, value_led_2);
        tm1640_write_led(3, g_time_tik_10ms / 100);
    }
}

void USER_PROGRAM_C_HALT_PREPARE()
{
    // function to execute before going into standby
}

void USER_PROGRAM_C_HALT_WAKEUP()
{
    // Functions that are executed after standby mode is interrupted and IO is woken up
}

void USER_PROGRAM_C_RETURN_MAIN()
{
    // function to execute when standby mode is switched back to work mode
}

void __attribute((interrupt(0x14))) timer_ISR(void) // 0.5ms TIMER interrupt service routine
{
    // count2ms++;
}
