#include<reg52.h>	  //包含头文件

#define uchar unsigned char
#define uint unsigned int	  //宏定义

#include "Data.h"			  //包含大小月判断函数
#include "DS1302.h"			  //包含1302函数
#include "AT24C02.h"		  //包含24c02函数
#include "LCD1602.h"
#include "DHT11.h"

extern uchar code tab1[];
extern uchar code tab2[];	
extern uchar code tab3[];	
extern uchar code tab4[];	
extern uchar code tab5[];


sbit COUNT_IN=P3^2;			  //霍尔传感器信号输入
sbit voice = P2^3;

//键盘定义
sbit K1=P3^4;	//设置时间
sbit K3=P3^6;	//减按键
sbit K2=P3^5;	//加按键
sbit K4=P3^7;	//设置半径安全距离
sbit BEEP=P2^0;	//蜂鸣器

uint count;					  //霍尔传感器计数变量
unsigned long Velocity,Mileage;	 //速度变量，里程变量

			   			
uchar Mode=0;							//设置时间的位置变量，值为1时设置年，值为2时设置月，以此类推...
uchar bike_set=0;						//设置半径，安全速度时的位置变量
uchar a;
char RADIUS,SAFE_SPEED;					//半径和安全速度变量
bit LED_SEC;							//秒灯闪烁变量
uchar before_sec;						//上一秒，用与控制秒灯闪烁

void READS();			//读取AT24C02的数据
void SETS();			//写入24c02数据
void delay(uint x)		//延时函数，大约延时1ms，不精确
{
	uint i,j;			//定义延时变量
	for(i=0;i<x;i++)	//执行x次
	for(j=0;j<110;j++);	//执行110次
}

void init()				//定时器初始化函数
{
	IT0=1;	//INT0负跳变触发	
	TMOD=0x01;//定时器工作于方式1
	TH0=0x3c;	  //50ms
	TL0=0xb0;
	EA=1; //CPU开中断总允许
	ET0=1;//开定时中断
	EX0=1;//开外部INTO中断
  TR0=1;//启动定时
}


