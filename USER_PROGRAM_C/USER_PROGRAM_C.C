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

#include    "USER_PROGRAM_C.INC"
#include "adc.h"
#include "tm1640.h"
#include "uart.h"
#include "ioConfig.h"
#include <string.h>
#include "BS86D12C.h"

// Lấy symbol từ thư viện Touch
// extern volatile unsigned char _TKS_ACTIVEF;
// extern volatile unsigned char _SCAN_CYCLEF;
// extern void _GET_KEY_BITMAP(void);
//extern unsigned char _DATA_BUF[];

void delay_ms(unsigned int ms) {
    unsigned long i;
    while (ms--) {
        for (i = 250; i > 0; i--) {
            GCC_NOP();
        }
        __asm__("clr wdt");  
    }
}

#define USER_TEST   1
void SystemInit(void)
{
    // //watchdog timer setup
    // _wdtc = (_wdtc & ~0x07) | 0x07;

    //IO init
 	io_init_outputs();  // PC0..PC3, PD5, PA4 -> output & OFF
    adc_init_vdd_ref();
    uart0_init();       // TX debug init

 	//init TM1640
    tm1640_init(7);     // brightness 0..7
}
////==============================================
////**********************************************
////==============================================
//void __attribute((interrupt(0x04))) Interrupt_Extemal(void)
//{
//	//Insert your code here
//}


static unsigned char key_prev = 0;
//==============================================
//**********************************************
//==============================================
void USER_PROGRAM_C_INITIAL()
{
    //Executes the USER_PROGRAM_C initialization function once
	SystemInit();
	tm1640_walk_grids_a(20);
    int i =0;
    for( i =0; i < 11; i++)
    {
    	tm1640_walk_segments(i, 10);
    }
    
    tm1640_show_1_to_9_on_grid1_to_9();
    tm1640_write_digit(0,5,1); // hiển thị số 8 ở grid 1
    tm1640_write_led(1, 321);
    tm1640_write_led(2, 456);
    tm1640_write_led(3, 777);
    uart0_send_byte('0');
    uart0_send_string("HELLO From BS86D12C\n");
}
	
//==============================================
//**********************************************
//==============================================


void USER_PROGRAM_C(void)
{
    static unsigned char key_prev = 0;
    static unsigned char t63_cnt = 0;   // ~63ms tick counter của lib
    if (!TKS_ACTIVEF) return;              // đợi lib init xong

    // 1) Xử lý phím: mỗi chu kỳ quét ~10ms
    if (SCAN_CYCLEF) {
        GET_KEY_BITMAP();                  // cập nhật DATA_BUF[]
        unsigned char k = DATA_BUF[0];     // KEY1..KEY8 -> bit0..bit7

        if (k != key_prev) {
            unsigned char changed  = k ^ key_prev;
            unsigned char pressed  = changed &  k;
            unsigned char released = changed & ~k;
            int i = 0;
            for (i = 0; i < 8; i++) {
                if (pressed  & (1u<<i))  { uart0_send_string("KEY"); uart0_send_number(i+1); uart0_send_string(" PRESSED\r\n"); }
                if (released & (1u<<i))  { uart0_send_string("KEY"); uart0_send_number(i+1); uart0_send_string(" RELEASED\r\n"); }
            }
            key_prev = k;
        }
    }

    // 2) Các việc "chậm" (ADC, LED…) hãy chạy định kỳ ~500ms bằng tick 63ms của lib
    if (TKS_63MSF) {
        if (++t63_cnt >= 16) {              // ≈ 1s
            uart0_send_string("----\r\n");
            t63_cnt = 0;

            uint16_t adc_pressure = adc_read_channel(0);
            uint16_t adc_temp     = adc_read_channel(1);

            tm1640_write_led(2, (int)adc_pressure/10);
            tm1640_write_led(3, (int)adc_temp/10);

            uint8_t mbar = pressure_sensor_read_mbar(adc_pressure);
            uart0_send_string("Pressure (mbar): "); uart0_send_number(mbar); uart0_send_string("\r\n");

            int tc = temperature_sensor_read(adc_temp);
            uart0_send_string("Temperature (C): ");  uart0_send_number(tc);  uart0_send_string("\r\n");
        }
    }
}

void USER_PROGRAM_C_HALT_PREPARE()
{
	//function to execute before going into standby

}

void USER_PROGRAM_C_HALT_WAKEUP()
{
	//Functions that are executed after standby mode is interrupted and IO is woken up

}

void USER_PROGRAM_C_RETURN_MAIN()
{
    //function to execute when standby mode is switched back to work mode
	
}