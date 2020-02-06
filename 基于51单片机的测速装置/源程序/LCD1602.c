#include "LCD1602.h"

//自定义字符
uchar code num[]={
						0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x00,//1	  //此处字符是方形的数字，用于和其他数字区分开，用于显示星期
						0x1f,0x01,0x01,0x1f,0x10,0x10,0x1f,0x00,//2
						0x1f,0x01,0x01,0x1f,0x01,0x01,0x1f,0x00,//3	 
						0x11,0x11,0x11,0x1f,0x01,0x01,0x01,0x00,//4
						0x1f,0x10,0x10,0x1f,0x01,0x01,0x1f,0x00,//5	 
						0x1f,0x10,0x10,0x1f,0x11,0x11,0x1f,0x00,//6
						0x1f,0x01,0x01,0x01,0x01,0x01,0x01,0x00,//7 	
};

uchar code tab1[]={"  /  /     :    "}; //14/09/10 16:34 3   	
uchar code tab2[]={"  0.000km 00km/h"};	//000.000km 00km/h
uchar code tab3[]={"Wheel Radius  cm"};	//显示车轮半径设置界面
uchar code tab4[]={"Safe Speed  km/h"};	//显示安全速度设置界面
uchar code tab5[]={"Sec :           "};	//时间设置时，秒的显示界面


void LCD1602_delay(uint x)		//延时函数，大约延时1ms，不精确
{
	uint i,j;			//定义延时变量
	for(i=0;i<x;i++)	//执行x次
	for(j=0;j<110;j++);	//执行110次
}

void write_1602com(uchar com)//****液晶写入指令函数****
{
	rs=0;//数据/指令选择置为指令
	P0=com;//送入数据
	LCD1602_delay(1);
	en=1;//拉高使能端，为制造有效的下降沿做准备
	LCD1602_delay(1);
	en=0;//en由高变低，产生下降沿，液晶执行命令

}


void write_1602dat(uchar dat)//***液晶写入数据函数****
{
	rs=1;//数据/指令选择置为数据
	P0=dat;//送入数据
	LCD1602_delay(1);
	en=1; //en置高电平，为制造下降沿做准备
	LCD1602_delay(1);
	en=0; //en由高变低，产生下降沿，液晶执行命令
}

void write_1602string(uchar *str)
{
	uchar *now = str;
	while(*now){
		write_1602dat(*now);
		now++;
	}
}

//自定义字符集
void Lcd_ram()      
{ 
	uint i,j,k=0,temp=0x40; 
	for(i=0;i<7;i++)
	{
	   for(j=0;j<8;j++)
	   {
	    write_1602com(temp+j);
	    write_1602dat(num[k]);
	    k++;
	   }
	   temp=temp+8;
	}
}

void lcd_init()//***液晶初始化函数****
{
	Lcd_ram();
	write_1602com(0x38);//设置液晶工作模式，意思：16*2行显示，5*7点阵，8位数据
	write_1602com(0x0c);//开显示不显示光标
	write_1602com(0x06);//整屏不移动，光标自动右移
	write_1602com(0x01);//清显示

	write_1602com(0x80);//显示固定符号从第一行第1个位置之后开始显示
	write_1602string(tab1);
	write_1602com(0x80+0x40);//显示固定符号写入位置，从第2个位置后开始显示
	write_1602string(tab2);

}