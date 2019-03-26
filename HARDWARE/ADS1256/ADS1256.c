#include "ads1256.h"
#include "delay.h"
#include "stm32f4xx_spi.h"

u32 ADS_Value[8];

void (*ADS_CS)(int);
void (*ADS_WAIT_DRDY)(void);

void ADS_WAIT_DRDY1(void)
{
	while(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_6));
}

void ADS_WAIT_DRDY2(void)
{
	while(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_4));
}

void ADS_CS1(int pin_value)
{
 BitAction value = (BitAction)pin_value;
 GPIO_WriteBit(GPIOD, GPIO_Pin_5, value);
}

void ADS_CS2(int pin_value)
{
 BitAction value = (BitAction)pin_value;
 GPIO_WriteBit(GPIOC, GPIO_Pin_12, value);
}


void change_cs_and_ready_chip(int chip)
{
	if(chip == 1)
	{
		ADS_CS2(1);
		ADS_CS = ADS_CS1;
		ADS_WAIT_DRDY = ADS_WAIT_DRDY1;
	}
	else
	{
		ADS_CS1(1);
		ADS_CS = ADS_CS2;
		ADS_WAIT_DRDY = ADS_WAIT_DRDY2;
	}
}

/*******************************************************************************
* Function Name  : SPI_FLASH_Init
* Description    : Initializes the peripherals used by the SPI FLASH driver.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_ADS1256_Init(void) 
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  SPI_InitTypeDef  SPI_InitStructure;
	
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//ʹ��GPIOBʱ��
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);//ʹ��SPI2ʱ��
 
  //GPIOFB3,4,5��ʼ������
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;//PB13~15���ù������	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
  GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��
	
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource13,GPIO_AF_SPI2); //PB3����Ϊ SPI2
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource14,GPIO_AF_SPI2); //PB4����Ϊ SPI2
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource15,GPIO_AF_SPI2); //PB5����Ϊ SPI2
 
	//����ֻ���SPI�ڳ�ʼ��
	RCC_APB2PeriphResetCmd(RCC_APB1Periph_SPI2,ENABLE);//��λSPI1
	RCC_APB2PeriphResetCmd(RCC_APB1Periph_SPI2,DISABLE);//ֹͣ��λSPI1

  /* SPI1 configuration */
  // ��SCLK�½��أ�ϵͳͨ��DIN��1256�������� 
  // ��SCLK�����أ�ϵͳͨ��DOUT ��ȡ1256����
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //����SPI�������˫�������ģʽ:SPI����Ϊ˫��˫��ȫ˫��
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		//����SPI����ģʽ:����Ϊ��SPI
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		//����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;		//����ͬ��ʱ�ӵĿ���״̬Ϊ�ߵ�ƽ
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;	//����ͬ��ʱ�ӵĵڶ��������أ��������½������ݱ�����
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		//NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ����
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_128;		//���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ256
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ
	SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRCֵ����Ķ���ʽ
	SPI_Init(SPI2, &SPI_InitStructure);  //����SPI_InitStruct��ָ���Ĳ�����ʼ������SPIx�Ĵ���
 
	SPI_Cmd(SPI2, ENABLE); //ʹ��SPI����
	SPI_SendByte(0xff);//��������
 
  ADS1256_GPIO_init();
	change_cs_and_ready_chip(1);
  ADS1256_Init();
	
  //SPI1_SetSpeed(SPI_BaudRatePrescaler_4);		//����Ϊ21Mʱ��,����ģʽ	
}

//SPI1�ٶ����ú���
//SPI�ٶ�=fAPB2/��Ƶϵ��
//@ref SPI_BaudRate_Prescaler:SPI_BaudRatePrescaler_2~SPI_BaudRatePrescaler_256  
//fAPB2ʱ��һ��Ϊ84Mhz��
void SPI2_SetSpeed(u8 SPI_BaudRatePrescaler)
{
  assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));//�ж���Ч��
	SPI2->CR1&=0XFFC7;//λ3-5���㣬�������ò�����
	SPI2->CR1|=SPI_BaudRatePrescaler;	//����SPI1�ٶ� 
	SPI_Cmd(SPI2,ENABLE); //ʹ��SPI1
} 

