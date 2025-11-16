/////******************************************Disclaimer****************************************************//**
/////*The material offered by Holtek Semiconductor Inc. (including its
/// subsidiaries, hereinafter
/////*collectively referred to as “HOLTEK”), including but not limited to
/// technical documentation and
/////*code, is provided “as is”, only for your reference, and may be superseded
/// by updates. HOLTEK
/////*reserves the right to revise the offered material at any time without
/// prior notice. You shall use the
/////*offered material at your own risk. HOLTEK disclaims any expressed,
/// implied, or statutory warranties,
/////*including but not limited to accuracy, suitability for commercialization,
/// satisfactory quality,
/////*specifications, characteristics, functions, fitness for a particular
/// purpose, and non-infringement of
/////*any third-party’s rights. HOLTEK disclaims all liability arising from the
/// offered material and its
/////*application. In addition, HOLTEK does not recommend the use of HOLTEK’s
/// products where there is
/////*a risk of personal hazard due to malfunction or other reasons. HOLTEK
/// hereby declares that it does
/////*not authorize the use of these products in life-saving, life-sustaining,
/// or safety-critical components.
/////*Any use of HOLTEK’s products in life-saving, sustaining, or safety
/// applications is entirely at your risk,
/////*and you agree to defend, indemnify, and hold HOLTEK harmless from any
/// damages, claims, suits, or
/////*expenses resulting from such use.
/////************************************************************************************************************/

//////***********************************Intellectual
/// Property*************************************************//**
/////*The offered material, including but not limited to the content, data,
/// examples, materials, graphs,
/////*and trademarks, is the intellectual property of HOLTEK (and its licensors,
/// where applicable) and is
/////*protected by copyright law and other intellectual property laws.
/////************************************************************************************************************/

#include "BS86D12C.h"
#include "USER_PROGRAM_C.INC"
#include "adc.h"
#include "ioConfig.h"
#include "keyIO.h"
#include "tm1640.h"
#include "uart.h"
#include "user_config.h"
#include <string.h>
#include <math.h>
static SupState prg_state = SUP_BOOT;
system_init_t system_config = {0};
sterilization_mode_t sterilization_mode = STERILIZATION_NULL;
static UiField ui_led2 = UI_IDLE;

uint32_t g_time_tik_10ms = 0;
static uint16_t g_adc_temp = 0; // ADC kênh nhiệt độ

void uart_debug()
{
  uart0_send_number(g_time_tik_10ms);
  uart0_send_string(":STERI: ");
  uart0_send_number(steri1_stt);
  uart0_send_string("-remain: ");
  uart0_send_number(remain_count);
  uart0_send_string(" : ");
  uart0_send_number(steri_tick);
  uart0_send_string(" : ");
  uart0_send_number(fabs(g_pressure));
  uart0_send_byte('\n');
}
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

  // default config
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
  uart0_send_string("HELLO From BS86D12C\n");
  uart0_send_byte('\n');
  // buzzer_start_ms(STARTUP_BUZZ_MS);
  //  tm1640_walk_grids_a(2);
  // int i = 0;
  // for (i = 0; i < 11; i++)
  // {
  //   tm1640_walk_segments(i, 1);
  // }

  // tm1640_show_1_to_9_on_grid1_to_9();
  tm1640_write_end(2);
  tm1640_write_err(1);
  tm1640_update_all();
}

//==============================================
//**********************************************

//==============================================

