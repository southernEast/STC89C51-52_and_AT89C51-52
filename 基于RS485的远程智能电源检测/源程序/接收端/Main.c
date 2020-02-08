#include <reg52.h>
#include <stdio.h>
#include "LCD1602.h"
#include "ReceiveMessage.h"

#define uchar unsigned char
#define uint  unsigned int


void delay(uint k);

void main(void)
{
	InitLcd();      // lcd初始化
	display_1602("Welcome to use!!", 0, 0);
	Init();					// 串口初始化
	RS485Init();		// RS485初始化

	while(1)
	{
		receiveString1();					// 接收第一行
		receiveString2();					// 接收第二行
		delay(100);

		show();										// 显示数据
	}
}

void delay(uint k)	
{						
	uint i,j;				
	for(i=0;i<k;i++)
	{			
		for(j=0;j<121;j++)			
		{;}
	}						
}