/*******************************************************************************
* Function Name  : SPI_FLASH_SendByte
* Description    : Sends a byte through the SPI interface and return the byte
*                  received from the SPI bus.
* Input          : byte : byte to send.
* Output         : None
* Return         : The value of the received byte.
*******************************************************************************/
u8 SPI_SendByte(u8 TxData)
{
  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET){}//�ȴ���������  
	SPI_I2S_SendData(SPI2, TxData); //ͨ������SPIx����һ��byte  ����
		
  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET){} //�ȴ�������һ��byte   
	return SPI_I2S_ReceiveData(SPI2); //����ͨ��SPIx������յ�����	
}


/*******************************************************************
����������ʼ��ADS1256�ӿ�
����ԭ�ͣ�void ADC_gpio_init()
����˵������
���أ�  ��
********************************************************************/
void ADS1256_GPIO_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	/////////CS  ��������
  ///////// DRDY��������
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_GPIOC, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin= GPIO_Pin_6|GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//��ͨ����ģʽ
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
	GPIO_Init(GPIOD , &GPIO_InitStructure);	//--������д���Ӧ��GPIO�Ĵ�����
	
	
	GPIO_InitStructure.GPIO_Pin= GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
	GPIO_Init(GPIOD , &GPIO_InitStructure);	//--������д���Ӧ��GPIO�Ĵ�����
	
	
	GPIO_InitStructure.GPIO_Pin= GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//��ͨ���ģʽ
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
	GPIO_Init(GPIOC , &GPIO_InitStructure);	//--������д���Ӧ��GPIO�Ĵ�����
}

//-----------------------------------------------------------------//
//	��    �ܣ�ADS1256 д����
//	��ڲ���: /
//	���ڲ���: /
//	ȫ�ֱ���: /
//	��    ע: ��ADS1256�е�ַΪregaddr�ļĴ���д��һ���ֽ�databyte
//-----------------------------------------------------------------//

void ADS1256WREG(unsigned char regaddr,unsigned char databyte)
{
	ADS_CS(0);
	ADS_WAIT_DRDY();//��ADS1256_DRDYΪ��ʱ����д�Ĵ���
//��Ĵ���д�����ݵ�ַ
	SPI_SendByte(ADS1256_CMD_WREG | (regaddr & 0xF));
	//д�����ݵĸ���n-1
	SPI_SendByte(0);
	//��regaddr��ַָ��ļĴ���д������databyte
	delay_us(5);
	SPI_SendByte(databyte);
	ADS_CS(1);
}

//-----------------------------------------------------------------//
//	��    �ܣ�ADS1256 ���Ĵ�������
//	��ڲ���: /
//	���ڲ���: /
//	ȫ�ֱ���: /
//	��    ע: ��ADS1256�е�ַΪregaddr�ļĴ�������һ���ֽ�databyte
//-----------------------------------------------------------------//

unsigned char ADS1256RREG(unsigned char regaddr)
{
    //��ADS1256�е�ַΪregaddr�ļĴ�������һ���ֽ�
    unsigned char r=0;
    ADS_CS(0);
	ADS_WAIT_DRDY();;//��ADS1256_DRDYΪ��ʱ����д�Ĵ���
	//д���ַ
    SPI_SendByte(ADS1256_CMD_RREG | (regaddr & 0xF));
    //д���ȡ���ݵĸ���n-1
    SPI_SendByte(0);
	  delay_us(10);  //min=50*(1/fCLKIN)=50*(1/7.68MHZ)=6500ns;max=whatever
    //����regaddr��ַָ��ļĴ���������
    r=SPI_SendByte(0);
	  ADS_CS(1);

    return r;//��������
}

