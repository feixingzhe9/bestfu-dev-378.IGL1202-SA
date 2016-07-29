/***************************Copyright BestFu ***********************************
**  ��    ����  Key.c
**  ��    �ܣ�  <<�߼���>> �������� 
**  ��    �룺  Keil uVision5 V5.10
**  ��    ����  V1.1
**  ��    д��  Seven
**  �������ڣ�  2014.08.20
**  �޸����ڣ�  2014.08.20
**  ˵    ����  
**  V1.1
    >> �̰���Ϊ�ͷ�ʱ��ִ�У�����ӳ�������Ӧ
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


/*��������ֵ*/
const u8 NONE_KEY=0xFF;       

/*��������ֵ--�ṩ�����Բ�ʹ��*/
KeyProp_t  KeyCh[MAX_TOUCH_KEY];        //KeyCh[0] ����

static u8  NewKey,OldKey,LstKey;
static u16 LongKeyCount;                //������������
const  u16 LONG_KEY_TIMES = 250;        //����������ʱ�� 250 *4ms = 1S

#define MAX_KEY_NUM 12
//����ӳ��� ���16������   ��λ��0����
//˳�򰴼� ƥ��� ʵ�ʰ���ֵ
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
**��    ���� Key_Init()
**��    �ܣ� ����ģ���ʼ��
**��    ���� void
**��    �أ� void
*******************************************************************************/
void Key_Init(void)
{
    WTC6216_Init();
    NewKey=NONE_KEY;
    OldKey=NONE_KEY;
    LstKey=NONE_KEY;
}
/*******************************************************************************
**��    ���� Key_Scan()
**��    �ܣ� ״̬����ʽɨ������
**��    ���� *keyFlag    ������־    0-�ް���  1-�̰���    2-������ 
**��    �أ� ����ֵ
**˵    ���� �ú��� ÿ�� 5~20ms ����һ��
*******************************************************************************/
u8 Key_Scan(u8 *keyFlag)
{
    u8 i;
    static u8 MidValidKey = NONE_KEY;                               //��Ч�����м䱣��ֵ

    *keyFlag = 0;
    NewKey = WTC6216_ReadKey();
    if(NewKey == OldKey)
    {
        if(NewKey==NONE_KEY)                                        //�ް��� �� �ͷŰ���    
        {   
            if((LstKey != NONE_KEY)&&(LongKeyCount<LONG_KEY_TIMES)) // �ͷŰ��� �� �ǳ�����
            {
                //if(LongKeyCount>2)  //���˳��̰��� �����ȿɵ�
                {
                    LstKey  = NONE_KEY;
                    LongKeyCount = 0;
                    *keyFlag =1;
                    return MidValidKey;                             //���ض̼��ͷ�
                }
            }
            else{                                                   
                LstKey = NONE_KEY;
                LongKeyCount = 0;
                return NONE_KEY;                    
            }
        }
        else if(NewKey==LstKey)                                     //��������
        {
            if(LongKeyCount++ == LONG_KEY_TIMES)
            {
                *keyFlag =2;
                return MidValidKey;                                 //���س���ȷ��
            }
        }
        else{                                                       //����
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
    else OldKey = NewKey;               //��������
    return NONE_KEY;
}
/*******************************************************************************
**��    ���� Key_Scan()
**��    �ܣ� ״̬����ʽɨ������
**��    ���� *keyFlag    ������־    0-�ް���  1-�̰���    2-������ 
**��    �أ� ����ֵ
**˵    ���� �ú��� ÿ�� 5~20ms ����һ��
*******************************************************************************/
u8 Key_FirstHandleScan(u8 *keyFlag)
{
    u8 i;
    static u8 MidValidKey = NONE_KEY;                               //��Ч�����м䱣��ֵ

    *keyFlag = 0;
    NewKey = WTC6216_ReadKey();
    if(NewKey == OldKey)
    {
        if(NewKey==NONE_KEY)                                        //�ް��� �� �ͷŰ���    
        {   
            if((LstKey != NONE_KEY)&&(LongKeyCount<LONG_KEY_TIMES)) // �ͷŰ��� �� �ǳ�����
            {
                if(LongKeyCount>4)  //���˳��̰��� �����ȿɵ�
                {               
                    LstKey  = NONE_KEY;
                    LongKeyCount = 0;
                    *keyFlag = KEY_LONG;
                    return MidValidKey;                             //���ض̼��ͷ�
                }
            }
            else{                                                   
                LstKey = NONE_KEY;
                LongKeyCount = 0;
                return NONE_KEY;                    
            }
        }
        else if(NewKey==LstKey)                                     //��������
        {
            LongKeyCount++ ;
        }
        else{                                                       //����
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
    else OldKey = NewKey;               //��������
    return NONE_KEY;
}
/*******************************************************************************
**��    ���� Key_Handle()
**��    �ܣ� ��������
**��    ���� void 
**��    �أ� void
**˵    ���� �ú��� ÿ�� 5~20ms ����һ��
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
            if(gPswFlag)        //���뿪��
            {
                switch(FirstPassWord)
                {
                    case 0:
                    {
                        FirstPassWord = 1;
                        OLED_ClearScreen();
                        OLED_ShowString(2,0,"����������",0);
                        OLED_ShowString(6,2,"----",0);
                    }
                    break;
                    case 1:
                    {
                        if(ch <= 6)      //123456������Ч
                        {
                            StandbyCountReset();
                            if(gWukeUpForPassWord)
                            {
                                OLED_ShowString(1,0,"��������������",0);
                                OLED_ShowString(4,1,"�������",0);
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
//                                    OLED_ShowString(4,1,"��ӭʹ��",0);
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
                                    OLED_ShowString(1,0,"��������������",0);
                                    OLED_ShowString(4,1,"�������",0);
                                    OLED_ShowString(6,2,"----",0);
                                }
                            }
                        }
                        else 
                        {
                            OLED_ShowString(1,0,"��������������",0);
                            OLED_ShowString(4,1,"�������",0);
                            OLED_ShowString(6,2,"----",0);
                        }
                    }
                    break;
                    default:break;
                }
            }
            else if( ch != NONE_KEY)
            {
                /*����ϴ�״̬*/
                for(i=0;i<MAX_TOUCH_KEY;i++)
                {
                    KeyCh[i].Status = KEY_NONE; 
                }    

                gSleepFlag = 0;               
                gPrevoiusKeyValue.KeyType = key_flag;
                gPrevoiusKeyValue.KeyValue= ch;
//                ShowLED_FlashLED(ch);           //��ͨ����Ч��
                StandbyCountReset();
                
                if(ch>6)    //��ͨ���� 7-12
                {
                    if(gUIMode == UIMODE_AIRCON)    OLED_ClearScreen();     //�ӿյ�ģʽ�л�����
                    gUIMode = UIMODE_NORMAL;              
                    
                    OLED_ShowHalfPicAt(4,PicTab[ch]);
                    if(1 == key_flag)   //�̰�
                    {
                        KeyCh[ch].Status = KEY_SHORT; 
                        PropEventFifo(1, ch, SRCEVENT , KEY_SHORT);     //50ms          
                        //Upload();                                       //2.5s//�ϱ�
                    }
                    else if(2 == key_flag)  //����
                    {
                        KeyCh[ch].Status = KEY_LONG;    
                        PropEventFifo(1, ch, SRCEVENT , KEY_LONG);        
                        //Upload();                                       //�ϱ�
                    }
                }
                else if(ch == 2)   //�յ���Դ����
                {
                    if(gAirContionData.PowerSwitch == AC_POWER_ON)
                    {
                        gAirContionData.PowerSwitch = AC_POWER_OFF;
                        OLED_ClearScreen();
                        OLED_ShowHalfPicAt(4,PicTab[19]);           //�յ��ر�ͼƬ
                        AirCondition_SendData();
                    }
                    else 
                    {
                        gAirContionData.PowerSwitch = AC_POWER_ON;
                        AirCondition_FullShow();
                    } 
                    gUIMode = UIMODE_AIRCON;                    
                }
                else if(ch <= 6)   //�յ������������ (������Դ����)
                {
                    if(gAirContionData.PowerSwitch == AC_POWER_ON)
                    {
                        if(gUIMode == UIMODE_NORMAL)
                        {
                            AirCondition_FullShow();
                        }
                        else    //���ÿյ�Ч��
                        {
                            if(gWukeUpForKey)
                            {
                                AirCondition_FirstShowPicture();
                                gWukeUpForKey = 0;
                            }
                            AirCondition_KeyHander(ch); 
                        }
                    }
                    else if(gAirContionData.PowerSwitch == AC_POWER_OFF)  //��Դ��
                    {
//                        OLED_ClearScreen();
//                        OLED_ShowHalfPicAt(4,PicTab[19]);           //�յ��ر�ͼƬ
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
**��    ��:  Key_FirstScan()
**��    ��:  ��������ɨ��
**��    ��:  void
**��    ��:  �״���Ч����ֵ 
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
**��    ��:  Key_FirstHandle()
**��    ��:  �״ΰ���������
**��    ��:  ����ֵ
**��    ��:  void
********************************************************************************/
#if 0
void Key_FirstHandle(u8 Key)
{
    if(gPswFlag)
    {
        OLED_ShowString(2,0,"����������",0);
        OLED_ShowString(6,2,"----",0);
    }
    else if( Key != NONE_KEY)
    {   
        ShowLED_FlashLED(Key);           //��ͨ����Ч��

        if(Key <= 6)   //�յ������������
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
        Upload(); //�ϱ�      
    }
}
#endif

/*******************************************************************************
**��    ��:  void Save_CurrentKeyState(void)
**��    ��:  ���浱ǰ�İ���״̬
**��    ��:  ��
**��    ��:  void
********************************************************************************/
void Save_CurrentKeyState(void)       
{
    WriteDataToEEPROM(ADDR_KEY_CURRENT_STATE,sizeof(KeySta_t),(u8*)&gPrevoiusKeyValue);
}

/*******************************************************************************
**��    ��:  static void Get_CurrentKeyState(u8 *)
**��    ��:  ��ȡ�浱ǰ�İ���״̬
**��    ��:  ��
**��    ��:  void
********************************************************************************/
void Get_CurrentKeyState(KeySta_t * pKeyState)
{
    ReadDataFromEEPROM(ADDR_KEY_CURRENT_STATE,sizeof(KeySta_t),(u8*)pKeyState);
}

/***************************Copyright BestFu **********************************/
