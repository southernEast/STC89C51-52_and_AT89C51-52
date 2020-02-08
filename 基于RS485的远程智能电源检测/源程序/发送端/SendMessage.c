#include "SendMessage.h"

void Init_RS485()
{
	RE = 1;
	DE = 1;							// 配置为发送模式
}


void Init_Timer0(void)
{
 TMOD |= 0x01;	  //使用模式1，16位定时器，使用"|"符号可以在使用多个定时器时不受影响		     
 TH0=(65536-20000)/256;		  //重新赋值 20ms
 TL0=(65536-20000)%256;
 EA=1;            //总中断打开
 ET0=1;           //定时器中断打开
 TR0=1;           //定时器开关打开
}

void Timer0_isr(void) interrupt 1 
{
	TH0=(65536-20000)/256;	//重新赋值 20ms
	TL0=(65536-20000)%256;	
	time_20ms++;
	if(time_20ms%2==0)		 //定时更新显示
	{
		disFlag = 1;
	}

}
void UART_Init(void)
{
    SCON  = 0x50;		       		 	// SCON: 模式 1, 8-bit UART, 使能接收  
    TMOD |= 0x20;               // TMOD: timer 1, mode 2, 8-bit 重装
    TH1   = 0xFD;               // TH1:  重装值 9600 波特率 晶振 11.0592MHz  
    TR1   = 1;                  // TR1:  timer 1 打开                         
    EA    = 1;                  //打开总中断
    ES    = 1;                  //打开串口中断
}

void uartSendByte(unsigned char dat)
{
	unsigned char time_out;
	time_out=0x00;
	SBUF = dat;			  //将数据放入SBUF中
	while((!TI)&&(time_out<100))  //检测是否发送出去
	{time_out++;DelayUs2x(10);}	//未发送出去 进行短暂延时
	TI = 0;						//清除ti标志
}

void uartSendStr(unsigned char *s,unsigned char length)
{
	unsigned char NUM;
	NUM=0x00;
	while(NUM<length)	//发送长度对比
	{
		uartSendByte(*s);  //放松单字节数据
		s++;		  //指针++
		NUM++;		  //下一个++
  }
}
void UART_SER (void) interrupt 4 //串行中断服务程序
{
	unsigned char u_buf;
  if(RI)                        //判断是接收中断产生
	{
		RI=0;                      //标志位清零
		u_buf = SBUF;
	}
  if(TI)  //如果是发送标志位，清零
		TI=0;
}

