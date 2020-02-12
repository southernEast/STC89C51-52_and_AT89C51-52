#include "ReceiveMessage.h"

void main(void)
{

	Init();					// 串口初始化
	
	while(1)
	{
		receive();		// 接收信息
		delay(100);
		if(select()){	// 判断是否发生火灾
			EA = 0;			// 关闭总中断
			beep = 0;		// 开启蜂鸣器
			led = 0;		// 开启应急灯
			delay(5000);
			EA = 1;			// 开启总中断
		}
		else{
			beep = 1;		// 关闭蜂鸣器
			led = 1;		// 关闭应急灯
		}
	}
}
