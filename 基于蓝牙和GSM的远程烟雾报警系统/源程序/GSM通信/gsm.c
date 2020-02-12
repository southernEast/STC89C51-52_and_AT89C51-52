#include "GSM.h"

xdata u8 Uart1_Buf[Buf_Max];
u8 First_Int = 0;
u16 count_20ms;
u8  *phone_num = "AT+CMGS=\"18844090554\""; //目标电话号码

void Timer0Init(void)		//20毫秒@115200
{
	TMOD &= 0xF0;		
	TMOD |= 0x01;		//
	TL0 = (65536-20000)%256;		//
	TH0 = (65536-20000)/256;		//
	TF0 = 0;		//
	ET0 = 1;    	  //
	TR0 = 1;		//开始计时
}

void Uart1Init(void)		//9600bps@11.05926MHz 配置定时器
{
	PCON &= 0x7F;		//
	SCON = 0x50;		//
	TMOD &= 0x0F;		//
	TMOD |= 0x20;		//
	TL1 = 0xFD;		  //
	TH1 = 0xFD;		  //
	ET1 = 0;		    //
	TR1 = 1;		    //
	ES  = 1;					//
	EA = 1;
}



/***************************************************************
注：当然你可以返回其他值，来确定到底是哪一步发送指令出现失败了。
****************************************************************/
int send_text_message(char *content)
{
	char end_char[2];
	
	end_char[0] = 0x1A;//结束字符
	end_char[1] = '\0';
	
	//设置存储位置

	UART1_Send_Command("AT+CSCS=\"GSM\"");
	delay_ms(50);
	UART1_Send_Command("AT+CMGF=1");
	delay_ms(80);
	UART1_Send_Command(phone_num);
	delay_ms(80);
	UART1_SendString(content); 
	UART1_Send_Command(end_char);
	
	return 1;
}

/*******************************************************************************
* 函数名 : CLR_Buf
* 描述   : 清除串口2缓存数据
* 输入   : 
* 输出   : 
* 返回   : 
* 注意   : 
*******************************************************************************/
void CLR_Buf(void)
{
	u8 k;
	for(k=0;k<Buf_Max;k++)      //将缓存内容清零
	{
		Uart1_Buf[k] = 0x00;
	}
    First_Int = 0;              //接收字符串的起始存储位置
}


/*******************************************************************************
* 函数名 : Find
* 描述   : 判断缓存中是否含有指定的字符串
* 输入   : 
* 输出   : 
* 返回   : unsigned char:1 找到指定字符，0 未找到指定字符 
* 注意   : 
*******************************************************************************/

u8 Find(u8 *a)
{ 
	ES = 0;  //改进程序
	if(strstr(Uart1_Buf,a)!=NULL)
	{
		ES = 1;
		return 1;
	}	
	else
	{
		ES = 1;
		return 0;
	}
		
}




/*----------------------------
UART1 发送串口数据
-----------------------------*/
void UART1_SendData(u8 dat)
{
	ES=0;			//关串口中断
	SBUF=dat;			
	while(TI!=1);	//等待发送成功
	TI=0;			//清除发送中断标志
	ES=1;			//开串口中断
}
/*----------------------------
UART1 发送字符串
-----------------------------*/
void UART1_SendString(char *s)
{
	while(*s)//检测字符串结束符
	{
		UART1_SendData(*s++);//发送当前字符
	}
}

/**********************************
发送命令
**********************************/
void UART1_Send_Command(char *s)
{
	CLR_Buf(); 
	while(*s)//检测字符串结束符
	{
		UART1_SendData(*s++);//发送当前字符
	}
	UART1_SendLR();
}

void UART1_Send_Command_END(char *s)
{
	CLR_Buf(); 
	while(*s)//检测字符串结束符
	{
		UART1_SendData(*s++);//发送当前字符
	}
}

/*******************************************************************************
* 函数名 : Second_AT_Command
* 描述   : 发送AT指令函数
* 输入   : 发送数据的指针、希望收到的应答、发送等待时间(单位：interval_time*20 ms)
* 输出   : 
* 返回   : 
* 注意   : 
*******************************************************************************/

u8 UART1_Send_AT_Command(u8 *b,u8 *a,u8 wait_time,u16 interval_time)         
{
	u8 i;

	CLR_Buf(); 
	i = 0;
	while(i < wait_time)                    
	{
		UART1_Send_Command(b);
		delay_ms(interval_time);
		if(Find(a))            //查找需要应答的字符
		{
			return 1;
		}
		i++;
	}
	
	return 0;
}

void delay_ms(u16 ms)
{
	 count_20ms = ms;
	 while(count_20ms);
}


u8 UART1_Send_AT_Command_END(u8 *b,u8 *a,u8 wait_time,u16 interval_time)         
{
	u8 i;

	CLR_Buf(); 
	i = 0;
	while(i < wait_time)                    
	{
		UART1_Send_Command_END(b);
		delay_ms(interval_time);
		if(Find(a))            //查找需要应答的字符
		{
			return 1;
		}
		i++;
	}
	
	return 0;
}

/*******************************************************************************
* 函数名 : Timer0_ISR
* 描述   : 定时器0中断服务入口函数,1ms中断一次
* 输入   : 
* 输出   : 
* 返回   : 
* 注意   : 
*******************************************************************************/
void Timer0_ISR() interrupt 1
{
	TR0=0;//关定时器
	TL0 = (65536-20000)%256;		//重设定时器初值
	TH0 = (65536-20000)/256;		//
	
	if(count_20ms > 0) //20ms延时计数器
	{
		count_20ms--;
	}	
	
	TR0=1;//开定时器
}


/********************* UART1中断函数************************/
void UART1_ISR (void) interrupt 4
{
	if (RI)
	{
		RI = 0;                           //清除RI位
		Uart1_Buf[First_Int] = SBUF;  	  //将接收到的字符串存到缓存中
		First_Int++;                	  //缓存指针向后移动
		if(First_Int >= Buf_Max)       	  //如果缓存满,将缓存指针指向缓存的首地址
		{
			First_Int = 0;
		}		
	}
	if (TI)
	{
		TI = 0;                          //清除TI位
	}
}
