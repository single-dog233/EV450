#include "sys.h"
void  Adc_Init(void)
{    
  GPIO_InitTypeDef  GPIO_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	ADC_InitTypeDef       ADC_InitStructure;
	
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); 

  //先初始化ADC1通道5 IO口
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2\
									|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;//模拟输入
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;//不带上下拉
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化  
 
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,ENABLE);	  //ADC1复位
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,DISABLE);	//复位结束	 
 
	
  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;//独立模式
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;//两个采样阶段之间的延迟5个时钟
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled; //DMA失能
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div8;//预分频4分频。ADCCLK=PCLK2/4=84/4=21Mhz,ADC时钟最好不要超过36Mhz 
  ADC_CommonInit(&ADC_CommonInitStructure);//初始化
	
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;//12位模式
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;//非扫描模式	
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;//关闭连续转换
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;//禁止触发检测，使用软件触发
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//右对齐	
  ADC_InitStructure.ADC_NbrOfConversion = 1;//1个转换在规则序列中 也就是只转换规则序列1 
  ADC_Init(ADC1, &ADC_InitStructure);//ADC初始化
	ADC_Cmd(ADC1, ENABLE);//开启AD转换器	
}		

u16 Get_Adc(u8 ch)   
{
	u16 ret;
	ADC_Cmd(ADC1, ENABLE);
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_480Cycles );	//ADC1,ADC通道,480个周期,提高采样时间可以提高精确度			     
	ADC_SoftwareStartConv(ADC1);		//使能指定的ADC1的软件转换启动功能		 
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//等待转换结束
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
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_Init(GPIOD, &GPIO_InitStructure);//初始化
	
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
	for(int adc_channel = 0; adc_channel<8; ++adc_channel)//读8次数据 adc0-adc7
	{	
		(frame_s->data)[0 + adc_channel*4] = (char)((double)Get_Adc(chanenl[adc_channel])/4096.0*150);
	}

	change_channel(2);
	for(int adc_channel = 0; adc_channel<8; adc_channel++)//读8次数据 adc0-adc7
	{	
		(frame_s->data)[1 + adc_channel*4] = (char)((double)Get_Adc(chanenl[adc_channel])/4096.0*150);		
	}	

	change_channel(5);
	for(int adc_channel = 0; adc_channel<8; adc_channel++)//读8次数据 adc0-adc7
	{	
		(frame_s->data)[3 + adc_channel*4] = (char)((double)Get_Adc(chanenl[adc_channel])/4096.0*150);		
	}
	change_channel(6);
	for(int adc_channel = 0; adc_channel<8; adc_channel++)//读8次数据 adc0-adc7
	{	
		(frame_s->data)[2 + adc_channel*4] = (char)((double)Get_Adc(chanenl[adc_channel])/4096.0*150);		
	}
	
	/********************/
	change_channel(1);
	for(int adc_channel = 0; adc_channel<8; adc_channel++)//读8次数据 adc0-adc7
	{	
		(frame_s->data)[78 - adc_channel*4] = (char)((double)Get_Adc(chanenl[adc_channel])/4096.0*150);		
	}
	change_channel(3);
	for(int adc_channel = 0; adc_channel<8; adc_channel++)//读8次数据 adc0-adc7
	{	
		(frame_s->data)[79 - adc_channel*4] = (char)((double)Get_Adc(chanenl[adc_channel])/4096.0*150);		
	}
	change_channel(4);
	for(int adc_channel = 0; adc_channel<8; adc_channel++)//读8次数据 adc0-adc7
	{	
		(frame_s->data)[77 - adc_channel*4] = (char)((double)Get_Adc(chanenl[adc_channel])/4096.0*150);		
	}
	change_channel(7);
	for(int adc_channel = 0; adc_channel<8; adc_channel++)//读8次数据 adc0-adc7
	{	
		(frame_s->data)[76 - adc_channel*4] = (char)((double)Get_Adc(chanenl[adc_channel])/4096.0*150);		
	}
}
