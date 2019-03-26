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
	
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//使能GPIOB时钟
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);//使能SPI2时钟
 
  //GPIOFB3,4,5初始化设置
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;//PB13~15复用功能输出	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化
	
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource13,GPIO_AF_SPI2); //PB3复用为 SPI2
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource14,GPIO_AF_SPI2); //PB4复用为 SPI2
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource15,GPIO_AF_SPI2); //PB5复用为 SPI2
 
	//这里只针对SPI口初始化
	RCC_APB2PeriphResetCmd(RCC_APB1Periph_SPI2,ENABLE);//复位SPI1
	RCC_APB2PeriphResetCmd(RCC_APB1Periph_SPI2,DISABLE);//停止复位SPI1

  /* SPI1 configuration */
  // 在SCLK下降沿，系统通过DIN向1256发送数据 
  // 在SCLK上升沿，系统通过DOUT 读取1256数据
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //设置SPI单向或者双向的数据模式:SPI设置为双线双向全双工
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		//设置SPI工作模式:设置为主SPI
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		//设置SPI的数据大小:SPI发送接收8位帧结构
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;		//串行同步时钟的空闲状态为高电平
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;	//串行同步时钟的第二个跳变沿（上升或下降）数据被采样
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		//NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_128;		//定义波特率预分频的值:波特率预分频值为256
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
	SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRC值计算的多项式
	SPI_Init(SPI2, &SPI_InitStructure);  //根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器
 
	SPI_Cmd(SPI2, ENABLE); //使能SPI外设
	SPI_SendByte(0xff);//启动传输
 
  ADS1256_GPIO_init();
	change_cs_and_ready_chip(1);
  ADS1256_Init();
	
  //SPI1_SetSpeed(SPI_BaudRatePrescaler_4);		//设置为21M时钟,高速模式	
}

//SPI1速度设置函数
//SPI速度=fAPB2/分频系数
//@ref SPI_BaudRate_Prescaler:SPI_BaudRatePrescaler_2~SPI_BaudRatePrescaler_256  
//fAPB2时钟一般为84Mhz：
void SPI2_SetSpeed(u8 SPI_BaudRatePrescaler)
{
  assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));//判断有效性
	SPI2->CR1&=0XFFC7;//位3-5清零，用来设置波特率
	SPI2->CR1|=SPI_BaudRatePrescaler;	//设置SPI1速度 
	SPI_Cmd(SPI2,ENABLE); //使能SPI1
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
  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET){}//等待发送区空  
	SPI_I2S_SendData(SPI2, TxData); //通过外设SPIx发送一个byte  数据
		
  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET){} //等待接收完一个byte   
	return SPI_I2S_ReceiveData(SPI2); //返回通过SPIx最近接收的数据	
}


/*******************************************************************
函数名：初始化ADS1256接口
函数原型：void ADC_gpio_init()
参数说明：无
返回：  无
********************************************************************/
void ADS1256_GPIO_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	/////////CS  引脚设置
  ///////// DRDY引脚设置
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_GPIOC, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin= GPIO_Pin_6|GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//普通输入模式
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(GPIOD , &GPIO_InitStructure);	//--将配置写入对应的GPIO寄存器中
	
	
	GPIO_InitStructure.GPIO_Pin= GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(GPIOD , &GPIO_InitStructure);	//--将配置写入对应的GPIO寄存器中
	
	
	GPIO_InitStructure.GPIO_Pin= GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//普通输出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(GPIOC , &GPIO_InitStructure);	//--将配置写入对应的GPIO寄存器中
}

//-----------------------------------------------------------------//
//	功    能：ADS1256 写数据
//	入口参数: /
//	出口参数: /
//	全局变量: /
//	备    注: 向ADS1256中地址为regaddr的寄存器写入一个字节databyte
//-----------------------------------------------------------------//

void ADS1256WREG(unsigned char regaddr,unsigned char databyte)
{
	ADS_CS(0);
	ADS_WAIT_DRDY();//当ADS1256_DRDY为低时才能写寄存器
//向寄存器写入数据地址
	SPI_SendByte(ADS1256_CMD_WREG | (regaddr & 0xF));
	//写入数据的个数n-1
	SPI_SendByte(0);
	//向regaddr地址指向的寄存器写入数据databyte
	delay_us(5);
	SPI_SendByte(databyte);
	ADS_CS(1);
}

