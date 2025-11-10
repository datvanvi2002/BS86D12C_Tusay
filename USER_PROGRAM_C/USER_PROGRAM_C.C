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
            if (Ta > TEMP_STARTUP_LOCK && Tb > TEMP_STARTUP_LOCK)
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

    if (prg_state = SUP_IDLE)
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
} steri_status_t;
bit steri_1_running = 0;
void sterilization_1_handle(void)
{
    static steri_status_t steri_stt = END_STERI_1;
    static uint32_t start_steri_1 = 0;
    uint16_t steri_time = system_config.time_steri;

    switch (steri_stt)
    {
    case START_STERI:
        RELAYA_ON();
        steri_stt = HEATING_1;
        /* code */
        break;
    case HEATING_1:
        /* code */
        uint8_t temp = temperature_sensor_read();
        if (temp >= system_config.temp_setting)
        {
            steri_stt = STERING_1;
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
            steri_stt = END_STERI_1;
        }
        break;
    case END_STERI_1:
        /* code */
        uint8_t temp = temperature_sensor_read();
        if (temp <= 70)
        {
            RELAYA_OFF();
            buzzer_start_ms(60 * 1000);
            // show end
            steri_stt = STOP_STERI_1;
        }
        break;

    case STOP_STERI_1:
        RELAYA_OFF();
        break;
    default:
        break;
    }
}

void sterilization_2_handle(void)
{
}

void sterilization_3_handle(void)
{
}

void sterilization_4_handle(void)
{
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
