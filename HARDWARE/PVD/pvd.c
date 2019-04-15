#include "PVD.h"
#include "24cxx.h" 
void PVD_Init(void)
{
	EXTI_InitTypeDef EXTI_InitStructure; 
	NVIC_InitTypeDef NVIC_InitStructure; 
	PWR_DeInit();
RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);//使能PWR时钟

NVIC_InitStructure.NVIC_IRQChannel = PVD_IRQn;           //使能PVD所在的外部中断通道
NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;//抢占优先级1
NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;       //子优先级0
NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;          //使能外部中断通道
NVIC_Init(&NVIC_InitStructure);

EXTI_StructInit(&EXTI_InitStructure);
EXTI_InitStructure.EXTI_Line = EXTI_Line16;             //PVD连接到中断线16上
EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;     //使用中断模式
EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  //电压低于阀值时产生中断
EXTI_InitStructure.EXTI_LineCmd = ENABLE;               //使能中断线
EXTI_Init(&EXTI_InitStructure);                         //初始

PWR_PVDLevelConfig(PWR_PVDLevel_7);//设定监控阀值   注意和F1的区别  F1的电压等级就更直观   PWR_PVDLevel_2V8 
PWR_PVDCmd(ENABLE);//使能PVD  


		
}		
extern unsigned char saveBak[80];
void  PVD_IRQHandler(void)  
{
		PWR_ClearFlag(PWR_FLAG_PVDO);
		EXTI_ClearITPendingBit(EXTI_Line16);
	AT24CXX_Write(0,(u8*)saveBak,80);	
}
