#ifndef __GSM_H__
#define __GSM_H__

#include<reg52.h>
#include "string.h"

typedef 	unsigned char	u8;
typedef 	unsigned int	u16;
typedef 	unsigned long	u32;

#define COMMUNITE_ERROR       -1 
#define NO_SIM_CARD_ERROR     -2
#define SIM_CARD_NO_REG_ERROR -3
#define CALL_ERROR			  -4
#define AT_CPMS_ERROR		  -5
#define AT_CMGF_ERROR		  -6
#define AT_CSCS_ERROR		  -7
#define AT_CMGS_ERROR         -8
#define END_CHAR_ERROR		  -9
#define AT_CSMP_ERROR		  -10

#define Buf_Max 	 	  70 	//串口1缓存长度
#define STABLE_TIMES	  7	//等待上电后稳定


//串口1发送回车换行
#define UART1_SendLR() UART1_SendData(0X0D)

extern void delay_ms(u16 ms);   //延时ms个20ms  ms*20ms
extern void CLR_Buf(void);      //清串口接收缓存
extern u8 Find(u8 *a);        //查找字符串
extern void UART1_SendData(u8 dat);   //串口1发送 1字节
extern void UART1_SendString(char *s);//串口1发送 字符串
extern void UART1_Send_Command(char *s);
extern void UART1_Send_Command_END(char *s);
extern u8 UART1_Send_AT_Command(u8 *b,u8 *a,u8 wait_time,u16 interval_time);//at命令发送
extern u8 UART1_Send_AT_Command_END(u8 *b,u8 *a,u8 wait_time,u16 interval_time);
extern int check_status(void);
extern u8 Wait_CREG(u8 query_times);

extern int send_text_message(char *content);
extern void Timer0Init(void);
extern void Uart1Init(void);

#endif