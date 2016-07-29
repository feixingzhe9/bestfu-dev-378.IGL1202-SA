/***************************Copyright BestFu ***********************************
**  文    件：  Key.c
**  功    能：  <<逻辑层>> 摁键处理 
**  编    译：  Keil uVision5 V5.10
**  版    本：  V1.1
**  编    写：  Seven
**  创建日期：  2014.08.20
**  修改日期：  2014.08.20
**  说    明：  
**  V1.1
    >> 短按键为释放时才执行，并添加长按键响应
*******************************************************************************/
#ifndef _KEY_H_
#define _KEY_H_

#include "BF_type.h"

#define MAX_TOUCH_KEY       		(13+1)  //通道0 保留
#define MAX_KEY_NUM 				(12)
#define NONE_KEY					(0xff)	//定义无键键值
#define LONG_KEY_TIMES				(250)	//定义长按键时间 250 * 5ms = 1.25s
#define ADDR_KEY_CURRENT_STATE     (0x4008) //保存按键状态的EEPROM地址

extern u8 gPassword[4];
extern u8 gPswFlag;

/*按键状态*/
typedef enum
{
    KEY_NONE  = 0,
    KEY_SHORT = 1,
    KEY_LONG  = 2
}KeyState_e;

typedef struct
{
    KeyState_e  Status;
}KeyProp_t;


typedef enum		//状态资源枚举定义
{
	SW_OFF = 0,
	SW_ON  = 1
}KeySWStatus_e;

typedef struct 
{
	KeySWStatus_e Status;
}KeyStatus_t;


typedef struct	//用于休眠时记录各个按键的状态
{
    u8    KeyValue;
    KeyState_e    KeyType; 
	KeySWStatus_e    KeyStatusVal[MAX_TOUCH_KEY];//按键的属性状态保存
}KeySta_t,*pKeySta_t;

extern KeyProp_t KeyCh[MAX_TOUCH_KEY];          //按键属性状态保存
extern KeySta_t gKeyPrevState;
extern KeyProp_t  gKeyCh[];
extern KeyStatus_t gKeyStatus[];

//Function declaration 
extern void Key_Init(void);                     //按键初始化
extern void Key_Handle(void);                   //普通按键处理
extern void Save_CurrentKeyState(void);
extern u8 Key_FirstScan(void);
extern void KeyEXTIHandle(void);
extern void KeyTIMHandle(void);
extern void KeyScanf_Init(void);
extern void Get_CurrentKeyState(KeySta_t * pKeyState);
#endif

/***************************Copyright BestFu **********************************/
