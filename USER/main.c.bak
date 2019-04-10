#include "sys.h"
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
	ADS1256_GPIO_init();
	SPI_ADS1256_Init();
	ADS1256_Init();
	PWM_Gpio_Init();

	while(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_1));
	
//	for(int i = 1; i < 41; i++)
//		chip_74hc573_send_data(i, 1);		
//	for(int i = 1; i < 41; i++)	
//		chip_74hc573_send_data(i, 0);
	while(1)
	{
		voltage_get();
		ADS_Read();
		pwm_get();
		uart_send_adc_message();		
//		printf("33 = %d 34 = %d    ", frame_s->data[32], frame_s->data[33]);
//		printf("37 = %d 38 = %d\r\n", frame_s->data[36], frame_s->data[37]);
		delay_ms(700);
		if(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_1))
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
	}    
}

