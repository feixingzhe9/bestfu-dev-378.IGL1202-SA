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
#include "Key.h"
#include "LinkAll.h"
#include "delay.h"
#include "LowPower.h"
#include "WTC6216.h"
#include "USART1.h"
#include "MsgPackage.h"
#include "ShakeMotor.h"
#include "OLED.h"
#include "ShowLED.h"
#include "TouchPadPic.h"
#include "Queue.h"
#include "sys.h"
#include "TIMx.h"
#include "EXTIx.h"
#include "Program.h"
#include "AirConditionPad.h"
#include "wdg.h"
#include "usart.h"
	
//Public Variable definitions
KeyProp_t  gKeyCh[MAX_TOUCH_KEY];        	//定义按键的动作资源 <gKeyCh[0]保留>
KeyStatus_t gKeyStatus[MAX_TOUCH_KEY];		//定义按键的状态资源 <gKeyStatus[0]保留>
KeySta_t gKeyPrevState = {0,KEY_NONE};		//定义按键休眠保存键值
u8 gPassword[4] = {0};						//定义真实的密码存储数组
u8 gPswFlag = 1u; 							//用于记录密码存在与否属性值
volatile KeyCurStatus_t keyTemp;			//记录按键状态

//Private Variable definition
//定义摁键值--提供给属性层使用    1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16
static const u8  KeyMapTab[17] = {0, 11, 7,15, 3,13, 5, 8, 4,10, 6, 2, 0, 0, 0, 0, 0};
static u8 InputPassword[4] = {0};			//定义用户输入的密码值缓冲数组
static Queue_t  gQueue;						//定义按键接收队列
static u8 PswBit = 0; 						//记录密码已输入的位数    
static u8 first = 1u;

//Private function declaration
static void CheckUserPassWordInput(u8 keynum);
static void SaveKeyValueToQueue(volatile KeyCurStatus_t* pKey_temp);

/*******************************************************************************
**函    数： Key_Init()
**功    能： 按键模块初始化
**参    数： void
**返    回： void
*******************************************************************************/
void Key_Init(void)
{
    WTC6216_Init();
}

/*******************************************************************************
**函    数： void KeyScanf_Init(void)
**功    能： 按键扫描模块初始化
**参    数： void
**返    回： void
*******************************************************************************/
void KeyScanf_Init(void)
{
	u32 count = 0xffffffff;
	while((Get_TouchFlag())||(0 == count))
	{
		count--;
	} 
	InitQueue(&gQueue);	
	EXTI_Init();
	TIMx_Init(TIM2 ,31999 , 1);
}

/*******************************************************************************
**函    数： void KeyEXTIHandle(void)
**功    能： 按键中断处理
**参    数： void
**返    回： void
*******************************************************************************/
void KeyEXTIHandle(void)
{
	u8 keyValue = 0xff,i = 0;
	if(1 == first)
	{
		first = 0;
		keyValue =  WTC6216_ReadKey();
		for(i=1 ; i<=MAX_KEY_NUM ; i++)
		{
			if( KeyMapTab[i] == keyValue )  
			{
				break;
			}
		}
		if(i >  MAX_KEY_NUM)
		{
			keyTemp.KeyValue = NONE_KEY;
			return;
		}
		keyTemp.KeyValue = i;
		Set_TIMx_CNTValue(TIM2, 0);
		TIMx_Enable(TIM2);
		Set_EXTI_Trigger(FTIR);
	}
	else
	{
		first = 1;
		Set_EXTI_Trigger(RTIR);
		keyTemp.KeyType = KEY_SHORT;
		SaveKeyValueToQueue(&keyTemp);
		TIMx_Disable(TIM2);
		Set_TIMx_CNTValue(TIM2, 0);
	}
}

/*******************************************************************************
**函    数： void KeyTIMHandle(void)
**功    能： 按键定时器处理
**参    数： void
**返    回： void
*******************************************************************************/
void KeyTIMHandle(void)
{
	
    first = 1;
	Set_EXTI_Trigger(RTIR);
	keyTemp.KeyType = KEY_LONG;
	SaveKeyValueToQueue(&keyTemp);
	TIMx_Disable(TIM2);
	Set_TIMx_CNTValue(TIM2, 0);
}

