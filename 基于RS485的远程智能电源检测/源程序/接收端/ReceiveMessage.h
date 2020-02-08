#ifndef __RECEIVEMESSAGE_H__
#define __RECEIVEMESSAGE_H__

#include <reg52.h>
#include <string.h>
#include "LCD1602.h"

sbit  RE = P1^1;					// 485Òý½Å
sbit  DE = P1^2;

extern void RS485Init();
extern void Init(void);
extern void receiveString1();
extern void receiveString2();
extern void show();

#endif