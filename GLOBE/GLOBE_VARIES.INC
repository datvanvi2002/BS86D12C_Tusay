;;;;;/******************************************Disclaimer****************************************************//**
;;;;;*The material offered by Holtek Semiconductor Inc. (including its subsidiaries, hereinafter 
;;;;;*collectively referred to as “HOLTEK”), including but not limited to technical documentation and 
;;;;;*code, is provided “as is”, only for your reference, and may be superseded by updates. HOLTEK 
;;;;;*reserves the right to revise the offered material at any time without prior notice. You shall use the 
;;;;;*offered material at your own risk. HOLTEK disclaims any expressed, implied, or statutory warranties, 
;;;;;*including but not limited to accuracy, suitability for commercialization, satisfactory quality, 
;;;;;*specifications, characteristics, functions, fitness for a particular purpose, and non-infringement of 
;;;;;*any third-party’s rights. HOLTEK disclaims all liability arising from the offered material and its 
;;;;;*application. In addition, HOLTEK does not recommend the use of HOLTEK’s products where there is 
;;;;;*a risk of personal hazard due to malfunction or other reasons. HOLTEK hereby declares that it does 
;;;;;*not authorize the use of these products in life-saving, life-sustaining, or safety-critical components. 
;;;;;*Any use of HOLTEK’s products in life-saving, sustaining, or safety applications is entirely at your risk, 
;;;;;*and you agree to defend, indemnify, and hold HOLTEK harmless from any damages, claims, suits, or 
;;;;;*expenses resulting from such use.
;;;;;************************************************************************************************************/

;;;;;/***********************************Intellectual Property*************************************************//**
;;;;;*The offered material, including but not limited to the content, data, examples, materials, graphs, 
;;;;;*and trademarks, is the intellectual property of HOLTEK (and its licensors, where applicable) and is 
;;;;;*protected by copyright law and other intellectual property laws.
;;;;;************************************************************************************************************/ 


;;;;;//*************************************************************************
;;;;;//* @file    CALCULATE_POSITION.INC                                       *
;;;;;//* @version V1.00                                                        *
;;;;;//* @date    11/13/2020                                                   *
;;;;;//* @brief   The header file of the CALCULATE_POSITION library.           *
;;;;;//*************************************************************************

;;;;;// ---------------------------------------------------------------------- //
;;;;;// MCU Base Config                                                        //
;;;;;// ---------------------------------------------------------------------- //

;;;;;// *MCU**MCU**MCU**MCU**MCU**MCU**MCU**MCU**MCU**MCU**MCU**MCU**MCU**MCU***
;////start mcu option
        #define _CCV3_

        ;// <h> System_Config
              ;// <o> Vdd
              ;// <i> Vdd (for selections of Internal RC)
              ;// <1 =>    5V
              ;// <2 =>    3.0V
              #define  vdd	0

              ;// <o> SystemClock
              ;// <i> auto calibration period select 80ms ~ 1280ms
              ;// <0 =>     8MHZ    
              ;// <1 =>    12MHZ
              ;// <2 =>    16MHZ
              #define  SystemClock  0
              
        ;// </h>
        
       
	;//<o>LOOP_TIMES
	     #define LOOP_TIMES		12
		 
		;//<s>LOOP_VAR_NAME
	    #define LOOP_VAR_NAME     "cn"
        #define cn 0	
        #include "..\DEVICE\BS86D12C.DEV"
;////end mcu option
;;;;;//
;;;;;// ---------------------------------------------------------------------- //
;;;;;// C & ASM Includes,define                                                //
;;;;;// *g**g**g**g**g**g**g**g**g**g**g**g**g**g**g**g**g**g**g**g**g**g**g**
      #define TouchLibraryVersion     512H

;;;;;//
;;;;;// ---------------------------------------------------------------------- //



;;;;;// ---------------------------------------------------------------------- //
;;;;;// assembly exported function,register                                    //
;;;;;// *asm**asm**asm**asm**asm**asm**asm**asm**asm**asm**asm**asm**asm**asm* //
;//<TOUCH>__KEY1_EN
;//<ALIAS>KEY1
#define ___KEY1 	PB0
#define ___KEY1_C 	PBC0
#define ___KEY1_PU 	PBPU0

;//<TOUCH>__KEY2_EN
;//<ALIAS>KEY2
#define ___KEY2 	PB1
#define ___KEY2_C 	PBC1
#define ___KEY2_PU 	PBPU1  

