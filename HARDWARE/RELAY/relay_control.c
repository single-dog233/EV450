#include "sys.h"


data_io data_io_infos[data_io_num];
chip_info lock_io_infos[CHIP_74HC573_NUM];
relay_info relay[RELAY_NUM];

unsigned char saveBak[80];

static void data_init(void)
{
	int i;
	lock_io_infos[1].port = GPIOA;
	lock_io_infos[1].pin = GPIO_Pin_15;
	lock_io_infos[2].port = GPIOC;
	lock_io_infos[2].pin = GPIO_Pin_10;
	lock_io_infos[3].port = GPIOF;
	lock_io_infos[3].pin = GPIO_Pin_15;
	lock_io_infos[4].port = GPIOF;
	lock_io_infos[4].pin = GPIO_Pin_14;
	lock_io_infos[5].port = GPIOC;
	lock_io_infos[5].pin = GPIO_Pin_11;
	for(i = 1; i < 9; i++)
	{
		data_io_infos[i].port = GPIOG;
		data_io_infos[i].pin = GPIO_Pin_8>>i;
	}
	
	for(i = 1; i < 9; i++)
	{
		relay[i].chip_info_data = &lock_io_infos[1];
		relay[i].data_io_info = &data_io_infos[i];
	}
	
	for(i = 9; i < 17; i++)
	{
		relay[i].chip_info_data = &lock_io_infos[2];
		relay[i].data_io_info = &data_io_infos[i-8];
	}
	
	
	for(i = 17; i < 21; i++)
	{
		relay[i].chip_info_data = &lock_io_infos[3];
		relay[i].data_io_info = &data_io_infos[i-12];
	}
	
	for(i = 21; i < 25; i++)
	{
		relay[i].chip_info_data = &lock_io_infos[3];
		relay[i].data_io_info = &data_io_infos[i-20];
	}
	
		for(i = 25; i < 33; i++)
	{
		relay[i].chip_info_data = &lock_io_infos[4];
		relay[i].data_io_info = &data_io_infos[i-24];
	}
	
	for(i = 33; i < 41; i++)
	{
		relay[i].chip_info_data = &lock_io_infos[5];
		relay[i].data_io_info = &data_io_infos[i-32];
	}

}


void chip_74hc573_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1| GPIO_Pin_2| \
																GPIO_Pin_3 | GPIO_Pin_4| GPIO_Pin_5| \
																GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_Init(GPIOG, &GPIO_InitStructure);
	
	GPIO_ResetBits(GPIOG, GPIO_InitStructure.GPIO_Pin);
	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOA, GPIO_InitStructure.GPIO_Pin);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOC, GPIO_InitStructure.GPIO_Pin);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14|GPIO_Pin_15;
  GPIO_Init(GPIOF, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOF, GPIO_InitStructure.GPIO_Pin);
	
	data_init();
}

static void chip_74hc573_data_refresh(chip_info * lock_io_info)
{
	GPIO_SetBits(lock_io_info->port, lock_io_info->pin);
	delay_ms(10);
	GPIO_ResetBits(lock_io_info->port, lock_io_info->pin);
	delay_ms(10);
}


void chip_74hc573_send_data(int chip_num, int data)
{
	if(chip_num>0&&chip_num<RELAY_NUM)
	{
		int last_data= relay[chip_num].chip_info_data->lastValue;//此芯片的最后一次输出
		data_io * data_io_info = relay[chip_num].data_io_info; //取出对应的data引脚

		if(data)
		{
			GPIO_Write(data_io_info->port,  last_data);
			GPIO_SetBits(data_io_info->port, data_io_info->pin);
			saveBak[chip_num] = 1;
			printf("77");
		}
		else
		{
			GPIO_Write(data_io_info->port,  last_data);
			GPIO_ResetBits(data_io_info->port, data_io_info->pin);
			saveBak[chip_num] = 0;
			printf("88");
		}
			
		chip_74hc573_data_refresh(relay[chip_num].chip_info_data);
		relay[chip_num].chip_info_data->lastValue = GPIO_ReadOutputData(data_io_info->port);//将本次输出保存
	}
}


