#include "main.h"
#include "LCD1602.h"
#include "HX711.h"
#include "EEPROM.H"

//定义变量
unsigned char KEY_NUM = 0;   //用来存放按键按下的键值
unsigned long HX711_Buffer = 0;  //用来存放HX711读取出来的数据
unsigned long Weight_Maopi = 0; //用来存放毛皮数据
long Weight_Shiwu = 0;          //用来存放实物重量
long Max_Value = 1;             //用来存放设置最大值
unsigned int maxValueTable[6] = {0};
unsigned char state = 0;    //用来存放设置状态
unsigned char Blink_Speed = 0;
#define Blink_Speed_Max 6          //该值可以改变设置指针闪烁频率

#define GapValue 24.5

//报警值，单位是g
#define AlarmValue 200000			



//****************************************************
//主函数
//****************************************************
void main()
{
	Init_LCD1602();								//初始化LCD1602

	LCD1602_write_com(0x80);			//设置LCD1602指针 初始化显示
	LCD1602_write_word("Welcome to use! ");
	LCD1602_write_com(0x80 + 0x40);	
	LCD1602_write_word("Designed by LiuS");
	play_record(3);

	
	Get_Maopi();
	Get_Maopi();
	Delay_ms(2000);		 						//延时2s
	Get_Maopi();
	Get_Maopi();									//称毛皮重  多次测量有利于HX711稳定
  LCD1602_write_com(0x01);    	//清屏
    
	//读取EEPROM中保存的报警值
	maxValueTable[0] = byte_read(0x2000);
	maxValueTable[1] = byte_read(0x2001);
	maxValueTable[2] = byte_read(0x2002);
	maxValueTable[3] = byte_read(0x2003);
	maxValueTable[4] = byte_read(0x2004);
	maxValueTable[5] = byte_read(0x2005);
	Max_Value = (long)maxValueTable[0]*100000+(long)maxValueTable[1]*10000+(long)maxValueTable[2]*1000+(long)maxValueTable[3]*100+(long)maxValueTable[4]*10+(long)maxValueTable[5]; 
    
	while(1)
	{	
    Get_Weight();		
		receive();									// 接收温湿度传感器信号
		//显示当前重量
		LCD1602_write_com(0x80);
    LCD1602_write_word("Weight=");
    LCD1602_write_data(Weight_Shiwu/100000 + 0x30);
		LCD1602_write_data(Weight_Shiwu%100000/10000 + 0x30);
		LCD1602_write_data(Weight_Shiwu%10000/1000 + 0x30);
		LCD1602_write_data('.');
		LCD1602_write_data(Weight_Shiwu%1000/100 + 0x30);
 		LCD1602_write_data(Weight_Shiwu%100/10 + 0x30);
		LCD1602_write_data(Weight_Shiwu%10 + 0x30);	       
		LCD1602_write_word("Kg");

		KEY_NUM = Scan_Key();		// 按键扫描
		if(KEY_NUM == 1)        // 按键1切换设置状态
		{
				state++;
				if(state == 7)
				{
						state = 0;
						SectorErase(0x2000);
						byte_write(0x2000,maxValueTable[0]);				//保存EEPROM数据
						byte_write(0x2001,maxValueTable[1]);
						byte_write(0x2002,maxValueTable[2]);				//保存EEPROM数据
						byte_write(0x2003,maxValueTable[3]);
						byte_write(0x2004,maxValueTable[4]);
						byte_write(0x2005,maxValueTable[5]);
						Max_Value = (long)maxValueTable[0]*100000+(long)maxValueTable[1]*10000+(long)maxValueTable[2]*1000+(long)maxValueTable[3]*100+(long)maxValueTable[4]*10+(long)maxValueTable[5];     //计算超限报警界限值				
				}
		}
		if(KEY_NUM == 2)        //按键加
		{
				if(state != 0)
				{
						maxValueTable[state-1]++;
						if(maxValueTable[state-1] > 9)
						{
								maxValueTable[state-1] = 0;
						}
				}
				
		}
		if(KEY_NUM == 3)        //按键减
		{
				if(state != 0)
				{
						maxValueTable[state-1]--;
						if(maxValueTable[state-1] < 0)
						{
								maxValueTable[state-1] = 9;
						}
				}
		}
		if(KEY_NUM == 4 && Weight_Shiwu != 0) 						// 乘客下车且有行李
		{
			play_record(1);	
		}
          
		if(state != 0)					// 调整模式
		{
			Blink_Speed++;
			if(Blink_Speed == Blink_Speed_Max)
			{
					Blink_Speed = 0;
			}
		}
		switch(state)
		{
				case 0:
						LCD1602_write_com(0x80+0x40);
						LCD1602_write_word("M");
						LCD1602_write_data('0' + maxValueTable[0]);
						LCD1602_write_data('0' + maxValueTable[1]);             
						LCD1602_write_data('0' + maxValueTable[2]);
						LCD1602_write_data('.');
						LCD1602_write_data('0' + maxValueTable[3]);
						LCD1602_write_data('0' + maxValueTable[4]);
						LCD1602_write_data('0' + maxValueTable[5]);
						display_tem();
						break;
				case 1:
						LCD1602_write_com(0x80+0x40);
						LCD1602_write_word("M");
						if(Blink_Speed < Blink_Speed_Max/2)
						{
								LCD1602_write_data('0' + maxValueTable[0]);
						}
						else
						{
								LCD1602_write_data(' ');
						}
						LCD1602_write_data('0' + maxValueTable[1]);             
						LCD1602_write_data('0' + maxValueTable[2]);
						LCD1602_write_data('.');
						LCD1602_write_data('0' + maxValueTable[3]);
						LCD1602_write_data('0' + maxValueTable[4]);
						LCD1602_write_data('0' + maxValueTable[5]);
						display_tem();
						break;
				case 2:
						LCD1602_write_com(0x80+0x40);
						LCD1602_write_word("M");               
						LCD1602_write_data('0' + maxValueTable[0]);
						if(Blink_Speed < Blink_Speed_Max/2)
						{
							LCD1602_write_data('0' + maxValueTable[1]);             
						}
						else
						{
								LCD1602_write_data(' ');
						}
						LCD1602_write_data('0' + maxValueTable[2]);
						LCD1602_write_data('.');               
						LCD1602_write_data('0' + maxValueTable[3]);
						LCD1602_write_data('0' + maxValueTable[4]);
						LCD1602_write_data('0' + maxValueTable[5]);
						display_tem();
						break;
				case 3:
						LCD1602_write_com(0x80+0x40);
						LCD1602_write_word("M");
						LCD1602_write_data('0' + maxValueTable[0]);
						LCD1602_write_data('0' + maxValueTable[1]);              
						if(Blink_Speed < Blink_Speed_Max/2)
						{
								LCD1602_write_data('0' + maxValueTable[2]);
						}
						else
						{
								LCD1602_write_data(' ');
						}
						LCD1602_write_data('.');
						LCD1602_write_data('0' + maxValueTable[3]);
						LCD1602_write_data('0' + maxValueTable[4]);
						LCD1602_write_data('0' + maxValueTable[5]);
						display_tem();
						break;
				case 4:
						LCD1602_write_com(0x80+0x40);
						LCD1602_write_word("M");
						LCD1602_write_data('0' + maxValueTable[0]);
						LCD1602_write_data('0' + maxValueTable[1]); 
						LCD1602_write_data('0' + maxValueTable[2]);
						LCD1602_write_data('.');
						if(Blink_Speed < Blink_Speed_Max/2)
						{
								LCD1602_write_data('0' + maxValueTable[3]);
						}
						else
						{
								LCD1602_write_data(' ');
						}                
						LCD1602_write_data('0' + maxValueTable[4]);
						LCD1602_write_data('0' + maxValueTable[5]);
						display_tem();
						break;
				case 5:
						LCD1602_write_com(0x80+0x40);
						LCD1602_write_word("M");
						LCD1602_write_data('0' + maxValueTable[0]);
						LCD1602_write_data('0' + maxValueTable[1]); 
						LCD1602_write_data('0' + maxValueTable[2]);
						LCD1602_write_data('.');
						LCD1602_write_data('0' + maxValueTable[3]);
						if(Blink_Speed < Blink_Speed_Max/2)
						{
								LCD1602_write_data('0' + maxValueTable[4]);
						}
						else
						{
								LCD1602_write_data(' ');
						} 
						LCD1602_write_data('0' + maxValueTable[5]);
						display_tem();
						break;
				case 6:
						LCD1602_write_com(0x80+0x40);
						LCD1602_write_word("M");
						LCD1602_write_data('0' + maxValueTable[0]);
						LCD1602_write_data('0' + maxValueTable[1]); 
						LCD1602_write_data('0' + maxValueTable[2]);
						LCD1602_write_data('.');
						LCD1602_write_data('0' + maxValueTable[3]);
						LCD1602_write_data('0' + maxValueTable[4]);
						if(Blink_Speed < Blink_Speed_Max/2)
						{
								LCD1602_write_data('0' + maxValueTable[5]);
						}
						else
						{
								LCD1602_write_data(' ');
						}                
						display_tem();
						break;
				default:
						break;
				
		}      
    //超限报警
		if(Weight_Shiwu >= Max_Value || Weight_Shiwu >= AlarmValue)	        //超过设置最大值或者传感器本身量程最大值报警	
		{
			play_record(2);
		}
		else
		{
//			SendString("Noting~\n");
		}
	}
}

