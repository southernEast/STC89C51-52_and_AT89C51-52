#include <reg52.h> 
#include <intrins.h>
#include <stdio.h>
#include "i2c.h"
#include "delay.h"
#include "SendMessage.h"

unsigned char disFlag ;		//更新显示标志
unsigned int times = 0;		//时间计数
unsigned long time_20ms=0;	//定时变量
float volt,current,power;	 //电压 电流 功率变量

unsigned char count =0;//采集计数
xdata float sumVolt,midV; //用于滤波 中间变量
xdata float sumAcur,midA;		//用于滤波 中间变量

unsigned char dis0[16];		 //液晶显示第一行
unsigned char dis1[16];		 //液晶显示第二行

void main (void)
{
	Init_RS485();						// 485初始化
	Init_Timer0();        	// 定时器0初始化
	UART_Init();	   				// 串口初始化 
	
	relay = 0;buzzer = 0;		// 上电蜂鸣器响一声
	DelayMs(120);
	DelayMs(20);          	// 延时有助于稳定

	buzzer = 1; 						// 关闭蜂鸣器
	relay = 1;							// 闭合继电器
	sumAcur = 0;						// 清空原有数据
	sumVolt = 0;						// 清空累计
	while(1)         				// 主循环
	{
		if(disFlag ==1)				// 更新显示标志 一定时间发送一次数据
		{
			disFlag = 0;
			midA=(float)ReadADC(0)*2.7*5.15/255;				//读取AD0
			if(midA < 0.08) midA = 0;
			DelayMs(2);          //延时有助于稳定

			midV=(float)ReadADC(1)*321*5.15/255;				//读取AD1
			DelayMs(2);          //延时有助于稳定

			sumAcur = sumAcur + midA;	//多次测量求平均
			sumVolt = sumVolt + midV;	//多次测量求平均

			count++;//采集次数
			if(count >= 5)
			{	  
				count = 0;
				current = 	sumAcur/5;  //Q求平均
				if(current < 0.08) current= 0;//滤波微小波动
				sumAcur = 0;	//清空原有数据
				
				volt = sumVolt/5;	 //计算电压值
				if(volt < 15) volt= 0;//滤波微小波动
				sumVolt = 0;//清空累计
				power =current*  volt;  //功率计算
				
				sprintf(dis0,"v:%5.1fV a:%3.1fA ",volt,current);   //打印电压电流值
				uartSendStr(dis0, 16);											// 485发送第一行
				
				sprintf(dis0,"P:%5.1fW  no<600 ",power);   //打印功率值
				uartSendStr(dis0, 16);											// 485发送第二行

				if(power>=600)//值对比
				{
				 	buzzer =0;	 //打开蜂鸣器
					relay = 0;	 //断开继电器
				}
			}									
		}
	}
}

