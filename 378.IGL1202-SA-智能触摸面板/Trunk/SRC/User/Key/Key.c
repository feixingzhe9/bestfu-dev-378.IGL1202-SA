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
#include "Queue.h"
#include "sys.h"
#include "TIMx.h"
#include "EXTIx.h"
#include "Program.h"
#include "AirConditionPad.h"
#include "wdg.h"
#include "usart.h"
	
//Public Variable definitions
KeyProp_t  gKeyCh[MAX_TOUCH_KEY];        	//���尴���Ķ�����Դ <gKeyCh[0]����>
KeyStatus_t gKeyStatus[MAX_TOUCH_KEY];		//���尴����״̬��Դ <gKeyStatus[0]����>
KeySta_t gKeyPrevState = {0,KEY_NONE};		//���尴�����߱����ֵ
u8 gPassword[4] = {0};						//������ʵ������洢����
u8 gPswFlag = 1u; 							//���ڼ�¼��������������ֵ
volatile KeyCurStatus_t keyTemp;			//��¼����״̬

//Private Variable definition
//��������ֵ--�ṩ�����Բ�ʹ��    1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16
static const u8  KeyMapTab[17] = {0, 11, 7,15, 3,13, 5, 8, 4,10, 6, 2, 0, 0, 0, 0, 0};
static u8 InputPassword[4] = {0};			//�����û����������ֵ��������
static Queue_t  gQueue;						//���尴�����ն���
static u8 PswBit = 0; 						//��¼�����������λ��    
static u8 first = 1u;

//Private function declaration
static void CheckUserPassWordInput(u8 keynum);
static void SaveKeyValueToQueue(volatile KeyCurStatus_t* pKey_temp);

/*******************************************************************************
**��    ���� Key_Init()
**��    �ܣ� ����ģ���ʼ��
**��    ���� void
**��    �أ� void
*******************************************************************************/
void Key_Init(void)
{
    WTC6216_Init();
}

/*******************************************************************************
**��    ���� void KeyScanf_Init(void)
**��    �ܣ� ����ɨ��ģ���ʼ��
**��    ���� void
**��    �أ� void
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
**��    ���� void KeyEXTIHandle(void)
**��    �ܣ� �����жϴ���
**��    ���� void
**��    �أ� void
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
**��    ���� void KeyTIMHandle(void)
**��    �ܣ� ������ʱ������
**��    ���� void
**��    �أ� void
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
**��    ���� void SaveKeyValueToQueue(KeyCurStatus_t Key_temp)
**��    �ܣ� ���水��ֵ
**��    ���� Key_temp 
**��    �أ� ��
**˵    ���� 
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
**��    ���� Key_Handle()
**��    �ܣ� ��������
**��    ���� void 
**��    �أ� void
**˵    ���� �ú��� ÿ�� 5~20ms ����һ��
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
		if(gPswFlag > 0)        //���뿪��
		{
			CheckUserPassWordInput(keyTemp.KeyValue);
		}
		else if( keyTemp.KeyValue != NONE_KEY)
		{
			/*����ϴ�״̬*/
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
					OLED_ClearScreen();     //�ӿյ�ģʽ�л�����
				}
                gUIMode = UIMODE_NORMAL; 
				switch(keyTemp.KeyType)
				{
					case KEY_SHORT:
					{
						if(SW_OFF == gKeyStatus[keyTemp.KeyValue].Status)   //״̬��Դ��
						{
							gKeyStatus[keyTemp.KeyValue].Status = SW_ON;
							OLED_ShowHalfPicAt(4,PicTab[keyTemp.KeyValue]);
							PropEventFifo(1, (keyTemp.KeyValue + 0x10), SRCEVENT , SW_ON);
						}
						else								//״̬��Դ��
						{
							gKeyStatus[keyTemp.KeyValue].Status = SW_OFF;
							OLED_ShowHalfPicAt(4,PicTab[keyTemp.KeyValue + 32]);
							PropEventFifo(1, (keyTemp.KeyValue + 0x10), SRCEVENT , SW_OFF);
						}	
						gKeyPrevState.KeyStatusVal[keyTemp.KeyValue] = gKeyStatus[keyTemp.KeyValue].Status;
						gKeyCh[keyTemp.KeyValue].Status = KEY_SHORT; 
						PropEventFifo(1, keyTemp.KeyValue, SRCEVENT , KEY_SHORT);     //50ms 
						Upload(1);                                       //2.5s//�ϱ�
					}
					break;
					case KEY_LONG:
					{
						OLED_ShowHalfPicAt(4,PicTab[keyTemp.KeyValue + 16]);
						gKeyCh[keyTemp.KeyValue].Status = KEY_LONG;    
						PropEventFifo(1, keyTemp.KeyValue, SRCEVENT , KEY_LONG);        
						Upload(1);                                       //�ϱ�
					}
					break;
					default:break;
				} 
			}	
			else if(2 == keyTemp.KeyValue)//�յ���Դ����
			{
				if(gAirContionData.PowerSwitch == AC_POWER_ON)
				{
					gAirContionData.PowerSwitch = AC_POWER_OFF;
					OLED_ClearScreen();
					OLED_ShowHalfPicAt(4,PicTab[46]);           //�յ��ر�ͼƬ
					AirCondition_SendData();
				}
				else 
				{
					gAirContionData.PowerSwitch = AC_POWER_ON;
					AirCondition_FullShow();
				} 
				gUIMode = UIMODE_AIRCON;                    
            }
			else if(keyTemp.KeyValue <= 6)   //�յ������������ (������Դ����)
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
						AirCondition_KeyHander(keyTemp.KeyValue); 
					}
				}
				else if(gAirContionData.PowerSwitch == AC_POWER_OFF)  //��Դ��
				{
//					OLED_ClearScreen();
//					OLED_ShowHalfPicAt(4,PicTab[19]);           //�յ��ر�ͼƬ
					gAirContionData.PowerSwitch = AC_POWER_ON;
					AirCondition_FullShow();
				}
				gUIMode = UIMODE_AIRCON;
			}
			
		}
    }
}

/*******************************************************************************
**��    ���� static void CheckUserPassWordInput(u8 keynum)
**��    �ܣ� �û����봦����
**��    ���� keynum���û�����İ���ֵ 
**��    �أ� void
**˵    ���� �ú��� ÿ�� 5~20ms ����һ��
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
			OLED_ShowString(2,0,"����������",0);
			OLED_ShowString(6,2,"----",0);
		}
		break;
		case 1:
		{
			if(keynum <= 6)      //123456������Ч
			{
				StandbyCountReset();
				if(gWukeUpForPassWord > 0u)
				{
					OLED_ShowString(1,0,"��������������",0);
					OLED_ShowString(4,1,"�������",0);
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
//                      OLED_ShowString(4,1,"��ӭʹ��",0);
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
						OLED_ShowString(1,0,"��������������",0);
						OLED_ShowString(4,1,"�������",0);
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
**��    ��:  void Save_CurrentKeyState(void)
**��    ��:  ���浱ǰ�İ���״̬
**��    ��:  ��
**��    ��:  void
********************************************************************************/
void Save_CurrentKeyState(void)       
{
    WriteDataToEEPROM(ADDR_KEY_CURRENT_STATE,sizeof(KeySta_t),(u8*)&gKeyPrevState);
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
