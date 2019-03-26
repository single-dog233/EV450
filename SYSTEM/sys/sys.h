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

//����Ϊ��ຯ��
void WFI_SET(void);		//ִ��WFIָ��
void INTX_DISABLE(void);//�ر������ж�
void INTX_ENABLE(void);	//���������ж�
void MSR_MSP(u32 addr);	//���ö�ջ��ַ 
#endif










