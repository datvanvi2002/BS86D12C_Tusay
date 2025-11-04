 


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




;;;;;/*********************************************************************************************************//**
;;;;;// * @file    MAIN_PROGRAM.INC
;;;;;// * @version V1.00
;;;;;// * @date    11/13/2020
;;;;;// * @brief   The header file of the MAIN_PROGRAM library.
;;;;;// *************************************************************************************************************
;;;;;// * @attention
;;;;;// * <h2><center>Copyright (C) Best Solution Inc. All rights reserved</center></h2>
;;;;;// ************************************************************************************************************/
;;;;;///* ---------------------------------------------------------------------------------------------------------*/

;;;;;///* ---------------------------------------------------------------------- */
;;;;;///* assembly exported function,register                                    */
;;;;;///* ---------------------------------------------------------------------- */
;;;;;/*
;;;;;//-SUBROUTIN

#define MAIN_PROGRAM_DEF
#include "..\GLOBE\GLOBE_VARIES.INC"
#include "..\GLOBE\GLOBE_INCLUDE.INC"


;//;;;;;//-FUNCTION DEFINE               ;-FUNCTION NAME
#include "..\BS86D12C_CTOUCH\BS86D12C_CTOUCH.INI"
#define EXTEND_FUNCTION_1A_INITIAL     _BS86D12C_CTOUCH_INITIAL
#define EXTEND_FUNCTION_1A             _BS86D12C_CTOUCH
#define EXTEND_FUNCTION_1A_HALT_PREPARE _BS86D12C_CTOUCH_HALT_PREPARE
#define EXTEND_FUNCTION_1A_HALT_WAKEUP _BS86D12C_CTOUCH_HALT_WAKEUP
#define EXTEND_FUNCTION_1A_RETURN_MAIN _BS86D12C_CTOUCH_RETURN_MAIN

#define EXTEND_FUNCTION_1B_INITIAL     _USER_PROGRAM_C_INITIAL
#define EXTEND_FUNCTION_1B             _USER_PROGRAM_C
#define EXTEND_FUNCTION_1B_HALT_PREPARE _USER_PROGRAM_C_HALT_PREPARE
#define EXTEND_FUNCTION_1B_HALT_WAKEUP _USER_PROGRAM_C_HALT_WAKEUP
#define EXTEND_FUNCTION_1B_RETURN_MAIN _USER_PROGRAM_C_RETURN_MAIN


;;;;;//-FLAG DEFINE
;;;;;*/
;;;;;////--------------------------------------------------------------------------
;;;;;//
;;;;;///**
;;;;;//  * @}
;;;;;//  */
;;;;;///* ==================================================================================================================================================*/


