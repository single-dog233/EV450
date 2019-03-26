#include "sys.h"
int switch_vlaue;
#define ADD1 (GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_1)<<0) 
#define ADD2 (GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_2)<<1) 
#define ADD3 (GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_3)<<2) 
#define ADD4 (GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_0)<<3) 
void switch_Init(void)
{	
	GPIO_InitTypeDef  GPIO_InitStructure;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//普通输入模式
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;//上拉
  GPIO_Init(GPIOD, &GPIO_InitStructure);//初始化GPIOE2,3,4 
retry:	
	switch_vlaue = ADD1+ADD2+ADD3+ADD4;
	delay_ms(100);
	if(	switch_vlaue != (ADD1+ADD2+ADD3+ADD4))
		goto retry;
} 





















