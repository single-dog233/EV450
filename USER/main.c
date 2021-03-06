#include "sys.h"
#include "24cxx.h" 
extern char adc_stm32_value_get[80];
extern int bak;
extern frame_send * frame_s;
int first_start = 1;

int main(void)
{ 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	delay_init(168);     
	uart_init(115200);	
	switch_Init();
	wifi_init();	//发送信息，和接收信息初始化	
	chip_74hc573_init();
	chip_74hc4051_adc_init();
	LED_Init();
//		PVD_Init();
//	AT24CXX_Init();	
//	ADS1256_GPIO_init();
//	SPI_ADS1256_Init();
//	ADS1256_Init();
//	PWM_Gpio_Init();
	GPIO_WriteBit(GPIOE, GPIO_Pin_3, Bit_RESET);
  GPIO_WriteBit(GPIOE,GPIO_Pin_2, Bit_SET);
	while(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_6));
	
//	for(int i = 1; i < 41; i++)
//	{
//		chip_74hc573_send_data(i, 1);	
//		delay_ms(50);
//	}
//	for(int i = 1; i < 41; i++)	
//	{
//		chip_74hc573_send_data(i, 0);
//		delay_ms(50);
//	}
	
//	AT24CXX_Read(0,saveBak,80);
//	for(int i = 1; i<41; i++)
//	{
//		if(saveBak[i])
//			chip_74hc573_send_data(i, 1);
//		else
//			chip_74hc573_send_data(i, 0);
//		printf("pp %d\r\n", saveBak[i]);
//		delay_ms(10);
//	}
	delay_ms(1000);delay_ms(1000);
	while(1)
	{				
		#define x 28
		printf("1 = %d 2 = %d 3 = %d 4 = %d\r\n", frame_s->data[x], frame_s->data[x+1], frame_s->data[x+2], frame_s->data[x+3]);
//		printf("    1 = %d 2 = %d 3 = %d 4 = %d\r\n", frame_s->data[x+40], frame_s->data[40+x+1], frame_s->data[40+x+2], frame_s->data[40+x+3]);
//		printf("37 = %d 38 = %d\r\n", frame_s->data[36], frame_s->data[37]);	
		if(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_6))
		{
			first_start = 1;
			GPIO_WriteBit(GPIOE, GPIO_Pin_3, Bit_RESET);
			GPIO_WriteBit(GPIOE,GPIO_Pin_2, Bit_SET);
		}
		else
		{
			if(first_start == 1)
			{
				first_start = 0;
				information_retrieval();
			}
			GPIO_WriteBit(GPIOE,GPIO_Pin_3, Bit_SET);
			GPIO_ToggleBits(GPIOE,GPIO_Pin_2);			
		}
		delay_ms(700);
		voltage_get();
		uart_send_adc_message();
	}    
}