//****************************************************
//称重
//****************************************************
void Get_Weight()
{
	Weight_Shiwu = HX711_Read();
	Weight_Shiwu = Weight_Shiwu - Weight_Maopi;		//获取净重
	if(Weight_Shiwu >= 0)			
	{	
		Weight_Shiwu = (unsigned long)((float)Weight_Shiwu/GapValue); 	//计算实物的实际重量
	}
	else
	{
		Weight_Shiwu = 0;
	}
	
}

//****************************************************
//获取毛皮重量
//****************************************************
void Get_Maopi()
{
	Weight_Maopi = HX711_Read();	
} 


//****************************************************
//MS延时函数(12M晶振下测试)
//****************************************************
void Delay_ms(unsigned int n)
{
	unsigned int  i,j;
	for(i=0;i<n;i++)
		for(j=0;j<123;j++);
}



//****************************************************
//按键扫描程序
//****************************************************
unsigned char Scan_Key()
{	
	if( KEY1 == 0 )						//按键扫描
	{
		Delay_ms(10);					//延时去抖
		if( KEY1 == 0 )
		{
			while(KEY1 == 0);			//等待松手
			return 1;
		}
	}
	if( KEY2 == 0 )						//按键扫描
	{
		Delay_ms(10);					//延时去抖
		if( KEY2 == 0 )
		{
			while(KEY2 == 0);			//等待松手
			return 2;
		}
	}
	if( KEY3 == 0 )						//按键扫描
	{
		Delay_ms(10);					//延时去抖
		if( KEY3 == 0 )
		{
			while(KEY3 == 0);			//等待松手
			return 3;
		}
	}
	if( KEY4 == 0 )						//按键扫描
	{
		Delay_ms(10);					//延时去抖
		if( KEY4 == 0 )
		{
			while(KEY4 == 0);			//等待松手
			return 4;
		}
	}
	
  return 0;
}

