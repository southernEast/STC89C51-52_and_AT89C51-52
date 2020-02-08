#ifndef __LCD1602_H__
#define __LCD1602_H__

#include <reg52.h>
#include <INTRINS.H>

#define uchar unsigned char
#define uint  unsigned int

#define DataPort P0	   //LCD1602 数据端口
	
sbit LCM_RS=P2^5;   //LCD1602 控制端口		
sbit LCM_RW=P2^6;   //LCD1602 控制端口	
sbit LCM_EN=P2^7;   //LCD1602 控制端口

extern void InitLcd();
extern void WriteDataLCM(uchar dataW);
extern void WriteCommandLCM(uchar CMD,uchar Attribc);
extern void DisplayOneChar(uchar X,uchar Y,uchar DData);
extern void display_1602(uchar *DData,X,Y);

#endif