//-----------------------------------------------------------------//
//	��    �ܣ�ADS1256��ʼ���ӳ���
//	��ڲ���: /
//	���ڲ���: /
//	ȫ�ֱ���: /
//	��    ע: /
//-----------------------------------------------------------------//
void ADS1256_Init(void)
{
	ADS_CS(0);
	SPI_SendByte(ADS1256_CMD_REST);
	delay_ms(10);
  ADS_WAIT_DRDY();
	SPI_SendByte(ADS1256_CMD_SYNC);                 //ͬ������
	SPI_SendByte(ADS1256_CMD_WAKEUP);               //ͬ������	
	ADS_WAIT_DRDY();;
	SPI_SendByte(ADS1256_CMD_WREG | ADS1256_STATUS);//����д��4���Ĵ���
	SPI_SendByte(3);
	SPI_SendByte(0x06);                             //��λ��ǰ��ʹ���ڲ�У׼��ʹ�û���
	SPI_SendByte(ADS1256_MUXP_AIN0 | ADS1256_MUXN_AINCOM);//�˿�����A2Ϊ����A3λ��
	SPI_SendByte(ADS1256_GAIN_1);                   //�Ŵ�������
	SPI_SendByte(ADS1256_DRATE_50SPS);             //�ɼ��ٶ�����
	ADS1256WREG(ADS1256_IO,0x00);
	delay_us(100);
	ADS_WAIT_DRDY();
  SPI_SendByte(ADS1256_CMD_SELFCAL);              //ƫ�ƺ������Զ�У׼
  ADS_CS(1);
	
}

//-----------------------------------------------------------------//
//	��    �ܣ�
//	��ڲ���: /
//	���ڲ���: /
//	ȫ�ֱ���: /
//	��    ע:
//-----------------------------------------------------------------//

unsigned long ADS1256ReadData(void)  
{
    unsigned char i=0;
    unsigned long sum=0;
		unsigned long r=0;
		ADS_CS(0);

		ADS_WAIT_DRDY();               //��ADS1256_DRDYΪ��ʱ����д�Ĵ��� 	
		SPI_SendByte(ADS1256_CMD_SYNC);	  
		SPI_SendByte(ADS1256_CMD_WAKEUP);//SPI_SendByte(0xff);
	  ADS_WAIT_DRDY();
		SPI_SendByte(ADS1256_CMD_RDATA);
		delay_us(10);                  //min=50*(1/fCLKIN)=50*(1/7.68MHZ)=6500ns;max=whatever
			for(i=0;i<3;i++)
			{
				sum = sum << 8;
			  r = SPI_SendByte(0);
				sum |= r;
			}
		ADS_CS(1);		 
    return sum;
}

