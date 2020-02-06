#ifndef __TEMANDHUM_H__
#define __TEMANDHUM_H__

#include <reg52.h>
#include "LCD1602.h"

#define uchar unsigned char
#define uint unsigned int

sbit io = P2^2;

extern void DHT11_start();
extern unsigned char receive_byte();
extern void receive();
extern void delay2(uchar ms);
extern void delay1();
extern void ShowTemAndHum();

#endif