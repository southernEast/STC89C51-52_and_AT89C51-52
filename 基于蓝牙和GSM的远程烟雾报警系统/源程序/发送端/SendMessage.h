#ifndef __SENDMESSAGE_H__
#define __SENDMESSAGE_H__

#include <reg52.h>

#define uchar unsigned char
#define uint  unsigned int

sbit led1 = P2^0;
sbit led2 = P2^2;
sbit gasDet = P2^1;

extern void Delay(uint k);
extern void Init(void);
extern void Send(uchar ShowData);
extern void Gas();


#endif