/***************************Copyright BestFu ***********************************
**  ?    ?:  OLED.c
**  ?    ?:  <<???>> 1.3?OLED????
**  ?    ?:  Keil uVision5 V5.10
**  ?    ?:  V1.0
**  ?    ?:  Seven
**  ????:  2014.06.30
**  ????:  2014.08.20
**  ?    ?:  SH1106 ????
**              OLED ????????
**              ??:???,????,????,???,???
*******************************************************************************/

/******************************************************************************
**        OLED Module Message
** LCD Module:        VGM12864C3W01 -- 1.3 inch
** Display Size:      132x64
** Driver/Controller: SH1106
** Interface:         IIC????(400KHz) (???? 8080/SPI??)
******************************************************************************/
#include "OLED.h"
#include "IIC.h"
#include "delay.h"
#if SUPPORT_OLED_PUT_ASCII
 #include "Font.h"
#endif
//#include "OLED_pic.h"


/******************************************************************************
**?    ?:  OLED_WriteCmd()
**?    ?:  OLED???
**?    ?:  Command     ??
**?    ?:  void
******************************************************************************/
#define OLED_WriteCmd(Command)      IIC_WriteByte(0x00,Command)

/******************************************************************************
**?    ?:  OLED_WriteData()
**?    ?:  OLED???
**?    ?:  Data        ??
**?    ?:  void
*******************************************************************************/
#define OLED_WriteData(Data)        IIC_WriteByte(0x40,Data)

/******************************************************************************
**?    ?:  Init_OLED()
**?    ?:  OLED12864????  -- IIC ????
**?    ?:  null
**?    ?:  null
**?    ?: ???? SSD1306/SH1106
*******************************************************************************/
void Init_OLED()        
{
    IIC_InitPort();
    GPIOx_Cfg(PORT_OLED_POW, PIN_OLED_POW, OUT_PP_400K);
    OLED_POW_1;
    GPIOx_Cfg(PORT_OLED_RST, PIN_OLED_RST, OUT_PP_400K);
    GPIOx_Rst(PORT_OLED_RST, PIN_OLED_RST); 
    delay_ms(20);
    GPIOx_Set(PORT_OLED_RST, PIN_OLED_RST);

    OLED_WriteCmd(0xAE);    //display off
    OLED_WriteCmd(0x20);    //Set Memory Addressing Mode    
    OLED_WriteCmd(0x10);    //00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
    OLED_WriteCmd(0xb0);    //Set Page Start Address for Page Addressing Mode,0-7
    //OLED_WriteCmd(0xc8);    //Set COM Output Scan Direction
    OLED_WriteCmd(0xc0);    //Set COM Output Scan Direction
    OLED_WriteCmd(0x00);    //--set low column address
    OLED_WriteCmd(0x10);    //--set high column address
    OLED_WriteCmd(0x40);    //--set start line address
    OLED_WriteCmd(0x81);    //--?????  0x00--0xFF  
    OLED_WriteCmd(0x7f);    
    //OLED_WriteCmd(0xa1);    //--set segment re-map 0 to 127
    OLED_WriteCmd(0xa0);    //--set segment re-map 0 to 127
    OLED_WriteCmd(0xa6);    //--set normal display
    OLED_WriteCmd(0xa8);    //--set multiplex ratio(1 to 64)
    OLED_WriteCmd(0x3F);    //
    OLED_WriteCmd(0xa4);    //0xa4,Output follows RAM content;0xa5,Output ignores RAM content
    OLED_WriteCmd(0xd3);    //-set display offset
    OLED_WriteCmd(0x00);    //-not offset
    OLED_WriteCmd(0xd5);    //--set display clock divide ratio/oscillator frequency
    OLED_WriteCmd(0xf0);    //--set divide ratio
    OLED_WriteCmd(0xd9);    //--set pre-charge period
    OLED_WriteCmd(0x22);
    OLED_WriteCmd(0xda);    //--set com pins hardware configuration
    OLED_WriteCmd(0x12);
    OLED_WriteCmd(0xdb);    //--set vcomh
    OLED_WriteCmd(0x20);    //0x20,0.77xVcc
    OLED_WriteCmd(0x8d);    //--set DC-DC enable
    OLED_WriteCmd(0x14);    //
    delay_ms(5);
    OLED_ClearScreen();   
    //OLED_ShowPicture(Bestfu_BMP);
    OLED_WriteCmd(0xAF);    //--turn on oled panel 
}