;//<TOUCH>__KEY3_EN
;//<ALIAS>KEY3
#define ___KEY3 	PB2
#define ___KEY3_C 	PBC2
#define ___KEY3_PU 	PBPU2  

;//<TOUCH>__KEY4_EN
;//<ALIAS>KEY4
#define ___KEY4 	PB3
#define ___KEY4_C 	PBC3
#define ___KEY4_PU 	PBPU3

;//<TOUCH>__KEY5_EN
;//<ALIAS>KEY5
#define ___KEY5 	PB4
#define ___KEY5_C 	PBC4
#define ___KEY5_PU 	PBPU4  

;//<TOUCH>__KEY6_EN
;//<ALIAS>KEY6
#define ___KEY6 	PB5
#define ___KEY6_C 	PBC5
#define ___KEY6_PU 	PBPU5

;//<TOUCH>__KEY7_EN
;//<ALIAS>KEY7
#define ___KEY7 	PB6
#define ___KEY7_C 	PBC6
#define ___KEY7_PU 	PBPU6  

;//<TOUCH>__KEY8_EN
;//<ALIAS>KEY8
#define ___KEY8 	PB7
#define ___KEY8_C 	PBC7
#define ___KEY8_PU 	PBPU7  

;//<TOUCH>__KEY9_EN
;//<ALIAS>KEY9
#define ___KEY9 	0
#define ___KEY9_C 	0
#define ___KEY9_PU 	0 


;//<TOUCH>__KEY10_EN
;//<ALIAS>KEY10
#define ___KEY10 	0
#define ___KEY10_C 	0
#define ___KEY10_PU 0

;//<TOUCH>__KEY11_EN
;//<ALIAS>KEY11
#define ___KEY11 	0
#define ___KEY11_C 	0
#define ___KEY11_PU 0

;//<TOUCH>__KEY12_EN
;//<ALIAS>KEY12
#define ___KEY12 	0
#define ___KEY12_C 	0
#define ___KEY12_PU 0



