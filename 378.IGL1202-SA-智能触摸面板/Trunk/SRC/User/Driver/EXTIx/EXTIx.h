/***************************Copyright BestFu ***********************************
**  文    件：  EXTIx.h
**  功    能：  外部中断驱动头文件
**  编    译：  Keil uVision5 V5.10
**  芯    片：  STM32L151
**  版    本：  V1.0
**  编    写：  jay
**  创建日期：  2016-05-12
**  修改日期：  无
**  说    明：  
**  V1.0
    >> 
*******************************************************************************/
#ifndef _EXTI_H_
#define _EXTI_H_

#include "stm32l1xx.h"
#include "BF_TYPE.H"    

//fuction declaration
void EXTI_Init(void);
void Set_EXTI_Trigger(u8 TRIM);

#endif

/********************************* END FILE ***********************************/
