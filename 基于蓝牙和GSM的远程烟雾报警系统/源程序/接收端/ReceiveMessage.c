#include "ReceiveMessage.h"

unsigned char flag;

void Init(void)		//串口配置
{
    TMOD=0x20;   	//定时器1，工作方式2              
    TH1=0xfd;		//波特率为9600
    TL1=0xfd;
    PCON=0x00;              
    TR1=1;     		//定时器1开始工作 
    SCON=0x50;		//0101 0000B，工作方式1,8位UART，波特率可变，允许接收             
		RI = 0;
		TI = 0;   
} 

void delay(unsigned int i)
{
	while(i--);
}

void receive()			// 接收信息
{
	while(RI==0)
		;
	flag = SBUF;
	RI = 0;
//	send(flag);
}

void send(uchar ShowData)
{
	SBUF=ShowData;	 		//写发送寄存器
	while(TI==0)		 	//等待串行发送完成
	{
	;
	}
	TI=0;  					//对标志位清零
}

int select()
{
	if(flag == 1)		// 发生火灾
		return 1;
	else						// 未发生火灾
		return 0;
}


