/***************************Copyright BestFu 2014-05-14*************************
文	件：    DataEncrypt.h
说	明:	    433信号数据加密
编	译：    Keil uVision4 V4.72.10.0
版	本：    v0.3
编	写：    Joey
日	期：    2015.1.27
修  改:     
v0.3 2015.1.27
1.修改EncryptData函数原型，去掉加密因子KEY。
2.代码中去掉默认超级用户BEST记录，通信时不判断这个位置，无意义。
3.密钥方法较V0.2没有改变。
*******************************************************************************/

#ifndef _DATA_ENCRYPT_H
#define _DATA_ENCRYPT_H

/************************************************************************
函 数 名:  	EncryptInit
功能说明: 	初始化加密方法，事先算好本地址的值，以后不用再算了
参    数:  	Data-加密因子，实际为设备的地址
返 回 值:  	无
************************************************************************/
void EncryptInit(unsigned char *Data);

/************************************************************************
函 数 名:  	DecryptData
功能说明: 	  对数据进行解密
参    数:    *Data-待解密的数据指针，解密成功后有效数据在Data[0]起始位置
返 回 值:  	unsigned char，0表示成功，1表示解密失败，2表示CRC校验失败，
                            3表示版本号不对
************************************************************************/
unsigned char DecryptData(unsigned char *Data);

/************************************************************************
函 数 名:  	EncryptData
功能说明: 	对数据进行加密
参    数:   *Data-待加密的数据指针，加密成功后有效数据在Data[0]起始位置
            Len-数据长度
返 回 值:  	unsigned char，1表示成功，0表示失败
************************************************************************/
unsigned char EncryptData(unsigned char *Data, unsigned char Len);
#endif

