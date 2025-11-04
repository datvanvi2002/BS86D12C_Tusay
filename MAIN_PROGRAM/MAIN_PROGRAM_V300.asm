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



message '****************************************************************'
message '*PROJECT NAME :MAIN PROGRAM                                    *'
message '*     VERSION :V300                                            *'
message '* ICE VERSION :                                                *'
message '*      DATE   :20200930                                        *'
message '****************************************************************'
                PUBLIC  _DATA_BUF  
                PUBLIC  _HALT_STATE

                #define ESCfunctionIDcheck

                ;==============
                ;=DATA SETCTION
                ;==============
MAIN_DATA       .SECTION          'DATA'
_DATA_BUF       DB      8 DUP(?)
_HALT_STATE     DB      ?      
ifndef   ESCfunctionIDcheck
_FUNCTION_ID    DB      ?
endif
                 ;=INCLUDE REFERENCE FILE
                INCLUDE MAIN_PROGRAM.INC
 

.LISTMACRO
EXECUTE         MACRO   FUNCTION,IDnumber,DefineName
        ifdef   FUNCTION
                ifndef DefineName
                    #define DefineName
                endif
                
                ifdef   PBP
                    MOV     A,BANK FUNCTION
                    MOV     PBP,A
                endif
                    ;-FUCNTION ID CHECK 
                ifndef      ESCfunctionIDcheck
                    if      IDnumber != 0
                    MOV     A,IDnumber
                    MOV     _FUNCTION_ID,A
                    endif     
                endif
                    ;--------------- 
                    CALL    FUNCTION
                    ;---------------
                ifndef      ESCfunctionIDcheck
                    if      IDnumber != 0
                    MOV     A,IDnumber
                    XOR     A,_FUNCTION_ID
                    SZ      ACC
                    JMP     PROGRAM_RESET           ;PCL Abnormal
                    endif
                endif     
                    ifdef   PBP
                    MOV     A,BANK MAIN_LOOP
                    MOV     PBP,A
                    endif  
        endif
                ENDM


EXTEND_FUNCTION_INITIAL MACRO
                EXECUTE EXTEND_FUNCTION_1A_INITIAL,040H,DEF_INITIAL 
                EXECUTE EXTEND_FUNCTION_1B_INITIAL,041H,DEF_INITIAL 
                EXECUTE EXTEND_FUNCTION_1C_INITIAL,042H,DEF_INITIAL 
                EXECUTE EXTEND_FUNCTION_1D_INITIAL,043H,DEF_INITIAL 
                EXECUTE EXTEND_FUNCTION_1E_INITIAL,044H,DEF_INITIAL 
                EXECUTE EXTEND_FUNCTION_1F_INITIAL,045H,DEF_INITIAL 
                EXECUTE EXTEND_FUNCTION_1G_INITIAL,046H,DEF_INITIAL 
                EXECUTE EXTEND_FUNCTION_1H_INITIAL,047H,DEF_INITIAL 
                EXECUTE EXTEND_FUNCTION_2A_INITIAL,048H,DEF_INITIAL
                EXECUTE EXTEND_FUNCTION_2B_INITIAL,049H,DEF_INITIAL
                EXECUTE EXTEND_FUNCTION_2C_INITIAL,04AH,DEF_INITIAL
                EXECUTE EXTEND_FUNCTION_2D_INITIAL,04BH,DEF_INITIAL
                EXECUTE EXTEND_FUNCTION_2E_INITIAL,04CH,DEF_INITIAL
                EXECUTE EXTEND_FUNCTION_2F_INITIAL,04DH,DEF_INITIAL
                EXECUTE EXTEND_FUNCTION_2G_INITIAL,04EH,DEF_INITIAL
                EXECUTE EXTEND_FUNCTION_2H_INITIAL,04FH,DEF_INITIAL
                ENDM


EXTEND_FUNCTION_WORK MACRO
                EXECUTE EXTEND_FUNCTION_1A,050H,WORK
                EXECUTE EXTEND_FUNCTION_1B,051H,WORK
                EXECUTE EXTEND_FUNCTION_1C,052H,WORK
                EXECUTE EXTEND_FUNCTION_1D,053H,WORK
                EXECUTE EXTEND_FUNCTION_1E,054H,WORK
                EXECUTE EXTEND_FUNCTION_1F,055H,WORK
                EXECUTE EXTEND_FUNCTION_1G,056H,WORK
                EXECUTE EXTEND_FUNCTION_1H,057H,WORK
                EXECUTE EXTEND_FUNCTION_2A,058H,WORK
                EXECUTE EXTEND_FUNCTION_2B,059H,WORK
                EXECUTE EXTEND_FUNCTION_2C,05AH,WORK
                EXECUTE EXTEND_FUNCTION_2D,05BH,WORK
                EXECUTE EXTEND_FUNCTION_2E,05CH,WORK
                EXECUTE EXTEND_FUNCTION_2F,05DH,WORK
                EXECUTE EXTEND_FUNCTION_2G,05EH,WORK
                EXECUTE EXTEND_FUNCTION_2H,05FH,WORK
                ENDM
                
