#include "cars.h"
#include "GSM.h"

extern uint num;

void main()
{
	Init_LCD1602();		// 初始化LCD1602
	GSM_init();				// 初始化GSM
	num = 0;					// 初始化车辆计数器
	
	while(1){
		showInfo();			// 显示信息
		GSM_scan();			// 扫描GSM接口 是否接收到信息
	}
}