/***************************Copyright BestFu ***********************************
**  文    件：  TIMx.c
**  功    能：  <<驱动层>> 通用定时器驱动 STM32L151 TIM2/TIM3/TIM4/TIM5
**  编    译：  Keil uVision5 V5.10
**  芯    片：  STM32L151
**  版    本：  V1.0
**  编    写：  Seven
**  创建日期：  2014.08.20
**  修改日期：  2014.08.20
**  说    明：  
**  V1.0
    >> 
*******************************************************************************/
#include "sys.h"
#include "TIMx.h"
#include "key.h"

/*******************************************************************************
**函    数:  void TIMx_Init(TIM_TypeDef *TIMx , u16 psc , u16 arr)
**功    能:  通用定时器TIM2/TIM3/TIM4/TIM5 初始化
**参    数:  *TIMx    -- TIM2/TIM3/TIM4/TIM5
**           psc     -- 预分频值
**           arr     -- 自动重装值
**返    回:  null
**说    明： Init_TIMx(TIM2,31,Count);   //32Mhz  1us分频单位 默认向上计数           
********************************************************************************/
void TIMx_Init(TIM_TypeDef *TIMx , u16 psc , u16 arr)
{
    RCC->APB1ENR |= (1u << ((u32)TIMx - TIM2_BASE)/(0x0400)); 

    TIMx->ARR  = arr;           //自动重装值
    TIMx->PSC  = psc;           //预分配器    CK_CNT = fck_psc/(psc+ 1) = 32M/(psc+1)   
    TIMx->CR2 &= ~(0xF<<4);     //清主模式
    TIMx->CR2 |= 4 << 4;        
    TIMx->DIER|= 1 << 0;		//使能中断  
    TIMx->CNT  = 0;             //清计数器  
    TIMx->SR   = 0;
//    TIMx->CR1 &=~(1 << 0);      //禁止定时器 
    TIMx->CR1 |= (1 << 0);      //启动定时器 
	
	MY_NVIC_Init(2,3,TIM2_IRQn,2);  
}

/*******************************************************************************
**函    数:  void TIMx_Enable(TIM_TypeDef *TIMx)
**功    能:  通用定时器TIM2/TIM3/TIM4/TIM5使能
**参    数:  *TIMx    -- TIM2/TIM3/TIM4/TIM5
**           
**返    回:  null
**说    明： 无        
********************************************************************************/
void TIMx_Enable(TIM_TypeDef *TIMx)
{
    TIMx->CR1 |= (1 << 0);      //使能定时器 
}

/*******************************************************************************
**函    数:  void Set_TIMx_CNTValue(TIM_TypeDef *TIMx,u16 value)
**功    能:  设置定时器的值
**参    数:  *TIMx    -- TIM2/TIM3/TIM4/TIM5
**           value ：设置的值
**返    回:  null
**说    明： 无        
********************************************************************************/
void Set_TIMx_CNTValue(TIM_TypeDef *TIMx,u16 value)
{
    TIMx->CNT = value;      //设置定时器的值
}

/*******************************************************************************
**函    数:  void TIMx_Disable(TIM_TypeDef *TIMx)
**功    能:  通用定时器TIM2/TIM3/TIM4/TIM5禁能
**参    数:  *TIMx    -- TIM2/TIM3/TIM4/TIM5
**           
**返    回:  null
**说    明： 无        
********************************************************************************/
void TIMx_Disable(TIM_TypeDef *TIMx)
{
    TIMx->CR1 &=~(1 << 0);      //使能定时器 
}

/******************************************************************************
**函    数: TIM2_IRQHandler
**功    能: 中断服务程序
******************************************************************************/
void TIM2_IRQHandler(void)
{ 
    static u8 fisrt_flag = 0;
    
    if(TIM2->SR&0X0001)
    {  
        if(!fisrt_flag)
        {
            fisrt_flag = 1;
            TIM2->ARR = 1500;       //设置长按键的时间
            TIM2->CR1 &=~(1 << 0);
            TIM2->CNT  = 0;
        }
        else
        {
            KeyTIMHandle(); 
        }
    }                  
    TIM2->SR&=~(1<<0);
}

/********************************* END FILE ***********************************/