EXTEND_FUNCTION_HALT_PREPARE MACRO
                EXECUTE EXTEND_FUNCTION_1A_HALT_PREPARE,070H,DEF_HALT_PREPARE
                EXECUTE EXTEND_FUNCTION_1B_HALT_PREPARE,071H,DEF_HALT_PREPARE
                EXECUTE EXTEND_FUNCTION_1C_HALT_PREPARE,072H,DEF_HALT_PREPARE
                EXECUTE EXTEND_FUNCTION_1D_HALT_PREPARE,073H,DEF_HALT_PREPARE
                EXECUTE EXTEND_FUNCTION_1E_HALT_PREPARE,074H,DEF_HALT_PREPARE
                EXECUTE EXTEND_FUNCTION_1F_HALT_PREPARE,075H,DEF_HALT_PREPARE
                EXECUTE EXTEND_FUNCTION_1G_HALT_PREPARE,076H,DEF_HALT_PREPARE
                EXECUTE EXTEND_FUNCTION_1H_HALT_PREPARE,077H,DEF_HALT_PREPARE
                EXECUTE EXTEND_FUNCTION_2A_HALT_PREPARE,078H,DEF_HALT_PREPARE
                EXECUTE EXTEND_FUNCTION_2B_HALT_PREPARE,079H,DEF_HALT_PREPARE
                EXECUTE EXTEND_FUNCTION_2C_HALT_PREPARE,07AH,DEF_HALT_PREPARE
                EXECUTE EXTEND_FUNCTION_2D_HALT_PREPARE,07BH,DEF_HALT_PREPARE
                EXECUTE EXTEND_FUNCTION_2E_HALT_PREPARE,07CH,DEF_HALT_PREPARE
                EXECUTE EXTEND_FUNCTION_2F_HALT_PREPARE,07DH,DEF_HALT_PREPARE
                EXECUTE EXTEND_FUNCTION_2G_HALT_PREPARE,07EH,DEF_HALT_PREPARE
                EXECUTE EXTEND_FUNCTION_2H_HALT_PREPARE,07FH,DEF_HALT_PREPARE
                ENDM

EXTEND_FUNCTION_HALT_WAKEUP MACRO
                EXECUTE EXTEND_FUNCTION_1A_HALT_WAKEUP,070H,DEF_HALT_WAKEUP
                EXECUTE EXTEND_FUNCTION_1B_HALT_WAKEUP,071H,DEF_HALT_WAKEUP
                EXECUTE EXTEND_FUNCTION_1C_HALT_WAKEUP,072H,DEF_HALT_WAKEUP
                EXECUTE EXTEND_FUNCTION_1D_HALT_WAKEUP,073H,DEF_HALT_WAKEUP
                EXECUTE EXTEND_FUNCTION_1E_HALT_WAKEUP,074H,DEF_HALT_WAKEUP
                EXECUTE EXTEND_FUNCTION_1F_HALT_WAKEUP,075H,DEF_HALT_WAKEUP
                EXECUTE EXTEND_FUNCTION_1G_HALT_WAKEUP,076H,DEF_HALT_WAKEUP
                EXECUTE EXTEND_FUNCTION_1H_HALT_WAKEUP,077H,DEF_HALT_WAKEUP
                EXECUTE EXTEND_FUNCTION_2A_HALT_WAKEUP,078H,DEF_HALT_WAKEUP
                EXECUTE EXTEND_FUNCTION_2B_HALT_WAKEUP,079H,DEF_HALT_WAKEUP
                EXECUTE EXTEND_FUNCTION_2C_HALT_WAKEUP,07AH,DEF_HALT_WAKEUP
                EXECUTE EXTEND_FUNCTION_2D_HALT_WAKEUP,07BH,DEF_HALT_WAKEUP
                EXECUTE EXTEND_FUNCTION_2E_HALT_WAKEUP,07CH,DEF_HALT_WAKEUP
                EXECUTE EXTEND_FUNCTION_2F_HALT_WAKEUP,07DH,DEF_HALT_WAKEUP
                EXECUTE EXTEND_FUNCTION_2G_HALT_WAKEUP,07EH,DEF_HALT_WAKEUP
                EXECUTE EXTEND_FUNCTION_2H_HALT_WAKEUP,07FH,DEF_HALT_WAKEUP
                ENDM
                
