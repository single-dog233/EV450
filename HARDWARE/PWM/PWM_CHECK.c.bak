#include "PWM_CHECK.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stdio.h"

void EXTI0_IRQHandler(void)
{
	 delay_ms(10);	//消抖
	 printf("exit0 int \r\n");
	 EXTI_ClearITPendingBit(EXTI_Line0); //清除LINE0上的中断标志位 
}	
//外部中断2服务程序
void EXTI1_IRQHandler(void)
{
	 delay_ms(10);	//消抖
	 printf("exit1 int \r\n");
	 EXTI_ClearITPendingBit(EXTI_Line1);//清除LINE2上的中断标志位 
}
/*
*	all Period is 50000 Period time = 50000 / (84000000 / 840)
*/
void TIM2_Init()
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE); 
  TIM_TimeBaseInitStructure.TIM_Period = 50000-1; 	//
	TIM_TimeBaseInitStructure.TIM_Prescaler=0;  
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStructure);
	TIM_ARRPreloadConfig(TIM2, ENABLE);
	TIM_Cmd(TIM2,DISABLE);
}

//ADA31 PB0 ADB31 PB1
//ADA32 PC0 ADB32 PC1
void PWM_Gpio_Init(void)
{
	GPIO_InitTypeDef	GPIO_InitStructure;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC, ENABLE);	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	TIM2_Init();
}
int high, all;
float READ_Pwm_chanel(GPIO_TypeDef * GPIOx, uint16_t GPIO_Pin)
{ 
	int x = 600;
	TIM_Cmd(TIM2,DISABLE);
	TIM_SetCounter(TIM2, 0);
	while(GPIO_ReadInputDataBit(GPIOx, GPIO_Pin));//过滤掉不完整的波
	while(!GPIO_ReadInputDataBit(GPIOx, GPIO_Pin));//高电平触发采集
	TIM_Cmd(TIM2,ENABLE);
	while(GPIO_ReadInputDataBit(GPIOx, GPIO_Pin));
	high = TIM_GetCounter(TIM2)+x;
	while(!GPIO_ReadInputDataBit(GPIOx, GPIO_Pin));
	all = TIM_GetCounter(TIM2);
	return high/all;
}
