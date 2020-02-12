#include "SendMessage.h"


void main(void)
{
    Init();				// 定时器和串口初始化
 
    while(1)
    {
        Gas();		// 检测是否有烟雾
        Delay(1000);
    }
}