EXTEND_FUNCTION_RETURN_MAIN MACRO
                EXECUTE EXTEND_FUNCTION_1A_RETURN_MAIN,080H,DEF_RETURN_MAIN
                EXECUTE EXTEND_FUNCTION_1B_RETURN_MAIN,081H,DEF_RETURN_MAIN
                EXECUTE EXTEND_FUNCTION_1C_RETURN_MAIN,082H,DEF_RETURN_MAIN
                EXECUTE EXTEND_FUNCTION_1D_RETURN_MAIN,083H,DEF_RETURN_MAIN
                EXECUTE EXTEND_FUNCTION_1E_RETURN_MAIN,084H,DEF_RETURN_MAIN
                EXECUTE EXTEND_FUNCTION_1F_RETURN_MAIN,085H,DEF_RETURN_MAIN
                EXECUTE EXTEND_FUNCTION_1G_RETURN_MAIN,086H,DEF_RETURN_MAIN
                EXECUTE EXTEND_FUNCTION_1H_RETURN_MAIN,087H,DEF_RETURN_MAIN
                EXECUTE EXTEND_FUNCTION_2A_RETURN_MAIN,088H,DEF_RETURN_MAIN
                EXECUTE EXTEND_FUNCTION_2B_RETURN_MAIN,089H,DEF_RETURN_MAIN
                EXECUTE EXTEND_FUNCTION_2C_RETURN_MAIN,08AH,DEF_RETURN_MAIN
                EXECUTE EXTEND_FUNCTION_2D_RETURN_MAIN,08BH,DEF_RETURN_MAIN
                EXECUTE EXTEND_FUNCTION_2E_RETURN_MAIN,08CH,DEF_RETURN_MAIN
                EXECUTE EXTEND_FUNCTION_2F_RETURN_MAIN,08DH,DEF_RETURN_MAIN
                EXECUTE EXTEND_FUNCTION_2G_RETURN_MAIN,08EH,DEF_RETURN_MAIN
                EXECUTE EXTEND_FUNCTION_2H_RETURN_MAIN,08FH,DEF_RETURN_MAIN
                ENDM


                ;==============
                ;=CODE SETCTION
                ;==============
PROGRAM_ENTRY   .SECTION  AT 000H 'CODE'
        ifdef   PBP
                MOV     A,BANK PROGRAM_RESET
                MOV     PBP,A
        endif
                JMP     PROGRAM_RESET

                ;==============
                ;=MAIN PROGRAM=
                ;==============
MAIN_PROGRAM    .SECTION          'CODE'

                ;;***********************
PROGRAM_RESET:  ;;* PROGRAM ENTRY *******
                ;;***********************
                ;---------------------
                ;-MCU HARDWARE INITIAL
                ;---------------------
                MCU_HARDWARE_INITIAL
                
                ;-IF POWER SAVING ENABLE
            ifdef   PBP
                MOV     A,BANK WDT_WAKEUP
                MOV     PBP,A
            endif
        
                SNZ     PDF
                JMP     $+3
                SZ      TO
                JMP     WDT_WAKEUP              ;IF WAKEUP BY WDT TIME OUT

                ;------------------------
                ;-CLEAR RAM DATA
                ;------------------------
                CLEAR_RAM_DATA
                ;------------------------
                ;-EXTEND FUNCTION INITIAL
                ;------------------------
                EXTEND_FUNCTION_INITIAL
                

                ;;-----------------------
MAIN_LOOP:      ;;- MAIN PROGRAM LOOP ---
                ;;-----------------------
                CLR     WDT
                CLR     WDT1
                CLR     WDT2
                ;----------------------
                ;-RE INITIAL SYS. CLOCK
                ;----------------------
                
                RELOAD_SYS_CLOCK
                
                SET     _HALT_STATE
                
                EXTEND_FUNCTION_WORK
            
            
            
        ifdef   PBP
                MOV     A,BANK MAIN_LOOP
                MOV     PBP,A
        endif
            
                SNZ     _HALT_STATE.7       ;CHECK ALL PROGRAM CONTORL _HALT_STATE
                SNZ     _HALT_STATE.0
                JMP     MAIN_LOOP
                
                ;----------------------
ENTER_HALT_MODE:;-HALE INITIAL FUNCTION
                ;----------------------
                ;--------------------
WDT_WAKEUP:     ;-WDT WAKEUP FUNCTION     
                ;--------------------
                
                SET     _HALT_STATE
                
                EXTEND_FUNCTION_HALT_PREPARE
                
        ifdef   PBP
                MOV     A,BANK PROGRAM_WAKEUP
                MOV     PBP,A
        endif
        
                SNZ     _HALT_STATE.7       ;CHECK ALL PROGRAM CONTORL _HALT_STATE
                SNZ     _HALT_STATE.0
                JMP     PROGRAM_WAKEUP
                
ENTER_HALT:
                HALT
                
                
                ;--------------------
HALT_WAKEUP:    ;-HALT WAKEUP FUNCTION     
                ;--------------------
                
                SET     _HALT_STATE
                
                EXTEND_FUNCTION_HALT_WAKEUP
                
        ifdef   PBP
                MOV     A,BANK ENTER_HALT
                MOV     PBP,A
        endif
        
        
        ifdef DEF_HALT_WAKEUP
                SNZ     _HALT_STATE.7       ;CHECK ALL PROGRAM CONTORL _HALT_STATE
                SZ      _HALT_STATE
                JMP     ENTER_HALT 
        endif
                
                ;--------------------
PROGRAM_WAKEUP: ;-WAKEUP FUNCTION     
                ;--------------------
                
        ifdef   PBP
                MOV     A,BANK MAIN_LOOP
                MOV     PBP,A
        endif
        
                EXTEND_FUNCTION_RETURN_MAIN
                JMP     MAIN_LOOP



