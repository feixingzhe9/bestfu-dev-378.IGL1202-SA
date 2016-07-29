/***************************Copyright BestFu ***********************************
**  ��    ����  AirContionPad.h
**  ��    �ܣ�  �豸����--�յ���岿��
**  ��    �룺  Keil uVision5 V5.10
**  ��    ����  V1.1
**  ��    д��  Seven
**  �������ڣ�  2014.08.26
**  �޸����ڣ�  2014.09.01
**  ˵    ����  >>
*******************************************************************************/
#ifndef _AIR_CONDITION_PAD_H
#define _AIR_CONDITION_PAD_H

#include "BF_type.h"

#define AC_POWER_OFF     0      //��Դ��
#define AC_POWER_ON      1      //��Դ��

#define AC_MIN_TEMPER   16      //�¶���Сֵ
#define AC_MAX_TEMPER   30      //�¶����ֵ

#define MAX_TEMP 			(35)
#define MIN_TEMP 			(10)

#define AC_MODE_MAX         4   //ģʽ���ֵ
    #define AC_MODE_AUTO    0   //�Զ�        �Զ�
    #define AC_MODE_COOL    1   //����
    #define AC_MODE_WATER   2   //��ʪ
    #define AC_MODE_WIND    3   //�ͷ�
    #define AC_MODE_HOT     4   //����

#define AC_WIND_MAX         3   //��ģʽ���ֵ
    #define AC_WIND_AUTO    0   //�Զ�����
    #define AC_WIND_LOW     1   //�ͷ���
    #define AC_WIND_MID     2   //�з���
    #define AC_WIND_HIGH    3   //�߷���
    //#define AC_WIND_CLOSE   4   //�ر�    ...ң��������Ч

#define AC_WINDSWEEP_OFF        0   //ɨ���
#define AC_WINDSWEEP_OpDown     1   //ɨ�翪  ���°ڷ�
#define AC_WINDSWEEP_LeftWright 2   //ɨ�翪  ���Ұڷ�
#define AC_WINDSWEEP_All        3   //ɨ�翪  ���� + ���Ұڷ�

#pragma pack(1)
//�յ������ݽṹ��
typedef struct
{
    u8      PowerSwitch;    //��Դ����
    u8      SetTemper;      //�����¶�
    u8      SetMode;        //����ģʽ      
    u8      SetWindSpeed;   //���÷���
    u8      SetWindSweep;   //ɨ�翪��
	u8		SetMaxTemp;			//�����¶�
	u8		SetMinTemp;			//�����¶�
} AirConditionData_t;

//�豸�����ṹ��
typedef  struct 
{
    u8 UseFlag;         //ʹ�ñ�־  0x00.δʹ��   0x01.ʹ��          0x02.����              
    u8 RelateType;      //��������  0x00.δ����   0x01.�豸���ڵ�    0x02�鲥         0x04.�豸���͹㲥                      
    u8 ObjectID[4];     //�����ַ                |--�豸�ڵ�ID      |-- �������ַ   |-- �豸���͹㲥��ַ
    u8 AssistID[2];     //������ַ                |--�豸����        |-- ���         |-- �豸����
    u8 UnitNum;			//��Ԫ��
}DeveceRelate_s;
#pragma pack()

extern AirConditionData_t gAirContionData;
extern DeveceRelate_s     gDeveceRelate;
extern u8  gUIMode;  

#define UIMODE_NORMAL     0     //��ͨģʽ
#define UIMODE_AIRCON     1     //�յ�ģʽ

extern void  AirCondition_SaveData(void);
extern void AirCondition_SaveRelateData(void);
extern void  Init_AirCondition(void);
extern void  AirCondition_KeyHander(u8 keyNum);
extern void  AirCondition_FullShow(void);
extern void  AirCondition_SendData(void);
extern void AirCondition_FirstShowPicture(void);
#endif

/***************************Copyright BestFu **********************************/
