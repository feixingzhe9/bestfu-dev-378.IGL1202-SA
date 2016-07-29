/***************************Copyright BestFu ***********************************
**  文    件：  WTC6216.c
**  功    能：  <<驱动层>> WTC6216 十六通道电容触摸芯片
**  编    译：  Keil uVision5 V5.10
**  版    本：  V1.0
**  编    写：  Seven
**  创建日期：  2014.08.20
**  修改日期：  2014.08.20
**  说    明：  数据接口: INT_Flag线+四线BCD码
**  
*******************************************************************************/
#ifndef _WTC6216_H_
#define _WTC6216_H_

//#include "stm32l1xx.h"
#include "GPIO.h"
#include "BF_type.h"	

#define GPIO_WTC_TouchFlag		GPIOA
#define PORT_WTC_TouchFlag		GPIO_A
#define PIN_WTC_TouchFlag		(1<<0)

#define GPIO_WTC_DATA			GPIOC
#define PORT_WTC_DATA			GPIO_C
#define DATA0 		            (1<<1)	//PA1
#define DATA1 		            (1<<2)	//PA2
#define DATA2 		            (1<<3)	//PA3
#define DATA3 		            (1<<0)	//PA4

	
#define Get_TouchFlag()		( GPIO_WTC_TouchFlag->IDR & PIN_WTC_TouchFlag)
#define Get_TouchKey()   	(GPIO_WTC_DATA		->IDR & 0x0F)				//低四位 BCD 摁键

void WTC6216_Init(void);
u8   WTC6216_ReadKey(void);

#endif	   

/********************************* END FILE ***********************************/
