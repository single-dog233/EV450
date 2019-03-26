#ifndef _WIFI_H
#define _WIFI_H
#include "dma.h"																	   	  
#include "delay.h"	
#include "usart.h"
#include "relay_control.h"
#define TEXT_TO_SEND 5
typedef struct data_frame{
	char star[2];
	char addr[1];
	char command[1];	
	char data_length[1];
	char channel[1];
	char link[1];
	char end[2];
}data_frame;

typedef struct frame_send
{
	char star[2];
	char addr[1];
	char command[1];
	char data_length[1];
	char data[80];
	char end[2];
}frame_send;

void wifi_init(void);
void uart_send_message(u32 addr, int length);
void uart_send_adc_message(void);
#endif


