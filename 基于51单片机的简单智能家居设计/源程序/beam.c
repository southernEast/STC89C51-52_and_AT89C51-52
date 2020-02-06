#include "beam.h"
#include "LCD1602.h"

uchar BUF[8];                         //接收数据缓存区      	
uchar ge,shi,bai,qian,wan;            //显示变量
int dis_data;                       //变量

void conversion(uint temp_data)  //  数据转换出 个，十，百，千，万
{  
    wan=temp_data/10000+0x30 ;
    temp_data=temp_data%10000;   //取余运算
		qian=temp_data/1000+0x30 ;
    temp_data=temp_data%1000;    //取余运算
    bai=temp_data/100+0x30   ;
    temp_data=temp_data%100;     //取余运算
    shi=temp_data/10+0x30    ;
    temp_data=temp_data%10;      //取余运算
    ge=temp_data+0x30; 	
}

//毫秒延时**************************
void delay_nms(unsigned int k)	
{						
	unsigned int i,j;				
	for(i=0;i<k;i++)
	{			
		for(j=0;j<121;j++)			
			;
	}						
}			

/**************************************
延时5微秒(STC90C52RC@12M)
不同的工作环境,需要调整此函数，注意时钟过快时需要修改
当改用1T的MCU时,请调整此延时函数
**************************************/
void Delay5us()
{
	_nop_();_nop_();_nop_();_nop_();
	_nop_();_nop_();_nop_();_nop_();
	_nop_();_nop_();_nop_();_nop_();
	_nop_();_nop_();_nop_();_nop_();
}

/**************************************
延时5毫秒(STC90C52RC@12M)
不同的工作环境,需要调整此函数
当改用1T的MCU时,请调整此延时函数
**************************************/
void Delay5ms()
{
    uint n = 560;

    while (n--);
}

/**************************************
起始信号
**************************************/
void BH1750_Start()
{
    SDA = 1;                    //拉高数据线
    SCL = 1;                    //拉高时钟线
    Delay5us();                 //延时
    SDA = 0;                    //产生下降沿
    Delay5us();                 //延时
    SCL = 0;                    //拉低时钟线
}

/**************************************
停止信号
**************************************/
void BH1750_Stop()
{
    SDA = 0;                    //拉低数据线
    SCL = 1;                    //拉高时钟线
    Delay5us();                 //延时
    SDA = 1;                    //产生上升沿
    Delay5us();                 //延时
}

/**************************************
发送应答信号
入口参数:ack (0:ACK 1:NAK)
**************************************/
void BH1750_SendACK(bit ack)
{
    SDA = ack;                  //写应答信号
    SCL = 1;                    //拉高时钟线
    Delay5us();                 //延时
    SCL = 0;                    //拉低时钟线
    Delay5us();                 //延时
}

/**************************************
接收应答信号
**************************************/
bit BH1750_RecvACK()
{
    SCL = 1;                    //拉高时钟线
    Delay5us();                 //延时
    CY = SDA;                   //读应答信号
    SCL = 0;                    //拉低时钟线
    Delay5us();                 //延时

    return CY;
}

/**************************************
向IIC总线发送一个字节数据
**************************************/
void BH1750_SendByte(uchar dat)
{
    uchar i;

    for (i=0; i<8; i++)         //8位计数器
    {
        dat <<= 1;              //移出数据的最高位
        SDA = CY;               //送数据口
        SCL = 1;                //拉高时钟线
        Delay5us();             //延时
        SCL = 0;                //拉低时钟线
        Delay5us();             //延时
    }
    BH1750_RecvACK();
}

/**************************************
从IIC总线接收一个字节数据
**************************************/
uchar BH1750_RecvByte()
{
    uchar i;
    uchar dat = 0;

    SDA = 1;                    //使能内部上拉,准备读取数据,
    for (i=0; i<8; i++)         //8位计数器
    {
        dat <<= 1;
        SCL = 1;                //拉高时钟线
        Delay5us();             //延时
        dat |= SDA;             //读数据               
        SCL = 0;                //拉低时钟线
        Delay5us();             //延时
    }
    return dat;
}

//*********************************

void Single_Write_BH1750(uchar REG_Address)
{
    BH1750_Start();                  //起始信号
    BH1750_SendByte(SlaveAddress);   //发送设备地址+写信号
    BH1750_SendByte(REG_Address);    //内部寄存器地址，
  //  BH1750_SendByte(REG_data);       //内部寄存器数据，
    BH1750_Stop();                   //发送停止信号
}

//********单字节读取*****************************************
/*
uchar Single_Read_BH1750(uchar REG_Address)
{  uchar REG_data;
    BH1750_Start();                          //起始信号
    BH1750_SendByte(SlaveAddress);           //发送设备地址+写信号
    BH1750_SendByte(REG_Address);                   //发送存储单元地址，从0开始	
    BH1750_Start();                          //起始信号
    BH1750_SendByte(SlaveAddress+1);         //发送设备地址+读信号
    REG_data=BH1750_RecvByte();              //读出寄存器数据
	BH1750_SendACK(1);   
	BH1750_Stop();                           //停止信号
    return REG_data; 
}
*/
//*********************************************************
//
//连续读出BH1750内部数据
//
//*********************************************************
void Multiple_read_BH1750(void)
{  
	 uchar i;	
   BH1750_Start();                          //起始信号
   BH1750_SendByte(SlaveAddress+1);         //发送设备地址+读信号
	
	 for (i=0; i<3; i++)                      //连续读取2个地址数据，存储中BUF
   {
			BUF[i] = BH1750_RecvByte();          //BUF[0]存储0x32地址中的数据
			if (i == 3)
			{

				 BH1750_SendACK(1);                //最后一个数据需要回NOACK
			}
			else
			{		
				BH1750_SendACK(0);                //回应ACK
			}
   }

    BH1750_Stop();                          //停止信号
    Delay5ms();
}


//初始化BH1750，根据需要请参考pdf进行修改****
void Init_BH1750()
{
   Single_Write_BH1750(0x01);  
}

void Show_Beam()
{
	float temp;
	Single_Write_BH1750(0x01);   // power on
	Single_Write_BH1750(0x10);   // H- resolution mode
	delay_nms(180);              //延时180ms
	Multiple_Read_BH1750();       //连续读出数据，存储在BUF中
	dis_data=BUF[0];
	dis_data=(dis_data<<8)+BUF[1];//合成数据，即光照数据
	temp=(float)dis_data/1.2;
	conversion(temp);         //计算数据和显示
	LCD1602_write_com(0x80);
	LCD1602_write_word("Light: ");
	LCD1602_write_data(wan);
	LCD1602_write_data(qian);
	LCD1602_write_data(bai);
	LCD1602_write_data(shi);
	LCD1602_write_data(ge);
	LCD1602_write_word("  lx");
}