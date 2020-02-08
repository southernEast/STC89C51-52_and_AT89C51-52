#ifndef __SENDMESSAGE_H__
#define __SENDMESSAGE_H__
#include <reg52.h>
#include "delay.h"

sbit relay =P1^0;						// 继电器
sbit buzzer =P1^3;					// 蜂鸣器
sbit RE = P0^7;							// 485引脚
sbit DE = P0^6;

extern unsigned char disFlag;		//更新显示标志
extern unsigned int times;			//时间计数
extern unsigned long time_20ms;	//定时变量
extern float volt,current,power;	//电压 电流 功率变量

extern unsigned char count;				//采集计数
extern xdata float sumVolt,midV;	//用于滤波 中间变量
extern xdata float sumAcur,midA;	//用于滤波 中间变量

extern unsigned char dis0[16];		//液晶显示第一行
extern unsigned char dis1[16];		//液晶显示第二行


extern void Init_RS485();
extern void Init_Timer0(void);		//函数声明
extern void UART_Init(void);
extern void uartSendByte(unsigned char dat);
extern void uartSendStr(unsigned char *s,unsigned char length);

#endif