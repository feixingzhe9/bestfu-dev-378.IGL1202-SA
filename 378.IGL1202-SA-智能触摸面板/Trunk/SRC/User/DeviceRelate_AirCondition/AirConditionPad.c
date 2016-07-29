/***************************Copyright BestFu ***********************************
**  ��    ����  AirContionPad.c
**  ��    �ܣ�  �豸����--�յ���岿��
**  ��    �룺  Keil uVision5 V5.10
**  ��    ����  V1.1
**  ��    д��  Seven
**  �������ڣ�  2014.08.26
**  �޸����ڣ�  2014.09.01
**  ˵    ����  >>
*******************************************************************************/
#include "EEPROM.h"
#include "MSGPackage.h"
#include "OLED.h"
#include "TouchPadPic.h"
#include "Thread.h"
#include "AirConditionPad.h"

#define DEVICE_RELATE_START_ADDR 		(0x3600)     //EEPROM�洢��ַ
#define DEVICE_AIRCONDITION_START_ADDR	(0x4000)     //EEPROM�յ��洢��ַ

AirConditionData_t gAirContionData;         //�յ��������
DeveceRelate_s     gDeveceRelate;           //�豸�����ṹ��
u8  gUIMode;    

void Show_Temper(void);
void AirCondition_SaveData(void);
void AirCondition_ReadSaveData(void);
void AirCondition_SendData(void);
//void AirCondition_CMDSend(ObjectType_t relateType);
static void AirCondition_CMDSend(void);
//const u8  AC_KeyMapTab[5] = {11,  9, 1,14, 6,10, 2,12, 4, 7,11, 3,13, 5, 0, 0, 0};

/*******************************************************************************
**��    ���� Init_AirCondition
**��    �ܣ� ��EEPROM��ȡ����ʼ���յ��������
**��    ���� void
**��    �أ� void
*******************************************************************************/
void Init_AirCondition(void)
{
    AirCondition_ReadSaveData();

    //�豸����������ݳ�ʼ��
    if(gDeveceRelate.UseFlag>0x02)         gDeveceRelate.UseFlag    = 0;           //ʹ�ñ�־-Ĭ�Ϲر�
    if(gDeveceRelate.RelateType>0x04)      gDeveceRelate.RelateType = 0;           //��������-Ĭ���޽ڵ�
    if(*(u32*)&gDeveceRelate.ObjectID > 0xfffffff0)  
                                   *(u32*)&gDeveceRelate.ObjectID   = 0;           //Ŀ���ַ
    if(*(u16*)&gDeveceRelate.AssistID > 0xfff0)      
                                   *(u16*)&gDeveceRelate.AssistID   = 0;           //������ַ
    if(gDeveceRelate.UnitNum>0x80)         gDeveceRelate.UnitNum    = 0;           //��Ԫ��
    
    //�յ������ݳ�ʼ��
	if(gAirContionData.PowerSwitch>0x01)   gAirContionData.PowerSwitch  = 0;       //��Դ����
/*	if((gAirContionData.SetTemper>30)||(gAirContionData.SetTemper<16))
                                           gAirContionData.SetTemper    = 26;  */    //�����¶�
	if((gAirContionData.SetTemper > gAirContionData.SetMaxTemp) || (gAirContionData.SetTemper < gAirContionData.SetMinTemp))
                                           gAirContionData.SetTemper    = 25;      //�����¶�
	if(gAirContionData.SetMode>0x04)       gAirContionData.SetMode      = 0;       //����ģʽ		
	if(gAirContionData.SetWindSpeed>0x04)  gAirContionData.SetWindSpeed = 0;	   //���÷���
	if(gAirContionData.SetWindSweep>0x03)  gAirContionData.SetWindSweep = 0;	   //ɨ�翪��

}

/*******************************************************************************
**��    ���� AirCondition_SaveRelateData
**��    �ܣ� �յ���屣������������ݵ�EEPROM
**��    ���� void
**��    �أ� void
*******************************************************************************/
void AirCondition_SaveRelateData(void)
{
	WriteDataToEEPROM(DEVICE_RELATE_START_ADDR,sizeof(DeveceRelate_s), (u8 *)&gDeveceRelate);
}

/*******************************************************************************
**��    ���� AirCondition_SaveData
**��    �ܣ� �յ���屣�����ݵ�EEPROM
**��    ���� void
**��    �أ� void
*******************************************************************************/
void AirCondition_SaveData(void)
{
//    WriteDataToEEPROM(DEVICE_RELATE_START_ADDR,sizeof(DeveceRelate_s), (u8 *)&gDeveceRelate);
    WriteDataToEEPROM(DEVICE_AIRCONDITION_START_ADDR,sizeof(gAirContionData), (u8 *)&gAirContionData);
}

