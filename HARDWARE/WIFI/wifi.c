#include "sys.h"

char RecBuff[sizeof(data_frame)*2-1];
char frame_send_char[sizeof(frame_send)];
char ok_ack[] = {0x76, 0xf5, 0x01, 0x01, 0x01, 0x59, 0xff, 0xcc};
char get_value[] = {0x76, 0xf5, 0xff, 0x03, 0x01, 0xAA, 0xff, 0xcc};
extern int switch_vlaue;
data_frame * point = (data_frame * )RecBuff;
frame_send * frame_s = (frame_send *)frame_send_char;
void send_fram_init(void)
{
		(frame_s->star)[0] = 0x76;
		(frame_s->star)[1] = 0xf5;
		(frame_s->addr)[0] = switch_vlaue;
		(frame_s->command)[0] = 0x02;
		(frame_s->data_length)[0] = 80;
		(frame_s->end)[0] = 0xff;
		(frame_s->end)[1] = 0xcc;
}

void uart_send_message(u32 addr, int length)
{
	while(DMA_GetCurrDataCounter(DMA2_Stream6));
	MYDMA_Config(DMA2_Stream6,DMA_Channel_5,(u32)&USART6->DR,addr,length, 1);
	MYDMA_Enable(DMA2_Stream6,length);
}

void uart_send_adc_message(void)
{
	uart_send_message((u32)&frame_send_char,sizeof(frame_send_char));
}

void information_retrieval(void)
{
	uart_send_message((u32)get_value, sizeof(get_value));
}

void TIM3_Int_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE); 
  TIM_TimeBaseInitStructure.TIM_Period = arr; 	
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;  
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);
	TIM_ARRPreloadConfig(TIM3, ENABLE);
	TIM_Cmd(TIM3,DISABLE);
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);
	NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x00;//响应控制 优先级最高
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x00; 
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
}

extern int switch_vlaue;
void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET)
	{
		for(int i =0; i < sizeof(data_frame); i++)
			if(0x76f5 ==  (RecBuff[i]<<8)+RecBuff[i+1])//验证帧头
			{	
				point = (data_frame *)&RecBuff[i];//重新标定起点
				if((0xffcc == (point->end[0]<<8)+point->end[1]))//验证尾部
				{
					if((point->command)[0] == 0x04)//使用数据
					{
						for(int i = 0; i < 41; i++)
							chip_74hc573_send_data(i, 0);
					}
					else
					{
						point = (data_frame * )(&RecBuff[i]);
						chip_74hc573_send_data(*(point->channel), *(point->link));
						uart_send_message((u32)ok_ack,sizeof(ok_ack));
					}
				}
			}
	}
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);  
	TIM_Cmd(TIM3,DISABLE);
	MYDMA_Enable(DMA2_Stream2,sizeof(data_frame));
}
void wifi_gpio_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE|RCC_AHB1Periph_GPIOC, ENABLE);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;//wifi pin reset e5 wifi pin reset e5
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_Init(GPIOE, &GPIO_InitStructure);//初始化
	GPIO_SetBits(GPIOE,GPIO_Pin_5);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;//wifi pin reladc13
	GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化
	GPIO_SetBits(GPIOC,GPIO_Pin_13);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;//wifi pin reladc13
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOE, &GPIO_InitStructure);//初始化
}

void wifi_init(void)
{
	wifi_gpio_init();
	uart6_init(57600);
	send_fram_init();
	MYDMA_Config(DMA2_Stream6,DMA_Channel_5,(u32)&USART6->DR,(u32)&frame_send_char,sizeof(frame_send_char), 1);//74 - 65
	MYDMA_Config(DMA2_Stream2,DMA_Channel_5,(u32)&USART6->DR,(u32)RecBuff,sizeof(RecBuff), 0);//54- 25
	NVIC_DMA();
	USART_DMACmd(USART6,USART_DMAReq_Tx,ENABLE);  //使能串口1的DMA发送  
	USART_DMACmd(USART6,USART_DMAReq_Rx,ENABLE);	
	MYDMA_Enable(DMA2_Stream2,sizeof(RecBuff));
	MYDMA_Enable(DMA2_Stream6,sizeof(frame_send_char));
	TIM3_Int_Init(400-1,8400-1);	
}


void DMA2_Stream6_IRQHandler(void)
{
		if(DMA_GetITStatus(DMA2_Stream6, DMA_IT_TCIF6 ))
    {
        DMA_ClearITPendingBit(DMA2_Stream6, DMA_IT_TCIF6); 
    }
}
//先接收数据， 利用定时器做后续处理
void DMA2_Stream2_IRQHandler(void)
{
		if(DMA_GetITStatus(DMA2_Stream2, DMA_IT_HTIF2))
    {
			DMA_ClearITPendingBit(DMA2_Stream2, DMA_IT_HTIF2);
			TIM_Cmd(TIM3,ENABLE);
    }	
		
		if(DMA_GetITStatus(DMA2_Stream2, DMA_IT_TCIF2))
    {
      DMA_ClearITPendingBit(DMA2_Stream2, DMA_IT_TCIF2); 
			MYDMA_Enable(DMA2_Stream2,sizeof(RecBuff));
    }		
//		printf("%2X %2X %2X %2X %2X\r\n", RecBuff[0], RecBuff[1], RecBuff[2], RecBuff[3],RecBuff[4],RecBuff[5],RecBuff[7]);		
		
}