//-----------------------------------------------------------------//
//	功    能：ADS1256 读寄存器数据
//	入口参数: /
//	出口参数: /
//	全局变量: /
//	备    注: 从ADS1256中地址为regaddr的寄存器读出一个字节databyte
//-----------------------------------------------------------------//

unsigned char ADS1256RREG(unsigned char regaddr)
{
    //从ADS1256中地址为regaddr的寄存器读出一个字节
    unsigned char r=0;
    ADS_CS(0);
	ADS_WAIT_DRDY();;//当ADS1256_DRDY为低时才能写寄存器
	//写入地址
    SPI_SendByte(ADS1256_CMD_RREG | (regaddr & 0xF));
    //写入读取数据的个数n-1
    SPI_SendByte(0);
	  delay_us(10);  //min=50*(1/fCLKIN)=50*(1/7.68MHZ)=6500ns;max=whatever
    //读出regaddr地址指向的寄存器的数据
    r=SPI_SendByte(0);
	  ADS_CS(1);

    return r;//返回数据
}

//-----------------------------------------------------------------//
//	功    能：ADS1256初始化子程序
//	入口参数: /
//	出口参数: /
//	全局变量: /
//	备    注: /
//-----------------------------------------------------------------//
void ADS1256_Init(void)
{
	ADS_CS(0);
	SPI_SendByte(ADS1256_CMD_REST);
	delay_ms(10);
  ADS_WAIT_DRDY();
	SPI_SendByte(ADS1256_CMD_SYNC);                 //同步命令
	SPI_SendByte(ADS1256_CMD_WAKEUP);               //同步唤醒	
	ADS_WAIT_DRDY();;
	SPI_SendByte(ADS1256_CMD_WREG | ADS1256_STATUS);//连续写入4个寄存器
	SPI_SendByte(3);
	SPI_SendByte(0x06);                             //高位在前，使用内部校准，使用缓存
	SPI_SendByte(ADS1256_MUXP_AIN0 | ADS1256_MUXN_AINCOM);//端口输入A2为正，A3位负
	SPI_SendByte(ADS1256_GAIN_1);                   //放大倍数设置
	SPI_SendByte(ADS1256_DRATE_50SPS);             //采集速度设置
	ADS1256WREG(ADS1256_IO,0x00);
	delay_us(100);
	ADS_WAIT_DRDY();
  SPI_SendByte(ADS1256_CMD_SELFCAL);              //偏移和增益自动校准
  ADS_CS(1);
	
}

//-----------------------------------------------------------------//
//	功    能：
//	入口参数: /
//	出口参数: /
//	全局变量: /
//	备    注:
//-----------------------------------------------------------------//