void display()
{
	//			1km/h=100m/0.1h	  	 360s
	//			10km/h=100m/0.01h 	 36s
	//			100km/h=100m/0.001h  3.6s
	if(Mode==0&&bike_set==0)			   //不在设置状态时
	{
		//读时间
			Ds1302_Read_Time();			   //读取1302内部的时间数据
			//显示时间
			write_1602com(0x80);				   //第一行第一列显示，一下内容依次显示
			write_1602dat(0x30+time_buf1[1]/10);   //显示年的十位
			write_1602dat(0x30+time_buf1[1]%10);   //显示年的个位
			write_1602com(0x80+3);				   //第一行第三列（第二列显示“：”），液晶是从第0列开始
			write_1602dat(0x30+time_buf1[2]/10);   //显示月的十位
			write_1602dat(0x30+time_buf1[2]%10);   //显示月的个位
			write_1602com(0x80+6);				   //下同
			write_1602dat(0x30+time_buf1[3]/10);
			write_1602dat(0x30+time_buf1[3]%10);
			write_1602com(0x80+9);
			write_1602dat(0x30+time_buf1[4]/10);
			write_1602dat(0x30+time_buf1[4]%10);
			write_1602com(0x80+12);
			write_1602dat(0x30+time_buf1[5]/10);
			write_1602dat(0x30+time_buf1[5]%10);
			write_1602com(0x80+15);					//第一行第十五列，显示星期数据
			write_1602dat(time_buf1[7]-1);			//显示星期

			if(before_sec!=time_buf1[6])			//秒灯闪烁
			{
				before_sec=time_buf1[6];			//读秒的个位
				write_1602com(0x80+11);				//选中第一行11列
				write_1602dat(':');					//显示：
				LED_SEC=1;							//秒灯变量置1，定时器开始计时500ms后熄灭
			}
			if(LED_SEC==0)							//定时器定时时间到
			{
				write_1602com(0x80+11);				//第一行第11列
				write_1602dat(' ');					//不显示
			}

			write_1602com(0x80+0x40);				//第二行
			if(Mileage/1000000==0)					//里程的最高位是否为0
			write_1602dat(' ');						//为0就不显示
			else									//不为0
			write_1602dat(0x30+Mileage/1000000);//数字+0x30得到该数字的LCD1602显示码
			if(Mileage%1000000/100000==0)			//同上
			write_1602dat(' ');
			else	
			write_1602dat(0x30+Mileage%1000000/100000);//数字+0x30得到该数字的LCD1602显示码
			write_1602dat(0x30+Mileage%1000000%100000/10000);//数字+0x30得到该数字的LCD1602显示码 
			write_1602com(0x80+0x40+4);				 //第二行第4列
			write_1602dat(0x30+Mileage%1000000%100000%10000/1000);//数字+30得到该数字的LCD1602显示码
			write_1602dat(0x30+Mileage%1000000%100000%10000%1000/100);//数字+30得到该数字的LCD1602显示码
			write_1602dat(0x30+Mileage%1000000%100000%10000%1000%100/10);//数字+30得到该数字的LCD1602显示码
			SETS();							//每次刷新里程数据后都保存一次里程数据，掉电存储
	
			write_1602com(0x80+0x40+9);							//第二行第9列
			if(Velocity/100==0)
			write_1602dat(' ');
			else
			write_1602dat(0x30+Velocity/100);
			if(Velocity/10==0)
			write_1602dat(' ');
			else
			write_1602dat(0x30+Velocity%100/10);						//显示速度数据
			write_1602dat(0x30+Velocity%100%10);//数字+30得到该数字的LCD1602显示码
	}
	else if(Mode!=0)			 //在设置万年历时钟模式时
	{
		switch(Mode)			 //判断Mode数据
		{
			case 1:												   //值为1时
				write_1602com(0x80+0x40);//显示固定符号写入位置	   //第二行
				for(a=0;a<16;a++)
				{
					write_1602dat(tab5[a]);//写显示固定符号
				}
				write_1602com(0x80+0x40+14);				//第二行第14列
				write_1602dat(0x30+time_buf1[6]/10);		//显示秒
				write_1602dat(0x30+time_buf1[6]%10);	
				write_1602com(0x0F);	 //打开闪烁
				write_1602com(0x80+1);	 //第一行第一列闪烁，年的位置闪烁
				break;						  
			case 2:
				write_1602com(0x80+4);	 //月的位置闪烁
				break;
			case 3:
				write_1602com(0x80+7);	 //日的位置闪烁
				break;
			case 4:
				write_1602com(0x80+10);	 //小时的位置闪烁
				break;
			case 5:
				write_1602com(0x80+13);	 //分钟的位置闪烁
				break;
			case 6:
				write_1602com(0x80+0x40+15); //秒的位置闪烁
				break;
			case 7:
				write_1602com(0x80+15);		 //星期的位置闪烁
				break;
			case 8:
				write_1602com(0x0c); //设置完后关闭闪烁
				write_1602com(0x80);//显示固定符号从第一行第1个位置之后开始显示
				for(a=0;a<16;a++)
				{
					write_1602dat(tab1[a]);//向液晶屏写固定符号部分
				}
				write_1602com(0x80+0x40);//显示固定符号写入位置，从第2个位置后开始显示
				for(a=0;a<16;a++)
				{
					write_1602dat(tab2[a]);//写显示固定符号
				}
				break;
		}
	}
	else if(bike_set!=0)				   //设置半径和安全速度
	{
		switch(bike_set)								 //判断bike_set
		{
			case 1:	
				write_1602com(0x80);//显示固定符号写入位置
				for(a=0;a<16;a++)
				{
					write_1602dat(tab3[a]);//写显示固定符号
				}
				write_1602com(0x80+0x40);//显示固定符号写入位置
				for(a=0;a<16;a++)
				{
					write_1602dat(tab4[a]);//写显示固定符号
				}
				write_1602com(0x80+12);
				write_1602dat(0x30+RADIUS/10);		   //车轮半径
				write_1602dat(0x30+RADIUS%10);
				write_1602com(0x80+0x40+10);
				write_1602dat(0x30+SAFE_SPEED/10);		   //安全速度
				write_1602dat(0x30+SAFE_SPEED%10);	
				write_1602com(0x0F);	 //打开闪烁
				write_1602com(0x80+13);
				break;						  
			case 2:
				write_1602com(0x80+0x40+11);
				break;
			case 3:
				write_1602com(0x0c);
				write_1602com(0x80);//显示固定符号从第一行第1个位置之后开始显示
				for(a=0;a<16;a++)
				{
					write_1602dat(tab1[a]);//向液晶屏写固定符号部分
				}
				write_1602com(0x80+0x40);//显示固定符号写入位置，从第2个位置后开始显示
				for(a=0;a<16;a++)
				{
					write_1602dat(tab2[a]);//写显示固定符号
				}
				break;
		}
	}	
}

