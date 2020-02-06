#include "GSM.h"
#include "LCD1602.h"
#include "BUZZ.h"
#include "beam.h"
#include "TemAndHum.h"
#include "Gas.h"

int sendFlag = 0;

void main(void)
{
	u8 i;
	int ret;
	Int0Init();			// 初始化外部中断0
	Int1Init();			// 初始化外部中断1
	Init_BH1750();
	Init_LCD1602();
	Uart1Init();   	// 串口初始化
	Timer0Init();		// TIM0初始化
	
	for(i = 0;i < STABLE_TIMES;i++){ 				//等待网络稳定
		delay_ms(50);
	}
	LCD1602_write_com(0x80);
	LCD1602_write_word("Welcome to use! ");
	delay_ms(50);
	while(1){
		Show_Beam();												// 显示光照度
		ShowTemAndHum();										// 显示温湿度
		delay_ms(10);	
		if(sendFlag == 1){
			LCD1602_write_com(0x80);
			LCD1602_write_word("Sending message!");
			LCD1602_write_com(0x80 + 0x40);
			LCD1602_write_word("                ");
			ret = send_text_message("Attention the fire!");// 发送TEXT短信
			if(ret == 1){
				LCD1602_write_com(0x80);
				LCD1602_write_word("Send Successful!");
				LCD1602_write_com(0x80 + 0x40);
				LCD1602_write_word("                ");
				delay_ms(50);
			}
			else{
				LCD1602_write_com(0x80 + 0x40);
				LCD1602_write_word("      Fail      ");
			}	
			sendFlag = 0;
		}
	}
}



