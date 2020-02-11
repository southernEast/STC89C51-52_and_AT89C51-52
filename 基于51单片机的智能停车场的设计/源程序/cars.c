#include "cars.h"


/**统计车辆**/
uchar outBuf[16];				// 输出字符串数组
uchar parkingFlag[4];		// 四个停车位标志
uint num;								// 车辆数目计数

void cars_delay(uint i)
{
	while(i--);
}

/**统计当前停车场车辆数目**/
void countNum()
{
	if(inDetect == 0){		// 进入车辆
		while(!inDetect)
			;
		num++;
	}
	if(outDetect == 0){		// 离开车辆
		while(!outDetect)
			;
		if(num > 0)
			num--;
	}
}

/**扫描车位**/
void parkingPlaceScan()
{
	if(parking0 == 0){			// 检测到车辆信号
			cars_delay(100);
			if(parking0 == 0){
				parkingFlag[0] = 'Y';
			}
		}
		else{									// 无车辆信号
			parkingFlag[0] = 'N';
		}
		if(parking1 == 0){
			cars_delay(100);
			if(parking1 == 0){
				parkingFlag[1] = 'Y';
			}
		}
		else{
			parkingFlag[1] = 'N';
		}
		if(parking2 == 0){
			cars_delay(100);
			if(parking2 == 0){
				parkingFlag[2] = 'Y';
			}
		}
		else{
			parkingFlag[2] = 'N';
		}
		if(parking3 == 0){
			cars_delay(100);
			if(parking3 == 0){
				parkingFlag[3] = 'Y';
			}
		}
		else{
			parkingFlag[3] = 'N';
		}
}

void showInfo()
{
	sprintf(outBuf, "%c %c %c %c  %4d   ", 
					parkingFlag[0], parkingFlag[1], parkingFlag[2], parkingFlag[3], num);			// 将需要显示的信息存入字符数组
	parkingPlaceScan();							// 扫描车位信息
	LCD1602_write_com(0x80);
	LCD1602_write_word("1 2 3 4 NOW NUM:");	// LCD显示第一行
	LCD1602_write_com(0xc0);
	LCD1602_write_word(outBuf);			// LCD显示第二行
	countNum();

}