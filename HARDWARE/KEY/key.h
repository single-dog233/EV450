#ifndef __SWITCH_H
#define __SWITCH_H	 
#include "sys.h" 
#define switch_vlaue_ret() (0xf&GPIO_ReadInputData(GPIOD))
void switch_Init(void);	//IO≥ı ºªØ

#endif
