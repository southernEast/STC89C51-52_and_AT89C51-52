#ifndef __BEAM_H__
#define __BEAM_H__

#include <reg52.h>
#include <INTRINS.H>

#define   uchar unsigned char
#define   uint unsigned int	
#define	  SlaveAddress   0x46 //定义器件在IIC总线中的从地址,根据ALT  ADDRESS地址引脚不同修改
                              //ALT  ADDRESS引脚接地时地址为0x46，接电源时地址为0xB8	

sbit SCL=P2^3;      //IIC时钟引脚定义
sbit SDA=P2^2;      //IIC数据引脚定义

extern void conversion(uint temp_data);
extern void delay_nms(unsigned int k);
extern void Delay5us();
extern void Delay5ms();
extern void BH1750_Start();
extern void BH1750_Stop();
extern void BH1750_SendACK(bit ack);
extern bit BH1750_RecvACK();
extern void BH1750_SendByte(uchar dat);
extern uchar BH1750_RecvByte();
extern void Single_Write_BH1750(uchar REG_Address);
extern void Multiple_Read_BH1750(); 
extern void Init_BH1750();
extern void Show_Beam();

#endif