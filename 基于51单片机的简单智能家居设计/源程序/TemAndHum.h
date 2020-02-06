#ifndef __TEMANDHUM_H__
#define __TEMANDHUM_H__

#include <reg52.h>

#define uchar unsigned char
#define uint unsigned int

sbit io = P2^1;

extern void start();
extern unsigned char receive_byte();
extern void receive();
extern void delay2(uchar ms);
extern void delay1();
extern void ShowTemAndHum();

#endif