//-----------------------------------------------------------------//
//	��    �ܣ���ȡADS1256��·����
//	��ڲ���: /
//	���ڲ���: /
//	ȫ�ֱ���: /
//	��    ע: /
//-----------------------------------------------------------------//
void ADS_Read(void)
{
	static u8 j=0;
/*
	ADS_sum(a,b)���������ܣ�������ѹa-b
	��˵��ADS1256_MUXP_AIN��0~7������ͨ��0~7������� P��positive
			ADS1256_MUXN_AIN��0~7������ͨ��0~7����ָ� N��negative
	
	������ֲ���ͨ��0��1��ADS_sum(ADS1256_MUXP_AIN0 | ADS1256_MUXN_AIN1);    ����ͨ��0-ͨ��1�ĵ�ѹֵ
		  ���˲���ͨ��0��   ADS_sum(ADS1256_MUXP_AIN0 | ADS1256_MUXN_AINCOM);  ����ͨ��0�Եصĵ�ѹֵ
*/	
	switch(j)
	{
		case 0:
			ADS1256WREG(ADS1256_MUX,ADS1256_MUXP_AIN0 | ADS1256_MUXN_AINCOM);		//����ͨ��
		  SPI_SendByte(ADS1256_CMD_SELFCAL);    //ƫ�ƺ������Զ�У׼
			ADS_Value[j] = ADS1256ReadData();//��ȡADֵ������24λ���ݡ�	
		  if(ADS_Value[j]>=0x800000)    //��õ�ѹֵΪ�������
			ADS_Value[j]=0xfffff-ADS_Value[j];	   //ȡ������
      ADS_Value[j]/=64;	
			j=1;
			break;
		case 1:
			ADS1256WREG(ADS1256_MUX,ADS1256_MUXP_AIN1 | ADS1256_MUXN_AINCOM);		//����ͨ��
			SPI_SendByte(ADS1256_CMD_SELFCAL);    //ƫ�ƺ������Զ�У׼
			ADS_Value[j] = ADS1256ReadData();//��ȡADֵ������24λ���ݡ�
		  if(ADS_Value[j]>=0x800000)    //��õ�ѹֵΪ�������
			ADS_Value[j]=0xfffff-ADS_Value[j];	   //ȡ������
			ADS_Value[j]/=64;			
			j=2;		
			break;
		case 2:
			ADS1256WREG(ADS1256_MUX,ADS1256_MUXP_AIN2 | ADS1256_MUXN_AINCOM);		//����ͨ��
			SPI_SendByte(ADS1256_CMD_SELFCAL);    //ƫ�ƺ������Զ�У׼
			ADS_Value[j] = ADS1256ReadData();//��ȡADֵ������24λ���ݡ�
		  if(ADS_Value[j]>=0x800000)    //��õ�ѹֵΪ�������
			ADS_Value[j]=0xfffff-ADS_Value[j];	   //ȡ������
			ADS_Value[j]/=64;				
			j=3;		
			break;
		case 3:
			ADS1256WREG(ADS1256_MUX,ADS1256_MUXP_AIN3 | ADS1256_MUXN_AINCOM);		//����ͨ��
			SPI_SendByte(ADS1256_CMD_SELFCAL);    //ƫ�ƺ������Զ�У׼
			ADS_Value[j] = ADS1256ReadData();//��ȡADֵ������24λ���ݡ�	
		  if(ADS_Value[j]>=0x800000)    //��õ�ѹֵΪ�������
			ADS_Value[j]=0xfffff-ADS_Value[j];	   //ȡ������
			ADS_Value[j]/=64;	
			j=4;		
			break;
		case 4:
			ADS1256WREG(ADS1256_MUX,ADS1256_MUXP_AIN4 | ADS1256_MUXN_AINCOM);		//����ͨ��
			SPI_SendByte(ADS1256_CMD_SELFCAL);    //ƫ�ƺ������Զ�У׼
			ADS_Value[j] = ADS1256ReadData();//��ȡADֵ������24λ���ݡ�
		  if(ADS_Value[j]>=0x800000)    //��õ�ѹֵΪ�������
			ADS_Value[j]=0xfffff-ADS_Value[j];	   //ȡ������
			ADS_Value[j]/=64;				
			j=5;		
			break;
		case 5:
			ADS1256WREG(ADS1256_MUX,ADS1256_MUXP_AIN5 | ADS1256_MUXN_AINCOM);		//����ͨ��
			SPI_SendByte(ADS1256_CMD_SELFCAL);    //ƫ�ƺ������Զ�У׼
			ADS_Value[j] = ADS1256ReadData();//��ȡADֵ������24λ���ݡ�	
		  if(ADS_Value[j]>=0x800000)    //��õ�ѹֵΪ�������
			ADS_Value[j]=0xfffff-ADS_Value[j];	   //ȡ������
			ADS_Value[j]/=64;	
			j=6;		
			break;
		case 6:
			ADS1256WREG(ADS1256_MUX,ADS1256_MUXP_AIN6 | ADS1256_MUXN_AINCOM);		//����ͨ��
			SPI_SendByte(ADS1256_CMD_SELFCAL);    //ƫ�ƺ������Զ�У׼
			ADS_Value[j] = ADS1256ReadData();//��ȡADֵ������24λ���ݡ�
		  if(ADS_Value[j]>=0x800000)    //��õ�ѹֵΪ�������
			ADS_Value[j]=0xfffff-ADS_Value[j];	   //ȡ������
			ADS_Value[j]/=64;				
			j=7;		
			break;
		case 7:
			ADS1256WREG(ADS1256_MUX,ADS1256_MUXP_AIN7 | ADS1256_MUXN_AINCOM);		//����ͨ��
			SPI_SendByte(ADS1256_CMD_SELFCAL);    //ƫ�ƺ������Զ�У׼
			ADS_Value[j] = ADS1256ReadData();//��ȡADֵ������24λ���ݡ�
		  if(ADS_Value[j]>=0x800000)    //��õ�ѹֵΪ�������
			ADS_Value[j]=0xfffff-ADS_Value[j];	   //ȡ������
			ADS_Value[j]/=64;	
			j=0;		
			break;
    default:j=0;
		  break; 		
	}	
	for(int j = 0; j < 8; j++)
	printf("%7d",ADS_Value[j]);
	printf("\r\n");

}





