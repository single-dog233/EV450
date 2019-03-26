#include "sys.h"
extern  char adc_stm32_value_get[80];
extern int bak;
extern frame_send * frame_s;
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
//	for(int i = 1; i < 41; i++)
//		chip_74hc573_send_data(i, 1);		
//	for(int i = 1; i < 41; i++)	
//		chip_74hc573_send_data(i, 0);
	
	while(1)
	{
//		voltage_get();
//		//ads1256_send();
//		uart_send_adc_message();		
//		printf("19 = %d 62 = %d\r\n", frame_s->data[18], frame_s->data[61]);
//		delay_ms(500);
//		if(!GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_6))
//		{
//			GPIO_WriteBit(GPIOE, GPIO_Pin_3, Bit_RESET);
//			GPIO_WriteBit(GPIOE,GPIO_Pin_2, Bit_SET);
//		}
//		else
//		{
//			GPIO_WriteBit(GPIOE,GPIO_Pin_3, Bit_SET);
//			GPIO_ToggleBits(GPIOE,GPIO_Pin_2);			
//		}
//		select_exit_chanel(EXTI_Line0 | EXTI_Line2 | EXTI_Line3 | EXTI_Line4);
	//	printf("77\r\n");
		extern int high, all;
		printf("%f, %f\r\n",(float)high/(float)all, READ_Pwm_chanel(GPIOB, GPIO_Pin_0));
	}    
}