unsigned long ADS1256ReadData(void)  
{
    unsigned char i=0;
    unsigned long sum=0;
		unsigned long r=0;
		ADS_CS(0);

		ADS_WAIT_DRDY();               //当ADS1256_DRDY为低时才能写寄存器 	
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
//	功    能：读取ADS1256单路数据
//	入口参数: /
//	出口参数: /
//	全局变量: /
//	备    注: /
//-----------------------------------------------------------------//
void ADS_Read(void)
{
	static u8 j=0;
/*
	ADS_sum(a,b)：函数功能，测量电压a-b
	解说：ADS1256_MUXP_AIN（0~7）代表通道0~7，差分正 P：positive
			ADS1256_MUXN_AIN（0~7）代表通道0~7，差分负 N：negative
	
	例：差分测量通道0和1：ADS_sum(ADS1256_MUXP_AIN0 | ADS1256_MUXN_AIN1);    返回通道0-通道1的电压值
		  单端测量通道0：   ADS_sum(ADS1256_MUXP_AIN0 | ADS1256_MUXN_AINCOM);  返回通道0对地的电压值
*/	
	switch(j)
	{
		case 0:
			ADS1256WREG(ADS1256_MUX,ADS1256_MUXP_AIN0 | ADS1256_MUXN_AINCOM);		//设置通道
		  SPI_SendByte(ADS1256_CMD_SELFCAL);    //偏移和增益自动校准
			ADS_Value[j] = ADS1256ReadData();//读取AD值，返回24位数据。	
		  if(ADS_Value[j]>=0x800000)    //测得电压值为负的情况
			ADS_Value[j]=0xfffff-ADS_Value[j];	   //取反运算
      ADS_Value[j]/=64;	
			j=1;
			break;
		case 1:
			ADS1256WREG(ADS1256_MUX,ADS1256_MUXP_AIN1 | ADS1256_MUXN_AINCOM);		//设置通道
			SPI_SendByte(ADS1256_CMD_SELFCAL);    //偏移和增益自动校准
			ADS_Value[j] = ADS1256ReadData();//读取AD值，返回24位数据。
		  if(ADS_Value[j]>=0x800000)    //测得电压值为负的情况
			ADS_Value[j]=0xfffff-ADS_Value[j];	   //取反运算
			ADS_Value[j]/=64;			
			j=2;		
			break;
		case 2:
			ADS1256WREG(ADS1256_MUX,ADS1256_MUXP_AIN2 | ADS1256_MUXN_AINCOM);		//设置通道
			SPI_SendByte(ADS1256_CMD_SELFCAL);    //偏移和增益自动校准
			ADS_Value[j] = ADS1256ReadData();//读取AD值，返回24位数据。
		  if(ADS_Value[j]>=0x800000)    //测得电压值为负的情况
			ADS_Value[j]=0xfffff-ADS_Value[j];	   //取反运算
			ADS_Value[j]/=64;				
			j=3;		
			break;
		case 3:
			ADS1256WREG(ADS1256_MUX,ADS1256_MUXP_AIN3 | ADS1256_MUXN_AINCOM);		//设置通道
			SPI_SendByte(ADS1256_CMD_SELFCAL);    //偏移和增益自动校准
			ADS_Value[j] = ADS1256ReadData();//读取AD值，返回24位数据。	
		  if(ADS_Value[j]>=0x800000)    //测得电压值为负的情况
			ADS_Value[j]=0xfffff-ADS_Value[j];	   //取反运算
			ADS_Value[j]/=64;	
			j=4;		
			break;
		case 4:
			ADS1256WREG(ADS1256_MUX,ADS1256_MUXP_AIN4 | ADS1256_MUXN_AINCOM);		//设置通道
			SPI_SendByte(ADS1256_CMD_SELFCAL);    //偏移和增益自动校准
			ADS_Value[j] = ADS1256ReadData();//读取AD值，返回24位数据。
		  if(ADS_Value[j]>=0x800000)    //测得电压值为负的情况
			ADS_Value[j]=0xfffff-ADS_Value[j];	   //取反运算
			ADS_Value[j]/=64;				
			j=5;		
			break;
		case 5:
			ADS1256WREG(ADS1256_MUX,ADS1256_MUXP_AIN5 | ADS1256_MUXN_AINCOM);		//设置通道
			SPI_SendByte(ADS1256_CMD_SELFCAL);    //偏移和增益自动校准
			ADS_Value[j] = ADS1256ReadData();//读取AD值，返回24位数据。	
		  if(ADS_Value[j]>=0x800000)    //测得电压值为负的情况
			ADS_Value[j]=0xfffff-ADS_Value[j];	   //取反运算
			ADS_Value[j]/=64;	
			j=6;		
			break;
		case 6:
			ADS1256WREG(ADS1256_MUX,ADS1256_MUXP_AIN6 | ADS1256_MUXN_AINCOM);		//设置通道
			SPI_SendByte(ADS1256_CMD_SELFCAL);    //偏移和增益自动校准
			ADS_Value[j] = ADS1256ReadData();//读取AD值，返回24位数据。
		  if(ADS_Value[j]>=0x800000)    //测得电压值为负的情况
			ADS_Value[j]=0xfffff-ADS_Value[j];	   //取反运算
			ADS_Value[j]/=64;				
			j=7;		
			break;
		case 7:
			ADS1256WREG(ADS1256_MUX,ADS1256_MUXP_AIN7 | ADS1256_MUXN_AINCOM);		//设置通道
			SPI_SendByte(ADS1256_CMD_SELFCAL);    //偏移和增益自动校准
			ADS_Value[j] = ADS1256ReadData();//读取AD值，返回24位数据。
		  if(ADS_Value[j]>=0x800000)    //测得电压值为负的情况
			ADS_Value[j]=0xfffff-ADS_Value[j];	   //取反运算
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





