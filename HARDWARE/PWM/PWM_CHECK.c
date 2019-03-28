#include "PWM_CHECK.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stdio.h"
extern frame_send * frame_s;
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
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE); 
  TIM_TimeBaseInitStructure.TIM_Period = 50000-1; 	//
	TIM_TimeBaseInitStructure.TIM_Prescaler=0;  
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStructure);
	TIM_ARRPreloadConfig(TIM2, ENABLE);
	TIM_Cmd(TIM2,ENABLE);
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM2_IRQn; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; //抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03; //子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
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
int high, all, flag;
float READ_Pwm_chanel(GPIO_TypeDef * GPIOx, uint16_t GPIO_Pin)
{ 
	int x = 600;
	TIM_Cmd(TIM2,ENABLE);
	flag = 1;
	TIM2->CNT = 0;
	while(flag && (GPIOx->IDR & GPIO_Pin));//过滤掉不完整的波
	while(flag && (!(GPIOx->IDR & GPIO_Pin)));//高电平触发采集
	TIM2->CNT = 0;
	while(flag && (GPIOx->IDR & GPIO_Pin));
	high = TIM2->CNT+x;
	while(flag && (!(GPIOx->IDR & GPIO_Pin)));
	all = TIM2->CNT;
	if(all == 0)
	{
		high = 0;
		all = 1;
	}
	return (float)high*109/all;
}

void pwm_get(void)
{
	frame_s->data[30] = (char)READ_Pwm_chanel(GPIOB, GPIO_Pin_0);//31 - 50
	frame_s->data[31] = (char)READ_Pwm_chanel(GPIOC, GPIO_Pin_0);//32 - 49
	frame_s->data[49] = (char)READ_Pwm_chanel(GPIOB, GPIO_Pin_1);
	frame_s->data[48] = (char)READ_Pwm_chanel(GPIOC, GPIO_Pin_1);
}

void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2,TIM_IT_Update)==SET) //溢出中断
	{
		flag = 0; 
	}
	TIM_ClearITPendingBit(TIM2,TIM_IT_Update);  //清除中断标志位
	TIM_Cmd(TIM2,DISABLE);
}