/*******************************************************************************
**��    ���� AirCondition_ReadSaveData
**��    �ܣ� ��EEPROM��ȡ��������
**��    ���� void
**��    �أ� void
*******************************************************************************/
void AirCondition_ReadSaveData(void)
{
    ReadDataFromEEPROM(DEVICE_RELATE_START_ADDR,sizeof(DeveceRelate_s),(u8 *)&gDeveceRelate);
//    ReadDataFromEEPROM(DEVICE_RELATE_START_ADDR+sizeof(DeveceRelate_s),sizeof(gAirContionData), (u8 *)&gAirContionData); 
	ReadDataFromEEPROM(DEVICE_AIRCONDITION_START_ADDR,sizeof(gAirContionData), (u8 *)&gAirContionData);
	
    if(gDeveceRelate.UseFlag > 0x02)    gDeveceRelate.UseFlag       = 1 ; //Ĭ��ʹ��
    if(gDeveceRelate.RelateType > 0x04) gDeveceRelate.RelateType    = 0 ; //δ����
    if(gDeveceRelate.UnitNum > 0xF0)    gDeveceRelate.UnitNum    	= 1 ; //��Ԫ��1
    
    if(gAirContionData.PowerSwitch > AC_POWER_ON)     gAirContionData.PowerSwitch = AC_POWER_OFF;
    if(gAirContionData.SetMode > AC_MODE_MAX)         gAirContionData.SetMode     = AC_MODE_AUTO;
    if((gAirContionData.SetTemper > AC_MAX_TEMPER)|| \
        (gAirContionData.SetTemper < AC_MIN_TEMPER))  gAirContionData.SetTemper   = AC_MAX_TEMPER;
    if(gAirContionData.SetWindSpeed > AC_WIND_MAX)    gAirContionData.SetWindSpeed= AC_WIND_AUTO;
	if((gAirContionData.SetMaxTemp > MAX_TEMP)||(gAirContionData.SetMaxTemp < MIN_TEMP))			
		gAirContionData.SetMaxTemp  = 32;
	if((gAirContionData.SetMinTemp < MIN_TEMP) || (gAirContionData.SetMinTemp >= gAirContionData.SetMaxTemp))	
		gAirContionData.SetMinTemp  = 16;
}

/*******************************************************************************
**��    ���� AirCondition_KeyHander
**��    �ܣ� �յ���尴�����ݴ���
**��    ���� keyNum        --������
**��    �أ� void
*******************************************************************************/
void  AirCondition_KeyHander(u8 keyNum)
{
    switch(keyNum)
    {         
        case 1:                     // - �¶� -
            if(gAirContionData.SetMode != AC_MODE_AUTO)     //�Զ�ģʽ�����������¶�
            {
               // if(gAirContionData.SetTemper > AC_MIN_TEMPER)  gAirContionData.SetTemper--;
							if(gAirContionData.SetTemper > gAirContionData.SetMinTemp)
								gAirContionData.SetTemper--;
            }
            Show_Temper();
            break;  
/*			
        case 2:                     //��Դ����
            if(gAirContionData.PowerSwitch == AC_POWER_ON)
            {
                gAirContionData.PowerSwitch = AC_POWER_OFF;
                OLED_ClearScreen();
                //OLED_ShowHalfPicAt(4,PicTab[9]);           //�յ��ر�ͼƬ
				OLED_ShowHalfPicAt(4,PicTab[46]);
            }
            else 
            {
                gAirContionData.PowerSwitch = AC_POWER_ON;
                OLED_ClearScreen();
                //OLED_ShowHalfPicAt(4,PicTab[10]);           //�յ���ͼƬ
				OLED_ShowHalfPicAt(4,PicTab[46]); 
            }
            break;
*/
        case 3:                     //+ �¶� +
            if(gAirContionData.SetMode != AC_MODE_AUTO)     //�Զ�ģʽ�����������¶�
            {
               // if(gAirContionData.SetTemper < AC_MAX_TEMPER) gAirContionData.SetTemper++;
				if(gAirContionData.SetTemper < gAirContionData.SetMaxTemp)
				gAirContionData.SetTemper++;							
            }
            Show_Temper();
            break;
        case 4:                     // ģʽ
            if(gAirContionData.SetMode < AC_MODE_MAX)    gAirContionData.SetMode ++;
            else gAirContionData.SetMode = AC_MODE_AUTO;
        
            OLED_ShowQuartPicAt(0,0,PicTab_AC[gAirContionData.SetMode]);    //��ʾ �յ�ģʽͼƬ
            break;
        case 5:                     // ����
            if(gAirContionData.SetWindSpeed < AC_WIND_MAX)
                gAirContionData.SetWindSpeed ++;
            else gAirContionData.SetWindSpeed = AC_WIND_AUTO;
            
            OLED_ShowQuartPicAt(0,4,PicTab_AC[gAirContionData.SetWindSpeed+5]);     //��ʾ ����ģʽͼƬ
            break;  
        case 6:                     //ɨ�翪��
					/*
            if ( gAirContionData.SetWindSweep < AC_WINDSWEEP_All )
            {
                gAirContionData.SetWindSweep++;
            }
            else gAirContionData.SetWindSweep = AC_WINDSWEEP_OFF;
				*/
            // Һ������ʾ������
			if ( gAirContionData.SetWindSweep < AC_WINDSWEEP_All )
            {
                gAirContionData.SetWindSweep++;
            }
            else gAirContionData.SetWindSweep = AC_WINDSWEEP_OFF;
            // Һ������ʾ
            if(gAirContionData.SetWindSweep == AC_WINDSWEEP_OFF)
            {
                OLED_ShowString(9,3,"     ",0);
            }
            else if(gAirContionData.SetWindSweep == AC_WINDSWEEP_OpDown) //����ɨ��
            {
                OLED_ShowPicAt( 9, 6, 23, 16, &PicTab_Wind[0]);
                OLED_ShowString(11,3,"  ",0);
            }
            else if(gAirContionData.SetWindSweep == AC_WINDSWEEP_LeftWright) //����ɨ
            {
                OLED_ShowString(9, 3,"  ",0);                
                OLED_ShowPicAt(11, 6, 23, 16, &PicTab_Wind[1*46]);
            }
            else if(gAirContionData.SetWindSweep == AC_WINDSWEEP_All)
            {
                OLED_ShowPicAt( 9, 6, 23, 16, &PicTab_Wind[0*46]);
                OLED_ShowPicAt(11, 6, 23, 16, &PicTab_Wind[1*46]);
            }
            break;
        default : 
            return;//break;        
    } 
    //��������
    AirCondition_SendData();
}

