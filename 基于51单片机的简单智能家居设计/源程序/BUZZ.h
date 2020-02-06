#ifndef __BUZZ_H__
#define __BUZZ_H__

#include<reg52.h>

sbit BEEP=P2^4;

extern void Delay(int i);
extern void Ring();

#endif