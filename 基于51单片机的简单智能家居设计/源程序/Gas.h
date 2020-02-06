#ifndef __GAS_H__
#define __GAS_H__

#include <reg52.h>

#define uchar unsigned char 
#define uint unsigned int

sbit DOUT0 = P3^2;	
sbit DOUT1 = P3^3;

extern int sendFlag;
extern void Int0Init();
extern void Gas_Delay(uint i);
extern void Int1Init();

#endif