/*******************************************************************************
**函    数： void SaveKeyValueToQueue(KeyCurStatus_t Key_temp)
**功    能： 保存按键值
**参    数： Key_temp 
**返    回： 无
**说    明： 
*******************************************************************************/
static void SaveKeyValueToQueue(volatile KeyCurStatus_t* pKey_temp)
{
	if((NONE_KEY != pKey_temp->KeyValue))                 
	{
		CPU_SR_ALLOC();
		cpu_sr = CPU_SR_Save();
		Enqueue(&gQueue,*pKey_temp);
		CPU_SR_Restore(cpu_sr);
		ShowLED_FlashLED(pKey_temp->KeyValue);
		pKey_temp->KeyValue = NONE_KEY;
	}
}

/*******************************************************************************
**函    数： Key_Handle()
**功    能： 摁键处理
**参    数： void 
**返    回： void
**说    明： 该函数 每隔 5~20ms 调用一次
*******************************************************************************/
void Key_Handle(void)
{
    u8 i;
    KeyCurStatus_t keyTemp;
    u8 result = 0x00;
	
    CPU_SR_ALLOC();
    cpu_sr = CPU_SR_Save();
    result = Dequeue(&gQueue,&keyTemp);
    CPU_SR_Restore(cpu_sr);
    
    if(result > 0u)
    {
		if(gPswFlag > 0)        //密码开启
		{
			CheckUserPassWordInput(keyTemp.KeyValue);
		}
		else if( keyTemp.KeyValue != NONE_KEY)
		{
			/*清除上次状态*/
			for(i=0;i<MAX_TOUCH_KEY;i++)
			{
				gKeyCh[i].Status = KEY_NONE; 
			} 
			gSleepFlag = 0;               
			gKeyPrevState.KeyType = keyTemp.KeyType;
			gKeyPrevState.KeyValue= keyTemp.KeyValue;
			StandbyCountReset();
			
			if(keyTemp.KeyValue > 6)
			{
				if(gUIMode == UIMODE_AIRCON)    
				{
					OLED_ClearScreen();     //从空调模式切换过来
				}
                gUIMode = UIMODE_NORMAL; 
				switch(keyTemp.KeyType)
				{
					case KEY_SHORT:
					{
						if(SW_OFF == gKeyStatus[keyTemp.KeyValue].Status)   //状态资源关
						{
							gKeyStatus[keyTemp.KeyValue].Status = SW_ON;
							OLED_ShowHalfPicAt(4,PicTab[keyTemp.KeyValue]);
							PropEventFifo(1, (keyTemp.KeyValue + 0x10), SRCEVENT , SW_ON);
						}
						else								//状态资源开
						{
							gKeyStatus[keyTemp.KeyValue].Status = SW_OFF;
							OLED_ShowHalfPicAt(4,PicTab[keyTemp.KeyValue + 32]);
							PropEventFifo(1, (keyTemp.KeyValue + 0x10), SRCEVENT , SW_OFF);
						}	
						gKeyPrevState.KeyStatusVal[keyTemp.KeyValue] = gKeyStatus[keyTemp.KeyValue].Status;
						gKeyCh[keyTemp.KeyValue].Status = KEY_SHORT; 
						PropEventFifo(1, keyTemp.KeyValue, SRCEVENT , KEY_SHORT);     //50ms 
						Upload(1);                                       //2.5s//上报
					}
					break;
					case KEY_LONG:
					{
						OLED_ShowHalfPicAt(4,PicTab[keyTemp.KeyValue + 16]);
						gKeyCh[keyTemp.KeyValue].Status = KEY_LONG;    
						PropEventFifo(1, keyTemp.KeyValue, SRCEVENT , KEY_LONG);        
						Upload(1);                                       //上报
					}
					break;
					default:break;
				} 
			}	
			else if(2 == keyTemp.KeyValue)//空调电源按键
			{
				if(gAirContionData.PowerSwitch == AC_POWER_ON)
				{
					gAirContionData.PowerSwitch = AC_POWER_OFF;
					OLED_ClearScreen();
					OLED_ShowHalfPicAt(4,PicTab[46]);           //空调关闭图片
					AirCondition_SendData();
				}
				else 
				{
					gAirContionData.PowerSwitch = AC_POWER_ON;
					AirCondition_FullShow();
				} 
				gUIMode = UIMODE_AIRCON;                    
            }
			else if(keyTemp.KeyValue <= 6)   //空调面板所属按键 (除开电源按键)
			{
				if(gAirContionData.PowerSwitch == AC_POWER_ON)
				{
					if(gUIMode == UIMODE_NORMAL)
					{
						AirCondition_FullShow();
					}
					else    //设置空调效果
					{
						if(gWukeUpForKey)
						{
							AirCondition_FirstShowPicture();
							gWukeUpForKey = 0;
						}
						AirCondition_KeyHander(keyTemp.KeyValue); 
					}
				}
				else if(gAirContionData.PowerSwitch == AC_POWER_OFF)  //电源关
				{
//					OLED_ClearScreen();
//					OLED_ShowHalfPicAt(4,PicTab[19]);           //空调关闭图片
					gAirContionData.PowerSwitch = AC_POWER_ON;
					AirCondition_FullShow();
				}
				gUIMode = UIMODE_AIRCON;
			}
			
		}
    }
}