void KEY()					//按键函数
{	
	if(bike_set==0&&K1==0)	//不是设置半径模式时，可以按下K1
	{
		delay(20);			//延时去抖
		if(bike_set==0&&K1==0)//再次判断按键是否按下
		{
			BEEP=0;			  //蜂鸣器响
			delay(50);		  //延时
			BEEP=1;			  //关闭蜂鸣器，达到按键音的作用，下同，注释略
			Mode++;			  //Mode++；
			display();		  //显示函数
			if(Mode>=8)		  //设置完成
			{
				Mode=0;			   //变量清零
				Ds1302_Write_Time();//时间日期数据写入时钟芯片
			}
		}
		while(bike_set==0&&K1==0); //按键释放，只要按键没有松开，此处一直成立，不会跳出
	}
	if(K4==0&&Mode==0)			   //不是设置时间数据时才可以按下K4
	{
		delay(20);				   //延时去抖，下同
		if(K4==0&&Mode==0)		   //再次判断按键是否按下
		{
			BEEP=0;
			delay(50);
			BEEP=1;				   //按键音
			bike_set++;			   //变量加
			display();			   //调用显示函数
			if(bike_set>=3)		   //设置完成
			{
				bike_set=0;		   //变量清零
				SETS();			   //将设置好的数据保存到AT24C02中，掉电保存
			}
		}
		while(Mode==0&&K4==0);	   //按键释放
	}

	//+
	if(K2==0&&(Mode!=0||bike_set!=0))  //在设置状态时，按下K2
	{
		delay(20);
		//调时
		if(K2==0&&(Mode!=0||bike_set!=0))
		{
			BEEP=0;
			delay(50);
			BEEP=1;	
			switch(Mode)								//设置时钟时
			{
				case 1:									//设置年时
					time_buf1[1]++;						//年加
					if(time_buf1[1]>=100)				//年最大加到99，到100后清零
						time_buf1[1]=0;
					write_1602com(0x80);
					write_1602dat(0x30+time_buf1[1]/10);//显示年数据
					write_1602dat(0x30+time_buf1[1]%10);
					write_1602com(0x80+1);				//返回到年的位置闪烁
					break;								//跳出
				case 2:									//设置月份
					time_buf1[2]++;						//月份数据加
					if(time_buf1[2]>=13)				//最大是12
						time_buf1[2]=1;					//下同，注释略
					write_1602com(0x80+3);
					write_1602dat(0x30+time_buf1[2]/10);
					write_1602dat(0x30+time_buf1[2]%10);
					write_1602com(0x80+4);
					break;
				case 3:
					time_buf1[3]++;								   //日数据
					if(time_buf1[3]>=YDay(time_buf1[1],time_buf1[2])+1)
						time_buf1[3]=1;
					write_1602com(0x80+6);
					write_1602dat(0x30+time_buf1[3]/10);
					write_1602dat(0x30+time_buf1[3]%10);
					write_1602com(0x80+7);
					break;
				case 4:
					time_buf1[4]++;								//时
					if(time_buf1[4]>=24)
						time_buf1[4]=0;
					write_1602com(0x80+9);
					write_1602dat(0x30+time_buf1[4]/10);
					write_1602dat(0x30+time_buf1[4]%10);
					write_1602com(0x80+10);
					break;
				case 5:
					time_buf1[5]++;								 //分
					if(time_buf1[5]>=60)
						time_buf1[5]=0;
					write_1602com(0x80+12);
					write_1602dat(0x30+time_buf1[5]/10);
					write_1602dat(0x30+time_buf1[5]%10);
					write_1602com(0x80+13);
					break;
				case 6:
					time_buf1[6]++;
					if(time_buf1[6]>=60)					 //秒
						time_buf1[6]=0;
					write_1602com(0x80+0x40+14);
					write_1602dat(0x30+time_buf1[6]/10);
					write_1602dat(0x30+time_buf1[6]%10);
					write_1602com(0x80+0x40+15);
					break;
				case 7:
					time_buf1[7]++;
					if(time_buf1[7]>=8)						  //星期
						time_buf1[7]=1;
					write_1602com(0x80+15);
					write_1602dat(time_buf1[7]-1);
					write_1602com(0x80+15);
					break;
			}
			switch(bike_set)							 //设置半径安全速度
			{
				case 1:
					RADIUS++;							 //半径数据
					if(RADIUS>=71)
					RADIUS=0;
					write_1602com(0x80+12);
					write_1602dat(0x30+RADIUS/10);
					write_1602dat(0x30+RADIUS%10);
					write_1602com(0x80+13);
					break;
				case 2:
					SAFE_SPEED++;
					if(SAFE_SPEED>=100)					   //安全速度
					SAFE_SPEED=0;
					write_1602com(0x80+0x40+10);
					write_1602dat(0x30+SAFE_SPEED/10);
					write_1602dat(0x30+SAFE_SPEED%10);
					write_1602com(0x80+0x40+11);
					break;
			}
		}
		while(K2==0);							  //按键释放
	}

	//-
	if(K3==0&&(Mode!=0||bike_set!=0))			 //以下为K3，减按键，注释参考加按键，略
	{
		delay(20);
		//调时
		if(K3==0&&(Mode!=0||bike_set!=0))
		{
			BEEP=0;
			delay(50);
			BEEP=1;
			switch(Mode)
			{
				case 1:
					time_buf1[1]--;
					if(time_buf1[1]<0)
						time_buf1[1]=99;
					write_1602com(0x80);
					write_1602dat(0x30+time_buf1[1]/10);
					write_1602dat(0x30+time_buf1[1]%10);
					write_1602com(0x80+1);
					break;
				case 2:
					time_buf1[2]--;
					if(time_buf1[2]<=0)
						time_buf1[2]=12;
					write_1602com(0x80+3);
					write_1602dat(0x30+time_buf1[2]/10);
					write_1602dat(0x30+time_buf1[2]%10);
					write_1602com(0x80+4);
					break;
				case 3:
					time_buf1[3]--;
					if(time_buf1[3]<=0)
						time_buf1[3]=YDay(time_buf1[1],time_buf1[2]);
					write_1602com(0x80+6);
					write_1602dat(0x30+time_buf1[3]/10);
					write_1602dat(0x30+time_buf1[3]%10);
					write_1602com(0x80+7);
					break;
				case 4:
					time_buf1[4]--;
					if(time_buf1[4]<0)
						time_buf1[4]=23;
					write_1602com(0x80+9);
					write_1602dat(0x30+time_buf1[4]/10);
					write_1602dat(0x30+time_buf1[4]%10);
					write_1602com(0x80+10);
					break;
				case 5:
					time_buf1[5]--;
					if(time_buf1[5]<0)
						time_buf1[5]=59;
					write_1602com(0x80+12);
					write_1602dat(0x30+time_buf1[5]/10);
					write_1602dat(0x30+time_buf1[5]%10);
					write_1602com(0x80+13);
					break;
				case 6:
					time_buf1[6]--;
					if(time_buf1[6]<0)
						time_buf1[6]=59;
					write_1602com(0x80+0x40+14);
					write_1602dat(0x30+time_buf1[6]/10);
					write_1602dat(0x30+time_buf1[6]%10);
					write_1602com(0x80+0x40+15);
					break;
				case 7:
					time_buf1[7]--;
					if(time_buf1[7]<1)
						time_buf1[7]=7;
					write_1602com(0x80+15);
					write_1602dat(time_buf1[7]-1);
					write_1602com(0x80+15);
					break;
			}
			switch(bike_set)
			{
				case 1:
					RADIUS--;
					if(RADIUS<0)
					RADIUS=70;
					write_1602com(0x80+12);
					write_1602dat(0x30+RADIUS/10);
					write_1602dat(0x30+RADIUS%10);
					write_1602com(0x80+13);
					break;
				case 2:
					SAFE_SPEED--;
					if(SAFE_SPEED<0)
					SAFE_SPEED=99;
					write_1602com(0x80+0x40+10);
					write_1602dat(0x30+SAFE_SPEED/10);
					write_1602dat(0x30+SAFE_SPEED%10);
					write_1602com(0x80+0x40+11);
					break;
			}
		}
		while(K3==0);
	}	
	if(K2==0&&K3==0&&Mode==0&bike_set==0)	  //不是在设置状态时，两个按键同时按下
	{
		BEEP=0;
		delay(100);
		BEEP=1;
		delay(100);
		BEEP=0;
		delay(100);
		BEEP=1;						   //蜂鸣器快响两声
		delay(100);
		Mileage=0;						//里程数据清零
		SETS();							//将清零的数据写入24c02
		while(K2==0&&K3==0);			//按键释放
	}
	if(K2 == 0 && bike_set == 0 && Mode == 0){
		ShowTemAndHum();
		while(K2 == 0 && bike_set == 0 && Mode == 0);
		write_1602com(0x80);//显示固定符号从第一行第1个位置之后开始显示
		write_1602string(tab1);
		write_1602com(0x80+0x40);//显示固定符号写入位置，从第2个位置后开始显示
		write_1602string(tab2);
	}
}
void BJ_SAFE()						   //安全速度报警函数
{
	if(Velocity>SAFE_SPEED)			   //当前速度超过安全速度
	{
		BEEP=0;						  //蜂鸣器响
		voice = 0;
	}
	else							  //没超过
	{
		BEEP=1;						 //关闭蜂鸣器
		voice = 1;
	}
}

