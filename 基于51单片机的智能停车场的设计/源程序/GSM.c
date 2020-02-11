#include "GSM.h"

typedef unsigned char	u8;
typedef unsigned int	u16;
typedef unsigned long	u32;

/*************  本地变量声明	**************/
xdata u8 Uart1_Buf[Buf_Max];
u8 First_Int = 0;

u8 outInfo[50];						// 输出
u16 count_20ms;
u8  receive_mode     = 0;
u8  find_string_flag = 0;
idata char num_message[4];//暂时假设最多只能存999条短信
idata char AT_CMGR[15];

idata char  extract_phone_num[29];   // 号码暂存 
idata char AT_CMGS_TEMP[29];
idata char phone_number[15];				// 发送号码
idata char end_char[2] = {0x1a, 0};

/*************	本地函数声明	**************/
void delay_ms(u16 ms);   //延时ms个20ms  ms*20ms
void CLR_Buf(void);      //清串口接收缓存
u8 	 Find(u8 *a);        //查找字符串
void UART1_SendData(u8 dat);   //串口1发送 1字节
void UART1_SendString(char *s);//串口1发送 字符串
void UART1_Send_Command(char *s);
u8 UART1_Send_AT_Command(u8 *b,u8 *a,u8 wait_time,u16 interval_time);
u8 Wait_CREG(u8 query_times);


/**定时器初始化**/
void Timer0Init(void)		//20毫秒@115200
{
	TMOD &= 0xF0;					
	TMOD |= 0x01;					//		
	TL0 = (65536-20000)%256;		//
	TH0 = (65536-20000)/256;		//
	TF0 = 0;		//
	ET0 = 1;    	  //
	TR0 = 1;		//开始计时
}

/**串口初始化**/
void Uart1Init(void)		//9600bps@11.05926MHz
{
	PCON &= 0x7F;		// 波特率不倍增
	SCON = 0x50;		// 串口方式2
	TMOD &= 0x0F;		// 定时器8位自动重装载
	TMOD |= 0x20;		//
	TL1 = 0xFD;		  //
	TH1 = 0xFD;		  //
	ET1 = 0;		  //
	TR1 = 1;		    //
	ES  = 1;					//
}

/**等待模块注册**/
u8 Wait_CREG(u8 query_times)
{
	u8 i;
	u8 k;
	u8 j;
	i = 0;
	CLR_Buf();
	while(i == 0)        			
	{

		UART1_Send_Command("AT+CREG?");
		delay_ms(100); 
		
		for(k=0;k<Buf_Max;k++)      			
		{
			if((Uart1_Buf[k] == '+')&&(Uart1_Buf[k+1] == 'C')&&(Uart1_Buf[k+2] == 'R')&&(Uart1_Buf[k+3] == 'E')&&(Uart1_Buf[k+4] == 'G')&&(Uart1_Buf[k+5] == ':'))
			{
					 
				if((Uart1_Buf[k+7] == '1')&&((Uart1_Buf[k+9] == '1')||(Uart1_Buf[k+9] == '5')))
				{
					i = 1;
					return 1;
				}
				
			}
		}
		j++;
		if(j > query_times)
		{
			return 0;
		}
		
	}
	return 0;
}

/**检测状态**/
int check_status(void)
{
	int ret;
	
	ret = UART1_Send_AT_Command("AT","OK",3,50);//测试通信是否成功
	if(ret == 0)
	{
		return COMMUNITE_ERROR;
	}
	
	ret = UART1_Send_AT_Command("AT+CPIN?","READY",2,50);//查询卡是否插上
	if(ret == 0)
	{
		return NO_SIM_CARD_ERROR;
	}
	
	ret = Wait_CREG(3);//查询卡是否注册到网络
	if(ret == 0)
	{
		return SIM_CARD_NO_REG_ERROR;
	}
	return 1;
}