/*******************************************************************************
**��    ���� AirCondition_SendData
**��    �ܣ� ���Ϳյ�����豸��������
**��    ���� void
**��    �أ� void
*******************************************************************************/
void  AirCondition_SendData(void)
{
    if(1 == gDeveceRelate.UseFlag)                      //ʹ��״̬
    {
       /* AirCondition_CMDSend((ObjectType_t)gDeveceRelate.RelateType);  */
        Thread_Login(ONCEDELAY, 0, 600 ,&AirCondition_CMDSend);
    }
}
#if 0
/*******************************************************************************
**��    ���� AirCondition_CMDSend
**��    �ܣ� �յ�����Զ��������
**��    ���� relateType            --��������
**��    �أ� void
*******************************************************************************/
void AirCondition_CMDSend(ObjectType_t relateType)
{
    u8 pData[10];   
    
    pData[0] = gAirContionData.PowerSwitch; 
    pData[1] = gAirContionData.SetTemper;
    pData[2] = gAirContionData.SetMode;
    pData[3] = gAirContionData.SetWindSpeed;
    pData[4] = gAirContionData.SetWindSweep;  
    //              Ŀ¼����/��/����          Ŀ��ID       �顢������   ��Ԫ��   ���Ժ�   ���ݳ���   ����  
    //void Msg_Send(ObjectType_t objecttype, u32 objectID, u16 actNum, u8 unit, u8 cmd, u8 len, u8 *data);
    
    Msg_Send(relateType, *(u32*)&gDeveceRelate.ObjectID,*(u16*)&gDeveceRelate.AssistID, \
             gDeveceRelate.UnitNum , 0x50 , 5 , pData);
}
#endif

/*******************************************************************************
**��    ���� AirCondition_CMDSend
**��    �ܣ� �յ�����Զ��������
**��    ���� relateType            --��������
**��    �أ� void
*******************************************************************************/
static void AirCondition_CMDSend(void)
{
    u8 pData[10];   
    
    pData[0] = gAirContionData.PowerSwitch; 
    pData[1] = gAirContionData.SetTemper;
    pData[2] = gAirContionData.SetMode;
    pData[3] = gAirContionData.SetWindSpeed;
    pData[4] = gAirContionData.SetWindSweep;
    
    //              Ŀ¼����/��/����          Ŀ��ID       �顢������   ��Ԫ��   ���Ժ�   ���ݳ���   ����  
    //void Msg_Send(ObjectType_t objecttype, u32 objectID, u16 actNum, u8 unit, u8 cmd, u8 len, u8 *data);
    
    Msg_Send((ObjectType_t)gDeveceRelate.RelateType, *(u32*)&gDeveceRelate.ObjectID,*(u16*)&gDeveceRelate.AssistID, \
             gDeveceRelate.UnitNum , 0x50 , 5 , pData);
}

