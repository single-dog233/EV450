#ifndef _RELAY_CONTROL_H
#define _RELAY_CONTROL_H
#include "sys.h"


#define GPIO_SetBits(port, pin) GPIO_SetBits(port, pin)
#define GPIO_ResetBits(port, pin) GPIO_ResetBits(port,	pin)
#define data_io_num 8 + 1
#define CHIP_74HC573_NUM 5 + 1
#define RELAY_NUM 40 + 1

typedef struct data_io_infomation{
	GPIO_TypeDef * port;
	int pin;
}data_io;


typedef struct chip_infomation{
	char lastValue;
	GPIO_TypeDef * port;
	int pin;
}chip_info;

typedef struct relay_info{
	chip_info * chip_info_data;
	data_io * data_io_info;
}relay_info;

extern unsigned char saveBak[80];

void chip_74hc573_init(void);
void chip_74hc573_send_data(int chip_num, int data);


#endif


