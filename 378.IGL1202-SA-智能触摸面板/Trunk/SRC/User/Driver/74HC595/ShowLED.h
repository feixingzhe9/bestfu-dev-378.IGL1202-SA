/***************************Copyright BestFu ***********************************
**  文    件：  ShowLED.c
**  功    能：  <<逻辑层>> 玻璃面板（74HC595驱动LED灯）制作灯光闪烁效果
**  编    译：  Keil uVision5 V5.10
**  版    本：  V1.1
**  编    写：  Seven
**  创建日期：  2014.08.26
**  修改日期：  2014.08.27
**  说    明：  >>LED灯逻辑位置 与 物理位置 映射  最大支持16个灯
**              >>完成灯光圈左右渐灭渐亮效果
*******************************************************************************/

#ifndef _ShowLED_H_
#define _ShowLED_H_

#include "BF_type.h"


extern void ShowLED_OpenAll(void);
extern void ShowLED_CloseAll(void);
extern void ShowLED_LightON( u8 keyNum , u16 lightState);
extern void ShowLED_LightOFF(u8 keyNum , u16 lightState);
extern void ShowLED_CheckDevice(void);
extern void ShowLED_CycleLED(void);
extern void ShowLED_StartCycleLED(u8 key);
extern void ShowLED_FlashLED(u8 key);
extern void ShowLED_ReverLED(void);
#endif 

/********************************* END FILE ***********************************/
