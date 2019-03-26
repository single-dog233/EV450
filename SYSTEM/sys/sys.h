#ifndef __SYS_H
#define __SYS_H	 
#include "stm32f4xx.h" 
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"  
#include "dma.h"
#include "string.h"
#include "relay_control.h"
#include "wifi.h"
#include "74hc4051.h"
#include "ads1256.h"
#include "stm32f4xx_spi.h"
#include "stm32f4xx_tim.h"
#include "PWM_CHECK.h"

//以下为汇编函数
void WFI_SET(void);		//执行WFI指令
void INTX_DISABLE(void);//关闭所有中断
void INTX_ENABLE(void);	//开启所有中断
void MSR_MSP(u32 addr);	//设置堆栈地址 
#endif











