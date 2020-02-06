#ifndef __MAIN_H__
#define __MAIN_H__

#include <reg52.h>

#define uchar unsigned char
#define uint unsigned int
sbit play_01 = P1^0;
sbit play_02 = P1^1;
sbit play_03 = P1^2;
sbit io = P2^4;
sbit Buzzer = P2^4;

//按键 IO设置
sbit KEY1 = P2^3;
sbit KEY2 = P2^2;
sbit KEY3 = P2^1;
sbit KEY4 = P2^0;

//函数或者变量声明
extern void Delay_ms(unsigned int n);
extern unsigned char Scan_Key();
extern void Buzzer_Di();
extern void Get_Maopi();
extern void Get_Weight();

extern void delay(uchar ms);
extern void delay1();
extern void start();
extern uchar receive_byte();
extern void receive();
extern void display_tem();
extern void play_record(uchar index);

extern unsigned char KEY_NUM;

extern uchar data_byte; 
extern uchar RH,RL,TH,TL; 

#endif