//��ʾ�¶�
void Show_Temper(void)
{
    u8 a[3];
    a[0] = gAirContionData.SetTemper/10;
    a[1] = gAirContionData.SetTemper%10;
    OLED_ShowNum18x32(66,2,PicTab_NUM[a[0]]);
    OLED_ShowNum18x32(66+18,2,PicTab_NUM[a[1]]);
}
/*******************************************************************************
**��    ���� AirCondition_FirstCMD
**��    �ܣ� ��ʾ�������ͨ�����л�����
**��    ���� relateType            --��������
**��    �أ� void
*******************************************************************************/
void AirCondition_FullShow(void)
{
	if(gAirContionData.PowerSwitch == AC_POWER_ON)  //��״̬
	{
		OLED_ClearScreen();
		OLED_ShowQuartPicAt(0,0,PicTab_AC[gAirContionData.SetMode]);        //��ʾ �յ�ģʽͼƬ
		OLED_ShowQuartPicAt(0,4,PicTab_AC[gAirContionData.SetWindSpeed+5]); //��ʾ ����ģʽͼƬ
		
		OLED_DrawLine(64);  //������
		OLED_Show32x323PicAt(12,2,PicTab_32x32[0]);//���϶�
		Show_Temper();//��ʾ�¶�
					//ɨ��
		if(gAirContionData.SetWindSweep == AC_WINDSWEEP_OFF)
		{
			OLED_ShowString(9,3,"     ",0);
		}
		else if(gAirContionData.SetWindSweep == AC_WINDSWEEP_OpDown) //����ɨ��
		{
			OLED_ShowPicAt( 9, 6, 23, 16, &PicTab_Wind[0]);
			OLED_ShowString(11,3,"  ",0);
		}
		else if(gAirContionData.SetWindSweep == AC_WINDSWEEP_LeftWright) //����ɨ
		{
			OLED_ShowString(9, 3,"  ",0);                
			OLED_ShowPicAt(11, 6, 23, 16, &PicTab_Wind[1*46]);
		}
		else if(gAirContionData.SetWindSweep == AC_WINDSWEEP_All)
		{
			OLED_ShowPicAt( 9, 6, 23, 16, &PicTab_Wind[0*46]);
			OLED_ShowPicAt(11, 6, 23, 16, &PicTab_Wind[1*46]);
		}
	}
	else OLED_ShowHalfPicAt(4,PicTab[46]);           //�յ��ر�ͼƬ
	
	
	//��������
	AirCondition_SendData();
    
}

/*******************************************************************************
**��    ���� void AirCondition_FirstShowPicture(void)
**��    �ܣ� �յ���ʾ�ϴ�����ǰ��״̬
**��    ���� ��
**��    �أ� void
*******************************************************************************/
void AirCondition_FirstShowPicture(void)
{
    if(gAirContionData.PowerSwitch == AC_POWER_ON)  //��״̬
    {
        OLED_DrawLine(64);  //������
        OLED_Show32x323PicAt(12,2,PicTab_32x32[0]);//���϶�
        Show_Temper();
        OLED_ShowQuartPicAt(0,0,PicTab_AC[gAirContionData.SetMode]);    //��ʾ �յ�ģʽͼƬ
        OLED_ShowQuartPicAt(0,4,PicTab_AC[gAirContionData.SetWindSpeed+5]);     //��ʾ ����ģʽͼƬ
        switch(gAirContionData.SetWindSweep)
        {
            case AC_WINDSWEEP_OFF:
            {
                OLED_ShowString(9,3,"     ",0);
            }break;
            case AC_WINDSWEEP_OpDown:
            {
                OLED_ShowPicAt( 9, 6, 23, 16, &PicTab_Wind[0]);
                OLED_ShowString(11,3,"  ",0);
            }break;
            case AC_WINDSWEEP_LeftWright:
            {
                OLED_ShowString(9, 3,"  ",0);                
                OLED_ShowPicAt(11, 6, 23, 16, &PicTab_Wind[1*46]);
            }break;
            case AC_WINDSWEEP_All:
            {
                OLED_ShowPicAt( 9, 6, 23, 16, &PicTab_Wind[0*46]);
                OLED_ShowPicAt(11, 6, 23, 16, &PicTab_Wind[1*46]);
            }break;
            default:break;
        }
    }
    else
    {        
        OLED_ShowHalfPicAt(4,PicTab[46]);           //�յ��ر�ͼƬ 
    }
}

/***************************Copyright BestFu **********************************/