void play_record(uchar index)
{
	switch(index)
	{
		case 1:
			play_01 = 0;
			break;
		case 2:
			play_02 = 0;
			break;
		case 3:
			play_03 = 0;
			break;
	}
	Delay_ms(1000);
	switch(index)
	{
		case 1:
			play_01 = 1;
			break;
		case 2:
			play_02 = 1;
			break;
		case 3:
			play_03 = 1;
			break;
	}
}

void display_tem()							//显示温度和湿度
{
	LCD1602_write_word("R:");
	LCD1602_write_data(RH/10+0x30); 
	LCD1602_write_data(RH%10+0x30); 	
	LCD1602_write_word("T:");
	LCD1602_write_data(TH/10+0x30); 
	LCD1602_write_data(TH%10+0x30); 
}

void start() 										// dht11 开始信号
{ 
  io=1; 
  delay1(); 
  io=0; 
  delay(25);
  io=1;   
  delay1();
  delay1(); 
  delay1(); 
} 

 

uchar receive_byte()						// 接收一个字节
{ 
  uchar i,temp; 
  for(i=0;i<8;i++) 
  { 
		while(!io);
		delay1(); 
		delay1(); 
		delay1(); 
		temp=0;
		if(io==1) 
		 temp=1;  
		while(io);
		data_byte<<=1;
		data_byte|=temp; 						// 接收8bit数据
  } 

  return data_byte; 
} 

    

void receive()									// 接收数据
{ 
  uchar T_H,T_L,R_H,R_L,check,num_check,i; 
  start();
  io=1;   
  if(!io) 
  {  
		while(!io);
		while(io);
		R_H=receive_byte();						// 湿度高位
		R_L=receive_byte();						// 湿度低位
		T_H=receive_byte();						// 温度高位
		T_L=receive_byte();						// 温度低位
		check=receive_byte(); 				// 校验位
		io=0; 
		for(i=0;i<7;i++)							// 延时50μm
			delay1(); 
		io=1;
		num_check=R_H+R_L+T_H+T_L; 
		if(num_check==check) 					// 判断是否与校验位相同
		{ 
		 RH=R_H; 
		 RL=R_L; 
		 TH=T_H; 
		 TL=T_L; 
		 check=num_check; 
		} 
  } 
} 

 void delay(uchar ms)							// 延时
{ 
  uchar i; 
  while(ms--)                        
	for(i=0;i<100;i++); 
} 

void delay1()										//延时8μm
{ 
  uchar i; 
  for(i=0;i<1;i++); 
} 