;////start option_ASM
;// <h> TKL_Config
    
      ;// <o> DebounceTimes
      ;// <i> Debounce in output key status 
      ;// <0-15>
      #define DebounceTimes           2

      ;// <o> AutoCalibrationPeriod
      ;// <i> auto calibration period select 80ms to 1280ms.The faster speed the better the anti-interference and the worse the sensitivity.The slower speed the worse the anti-interference and the better the sensitivity
      ;// <0 =>     80mS    
      ;// <1 =>    160mS
      ;// <2 =>    240mS
      ;// <3 =>    320mS
      ;// <4 =>    400mS
      ;// <5 =>    480mS
      ;// <6 =>    560mS
      ;// <7 =>    640mS
      ;// <8 =>    720mS
      ;// <9 =>    800mS
      ;// <10=>    880mS
      ;// <11=>    960mS
      ;// <12=>   1040mS
      ;// <13=>   1120mS
      ;// <14=>   1200mS
      ;// <15=>   1280mS
      #define  AutoCalibrationPeriod  7

      ;// <o> HighSensitive 
      ;// <i> Sensitive double up,0-Normal  1-High Sensitive
      ;// <0=>     Normal
      ;// <1=>     High
      #define HighSensitive           1

      ;// <o> MaximumKeyHoldTime 
      ;// <i> key holding time ,if time out will reset key status to non-press,Range is 4S~60S or Disable the function
      ;// <0 =>  Disable    
      ;// <1 =>       4S
      ;// <2 =>       8S
      ;// <3 =>      12S
      ;// <4 =>      16S
      ;// <5 =>      20S
      ;// <6 =>      24S
      ;// <7 =>      28S
      ;// <8 =>      32S
      ;// <9 =>      36S
      ;// <10=>      40S
      ;// <11=>      44S
      ;// <12=>      48S
      ;// <13=>      52S
      ;// <14=>      56S
      ;// <15=>      60S
      #define  MaximumKeyHoldTime     1

      ;// <q> AutoFrequencyHopping
      ;// <i> enalbe hardware hopping function     
      #define AutoFrequencyHopping    1
      
      
      ;// <q> OneKeyActive
      ;// <i> Only one or all key active at the same time
      ;// <0 =>  all key active    
      ;// <1 =>  one key active
      #define OneKeyActive            0

      ;// <q> PowerSave
      ;// <i> Low power mode 
      #define PowerSave               0
  
      ;// <q> TKL_MovingCalibration
      ;// <i> Touch key Moving Calibration keys easier to operate
      #define MovingCalibration       1
      
      ;// <o>MainFreqSelect
      ;// <i> Touch frequency selection
      ;// <0 =>  3M&7M
      ;// <1 =>  3M&11M
      ;// <2 =>  1M&3M
      #define MainFreqSelect          1


   ;// </h>
  
  ;// <h> TKL_ENABLE_KEY
      
    ;// <i> If the setting over the maximum key, it will be set disable
    
    ;// <e> __KEY1_EN
      ;// <i> 0=__KEY1 disable 1=__KEY1 enalbe 
        #define __KEY1_EN                 1
      ;// <o> Key1Threshold
      ;// <i> suggestion range 12 ~ 63
      ;// <12-63>
        #define Key1Threshold            16
      ;// <o> __KEY1_WAKE
      ;// <i> sleep mode work Touch 
      ;// <0=>     Enable
      ;// <1=>     Disable
        #define __KEY1_WAKE               0
    ;// </e>
    
    ;// <e> __KEY2_EN
      ;// <i> 0=__KEY2 disable 1=__KEY2 enalbe 
        #define __KEY2_EN                 1
      ;// <o> Key2Threshold
      ;// <i> suggestion range 12 ~ 63
      ;// <12-63>
        #define Key2Threshold            16
      ;// <o> __KEY2_WAKE
      ;// <i> sleep mode work Touch 
      ;// <0=>     Enable
      ;// <1=>     Disable
        #define __KEY2_WAKE               0
    ;// </e>
    
    ;// <e> __KEY3_EN
      ;// <i> 0=__KEY3 disable 1=__KEY3 enalbe 
        #define __KEY3_EN                 1
      ;// <o> Key3Threshold
      ;// <i> suggestion range 12 ~ 63
      ;// <12-63>
        #define Key3Threshold            16
      ;// <o> __KEY3_WAKE
      ;// <i> sleep mode work Touch 
      ;// <0=>     Enable
      ;// <1=>     Disable
        #define __KEY3_WAKE               0
    ;// </e>
    
    ;// <e> __KEY4_EN
      ;// <i> 0=__KEY4 disable 1=__KEY4 enalbe 
        #define __KEY4_EN                 1
      ;// <o> Key4Threshold
      ;// <i> suggestion range 12 ~ 63
      ;// <12-63>
        #define Key4Threshold            16
      ;// <o> __KEY4_WAKE
      ;// <i> sleep mode work Touch 
      ;// <0=>     Enable
      ;// <1=>     Disable
        #define __KEY4_WAKE               0
    ;// </e>
    
    ;// <e> __KEY5_EN
      ;// <i> 0=__KEY5 disable 1=__KEY5 enalbe 
        #define __KEY5_EN                 1
      ;// <o> Key5Threshold
      ;// <i> suggestion range 12 ~ 63
      ;// <12-63>
        #define Key5Threshold            16
      ;// <o> __KEY5_WAKE
      ;// <i> sleep mode work Touch 
      ;// <0=>     Enable
      ;// <1=>     Disable
        #define __KEY5_WAKE               0
    ;// </e>
    
    ;// <e> __KEY6_EN
      ;// <i> 0=__KEY6 disable 1=__KEY6 enalbe 
        #define __KEY6_EN                 1
      ;// <o> Key6Threshold
      ;// <i> suggestion range 12 ~ 63
      ;// <12-63>
        #define Key6Threshold            16
      ;// <o> __KEY6_WAKE
      ;// <i> sleep mode work Touch 
      ;// <0=>     Enable
      ;// <1=>     Disable
        #define __KEY6_WAKE               0
    ;// </e>
    
    ;// <e> __KEY7_EN
      ;// <i> 0=__KEY7 disable 1=__KEY7 enalbe 
        #define __KEY7_EN                 1
      ;// <o> Key7Threshold
      ;// <i> suggestion range 12 ~ 63
      ;// <12-63>
        #define Key7Threshold            16
      ;// <o> __KEY7_WAKE
      ;// <i> sleep mode work Touch 
      ;// <0=>     Enable
      ;// <1=>     Disable
        #define __KEY7_WAKE               0
    ;// </e>
    
    ;// <e> __KEY8_EN
      ;// <i> 0=__KEY8 disable 1=__KEY8 enalbe 
        #define __KEY8_EN                 1
      ;// <o> Key8Threshold
      ;// <i> suggestion range 12 ~ 63
      ;// <12-63>
        #define Key8Threshold            16
      ;// <o> __KEY8_WAKE
      ;// <i> sleep mode work Touch 
      ;// <0=>     Enable
      ;// <1=>     Disable
        #define __KEY8_WAKE               0
    ;// </e>
    
    ;// <e> __KEY9_EN
      ;// <i> 0=__KEY9 disable 1=__KEY9 enalbe 
        #define __KEY9_EN                 0
      ;// <o> Key9Threshold
      ;// <i> suggestion range 12 ~ 63
      ;// <12-63>
        #define Key9Threshold            16
      ;// <o> __KEY9_WAKE
      ;// <i> sleep mode work Touch 
      ;// <0=>     Enable
      ;// <1=>     Disable
        #define __KEY9_WAKE               0
    ;// </e>
    
    ;// <e> __KEY10_EN
      ;// <i> 0=__KEY10 disable 1=__KEY10 enalbe 
        #define __KEY10_EN                 0
      ;// <o> Key10Threshold
      ;// <i> suggestion range 12 ~ 63
      ;// <12-63>
        #define Key10Threshold            16
      ;// <o> __KEY10_WAKE
      ;// <i> sleep mode work Touch 
      ;// <0=>     Enable
      ;// <1=>     Disable
        #define __KEY10_WAKE               0
    ;// </e>
    
    ;// <e> __KEY11_EN
      ;// <i> 0=__KEY11 disable 1=__KEY11 enalbe 
        #define __KEY11_EN                 0
      ;// <o> Key11Threshold
      ;// <i> suggestion range 12 ~ 63
      ;// <12-63>
        #define Key11Threshold            16
      ;// <o> __KEY11_WAKE
      ;// <i> sleep mode work Touch 
      ;// <0=>     Enable
      ;// <1=>     Disable
        #define __KEY11_WAKE               0
    ;// </e>
    
    ;// <e> __KEY12_EN
      ;// <i> 0=__KEY12 disable 1=__KEY12 enalbe 
        #define __KEY12_EN                 0
      ;// <o> Key12Threshold
      ;// <i> suggestion range 12 ~ 63
      ;// <12-63>
        #define Key12Threshold            16
      ;// <o> __KEY12_WAKE
      ;// <i> sleep mode work Touch 
      ;// <0=>     Enable
      ;// <1=>     Disable
        #define __KEY12_WAKE               0
    ;// </e>
  ;// </h> 

    #define IO_TOUCH_ATTR_IP0   (__KEY1_EN<<0|__KEY2_EN<<1|__KEY3_EN<<2|__KEY4_EN<<3)
    #define IO_TOUCH_ATTR_IP1   (__KEY5_EN<<4|__KEY6_EN<<5|__KEY7_EN<<6|__KEY8_EN<<7)
    #define IO_TOUCH_ATTR_IP2   (__KEY9_EN<<8|__KEY10_EN<<9|__KEY11_EN<<10|__KEY12_EN<<11)
    #define IO_TOUCH_ATTR       (IO_TOUCH_ATTR_IP0|IO_TOUCH_ATTR_IP1|IO_TOUCH_ATTR_IP2)
      
    #define WAKEUP_KEY_IP0      ((__KEY1_WAKE^1)<<0|(__KEY2_WAKE^1)<<1|(__KEY3_WAKE^1)<<2|(__KEY4_WAKE^1)<<3)
    #define WAKEUP_KEY_IP1      ((__KEY5_WAKE^1)<<4|(__KEY6_WAKE^1)<<5|(__KEY7_WAKE^1)<<6|(__KEY8_WAKE^1)<<7)
    #define WAKEUP_KEY_IP2      ((__KEY9_WAKE^1)<<8|(__KEY10_WAKE^1)<<9|(__KEY11_WAKE^1)<<10|(__KEY12_WAKE^1)<<11)
    #define WAKEUP_KEY          (WAKEUP_KEY_IP0|WAKEUP_KEY_IP1|WAKEUP_KEY_IP2)
    


;////end option_ASM
;;;;;//




;;;;;// ---------------------------------------------------------------------- //
;;;;;// C exported function,register                                           //
;;;;;// ---------------------------------------------------------------------- //
;/*
ifdef _INCLUDE_C_
;*/
;;;;;// *c**c**c**c**c**c**c**c**c**c**c**c**c**c**c**c**c**c**c**c**c**c**c**c //


;////start option_C


;////end option_C
;;;;;//
;/*
endif
;*/