//==============================================
static volatile uint8_t buzzer_ticks = 0;
static inline void buzzer_start_ms(unsigned int ms)
{
  if (!ms)
    return;
  uint16_t t = (ms + 9) / 10;
  if (!t)
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

static inline void buzzer_service(void)
{
  static bit buzze_state_blink = 0;
  static uint32_t tik_blink = 0;
  if (buzzer_ticks && !buzzer_blink)
  {
    if (--buzzer_ticks == 0)
      BUZZ_OFF();
    return;
  }
  else if (!buzzer_blink && prg_state == SUP_IDLE)
    BUZZ_OFF();

  if (buzzer_blink)
  {
    if (g_time_tik_10ms - tik_blink > (int)(t_blink / 10))
    {
      buzzer_blink--;
      // uart0_send_number(buzzer_blink);
      tik_blink = g_time_tik_10ms;

      buzze_state_blink = buzze_state_blink ? 0 : 1;
      if (buzze_state_blink)
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
uint8_t key23_state = 0;
uint8_t key56_state = 0;
static uint16_t key_led_timer = 0; // *63ms

/// @brief Tắt led của key sau khi hết thời gian timer
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

void blink_err(uint8_t led_num)
{
  static bit err_blink = 0;
  static uint32_t last_tik = 0;

  if (g_time_tik_10ms - last_tik > 100)
  {
    last_tik = g_time_tik_10ms;
    if (err_blink)
    {
      tm1640_clear_led(led_num);
      err_blink = 0;
      if (led_num == 2)
      {
        BUZZ_OFF();
      }
    }
    else
    {

      tm1640_write_err(led_num);
      err_blink = 1;
      if (led_num == 2)
        BUZZ_ON();
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
    system_config.time_steri =
        _limit_data(system_config.time_steri + delta, 0, 999);
    break;
  case UI_SET_T21:
    if (system_config.time_pressure_release_1 == 0 && delta < 0)
      break;
    system_config.time_pressure_release_1 =
        _limit_data(system_config.time_pressure_release_1 + delta, 0, 999);
    break;
  case UI_SET_T22:
    if (system_config.time_pressure_release_2 == 0 && delta < 0)
      break;
    system_config.time_pressure_release_2 =
        _limit_data(system_config.time_pressure_release_2 + delta, 0, 999);
    break;
  case UI_SET_T3:
    if (system_config.time_water_release == 0 && delta < 0)
      break;
    system_config.time_water_release =
        _limit_data(system_config.time_water_release + delta, 0, 999);
    break;
  case UI_SET_T4:
    if (system_config.time_drying == 0 && delta < 0)
      break;
    system_config.time_drying =
        _limit_data(system_config.time_drying + delta, 0, 999);
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
  if (system.time_drying && system.time_water_release &&
      system.time_pressure_release_1 && system.time_pressure_release_2)
    return STERILIZATION_4;
  // T21
  if (!system.time_drying && !system.time_water_release &&
      system.time_pressure_release_1 && !system.time_pressure_release_2)
    return STERILIZATION_3;
  // T22 T21
  if (!system.time_drying && !system.time_water_release &&
      system.time_pressure_release_1 && system.time_pressure_release_2)
    return STERILIZATION_2;

  if (!system.time_drying && !system.time_water_release &&
      !system.time_pressure_release_1 && !system.time_pressure_release_2)
    return STERILIZATION_1;
  // other
  return STERILIZATION_NULL;
}
bit danger_temp = 0; // 0 OKE, any Danger

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
    if (prg_state != SUP_IDLE || steri_running)
      return;

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
      system_config.temp_setting =
          system_config.temp_setting > 134 ? 134 : system_config.temp_setting;
      key23_state |= (1 << 1);
      key23_state &= ~(1 << 0);
    }
    else if (ev.pressed & KEYBIT(TEMP_DOWN_KEY))
    {
      system_config.temp_setting--;
      system_config.temp_setting =
          system_config.temp_setting < 121 ? 121 : system_config.temp_setting;
      key23_state |= (1 << 0);
      key23_state &= ~(1 << 1);
    }
    else if (ev.hold & KEYBIT(TEMP_DOWN_KEY) &&
             ev.hold | KEYBIT(TEMP_UP_KEY))
    {
      key23_state |= (1 << 0);
      key23_state &= ~(1 << 1);
      if (g_time_tik_10ms % STEP_PRESS_TIME == 0) // 50ms + 1
      {
        system_config.temp_setting--;
        system_config.temp_setting =
            system_config.temp_setting < 121 ? 121 : system_config.temp_setting;
      }
    }
    else if (ev.hold & KEYBIT(TEMP_UP_KEY) &&
             ev.hold | KEYBIT(TEMP_DOWN_KEY))
    {
      key23_state |= (1 << 1);
      key23_state &= ~(1 << 0);
      if (g_time_tik_10ms % STEP_PRESS_TIME == 0) // 50ms + 1
      {
        system_config.temp_setting++;
        system_config.temp_setting =
            system_config.temp_setting > 134 ? 134 : system_config.temp_setting;
      }
    }
  }

  //==================Handle Led 2: KEY 4  5  6============================
  if (ev.pressed & KEYBIT(SETTING_TIME_KEY))
  {
    if (prg_state == SUP_DANGER || danger_temp || steri_running)
      return;
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
    if (prg_state == SUP_DANGER && danger_temp)
      return;
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
    else if (ev.hold & KEYBIT(TIME_UP_KEY) &&
             ev.hold | KEYBIT(TIME_DOWN_KEY))
    {
      key56_state |= (1 << 1);
      key56_state &= ~(1 << 0);
      if (g_time_tik_10ms % STEP_PRESS_TIME == 0) // 50ms + 1
      {
        caculate_time_setup(+1);
      }
    }
    else if (ev.hold & KEYBIT(TIME_DOWN_KEY) &&
             ev.hold | KEYBIT(TIME_UP_KEY))
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
  if (ev.pressed & KEYBIT(QUICK_KEY) && prg_state == SUP_IDLE)
  {
    static uint16_t count = 0;
    count++;
    if (count % 3 == 0)
    {
      sterilization_mode = STERILIZATION_NULL;
    }
    sterilization_mode = QUICK_MODE;
    system_config.temp_setting =
        (system_config.temp_setting == 121) ? 134 : 121;
  }

  //==================Handle KEY 8: START_KEY============================
  if (ev.pressed & KEYBIT(START_KEY) && prg_state == SUP_IDLE)
  {
    if (steri_running)
    {
      return;
    }
    ui_led1 = UI_TEMP_NOW;
    ui_led2 = UI_IDLE;

    if (sterilization_mode == QUICK_MODE)
    {
      // start quick mode
      sterilization_mode = STERILIZATION_NULL;
    }
    else
    {
      sterilization_mode = check_mode_process(system_config);
      // uart0_send_number(system_config.time_steri);
      // uart0_send_number(system_config.time_pressure_release_1);
      // uart0_send_number(system_config.time_pressure_release_2);
      // uart0_send_number(system_config.time_water_release);
      // uart0_send_number(system_config.time_drying);

      // uart0_send_number(sterilization_mode);
      // uart0_send_byte('\n');
      if (sterilization_mode)
      {
        sterilization_start(sterilization_mode);
      }
      else
      {
        blink_err(2);
      }
    }
  }
  else if (ev.pressed & KEYBIT(START_KEY) && prg_state == SUP_DANGER)
  {
    if (danger_temp)
    {
      danger_temp = 0; // clear danger_temp;
      return;
    }
  }
  else if (ev.hold & KEYBIT(START_KEY) && steri_running)
  {
    sterilization_stop();
  }
}

void UI_handle()
{
  // LED 1: Select temperature show? temperature setting or now
  key_off_time();
  if (ui_led1 == UI_TEMP_NOW)
  {
    tm1640_write_led(1, (int)(g_adc_temp / 10));

    tm1640_keyring_clear(SETTING_TEMP_KEY);
    tm1640_keyring_clear(TEMP_UP_KEY);
    tm1640_keyring_clear(TEMP_DOWN_KEY);
  }
  else if (ui_led1 == UI_TEMP_SETTING)
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
  if (prg_state != SUP_DANGER && !danger_temp)
  {
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
      tm1640_write_led(2, value_led_2);
    }
    else
    {
      tm1640_keyring_clear(SETTING_TIME_KEY);
      tm1640_keyring_clear(TIME_DOWN_KEY);
      tm1640_keyring_clear(TIME_UP_KEY);

      if (steri1_running || steri2_running || steri3_running || steri4_running)
      {
      }
      else
      {
        // total time
        value_led_2 = system_config.time_steri + system_config.time_pressure_release_1 + system_config.time_pressure_release_2 + system_config.time_water_release + system_config.time_drying;
        tm1640_write_led(2, value_led_2);
      }
    }
  }

  // tm1640_write_led(3, g_time_tik_10ms / 100);

  tm1640_write_led(3, fabs(g_pressure));

  tm1640_update_all();
}
/*
"🔹Tiệt trùng 1: ( A =1, B = C = D =E = 0 )  không xả áp, không xả nước, không
sấy Thiết bị sử dụng: Relay A (điện trở):
- Quy trình:
Cài đặt nhiệt độ tiệt trùng Ta (121°C hoặc 134°C) và thời gian tiệt trùng T1 .
Nhấn nút START để bắt đầu.
Relay A bật → gia nhiệt đến nhiệt độ cài đặt.
Khi đạt nhiệt độ →  bắt đầu đếm thời lùi gian tiệt trùng ( Thời gian này được
cài đặt ). Kết thúc thời gian tiệt trùng T1 = 0 ( vì đếm ngược ) → relay A tắt (
A = 0 ) - Bật chuông báo ( E = 1 trong 60s rồi E về 0, Khi nhiệt độ dưới 70 độ C
)  → Khi kết thúc chương trình hiển thị End."

*/

void sterilization_start(sterilization_mode_t mode)
{

  steri_tick = g_time_tik_10ms;

  switch (mode)
  {
  case STERILIZATION_1:
    steri1_running = 1;
    steri1_stt = START_STERI_1;
    /* code */
    break;
  case STERILIZATION_2:
    steri2_running = 1;
    steri2_stt = START_STERI_2;
    /* code */
    break;
  case STERILIZATION_3:
    steri3_running = 1;
    steri3_stt = START_STERI_3;
    /* code */
    break;
  case STERILIZATION_4:
    steri4_running = 1;
    steri4_stt = START_STERI_4;
    /* code */
    break;
  case QUICK_MODE:
    quick_mode_running = 1;
    quick_mode = START_QUICK;
    /* code */
    break;

  default:
    break;
  }
}

void sterilization_stop()
{
  if (steri1_running)
  {
    RELAYA_OFF();
    steri1_running = 0;
    steri1_stt = STOP_STERI_1;
  }
  if (steri2_running)
  {
    RELAYA_OFF();
    steri2_running = 0;
    steri2_stt = STOP_STERI_2;
  }
  if (steri3_running)
  {
    steri3_running = 0;
    steri3_stt = STOP_STERI_3;
    RELAYA_OFF();
    TRIACB_OFF();
    TRIACC_OFF();
  }
  if (steri4_running)
  {
    steri4_running = 0;
    steri4_stt = STOP_STERI_4;
    RELAYA_OFF();
    TRIACB_OFF();
    TRIACC_OFF();
    RELAYD_OFF();
  }
}

void sterilization_1_handle(void)
{
  uint8_t temp = 0;
  switch (steri1_stt)
  {
  case START_STERI_1:
    RELAYA_ON();
    steri1_stt = HEATING_1;
    /* code */
    break;
  case HEATING_1:
    /* code */
    temp = TEMP_NOW();
    RELAYA_ON();
    tm1640_write_led(2, system_config.time_steri);
    if (temp >= system_config.temp_setting)
    {
      steri1_stt = STERING_1;
      steri_tick = g_time_tik_10ms;
    }

    break;
  case STERING_1:
    /* COUNTDOWN phut */
    {
      remain_count = (float)(system_config.time_steri - (float)(g_time_tik_10ms - steri_tick) / 6000);
      tm1640_write_led(2, remain_count);

      if (remain_count < 0)
      {
        steri1_stt = END_STERI_1;
        steri_tick = g_time_tik_10ms;
        RELAYA_OFF();
      }
      break;
    }

  case END_STERI_1:
    /* code */
    g_temperature = 60;
    RELAYA_OFF();
    if (g_temperature <= 70)
    {
      buzzer_start_ms(60U * 1000U);
      tm1640_write_end(2);
      if (g_time_tik_10ms - steri_tick >= 6000)
      {
        steri1_stt = STOP_STERI_1;
        BUZZ_OFF();
      }
    }
    break;

  case STOP_STERI_1:
    steri1_running = 0;
    RELAYA_OFF();
    steri1_stt = STERI_WAITTING_1;
    break;
  default:
    steri1_stt = STERI_WAITTING_1;
    break;
  }
}

/*
"🔹Tiệt trùng 2 ( A =  1, B = C = D = E = 0) có xả áp, không xả nước, không sấy
Thiết bị sử dụng: Relay A (điện trở), Relay B (van xả khí)

Quy trình:
Cài đặt nhiệt độ tiệt trùng Ta (121°C hoặc 134°C),  thời gian tiệt trùng T1,
thời gian xả áp lần T21.T22 Nhấn nút START để bắt đầu. Relay A bật → gia nhiệt
đến nhiệt độ cài đặt . Khi đạt nhiệt độ → relay B mở van xả khí ( B = 1)  trong
thời gian cài đặt. Sau khi xả khí ( Hết thời gian cài đặt xả )→ đóng relay B ( B
= 0) → tiếp tục gia nhiệt đến nhiệt độ đã cài đặt và bắt đầu đếm thời gian tiệt
trùng. Kết thúc thời gian tiệt trùng (T1 đếm về 0) → relay A tắt ( A = 0) →
relay B mở lần 2 ( B = 1)  để xả áp đến khi nhiệt độ xuống dưới 70°C, T22  đếm
ngược, T22 không đếm khi Temp = 40 độ C. Bật chuông báo trong 60 giây (E = 1)
hết thời gian 60s tắt chuông báo ( E = 0) → Khi kết thúc chương trình hiển thị
End."

*/

void sterilization_2_handle(void)
{
  if (!steri2_running)
  {
    steri2_stt = STERI_WAITTING_2;
  }

  switch (steri2_stt)
  {
  case START_STERI_2:

    RELAYA_ON();  // A = 1
    TRIACB_OFF(); // B = 0
    TRIACC_OFF(); // C = 0
    RELAYD_OFF(); // D = 0

    steri2_stt = HEATING_21;

    tm1640_write_led(2, system_config.time_pressure_release_1);
    break;

  case HEATING_21:
  {
    g_temperature = 133; /*Test*/

    if (g_temperature >= system_config.temp_setting)
    {
      // Đạt nhiệt độ cài đặt -> mở B xả lần 1 trong T21
      steri_tick = g_time_tik_10ms;
      TRIACB_ON(); // B = 1
      steri2_stt = RELEASE_21;
    }
  }
  break;

  case RELEASE_21:
    remain_count = (float)(system_config.time_pressure_release_1 - (float)(g_time_tik_10ms - steri_tick) / 6000);
    tm1640_write_led(2, remain_count);
    if (remain_count <= 0)
    {
      TRIACB_OFF(); // B = 0
      steri2_stt = HEATING_22;
    }
    break;

  case HEATING_22:
  {
    // Sau khi đóng B, tiếp tục gia nhiệt lại tới nhiệt độ cài đặt rồi bắt đầu
    // T1
    if (g_temperature >= system_config.temp_setting)
    {
      steri_tick = g_time_tik_10ms;
      tm1640_write_led(2, system_config.time_steri);
      steri2_stt = STERING_2;
    }
  }
  break;

  case STERING_2:
    // Đếm T1
    remain_count = (float)(system_config.time_steri - (float)(g_time_tik_10ms - steri_tick) / 6000);
    tm1640_write_led(2, remain_count);
    if (remain_count <= 0)
    {
      // Hết T1: A = 0, B = 1; bật còi 60s
      RELAYA_OFF(); // A = 0
      TRIACB_ON();  // B = 1 (xả lần 2)

      steri_tick = g_time_tik_10ms;
      tm1640_write_led(2, system_config.time_pressure_release_2);
      steri2_stt = RELEASE_22;
    }
    break;

  case RELEASE_22:
  {
    g_temperature = 60;
    if (g_temperature > 70)
    {
      __asm__("nop");
      break;
    }
    // T22 đếm ngược (chỉ để hiển thị/giới hạn), KHÔNG đếm khi Temp <= 40°C
    remain_count = (float)(system_config.time_pressure_release_2 - (float)(g_time_tik_10ms - steri_tick) / 6000);
    tm1640_write_led(2, remain_count);

    if ((g_temperature < 40 || remain_count <= 0))
    {
      steri_tick = g_time_tik_10ms;
      steri2_stt = END_STERI_2;
      TRIACB_OFF();
    }
  }
  break;

  case END_STERI_2:
    buzzer_start_ms(60U * 1000U);
    tm1640_write_end(2);
    if (g_time_tik_10ms - steri_tick >= 6000)
    {
      steri2_stt = STOP_STERI_2;
      BUZZ_OFF();
    }

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

void sterilization_3_handle(void)
{
  if (!steri3_running)
  {
    steri3_stt = STOP_STERI_3;
  }

  switch (steri3_stt)
  {
  case START_STERI_3:
    /*Nhấn nút START → relay A bật → gia nhiệt.*/
    RELAYA_ON();  // A=1
    TRIACB_OFF(); // B=0
    TRIACC_OFF(); // C=0
    RELAYD_OFF(); // D=0
    steri3_stt = HEATING_3;
    break;

  case HEATING_3:
  {
    /*Khi đạt nhiệt độ → relay B mở van xả khí
    → tiếp tục gia nhiệt đến nhiệt độ đã cài đặt và
    bắt đầu đếm thời gian tiệt trùng.
    */
    uint8_t temp = TEMP_NOW();
    if (temp >= system_config.temp_setting)
    {
      // Bắt đầu đếm T1; mở B và bắt đầu đếm T21 song song
      TRIACB_ON(); // B=1 (xả áp)
      steri_tick = g_time_tik_10ms;
      steri3_stt = VENT_AND_STER3;
    }
  }
  break;

  case VENT_AND_STER3:
  {
    // T1 đếm ngược
    remain_count = (float)(system_config.time_steri - (float)(g_time_tik_10ms - steri_tick) / 6000);
    tm1640_write_led(2, remain_count);

    if (remain_count <= 0)
    {
      // Kết thúc tiệt trùng
      RELAYA_OFF(); // A=0
      TRIACC_ON();  // C=1 (xả nước không giới hạn)
      TRIACB_OFF();
      steri3_stt = FINISH_3;
      steri_tick = g_time_tik_10ms;
    }
  }
  break;

  case FINISH_3:
  {
    buzzer_start_ms(60U * 1000U);
    uint8_t g_temperature = TEMP_NOW();
    // C đóng khi ≤ 40°C
    if (g_temperature <= 40)
    {
      TRIACC_OFF();
    }
    // Kết thúc chu trình khi < 70°C
    if (g_temperature < 70)
    {
      steri3_running = 0;
      steri3_stt = STOP_STERI_3;
      TRIACB_ON();
    }
  }
  break;

  case STOP_STERI_3:
  default:
    RELAYA_OFF();
    TRIACB_OFF();
    // C có thể vẫn ON nếu người dùng chưa về 40°C và chưa tắt nguồn; nhưng ở
    // STOP ta đảm bảo OFF: Nếu bạn muốn giữ nguyên hành vi "C không tắt đến khi
    // tắt nguồn", hãy bỏ dòng sau.
    TRIACC_OFF();
    break;
  }
}

void sterilization_4_handle(void)
{
  if (!steri4_running)
  {
    steri4_stt = STOP_STERI_4;
  }

  switch (steri4_stt)
  {
  case START_STERI_4:
    RELAYA_ON(); // A=1
    TRIACB_OFF();
    TRIACC_OFF();
    RELAYD_OFF();
    steri4_stt = HEATING_41;
    break;

  case HEATING_41:
  {
    uint8_t temp = TEMP_NOW();

    if (temp >= system_config.temp_setting)
    {
      // Đạt Ta: mở B; bắt đầu T1 & T21 song song
      TRIACB_ON(); // B=1
      steri_tick = g_time_tik_10ms;
      steri4_stt = VENT_AND_STER4;
    }
  }
  break;

  case VENT_AND_STER4:
  {

    if (g_time_tik_10ms - steri_tick >= 100)
    {
      steri_tick = g_time_tik_10ms;

      // Đếm T1
      remain_count = (float)(system_config.time_drying - (float)(g_time_tik_10ms - steri_tick) / 6000);
      tm1640_write_led(2, remain_count);

      // Đếm T21 cho B
    }
  }
  break;

  case DRAIN_4:
  {

    remain_count = (float)(system_config.time_pressure_release_2 - (float)(g_time_tik_10ms - steri_tick) / 6000);
    tm1640_write_led(2, remain_count);
    if (remain_count < 0)
    {
      TRIACC_OFF(); // đóng xả nước
      RELAYD_ON();  // D=1 (sấy)
      steri4_stt = DRY_HEAT_4;
    }
  }
  break;

  case DRY_HEAT_4:
  {
    if (g_temperature >= system_config.temp_setting /*Tb*/)
    {
      // Duy trì Tb trong T4
      steri_tick = g_time_tik_10ms;
      steri4_stt = DRY_HOLD_4;
    }
  }
  break;

  case DRY_HOLD_4:
  {
    uint8_t temp = TEMP_NOW();
    // Giữ D bật, bạn có thể PID/bật-tắt để duy trì Tb, ở đây giữ D=1 (tùy phần
    // cứng)
    remain_count = (float)(system_config.time_drying - (float)(g_time_tik_10ms - steri_tick) / 6000);
    tm1640_write_led(2, remain_count);

    if (remain_count <= 0 && temp < 70)
    {
      RELAYD_OFF();                 // kết thúc sấy
      buzzer_start_ms(60U * 1000U); // E=1 trong 60s
      steri4_stt = END_STERI_4;
    }
  }
  break;

  case END_STERI_4:
    steri4_running = 0;
    steri4_stt = STOP_STERI_4;
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

void handle_quick_mode(void)
{
  switch (quick_mode)
  {
  case START_QUICK:
    quick_mode_running = 1;
    quick_mode = HEATING;
    RELAYA_ON();
    /* code */
    break;
  case HEATING:
  {
    uint8_t temp_now = TEMP_NOW();
    if (temp_now >= system_config.temp_setting)
    {
      RELAYA_OFF();
      quick_mode = END_QUICK;
    }
  }
  break;
  case END_QUICK:
    /* code */
    quick_mode_running = 0;
    break;
  case STOP_QUICK:
    RELAYA_OFF();
    break;
  default:
    break;
  }
}

#define DEMO_BOOT_90 0
#define DEMO_BOOT_IDLE 1
#define DEMO_BOOT_DANGER 0
bit boot_started = 0;
void main_handle_servie()
{
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
      // buzzer_start_ms(STARTUP_BUZZ_MS);
      buzzer_start_blink_ms(1000, 3);
      tik_sup = g_time_tik_10ms;
    }

    if (g_time_tik_10ms - tik_sup > STARTUP_BUZZ_MS / 10)
    {
#if DEMO_BOOT_90
      g_temperature = 95;
#endif
#if DEMO_BOOT_IDLE
      g_temperature = 80;
#endif
      if (g_temperature > TEMP_STARTUP_LOCK)
      {
        TRIACB_ON();
        tik_sup = g_time_tik_10ms;
        tm1640_write_err(1);
        prg_state = SUP_LOCKED;
        ui_led1 = UI_ERR_BL;
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
      g_temperature = 50; /*Test*/
      if (g_temperature < TEMP_STARTUP_LOCK)
      {
        tm1640_clear_led(1);
        TRIACB_OFF();
        prg_state = SUP_IDLE;
        ui_led1 = UI_TEMP_NOW;
      }
    }

    // blink 1s err and beep
    blink_err(1);

    break;

  case SUP_IDLE:
/* code */
#if DEMO_BOOT_DANGER
    Ta = 150;
#endif
    if (g_temperature > TEMP_DANGER_C)
    {
      prg_state = SUP_DANGER;
      RELAYA_OFF();
      RELAYD_OFF();
      TRIACB_ON();
      danger_temp = 1;
    }
    break;
  case SUP_DANGER:
    /* 1. Nếu nhiệt độ quá 140 độ C thì lúc này A = D = 0 ( Không sấy, không gia nhiệt );  B = 1 để xả áp, E = 1 nhấp nháy còi và LED hiển thị sẽ  nhiệt độ thực tế ở LED-1 và Err ở  LED -2 dưới
     */
    tm1640_write_led(1, g_temperature);
    blink_err(2);
#if DEMO_BOOT_DANGER
    g_temperature = 150;

#endif
    if (g_temperature < TEMP_SAFE_EXIT_C || !danger_temp)
    {
      prg_state = SUP_IDLE;
      TRIACB_OFF();
    }
    break;
  default:
    break;
  }

  if (prg_state == SUP_IDLE)
  {
    steri_running = steri1_running | steri2_running | steri3_running | steri4_running | quick_mode_running;
    if (steri_running)
    {
      tm1640_keyring_add(11); // on led RUN
    }
    else
    {
      tm1640_keyring_clear(11); // off led RUN
    }
    switch (sterilization_mode)
    {
    case STERILIZATION_1:
      /* code */
      sterilization_1_handle();
      break;
    case STERILIZATION_2:
      /* code */
      sterilization_2_handle();
      break;
    case STERILIZATION_3:
      /* code */
      sterilization_3_handle();
      break;
    case STERILIZATION_4:
      /* code */
      sterilization_4_handle();
      break;
    case STERILIZATION_NULL:
      /* code */
      break;
    case QUICK_MODE:
      /* code */
      handle_quick_mode();
      break;
    default:
      break;
    }
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

    key_handle_service(evKey); // hành vi UI + phản hồi (bíp/LED vòng)

    buzzer_service();
  }

  // 2) Nhiệm vụ chậm (63ms)
  if (TKS_63MSF)
  {
    if (++t63_cnt >= 8)
    { // ~0.5 s (chỉnh 16 => 1s nếu muốn)
      t63_cnt = 0;

      g_pressure = pressure_sensor_read_kPa();
      g_adc_temp = adc_read_channel(1);

      // supervisor_run_1s(); // <<< gọi giám sát an toàn/khởi động
    }
    main_handle_servie();
    UI_handle();
  }

  if (TKS_250MSF)
  {
    uart_debug();
  }
}

void USER_PROGRAM_C_HALT_PREPARE()
{
  // function to execute before going into standby
}

void USER_PROGRAM_C_HALT_WAKEUP()
{
  // Functions that are executed after standby mode is interrupted and IO is
  // woken up
}

void USER_PROGRAM_C_RETURN_MAIN()
{
  // function to execute when standby mode is switched back to work mode
}

void __attribute((interrupt(0x14)))
timer_ISR(void) // 0.5ms TIMER interrupt service routine
{
  // count2ms++;
}
