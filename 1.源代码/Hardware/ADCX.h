#ifndef __ADCX_H
#define __ADCX_H

#include "stm32f10x.h"                  // Device header

/**
  * 函    数：ADC初始化
  * 参    数：无
  * 返 回 值：无
  */
void ADCX_Init(void);

/**
  * 函    数：获取滤波后的LUX数值
  * 参    数：输出lux数据
  * 返 回 值：无
  */
void Get_Average_LDR_LUX(uint16_t *lux);

/**
  * 函    数：获取滤波后的MQ2传感器的PPM
  * 参    数：输出ppm数据
  * 返 回 值：无
  */
void Get_Average_MQ2_PPM(uint16_t *ppm);

/**
  * 函    数：获取滤波后的MQ7传感器的PPM
  * 参    数：输出ppm数据
  * 返 回 值：无
  */
void Get_Average_MQ7_PPM(uint16_t *ppm);

/**
  * 函    数：获取滤波后的MQ135传感器的PPM
  * 参    数：输出ppm数据
  * 返 回 值：无
  */
void Get_Average_MQ135_PPM(uint16_t *ppm);

#endif
