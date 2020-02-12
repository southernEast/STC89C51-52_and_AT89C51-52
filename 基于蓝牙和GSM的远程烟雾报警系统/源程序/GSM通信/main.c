#include "gsm.h"

sbit gas = P1^0;
sbit led1 = P1^1;
sbit led2 = P1^2;
sbit beep = P0^0;

void main()
{
	Timer0Init();								// 定时器初始化
	Uart1Init();								// 串口初始化
	
	while(1){
		if(gas == 0){							// 如果检测到烟雾
			delay_ms(10);
			if(gas == 0){
				led1 = 0;							// 点亮应急灯
				led2 = 1;
				beep = 0;
				send_text_message("Attention the fire!");
				delay_ms(250);				// 5s内只触发一次
			}
		}
		led1 = 1;
		led2 = 0;
		beep = 1;
	}
}