/*******************************************************************************
**?    ?:  OLED_ClearScreen()
**?    ?:  ??
**?    ?:  null
**?    ?:  null
**?    ?:  
*******************************************************************************/
void OLED_ClearScreen(void)
{
  unsigned char x,y;
  for(y=0;y<8;y++)
    {
      OLED_WriteCmd(0xb0+y);
      OLED_WriteCmd(0x00);
      OLED_WriteCmd(0x10);
      for(x=0;x<132;x++)
        {
          OLED_WriteData(0);
        }
    }
}
/*******************************************************************************
**?    ?:  OLED_SetCursor()
**?    ?:  ????
**?    ?:  null
**?    ?:  null
**?    ?: ?????8?
*******************************************************************************/
void OLED_SetCursor(u8 XPos, u8 YPos)
{
    u8 Xtmp;
    XPos*=8;    
    Xtmp = XPos&0x0F;               //X ???
    XPos = ((XPos>>4)&0x0F)+0x10;   //X ???
    OLED_WriteCmd(YPos+0xB0);       //Y
    OLED_WriteCmd(Xtmp+2);          //????????
    OLED_WriteCmd(XPos);            //????????
}
/*******************************************************************************
**?    ?:  OLED_SetCursor_Normal()
**?    ?:  ????
**?    ?:  null
**?    ?:  null
**?    ?: ???????(????) 
*******************************************************************************/
void OLED_SetCursor_Normal(u8 XPos, u8 YPos)
{
    u8 Xtmp;
    Xtmp = XPos&0x0F;               //X ???
    XPos = ((XPos>>4)&0x0F)+0x10;   //X ???
    OLED_WriteCmd(YPos+0xB0);       //Y
    OLED_WriteCmd(Xtmp+2);          //????????
    OLED_WriteCmd(XPos);            //????????
}
/*******************************************************************************
**?    ?:  OLED_ShowPicture()
**?    ?:  OLED????
**?    ?:  null
**?    ?:  null
**?    ?: ?????? 
*******************************************************************************/
void OLED_ShowPicture(const u8 *p_Pic)
{
  unsigned char x,y;
  for(y=0;y<8;y++)
    {
      OLED_WriteCmd(0xb0+y);
      OLED_WriteCmd(0x00+2);
      OLED_WriteCmd(0x10);
      for(x=0;x<128;x++)
        {
          OLED_WriteData(*p_Pic++);
        }
    }
}

/*******************************************************************************
**?    ?:  OLED_ShowPicture()
**?    ?:  OLED????
**?    ?:  null
**?    ?:  null
**?    ?: ?????? 
*******************************************************************************/
void OLED_ShowPicAt(u8 x0, u8 y0, u8 xlen, u8 ylen,const u8 *p_Pic)
{
  unsigned char xtemp,ytemp;
    
  for(ytemp=y0; ytemp< (y0+(ylen/8));ytemp++)
    {
      OLED_SetCursor(x0,ytemp);
      for( xtemp =0; xtemp < xlen ; xtemp++ )
        {
          OLED_WriteData(*p_Pic++);
        }
    }
}

/*******************************************************************************
**?    ?:  OLED_ShowHalfPicAt()
**?    ?:  OLED????????(64*64)??
**?    ?:  x0       -- ?????
**?    ?:  null
**?    ?: ?????? 
*******************************************************************************/
void OLED_ShowHalfPicAt(u8 x0,const u32 *p_Pic)
{
    u8 x,y;
    u32 ptemp;
    
    for( y=0; y< 8; y++ )
    {
        OLED_SetCursor(x0,y);
        for( x =0; x < 16 ; x++ )
        {
            ptemp = *p_Pic++ ;
            OLED_WriteData( (ptemp)    &0xFF);
            OLED_WriteData( (ptemp>>8) &0xFF);
            OLED_WriteData( (ptemp>>16)&0xFF);
            OLED_WriteData( (ptemp>>24)&0xFF);
        }
    }
}
/*******************************************************************************
**?    ?:  OLED_ShowHalfPicAt()
**?    ?:  OLED???????????(64*32)??
**?    ?:  x0       -- ?????
**?    ?:  null
**?    ?: ?????? 
*******************************************************************************/
void OLED_ShowQuartPicAt(u8 x0,u8 y0,const u32 *p_Pic)
{
    u8 x,y;
    u32 ptemp;
    
    for( y=0; y< 4; y++ )
    {
        OLED_SetCursor(x0,y+y0);
        for( x =0; x < 16 ; x++ )
        {
            ptemp = *p_Pic++ ;
            OLED_WriteData( (ptemp)    &0xFF);
            OLED_WriteData( (ptemp>>8) &0xFF);
            OLED_WriteData( (ptemp>>16)&0xFF);
            OLED_WriteData( (ptemp>>24)&0xFF);
        }
    }
}