/*******************************************************************************
**函    数： static void CheckUserPassWordInput(u8 keynum)
**功    能： 用户密码处理函数
**参    数： keynum：用户输入的按键值 
**返    回： void
**说    明： 该函数 每隔 5~20ms 调用一次
*******************************************************************************/
static void CheckUserPassWordInput(u8 keynum)
{
	static u8 FirstPassWord = 0u;  
	
	switch(FirstPassWord)
	{
		case 0:
		{
			FirstPassWord = 1;
			OLED_ClearScreen();
			OLED_ShowString(2,0,"请输入密码",0);
			OLED_ShowString(6,2,"----",0);
		}
		break;
		case 1:
		{
			if(keynum <= 6)      //123456按键有效
			{
				StandbyCountReset();
				if(gWukeUpForPassWord > 0u)
				{
					OLED_ShowString(1,0,"请重新输入密码",0);
					OLED_ShowString(4,1,"密码错误",0);
					OLED_ShowString(6,2,"----",0);
					gWukeUpForPassWord = 0;
					PswBit = 0;
				}
				OLED_ShowChar(6+PswBit,2,keynum + '0',0);
				InputPassword[PswBit] = keynum;
				if(PswBit++ >= 3)
				{
					PswBit=0;
					if((gPassword[0] == InputPassword[0])&& \
					   (gPassword[1] == InputPassword[1])&& \
					   (gPassword[2] == InputPassword[2])&& \
					   (gPassword[3] == InputPassword[3]) )
					{
						gPswFlag = 0;
						OLED_ClearScreen();
//                      OLED_ShowString(4,1,"欢迎使用",0);
						gSleepFlag = 0; 
						
						if(gKeyPrevState.KeyValue<7)
						{
							AirCondition_FirstShowPicture();
						}
						else
						{
							if(KEY_SHORT == gKeyPrevState.KeyType)
							{
								OLED_ShowHalfPicAt(4,PicTab[gKeyPrevState.KeyValue]);
							}
							else 
							{
								OLED_ShowHalfPicAt(4,PicTab[gKeyPrevState.KeyValue + 16]);
							}
						}
					}
					else 
					{
						OLED_ShowString(1,0,"请重新输入密码",0);
						OLED_ShowString(4,1,"密码错误",0);
						OLED_ShowString(6,2,"----",0);
					}
				}
			}
		}
		break;
		default:break;
	}
}

/*******************************************************************************
**函    数:  Key_FirstScan()
**功    能:  按键唤醒扫描
**参    数:  void
**返    回:  首次有效按键值 
*******************************************************************************/
u8 Key_FirstScan(void)
{
    u8 i;
    u8 first_key = NONE_KEY;
    u8 temp_key  = NONE_KEY;
    
    first_key = WTC6216_ReadKey();
    delay_us(200);
    if(first_key == WTC6216_ReadKey())
    {
        for(i=1 ; i<=MAX_KEY_NUM ; i++)
        {
            if( KeyMapTab[i] == first_key )  
            {
                temp_key = i;
                break;
            }
        }
        if( i > MAX_KEY_NUM)              temp_key = NONE_KEY;           
    }
    return temp_key;
}

/*******************************************************************************
**函    数:  void Save_CurrentKeyState(void)
**功    能:  保存当前的按键状态
**参    数:  无
**返    回:  void
********************************************************************************/
void Save_CurrentKeyState(void)       
{
    WriteDataToEEPROM(ADDR_KEY_CURRENT_STATE,sizeof(KeySta_t),(u8*)&gKeyPrevState);
}

/*******************************************************************************
**函    数:  static void Get_CurrentKeyState(u8 *)
**功    能:  获取存当前的按键状态
**参    数:  无
**返    回:  void
********************************************************************************/
void Get_CurrentKeyState(KeySta_t * pKeyState)
{
    ReadDataFromEEPROM(ADDR_KEY_CURRENT_STATE,sizeof(KeySta_t),(u8*)pKeyState);
}
/***************************Copyright BestFu **********************************/