void main()							 //主函数
{
	//初始化
	Ds1302_Init();			 //时钟芯片初始化
	lcd_init();				 //液晶初始化
	initeeprom();			 //24c02初始化
	READS();					//读取初始参数
	init();						//定时器初始化
	before_sec=time_buf1[6];	//读取到秒数据
	while(1)					//进入循环
	{
		if(Mode==0&&bike_set==0)//不是设置状态时
		{
			display();		   //调用显示函数
			BJ_SAFE();			//判断是否超速
		}
		KEY();				   //扫描按键
	}
}

void EXINT0() interrupt 0		 //外部中断0
{
	count++;					 //霍尔有信号，该变量加1
}

void time0() interrupt 1		//定时器中断0
{
	uchar m,n;
	TH0=0x3c;
	TL0=0xb0;	 //50ms
	m++;
	if(LED_SEC==1)				//秒灯控制
	{
		n++;
		if(n>=10)				//到500ms时
		{
			n=0;
			LED_SEC=0;			//熄灭秒灯
		}
	}
	
	if(m>=10)					 //500ms时计算一次里程速度信息
	{
		m=0;
		Mileage=Mileage+10*(Velocity/3.6)/2;		 //里程m=里程+速度km/h/3.6/2
		Velocity=count *2*3.14*RADIUS /100000*2*3600  /40;//将500ms的距离经过运算得到km/h，将速度/100，方便显示
		count=0;									  //清零
	}
}


