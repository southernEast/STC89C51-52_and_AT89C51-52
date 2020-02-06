#include "Gas.h"
#include "BUZZ.h"
#include "LCD1602.h"

void Gas_Delay(uint i)
{
	while(i--);
}

void Int0Init()
{
	IT0 = 1;
	EX0 = 1;
	EA = 1;
}

void Int1Init()
{
	IT1 = 1;
	EX1 = 1;
	EA = 1;
}

void Int0() interrupt 0
{
	Gas_Delay(1000);
	if(DOUT0 == 0){
		Ring();													// ·äÃùÆ÷Ïì
		sendFlag = 1;
	}	
}

void Init1() interrupt 2
{
	Gas_Delay(1000);
	if(DOUT1 == 0){
		Ring();
		sendFlag = 1;
	}
}