/**配置接收信息模式**/
int config_format(void)
{
	u8 ret;

	UART1_Send_Command("ATE0");					// 关闭回显
	delay_ms(50);
	
	UART1_Send_Command("AT+CPMS=\"SM\",\"ME\",\"SM\""); 	// 设置存储位置
	delay_ms(50);

	ret = UART1_Send_AT_Command("AT+CNMI=2,1","OK",3,100);
	if(ret == 0)
	{
		return AT_CNMI_ERROR;
	}
	
	ret = UART1_Send_AT_Command("AT+CMGF=1","OK",3,50);		// 配置为TEXT模式
	if(ret == 0)
	{
		return AT_CMGF_ERROR;
	}
	
	ret = UART1_Send_AT_Command("AT+CSCS=\"GSM\"","OK",3,50);// 设置字符格式
	if(ret == 0)
	{
		return AT_CSCS_ERROR;
	}
	return 1;
}

/**获取信息位置 并发送读取接收信息指令**/
int read_message(void)
{
	char *buffer_ptr;
	u8 i,len;
	u8 ret;
	
	memset(num_message,'\0',4);			// 清空相关缓存区
	memset(AT_CMGR,'\0',15);
	buffer_ptr = strstr(Uart1_Buf,"\"SM\",");
	
	len = strlen("\"SM\",");

	i = len;
	if(buffer_ptr != NULL)					// 获取信息位置
	{
		while(*(buffer_ptr + i) != 0x0D && i < (len+3))
		{
			num_message[i-len] = *(buffer_ptr + i);
			i++;
		}
	}
	else
	{
		return NO_SMS_LEN_ERROR;
	}
	
	memset(AT_CMGR,'\0',15);
	strcpy(AT_CMGR,"AT+CMGR=");				// 设置读取信息指令
	strcat(AT_CMGR,num_message);
	
	ret = UART1_Send_AT_Command(AT_CMGR,"OK",3,50*4);	// 发送指令

	return ret;
}

/**从收件信息中获取发送人手机号码**/
void extract_phone_number(void)
{
	u8 pos;
	u8 i;

	memset(phone_number,'\0',12);		// 清空缓存
	
	for(i = 0;i < Buf_Max;i++)
	{
		if(Uart1_Buf[i] == '\"' && Uart1_Buf[i+1] == ',' && Uart1_Buf[i+2] == '\"')
		{
			pos = i + 3;
			break;
		}
	}
	i = 0;
	while(Uart1_Buf[pos] != '\"' && i < 14)
	{
		phone_number[i] = Uart1_Buf[pos];
		pos++;
		i++;
	}
	
	memset(AT_CMGS_TEMP,'\0',25);
	strcpy(AT_CMGS_TEMP,"AT+CMGS=\"");		// 设置发送信息指令
	strcat(AT_CMGS_TEMP,phone_number);
	strcat(AT_CMGS_TEMP,"\"\r");
	
	memset(extract_phone_num,'\0',30);
	strcpy(extract_phone_num,AT_CMGS_TEMP);
	
}

/**发送文本信息**/
int send_text_message(char *content)
{
	u8 ret;
	
	end_char[0] = 0x1A;//结束字符
	end_char[1] = '\0';
	
	
	ret = UART1_Send_AT_Command("AT+CMGF=1","OK",3,50);//配置为TEXT模式
	if(ret == 0)
	{
		return AT_CMGF_ERROR;
	}
	
	
	ret = UART1_Send_AT_Command("AT+CSCS=\"GSM\"","OK",3,50);//设置字符格式
	if(ret == 0)
	{
		return AT_CSCS_ERROR;
	}
	
	ret = UART1_Send_AT_Command(extract_phone_num,">",3,50);//输入收信人的电话号码
	if(ret == 0)
	{
        return AT_CMGS_ERROR;
	}    
	
	UART1_SendString(content); //此函数发送短信内容 不加回车换行
	ret = UART1_Send_AT_Command(end_char,"OK",1,250);//发送结束符，等待返回ok,等待5S发一次，因为短信成功发送的状态时间比较长
	if(ret == 0)
	{
		return END_CHAR_ERROR;
	}
	
	return 1;
}


/**定时器0中断服务入口函数,1ms中断一次**/
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


/** UART1中断函数**/
void UART1_ISR (void) interrupt 4
{
	if (RI)
	{
		RI = 0;                           // 清除RI位
		Uart1_Buf[First_Int] = SBUF;  	  // 将接收到的字符串存到缓存中
		First_Int++;                	  	// 缓存指针向后移动
		if(First_Int >= Buf_Max)       	  // 如果缓存满,将缓存指针指向缓存的首地址
		{
			First_Int = 0;
		}
		
		if(receive_mode == 1)
		{
			if(First_Int > 6)
			{
				find_string_flag = 1;
			}
	
		}
	}
	if (TI)
	{
		TI = 0;                          //清除TI位
	}
}