//??32*32??
void OLED_Show32x323PicAt(u8 x0,u8 y0,const u32 *p_Pic)
{
    u8 x,y;
    u32 ptemp;
    
    for( y=0; y< 4; y++ )
    {
        OLED_SetCursor(x0,y+y0);
        for( x =0; x < 8 ; x++ )
        {
            ptemp = *p_Pic++ ;
            OLED_WriteData( (ptemp)    &0xFF);
            OLED_WriteData( (ptemp>>8) &0xFF);
            OLED_WriteData( (ptemp>>16)&0xFF);
            OLED_WriteData( (ptemp>>24)&0xFF);
        }
    }
}
/*******************************************************************************
**?    ?:  OLED_ShowChar()
**?    ?:  ASCII???? ( 8*16 )
**?    ?:  (X0,Y0)  -- ????(??????8*16??) 
**           DspChar  -- ?????
**           DspMode  -- 0 ->??   1 ->??
**?    ?:  null
*******************************************************************************/
#if SUPPORT_OLED_PUT_ASCII
void OLED_ShowChar(unsigned char X0, unsigned char Y0, char DspChar, unsigned char DspMode)
{
    u8  i,k;
    u8  DspData;
    const u8 *DspTbl = NULL;

    DspTbl = &ASCII_1608[DspChar - ' '][0];
   
    for(i=0;i<2;i++)
    {
        OLED_SetCursor(X0,Y0*2+i);  
        for(k=0;k<8;k++)
        {
            DspData = *DspTbl++;
            if(DspMode)  DspData = ~DspData;    
            OLED_WriteData(DspData);
        }
    }
}
/*******************************************************************************
**?    ?:  OLED_ShowCN()
**?    ?:  ??????( 16*16 )
**?    ?:  (X0,Y0)  -- ????(??????16*16??) 
**           *DspCN   -- ?????
**           DspMode  -- 0 ->??   1 ->??
**?    ?:  null
*******************************************************************************/
void OLED_ShowCN(unsigned char X0, unsigned char Y0,const char *DspCN,unsigned char DspMode)
{
    u8  i,j,k;
    u8  DspData;
    const u8   *DspTbl = NULL;

    for(j=0; j<sizeof(GB1616_ZMo)/3; j++)
    {   
        if((*DspCN == GB1616_ZMo[j][0])&&(*(DspCN+1) == GB1616_ZMo[j][1]))
        {
            DspTbl = &GB1616[j][0];
        }
    }
   
    for(i=0;i<2;i++)
    {
        OLED_SetCursor(X0,Y0*2+i);  
        for(k=0;k<16;k++)
        {
            DspData = *DspTbl++;
            if(DspMode)  DspData = ~DspData;    
            OLED_WriteData(DspData);
        }
    }
}

/*******************************************************************************
**?    ?:  OLED_ShowString()
**?    ?:  ???(????)?? ( 8*16 )
**?    ?:  (X0,Y0)  -- ????(??????8*16??) 
**           *DspStr  -- ?????
**           DspMode  -- 0 ->??   1 ->??
**?    ?:  null
*******************************************************************************/
void OLED_ShowString(unsigned char X0, unsigned char Y0,const char *DspStr,unsigned char DspMode)
{

    while( *DspStr != '\0')
    {
        if(*DspStr > 0xA0)  {   //??
            OLED_ShowCN(X0, Y0, DspStr, DspMode);
            X0+=2;
            DspStr+=2;
        }
        else {                  //??  <0x80
            OLED_ShowChar(X0, Y0, *DspStr, DspMode);
            X0++;
            DspStr++;
        }
        
    }
}
/*******************************************************************************
**?    ?:  LCD_Pow()
**?    ?:  m^n??
**?    ?:  m    -- ??
**           n    -- ??
**?    ?:  n????
*******************************************************************************/
//static unsigned int LCD_Pow(unsigned char m, unsigned char n)
//{
//    unsigned int result = 1; 

//    while(n--)   result*= m;

//    return  result;
//}

/*******************************************************************************
**?    ?:  OLCD_ShowNumber()
**?    ?:  ?? int ???
**?    ?:  (X0,Y0)  -- ????(??????8*16??) 
**           Data     -- ?????
**           len      -- ??  ??5
**           DspMode  -- 0 ->??   1 ->??
**?    ?:  null
*******************************************************************************/
//void OLED_ShowNumber(unsigned char X0, unsigned char Y0,unsigned int Data,unsigned char len , unsigned char DspMode)
//{
//    unsigned char t,temp;
//    if(len>5) return;
//    for(t=0;t<len;t++)
//    {
//        temp=(Data/LCD_Pow(10,len-t-1))%10;
//        OLED_ShowChar(X0+t, Y0 , temp+'0', DspMode);
//    }
//}
#endif
/*******************************************************************************
**?    ?:  OLED_DrawLine()
**?    ?:  ???
**?    ?:  ???????
**?    ?:  null
*******************************************************************************/
void OLED_DrawLine(unsigned char x0)//,unsigned char y0,unsigned char x1,unsigned char y1)
{
    unsigned char i;
    
    for( i=0 ; i<8 ; i++)
    {
        OLED_SetCursor_Normal(x0,i);
        OLED_WriteData(0xff);
    }
}

//???? 18*32 ??
void OLED_ShowNum18x32(unsigned char x0, unsigned char y0,  const u32 *p_Pic)
{
    u8    x,y;
    u32   ptemp;
    const u8 *DspTbl = (u8*)&p_Pic[0];
    
    for( y=0; y< 4; y++ )
    {
        OLED_SetCursor_Normal(x0,y+y0);
        for( x =0; x < 18 ; x++ )
        {
            ptemp = *DspTbl++ ;
            OLED_WriteData(ptemp);
        }
    }
}

/********************************* END FILE ***********************************/
