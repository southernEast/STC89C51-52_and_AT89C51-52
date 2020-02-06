#include "BUZZ.h"

void Delay(int i)
{
	while(i--);
}

void Ring()
{
	int i;
	for(i = 0; i < 100; i++){
		BEEP = 0;
		Delay(100);
		BEEP = 1;
	}
}