/**清除串口2缓存数据**/
void CLR_Buf(void)
{
	u8 k;
	for(k=0;k<Buf_Max;k++)      //将缓存内容清零
	{
		Uart1_Buf[k] = 0x00;
	}
    First_Int = 0;              //接收字符串的起始存储位置
}


/**判断缓存中是否含有指定的字符串**/
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

/**UART1 发送串口数据**/
void UART1_SendData(u8 dat)
{
	ES=0;			//关串口中断
	SBUF=dat;			
	while(TI!=1);	//等待发送成功
	TI=0;			//清除发送中断标志
	ES=1;			//开串口中断
}

/**UART1 发送字符串**/
void UART1_SendString(char *s)
{
	while(*s)//检测字符串结束符
	{
		UART1_SendData(*s++);//发送当前字符
	}
}


/**发送命令**/
void UART1_Send_Command(char *s)
{
	CLR_Buf(); 
	while(*s)//检测字符串结束符
	{
		UART1_SendData(*s++);//发送当前字符
	}
	UART1_SendLR();
}

/**发送AT指令函数
输入: 发送数据的指针、希望收到的应答、发送等待时间(单位：interval_time*20 ms)
**/
u8 UART1_Send_AT_Command(u8 *b,u8 *a,u8 wait_time,u16 interval_time)         
{
	u8 i;

	CLR_Buf(); 							// 清空接收缓存
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

/**延时函数**/
void delay_ms(u16 ms)
{
	 count_20ms = ms;
	 while(count_20ms);
}


/**GSM初始化**/
void GSM_init()
{
	int i;
	Uart1Init(); 								// 串口初始化
	Timer0Init();								// TIM0初始化
	EA = 1;      								// 开启总中断
	
	for(i = 0;i < STABLE_TIMES;i++)	// 等待网络稳定
	{
		delay_ms(50);
	}
	
	check_status();
	config_format();
	
	CLR_Buf();
	receive_mode = 1;
	
	LCD1602_write_com(0x80);
	LCD1602_write_word(" Initialized OK ");
	delay_ms(100);
}

/**扫描接收信息**/
void GSM_scan()
{
	int ret;
	if(find_string_flag == 1)	// 接收到信号标志		
	{
		delay_ms(5);						// 延时一点，让串口把数据接收完成
		if(Find("+CMTI"))				// 接收到了短信
		{
			ret = read_message();	// 查看接收到的短信
			if(ret == 1)					// 短信接收成功
			{
				if(Find("information"))		
				{
					LCD1602_write_com(0xc0);
					LCD1602_write_word(" Find  Successed");
					delay_ms(100);
					extract_phone_number();	// 提取号码
					
					sprintf(outInfo, "park1:%c\n park2:%c\n park3:%c\n park4:%c\n now Num:%d", 
									parkingFlag[0], parkingFlag[1], parkingFlag[2], parkingFlag[3], num);
					ret = send_text_message(outInfo);
					if(ret == 1)
					{
							LCD1602_write_com(0xc0);
							LCD1602_write_word(" Send  Successed");
							delay_ms(100);
					}
					else
					{
							LCD1602_write_com(0xc0);
							LCD1602_write_word(" Send  Failed   ");
							delay_ms(100);
					}
					CLR_Buf();
					find_string_flag = 0;
				}
				else
				{
					LCD1602_write_com(0xc0);
					LCD1602_write_word(" Find    Failed ");
					delay_ms(100);
					extract_phone_number();
					ret = send_text_message("This command is not supported!");
					if(ret == 1)
					{
							LCD1602_write_com(0xc0);
							LCD1602_write_word(" Send  Successed");
							delay_ms(100);
					}
					else
					{
							LCD1602_write_com(0xc0);
							LCD1602_write_word(" Send  Failed   ");
							delay_ms(100);
					}
					CLR_Buf();
					find_string_flag = 0;
				}
			}
		}			
	}	
}
