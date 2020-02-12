#include "SendMessage.h"

uchar flag;

void Gas()
{
	uchar i = 0;
	if(gasDet == 0){					
		Delay(100);
		if(gasDet == 0){				// 检测到烟雾
			led1 = 0;							// 点亮应急灯
			led2 = 1;
			for(i = 0; i < 10; i++)
				Send(1);						// 发送信息
		}
	}
	else{											// 未检测到烟雾
		led1 = 1;								// 关闭应急灯
		led2 = 0;
		Send(0);								// 发送信息
	}
}

void Delay(uint k)
{
	uchar j;
	while((k--)!=0)           
	{
		for(j=0;j<125;j++)
			{;}
	}
}


void Init(void)
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
                        
void Send(uchar ShowData)
{
	SBUF=ShowData;	 		//写发送寄存器
	while(TI==0)		 	//等待串行发送完成
		;
	TI=0;  					//对标志位清零
}