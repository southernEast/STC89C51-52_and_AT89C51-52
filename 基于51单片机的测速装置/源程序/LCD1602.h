#ifndef __LCD1602_H__
#define __LCD1602_H__

#include <reg52.h>

#define uchar unsigned char
#define uint unsigned int	  //宏定义

sbit rs=P1^4;				  //定义1602相关管脚
sbit en=P1^0;

extern void write_1602com(uchar com);
extern void write_1602dat(uchar dat);
extern void Lcd_ram();
extern void lcd_init();
extern void write_1602string(uchar *str);

#endif