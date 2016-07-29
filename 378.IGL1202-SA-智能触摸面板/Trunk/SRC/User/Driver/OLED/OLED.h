/***************************Copyright BestFu ***********************************
**  ?    ?:  OLED.c
**  ?    ?:  <<???>> 1.3?OLED????
**  ?    ?:  Keil uVision5 V5.10
**  ?    ?:  V1.0
**  ?    ?:  Seven
**  ????:  2014.06.30
**  ????:  2014.08.20
**  ?    ?:  SH1106 ????
**              OLED ????????
**              ??:???,????,????,???,???
*******************************************************************************/

/******************************************************************************
**        OLED Module Message
** LCD Module:        VGM12864C3W01 -- 1.3 inch
** Display Size:      132x64
** Driver/Controller: SH1106
** Interface:         IIC????(400KHz) (???? 8080/SPI??)
******************************************************************************/
#ifndef _OLED_H_
#define _OLED_H_

#include "BF_type.h"
#include "GPIO.h"

#define SUPPORT_OLED_PUT_ASCII 1    //OLED?????? ???????   0 ???   1??

//====================================================
//*******OLED12864 Inteface STM32L151C8T6 I/O**********
//OLED????
#define GPIO_OLED_POW		GPIOC
#define PORT_OLED_POW		GPIO_C
#define  PIN_OLED_POW		(1<<9)

//RST ?????  ?????
#define GPIO_OLED_RST		GPIOB
#define PORT_OLED_RST		GPIO_B
#define  PIN_OLED_RST		(1<<5)
//SCL  ?????
//#define GPIO_OLED_SCL		GPIOB
//#define PORT_OLED_SCL		GPIO_B
//#define  PIN_OLED_SCL		(1<<6)
//SDA  ?????
//#define GPIO_OLED_SDA		GPIOB
//#define PORT_OLED_SDA		GPIO_B
//#define  PIN_OLED_SDA		(1<<7)

//=====================================================

#define OLED_POW_1      GPIO_OLED_POW->BSRRL |= PIN_OLED_POW
#define OLED_POW_0      GPIO_OLED_POW->BSRRH |= PIN_OLED_POW

#define OLED_RST_1      GPIO_OLED_RST->BSRRL |= PIN_OLED_RST
#define OLED_RST_0      GPIO_OLED_RST->BSRRH |= PIN_OLED_RST
//#define OLED_SCL_1		GPIO_OLED_SCL->BSRRL |= PIN_OLED_SCL
//#define OLED_SCL_0		GPIO_OLED_SCL->BSRRH |= PIN_OLED_SCL
//#define OLED_SDA_1		GPIO_OLED_SDA->BSRRL |= PIN_OLED_SDA
//#define OLED_SDA_0		GPIO_OLED_SDA->BSRRH |= PIN_OLED_SDA

extern void Init_OLED(void);
extern void OLED_ClearScreen(void);

#if SUPPORT_OLED_PUT_ASCII
extern void OLED_ShowChar(  unsigned char X0, unsigned char Y0,char DspChar		,unsigned char DspMode);
extern void OLED_ShowCN(	unsigned char X0, unsigned char Y0,const char *DspCN,unsigned char DspMode);
extern void OLED_ShowString(unsigned char X0, unsigned char Y0,const char *DspStr,unsigned char DspMode);
//extern void OLED_ShowNumber(unsigned char X0, unsigned char Y0,unsigned int Data,unsigned char len , unsigned char DspMode);
#endif
extern void OLED_ShowPicture(const u8 *p_Pic);
extern void OLED_ShowPicAt(u8 x0, u8 y0, u8 xlen, u8 ylen,const u8 *p_Pic);
extern void OLED_ShowHalfPicAt( u8 x0,const u32 *p_Pic);   //??1/2???
extern void OLED_ShowQuartPicAt(u8 x0,u8 y0,const u32 *p_Pic);  //??1/4???

extern void OLED_DrawLine(unsigned char x0);
extern void OLED_ShowNum18x32(unsigned char x0, unsigned char y0,  const u32 *p_Pic);
extern void OLED_Show32x323PicAt(u8 x0,u8 y0,const u32 *p_Pic);

#endif

/********************************* END FILE ***********************************/
