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
#include "AirConditionPad.h"
#include "Queue.h"
#include "sys.h"


/*定义摁键值*/
const u8 NONE_KEY=0xFF;       

/*定义摁键值--提供给属性层使用*/
KeyProp_t  KeyCh[MAX_TOUCH_KEY];        //KeyCh[0] 保留

static u8  NewKey,OldKey,LstKey;
static u16 LongKeyCount;                //长按键计数器
const  u16 LONG_KEY_TIMES = 250;        //长按键计数时间 250 *4ms = 1S

#define MAX_KEY_NUM 12
//按键映射表 最大16个按键   表位置0保留
//顺序按键 匹配的 实际按键值
//                             1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16
const u8  KeyMapTab[17] = {0, 11, 7,15, 3,13, 5, 8, 4,10, 6, 2, 0, 0, 0, 0, 0};
//const u8  KeyMapTab[17] = {0,  1, 2, 3, 4, 5, 6, 8, 8, 9,10,11,12,13,14, 0, 0};

u8 gPassword[4];
u8 gPswFlag=1;
u8 InputPassword[4];
u8 PswBit=0;
    
Queue_t  gQueue;
static u8 FirstPassWord = 0;   
u8 gWukeUpForPassWord = 0; 
u8 gWukeUpForKey = 0;    
KeySta_t gPrevoiusKeyValue = {0,KEY_NONE};

