/***************************Copyright BestFu 2014-05-14*************************
��	����	UnitCfg.c
˵	����	��Ԫģ������Դ����
��	�룺	Keil uVision4 V4.54.0.0
��	����	v1.0
��	д��	Unarty
��	�ڣ�	2014.07.14 
�ޡ��ģ�	����
*******************************************************************************/
#include "UnitCfg.h"
#include "UserData.h"


#define UNIT_CNT	(sizeof(UnitTab)/sizeof(Unit_st))		//�ܵ�Ԫ��

/*���: ������Ԫ����ͷ�ļ�*/
#include "SysExeAttr.h"
#include "KeyExeAttr.h"


/*��ӵ�Ԫ�ӿں����������Խӿ��б�*/
/*��ʽ�����ϵ��ʼ�������ӿڣ�EEPROM���ݳ�ʼ�������ӿڣ���Ԫ��֤�����ӿڣ���Ԫ�����б���,��Ԫ�����б�Ԫ�ظ�����*/
/*���Ͻӿں���������Ԫ����Ҫ�˹���ʱֱ����NULL��ʾ����Ԫ�����б�������Ϊһ��ȷ��*/
const Unit_st UnitTab[] = 
{
	{SysAttr_Init	, NULL			, NULL				, (AttrExe_st*)SysAttrTable		,Sys_UnitAttrNum },	//ϵͳ�������ԣ�����������޸�
	{KeyInit	    , KeyEepromInit	, KeyChecking_Attr  , (AttrExe_st*)KeyAttrTab	    ,Key_UnitAttrNum},  //���ܴ������
};



UnitData_t gUnitData[UNIT_CNT];

/*******************************************************************************
�� �� ����	UnitCnt_Get
����˵���� 	��ȡ��ǰ��Ԫ����
��	  ���� 	��
�� �� ֵ��	��ǰ��Ԫ��
*******************************************************************************/
u8 UnitCnt_Get(void)
{
	return ((sizeof(UnitTab)/sizeof(Unit_st)));
}

/*******************************************************************************
�� �� ����	Unit_Init
����˵���� 	��ʼ�����е�Ԫ
��	  ���� 	��
�� �� ֵ��	��
*******************************************************************************/
void Unit_Init(void)
{
	u8 i = UNIT_CNT;
	
	UnitPublicEEPROMData_Get(UNIT_CNT, gUnitData);	//��ȡ�û����õĵ�Ԫ����ֵ
	
	while (i)	
	{
		if (UnitTab[--i].Init != NULL)
		{
			UnitTab[i].Init(i);		//��ʼ������Ԫ����
		}
	}
}

/**************************Copyright BestFu 2014-05-14*************************/
