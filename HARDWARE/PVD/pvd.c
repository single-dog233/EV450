#include "PVD.h"
#include "24cxx.h" 
void PVD_Init(void)
{
	EXTI_InitTypeDef EXTI_InitStructure; 
	NVIC_InitTypeDef NVIC_InitStructure; 
	PWR_DeInit();
RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);//ʹ��PWRʱ��

NVIC_InitStructure.NVIC_IRQChannel = PVD_IRQn;           //ʹ��PVD���ڵ��ⲿ�ж�ͨ��
NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;//��ռ���ȼ�1
NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;       //�����ȼ�0
NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;          //ʹ���ⲿ�ж�ͨ��
NVIC_Init(&NVIC_InitStructure);

EXTI_StructInit(&EXTI_InitStructure);
EXTI_InitStructure.EXTI_Line = EXTI_Line16;             //PVD���ӵ��ж���16��
EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;     //ʹ���ж�ģʽ
EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  //��ѹ���ڷ�ֵʱ�����ж�
EXTI_InitStructure.EXTI_LineCmd = ENABLE;               //ʹ���ж���
EXTI_Init(&EXTI_InitStructure);                         //��ʼ

PWR_PVDLevelConfig(PWR_PVDLevel_7);//�趨��ط�ֵ   ע���F1������  F1�ĵ�ѹ�ȼ��͸�ֱ��   PWR_PVDLevel_2V8 
PWR_PVDCmd(ENABLE);//ʹ��PVD  


		
}		
extern unsigned char saveBak[80];
void  PVD_IRQHandler(void)  
{
		PWR_ClearFlag(PWR_FLAG_PVDO);
		EXTI_ClearITPendingBit(EXTI_Line16);
	AT24CXX_Write(0,(u8*)saveBak,80);	
}