extern u8 gSleepFlag;
/*******************************************************************************
**函    数： Key_Init()
**功    能： 按键模块初始化
**参    数： void
**返    回： void
*******************************************************************************/
void Key_Init(void)
{
    WTC6216_Init();
    NewKey=NONE_KEY;
    OldKey=NONE_KEY;
    LstKey=NONE_KEY;
}
/*******************************************************************************
**函    数： Key_Scan()
**功    能： 状态机方式扫描摁键
**参    数： *keyFlag    按键标志    0-无按键  1-短按键    2-长按键 
**返    回： 摁键值
**说    明： 该函数 每隔 5~20ms 调用一次
*******************************************************************************/
u8 Key_Scan(u8 *keyFlag)
{
    u8 i;
    static u8 MidValidKey = NONE_KEY;                               //有效按键中间保存值

    *keyFlag = 0;
    NewKey = WTC6216_ReadKey();
    if(NewKey == OldKey)
    {
        if(NewKey==NONE_KEY)                                        //无按键 或 释放按键    
        {   
            if((LstKey != NONE_KEY)&&(LongKeyCount<LONG_KEY_TIMES)) // 释放按键 且 非长按键
            {
                //if(LongKeyCount>2)  //过滤超短按键 灵敏度可调
                {
                    LstKey  = NONE_KEY;
                    LongKeyCount = 0;
                    *keyFlag =1;
                    return MidValidKey;                             //返回短键释放
                }
            }
            else{                                                   
                LstKey = NONE_KEY;
                LongKeyCount = 0;
                return NONE_KEY;                    
            }
        }
        else if(NewKey==LstKey)                                     //连续按键
        {
            if(LongKeyCount++ == LONG_KEY_TIMES)
            {
                *keyFlag =2;
                return MidValidKey;                                 //返回长键确认
            }
        }
        else{                                                       //单键
            LstKey = NewKey;
            for(i=1 ; i<=MAX_KEY_NUM ; i++)
            {
                if( KeyMapTab[i] == NewKey )  
                {
                    MidValidKey = i;
                    break;
                }
            }
            if( i > MAX_KEY_NUM)              MidValidKey = NONE_KEY;
            return NONE_KEY ;         
        }
    }
    else OldKey = NewKey;               //抖动处理
    return NONE_KEY;
}
/*******************************************************************************
**函    数： Key_Scan()
**功    能： 状态机方式扫描摁键
**参    数： *keyFlag    按键标志    0-无按键  1-短按键    2-长按键 
**返    回： 摁键值
**说    明： 该函数 每隔 5~20ms 调用一次
*******************************************************************************/
u8 Key_FirstHandleScan(u8 *keyFlag)
{
    u8 i;
    static u8 MidValidKey = NONE_KEY;                               //有效按键中间保存值

    *keyFlag = 0;
    NewKey = WTC6216_ReadKey();
    if(NewKey == OldKey)
    {
        if(NewKey==NONE_KEY)                                        //无按键 或 释放按键    
        {   
            if((LstKey != NONE_KEY)&&(LongKeyCount<LONG_KEY_TIMES)) // 释放按键 且 非长按键
            {
                if(LongKeyCount>4)  //过滤超短按键 灵敏度可调
                {               
                    LstKey  = NONE_KEY;
                    LongKeyCount = 0;
                    *keyFlag = KEY_LONG;
                    return MidValidKey;                             //返回短键释放
                }
            }
            else{                                                   
                LstKey = NONE_KEY;
                LongKeyCount = 0;
                return NONE_KEY;                    
            }
        }
        else if(NewKey==LstKey)                                     //连续按键
        {
            LongKeyCount++ ;
        }
        else{                                                       //单键
            LstKey = NewKey;
            for(i=1 ; i<=MAX_KEY_NUM ; i++)
            {
                if( KeyMapTab[i] == NewKey )  
                {
                    MidValidKey = i;
                    break;
                }
            }
            if( i > MAX_KEY_NUM)              MidValidKey = NONE_KEY;
            return NONE_KEY ;         
        }
    }
    else OldKey = NewKey;               //抖动处理
    return NONE_KEY;
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
//    u8 i,ch,key_flag; 
//    ch = Key_Scan(&key_flag);
    
    u8 i,ch,key_flag;
    static u8 FirstFlag = 1;
    KeySta_t keyTemp;
    u8 res=0x00;
    CPU_SR_ALLOC();
//    TravelQueue(&gQueue);
    cpu_sr = CPU_SR_Save();
    res = Dequeue(&gQueue,&keyTemp);
    CPU_SR_Restore(cpu_sr);
    
    if(res)
    {
        if(1 == FirstFlag)   
        {
            FirstFlag = 0;
            StandbyCountReset();
        }
        else
        {
            ch = keyTemp.KeyValue;
            key_flag = keyTemp.KeyType;
            if(gPswFlag)        //密码开启
            {
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
                        if(ch <= 6)      //123456按键有效
                        {
                            StandbyCountReset();
                            if(gWukeUpForPassWord)
                            {
                                OLED_ShowString(1,0,"请重新输入密码",0);
                                OLED_ShowString(4,1,"密码错误",0);
                                OLED_ShowString(6,2,"----",0);
                                gWukeUpForPassWord = 0;
                                PswBit = 0;
                            }
                            OLED_ShowChar(6+PswBit,2,ch+'0',0);
                            InputPassword[PswBit] = ch;
                            if(PswBit++ >= 3)
                            {
                                PswBit=0;
                                if((gPassword[0] == InputPassword[0])&& \
                                   (gPassword[1] == InputPassword[1])&& \
                                   (gPassword[2] == InputPassword[2])&& \
                                   (gPassword[3] == InputPassword[3]) )
                                {
                                    gPswFlag = 0;
                                    gSleepFlag = 0;
                                    OLED_ClearScreen();
//                                    OLED_ShowString(4,1,"欢迎使用",0);
//                                    OLED_ShowHalfPicAt(4,PicTab[gPrevoiusKeyValue.KeyValue]);   
                                    if(gPrevoiusKeyValue.KeyValue<7)
                                    {
                                        AirCondition_FirstShowPicture();
                                    }
                                    else
                                    {
                                        OLED_ShowHalfPicAt(4,PicTab[gPrevoiusKeyValue.KeyValue]);
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
                        else 
                        {
                            OLED_ShowString(1,0,"请重新输入密码",0);
                            OLED_ShowString(4,1,"密码错误",0);
                            OLED_ShowString(6,2,"----",0);
                        }
                    }
                    break;
                    default:break;
                }
            }
            else if( ch != NONE_KEY)
            {
                /*清除上次状态*/
                for(i=0;i<MAX_TOUCH_KEY;i++)
                {
                    KeyCh[i].Status = KEY_NONE; 
                }    

                gSleepFlag = 0;               
                gPrevoiusKeyValue.KeyType = key_flag;
                gPrevoiusKeyValue.KeyValue= ch;
//                ShowLED_FlashLED(ch);           //普通背光效果
                StandbyCountReset();
                
                if(ch>6)    //普通按键 7-12
                {
                    if(gUIMode == UIMODE_AIRCON)    OLED_ClearScreen();     //从空调模式切换过来
                    gUIMode = UIMODE_NORMAL;              
                    
                    OLED_ShowHalfPicAt(4,PicTab[ch]);
                    if(1 == key_flag)   //短按
                    {
                        KeyCh[ch].Status = KEY_SHORT; 
                        PropEventFifo(1, ch, SRCEVENT , KEY_SHORT);     //50ms          
                        //Upload();                                       //2.5s//上报
                    }
                    else if(2 == key_flag)  //长按
                    {
                        KeyCh[ch].Status = KEY_LONG;    
                        PropEventFifo(1, ch, SRCEVENT , KEY_LONG);        
                        //Upload();                                       //上报
                    }
                }
                else if(ch == 2)   //空调电源按键
                {
                    if(gAirContionData.PowerSwitch == AC_POWER_ON)
                    {
                        gAirContionData.PowerSwitch = AC_POWER_OFF;
                        OLED_ClearScreen();
                        OLED_ShowHalfPicAt(4,PicTab[19]);           //空调关闭图片
                        AirCondition_SendData();
                    }
                    else 
                    {
                        gAirContionData.PowerSwitch = AC_POWER_ON;
                        AirCondition_FullShow();
                    } 
                    gUIMode = UIMODE_AIRCON;                    
                }
                else if(ch <= 6)   //空调面板所属按键 (除开电源按键)
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
                            AirCondition_KeyHander(ch); 
                        }
                    }
                    else if(gAirContionData.PowerSwitch == AC_POWER_OFF)  //电源关
                    {
//                        OLED_ClearScreen();
//                        OLED_ShowHalfPicAt(4,PicTab[19]);           //空调关闭图片
                        gAirContionData.PowerSwitch = AC_POWER_ON;
                        AirCondition_FullShow();
                    }
                    gUIMode = UIMODE_AIRCON;
                }
                Upload(); 
            }
        }
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
    delay_us(10);
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
**函    数:  Key_FirstHandle()
**功    能:  首次按键处理函数
**参    数:  按键值
**返    回:  void
********************************************************************************/
#if 0
void Key_FirstHandle(u8 Key)
{
    if(gPswFlag)
    {
        OLED_ShowString(2,0,"请输入密码",0);
        OLED_ShowString(6,2,"----",0);
    }
    else if( Key != NONE_KEY)
    {   
        ShowLED_FlashLED(Key);           //普通背光效果

        if(Key <= 6)   //空调面板所属按键
        {
            gUIMode = UIMODE_AIRCON;            
            AirCondition_FullShow();
        }
        else if(Key <= 12)
        {
            gUIMode = UIMODE_NORMAL;            
            KeyCh[Key].Status = KEY_SHORT;            
//            OLED_ShowHalfPicAt(4,PicTab[Key-6]);
						OLED_ShowHalfPicAt(4,PicTab[Key]);
            PropEventFifo(1, Key, SRCEVENT , KEY_SHORT);     //50ms          
        }
        Upload(); //上报      
    }
}
#endif

/*******************************************************************************
**函    数:  void Save_CurrentKeyState(void)
**功    能:  保存当前的按键状态
**参    数:  无
**返    回:  void
********************************************************************************/
void Save_CurrentKeyState(void)       
{
    WriteDataToEEPROM(ADDR_KEY_CURRENT_STATE,sizeof(KeySta_t),(u8*)&gPrevoiusKeyValue);
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
