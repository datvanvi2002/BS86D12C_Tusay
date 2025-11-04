
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
;;;;;// * @file    GLOBE_INCLUDE.INC
;;;;;// * @version V1.00
;;;;;// * @date    11/13/2020
;;;;;// * @brief   The header file of the GLOBE_INCLUDE library.
;;;;;// *************************************************************************************************************
;;;;;// * @attention
;;;;;// * <h2><center>Copyright (C) Best Solution Inc. All rights reserved</center></h2>
;;;;;// ************************************************************************************************************/

;;;;;///* ==================================================================================================================================================*/
;;;;;///** @addtogroup GLOBE_INCLUDE
;;;;;//  * @{
;;;;;//  */
;;;;;///* ---------------------------------------------------------------------------------------------------------*/
;;;;;///* Exported constants --------------------------------------------------------------------------------------*/
;;;;;///* ---------------------------------------------------------------------------------------------------------*/
;;;;;///** @defgroup GLOBE_INCLUDE_exported_constants GLOBE_INCLUDE exported constants
;;;;;//  * @{
;;;;;//  */

#define USER_PROGRAM_C_DEF
#include "..\GLOBE\GLOBE_VARIES.INC"
#include "..\GLOBE\GLOBE_INCLUDE.INC"
;/*--------- <<<config>>> -----------------*/
;//<simple description>
;//<support MCU> all
;/*-------- <<<\config>>> ----------------*/
;;;;;//
;;;;;///**
;;;;;//  * @}
;;;;;//  */

;;;;;///* ---------------------------------------------------------------------- */
;;;;;///* assembly exported function,register                                    */
;;;;;///* ---------------------------------------------------------------------- */
;;;;;/*


;;;;;*/
;;;;;//--------------------------------------------------------------------------
;;;;;//--------------------------------------------------------------------------


;;;;;///* ---------------------------------------------------------------------- */
;;;;;///* C exported function,register                                           */
;;;;;///* ---------------------------------------------------------------------- */
;;;;;/*
ifdef _INCLUDE_C_
;;;;;*/
;;;;;//---------------------------------------------------------------


;;;;;//---------------------------------------------------------------
#ifdef  _CCV3_
;;;;;//-_CCV3_
#else
;;;;;//-_CCV2_
#endif

;;;;;//--------------------------------------------------------------------------
;;;;;//--------------------------------------------------------------------------
;;;;;/*
endif
;;;;;*/

;;;;;///* ---------------------------------------------------------------------- */
;;;;;///**
;;;;;//  * @}
;;;;;//  */
;;;;;///* ---------------------------------------------------------------------------------------------------------*/
;;;;;///**
;;;;;//  * @}
;;;;;//  */
;;;;;///* ==================================================================================================================================================*/

