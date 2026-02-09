#include "stm32f10x.h"                  // Device header
#include <math.h>
#include "delay.h"

static uint16_t AD_Value[4];					//存放AD转换结果

#define MQ2				GPIO_Pin_0
#define MQ7				GPIO_Pin_1
#define PROT1			GPIOB

#define LDR				GPIO_Pin_0
#define MQ135			GPIO_Pin_7
#define PROT2			GPIOA



/**
  * 函    数：ADC初始化
  * 参    数：无
  * 返 回 值：无
  */
void ADCX_Init(void)
{
	/*开启时钟*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);	//开启ADC1的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);	//开启GPIOA的时钟
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);		//开启DMA1的时钟
	
	/*设置ADC时钟*/
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);						//选择6分频
	
	/*GPIO初始化*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(PROT1, &GPIO_InitStructure);					//将PB0、PB1初始化为模拟输入
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(PROT2, &GPIO_InitStructure);					//将PA0、PA7初始化为模拟输入	
	
	/*规则组通道配置*/
	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_55Cycles5);	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_7, 2, ADC_SampleTime_55Cycles5);	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 3, ADC_SampleTime_55Cycles5);	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_9, 4, ADC_SampleTime_55Cycles5);	
	
	/*ADC初始化*/
	ADC_InitTypeDef ADC_InitStructure;											//定义结构体变量
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;							//独立模式
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;						//右对齐
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;			//软件触发
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;							//连续转换
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;								//扫描模式
	ADC_InitStructure.ADC_NbrOfChannel = 4;										//扫描规则组的前4个通道
	ADC_Init(ADC1, &ADC_InitStructure);											//配置ADC1
	
	/*DMA初始化*/
	DMA_InitTypeDef DMA_InitStructure;											//定义结构体变量
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;				//外设基地址
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;	//外设数据宽度
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;			//外设地址自增，选择失能
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)AD_Value;					//存储器基地址
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;			//存储器数据宽度
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;						//存储器地址自增，选择使能
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;							//数据传输方向
	DMA_InitStructure.DMA_BufferSize = 4;										//转运的转运次数
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;								//循环模式
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;								//存储器到存储器，选择失能
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;						//优先级，选择中等
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);								//配置DMA1的通道1
	
	/*DMA和ADC使能*/
	DMA_Cmd(DMA1_Channel1, ENABLE);							//DMA1的通道1使能
	ADC_DMACmd(ADC1, ENABLE);								//ADC1触发DMA1的信号使能
	ADC_Cmd(ADC1, ENABLE);									//ADC1使能
	
	/*ADC校准*/
	ADC_ResetCalibration(ADC1);								
	while (ADC_GetResetCalibrationStatus(ADC1) == SET);
	ADC_StartCalibration(ADC1);
	while (ADC_GetCalibrationStatus(ADC1) == SET);
	
	/*ADC触发*/
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);	//软件触发ADC开始工作
}

/**
  * 函    数：获取LUX数值
  * 参    数：无
  * 返 回 值：输出lux数据
  */
static uint16_t Get_LDR_LUX(void)
{
	float voltage = 0;	
	float R = 0;	
	voltage = AD_Value[0];
	voltage  = voltage / 4096 * 3.3f;	//计算出当前电压
	
	/*转换成LUX*/
	R = voltage / (3.3f - voltage) * 10000;
	return (40000 * pow(R, -0.6021));
}

/**
  * 函    数：获取MQ135传感器PPM
  * 参    数：无
  * 返 回 值：输出ppm数据
  */
static uint16_t Get_MQ135_PPM(void)
{
	uint16_t adc_value = 0;	//这是从MQ-135传感器模块电压输出的ADC转换中获得的原始数字值，该值的范围为0到4095，将模拟电压表示为数字值
	float voltage = 0;	//MQ-135传感器模块的电压输出
	adc_value = AD_Value[1];

    voltage  = ((3.3f/4096.f)*(adc_value)) * 2;	//计算出当前电压
	
	return (uint16_t)(voltage * 100.f);	//转换成ppm
}

/**
  * 函    数：获取MQ2传感器PPM
  * 参    数：无
  * 返 回 值：输出ppm数据
  */
static uint16_t Get_MQ2_PPM(void)
{
	uint16_t adc_value = 0;	//这是从MQ-2传感器模块电压输出的ADC转换中获得的原始数字值，该值的范围为0到4095，将模拟电压表示为数字值
	float voltage = 0;	//MQ-2传感器模块的电压输出
	adc_value = AD_Value[2];

    voltage  = ((3.3f/4096.f)*(adc_value)) * 2;	//计算出当前电压
	
	return (uint16_t)(voltage * 100.f);	//转换成ppm
}

/**
  * 函    数：获取MQ7传感器PPM
  * 参    数：无
  * 返 回 值：输出ppm数据
  */
static uint16_t Get_MQ7_PPM(void)
{
	uint16_t adc_value = 0;	//这是从MQ-7传感器模块电压输出的ADC转换中获得的原始数字值，该值的范围为0到4095，将模拟电压表示为数字值
	float voltage = 0;	//MQ-7传感器模块的电压输出
	adc_value = AD_Value[3];

    voltage  = ((3.3f/4096.f)*(adc_value)) * 2;	//计算出当前电压
	
	return (uint16_t)(voltage * 100.f);	//转换成ppm
}

/**
  * 函    数：获取滤波后的LUX数值
  * 参    数：输出lux数据
  * 返 回 值：无
  */
void Get_Average_LDR_LUX(uint16_t *lux)
{
	uint32_t temp;

	for (uint8_t i = 0; i < 10; i++)
	{
		temp += Get_LDR_LUX();
		Delay_ms(5);
	}
	
	temp /= 10;
	*lux = temp;
}

/**
  * 函    数：获取滤波后的MQ2传感器的PPM
  * 参    数：输出ppm数据
  * 返 回 值：无
  */
void Get_Average_MQ2_PPM(uint16_t *ppm)
{
	uint32_t temp;

	for (uint8_t i = 0; i < 10; i++)
	{
		temp += Get_MQ2_PPM();
		Delay_ms(5);
	}
	temp /= 10;
	*ppm = temp;
}

/**
  * 函    数：获取滤波后的MQ7传感器的PPM
  * 参    数：输出ppm数据
  * 返 回 值：无
  */
void Get_Average_MQ7_PPM(uint16_t *ppm)
{
	uint32_t temp;

	for (uint8_t i = 0; i < 10; i++)
	{
		temp += Get_MQ7_PPM();
		Delay_ms(5);
	}
	
	temp /= 10;
	*ppm = temp;
}

/**
  * 函    数：获取滤波后的MQ135传感器的PPM
  * 参    数：输出ppm数据
  * 返 回 值：无
  */
void Get_Average_MQ135_PPM(uint16_t *ppm)
{
	uint32_t temp;

	for (uint8_t i = 0; i < 10; i++)
	{
		temp += Get_MQ135_PPM();
		Delay_ms(5);
	}
	
	temp /= 10;
	*ppm = temp;
}