//读初值
void READS()
{
	uchar Mileage_H,Mileage_M,Mileage_L;   //定义将里程拆开得到的三个数据
	delay(10);
	RADIUS=read_add(0x01);				   //读取半径信息
	delay(10);							   //延时的作用是读写数据更稳定
	SAFE_SPEED=read_add(0x02);			   //读取安全速度信息

	delay(10);
	Mileage_H=read_add(0x03);			   //读取里程数据的高位
	delay(10);
	Mileage_M=read_add(0x04);			   //读取里程数据的中间位
	delay(10);
	Mileage_L=read_add(0x05);			   //读取里程数据的低位

	Mileage=Mileage_H*100000+Mileage_M*1000+Mileage_L*10;//将三个数据组合成里程数据
}


//写初值
void SETS()
{
	delay(10);
	write_add(0x01,RADIUS);				//将半径数据写入24c02
	delay(10);
	write_add(0x02,SAFE_SPEED);			//将安全速度信息写入

/*	Mileage_H=Mileage/10000;			 //123.4560
	Mileage_M=Mileage%10000/100;
	Mileage_L=Mileage%10000%100; */
	delay(10);
	write_add(0x03,Mileage/100000);		   //里程数据写入
	delay(10);
	write_add(0x04,Mileage%100000/1000);
	delay(10);
	write_add(0x05,Mileage%100000%1000/10);
}


