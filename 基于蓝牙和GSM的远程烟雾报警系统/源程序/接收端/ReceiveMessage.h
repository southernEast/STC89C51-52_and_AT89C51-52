#ifndef __RECEIVEMESSAGE_H__
#define __RECEIVEMESSAGE_H__

#include <reg52.h>

#define uchar unsigned char
#define uint  unsigned int

sbit beep = P2^0;
sbit led = P2^1;

extern void Init(void);
extern void receive();
extern int select();
extern void delay(unsigned int i);
extern void send(uchar ShowData);

#endif