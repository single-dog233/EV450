#include "sys.h"
void  Adc_Init(void)
{    
  GPIO_InitTypeDef  GPIO_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	ADC_InitTypeDef       ADC_InitStructure;
	
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); 

  //�ȳ�ʼ��ADC1ͨ��5 IO��
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2\
									|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;//ģ������
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;//����������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��  
 
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,ENABLE);	  //ADC1��λ
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,DISABLE);	//��λ����	 
 
	
  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;//����ģʽ
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;//���������׶�֮����ӳ�5��ʱ��
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled; //DMAʧ��
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div8;//Ԥ��Ƶ4��Ƶ��ADCCLK=PCLK2/4=84/4=21Mhz,ADCʱ����ò�Ҫ����36Mhz 
  ADC_CommonInit(&ADC_CommonInitStructure);//��ʼ��
	
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;//12λģʽ
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;//��ɨ��ģʽ	
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;//�ر�����ת��
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;//��ֹ������⣬ʹ����������
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//�Ҷ���	
  ADC_InitStructure.ADC_NbrOfConversion = 1;//1��ת���ڹ��������� Ҳ����ֻת����������1 
  ADC_Init(ADC1, &ADC_InitStructure);//ADC��ʼ��
	ADC_Cmd(ADC1, ENABLE);//����ADת����	
}		

u16 Get_Adc(u8 ch)   
{
	u16 ret;
	ADC_Cmd(ADC1, ENABLE);
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_480Cycles );	//ADC1,ADCͨ��,480������,��߲���ʱ�������߾�ȷ��			     
	ADC_SoftwareStartConv(ADC1);		//ʹ��ָ����ADC1������ת����������		 
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//�ȴ�ת������
	ret  = ADC_GetConversionValue(ADC1);		
	ADC_Cmd(ADC1, DISABLE);
	return ret;
}
char chanenl[8] = {adc0, adc1, adc2, adc3, adc4, adc5, adc6, adc7};

void chip_74hc4051_adc_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10; //2 1 0
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
  GPIO_Init(GPIOD, &GPIO_InitStructure);//��ʼ��
	
	Adc_Init();
}

void change_channel(char channel)
{
	GPIO_WriteBit(GPIOD, GPIO_Pin_10, (BitAction)(channel&(0x1<<0)));
	GPIO_WriteBit(GPIOD, GPIO_Pin_9, (BitAction)(channel&(0x1<<1)));
	GPIO_WriteBit(GPIOD, GPIO_Pin_8, (BitAction)(channel&(0x1<<2)));
	delay_ms(20);
}
int bak;
extern frame_send * frame_s;
void voltage_get(void)
{
	change_channel(0);
	for(int adc_channel = 0; adc_channel<8; ++adc_channel)//��8������ adc0-adc7
	{	
		(frame_s->data)[0 + adc_channel*4] = (char)((double)Get_Adc(chanenl[adc_channel])/4096.0*150);
	}

	change_channel(2);
	for(int adc_channel = 0; adc_channel<8; adc_channel++)//��8������ adc0-adc7
	{	
		(frame_s->data)[1 + adc_channel*4] = (char)((double)Get_Adc(chanenl[adc_channel])/4096.0*150);		
	}	

	change_channel(5);
	for(int adc_channel = 0; adc_channel<8; adc_channel++)//��8������ adc0-adc7
	{	
		(frame_s->data)[3 + adc_channel*4] = (char)((double)Get_Adc(chanenl[adc_channel])/4096.0*150);		
	}
	change_channel(6);
	for(int adc_channel = 0; adc_channel<8; adc_channel++)//��8������ adc0-adc7
	{	
		(frame_s->data)[2 + adc_channel*4] = (char)((double)Get_Adc(chanenl[adc_channel])/4096.0*150);		
	}
	
	/********************/
	change_channel(1);
	for(int adc_channel = 0; adc_channel<8; adc_channel++)//��8������ adc0-adc7
	{	
		(frame_s->data)[78 - adc_channel*4] = (char)((double)Get_Adc(chanenl[adc_channel])/4096.0*150);		
	}
	change_channel(3);
	for(int adc_channel = 0; adc_channel<8; adc_channel++)//��8������ adc0-adc7
	{	
		(frame_s->data)[79 - adc_channel*4] = (char)((double)Get_Adc(chanenl[adc_channel])/4096.0*150);		
	}
	change_channel(4);
	for(int adc_channel = 0; adc_channel<8; adc_channel++)//��8������ adc0-adc7
	{	
		(frame_s->data)[77 - adc_channel*4] = (char)((double)Get_Adc(chanenl[adc_channel])/4096.0*150);		
	}
	change_channel(7);
	for(int adc_channel = 0; adc_channel<8; adc_channel++)//��8������ adc0-adc7
	{	
		(frame_s->data)[76 - adc_channel*4] = (char)((double)Get_Adc(chanenl[adc_channel])/4096.0*150);		
	}
}