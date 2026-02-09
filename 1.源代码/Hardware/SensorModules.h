#ifndef	__SENSORMODULES_H_
#define __SENSORMODULES_H_

#include "stm32f10x.h"                  // Device header


typedef struct
{
	uint8_t humi;
	uint8_t temp;
	uint16_t lux;	
	uint16_t smoke;
	uint16_t CO;	
	uint16_t AQI;
	uint16_t hPa;
}SensorModules;

typedef struct
{
	uint8_t humiValue;
	uint8_t tempValue;
	uint16_t luxValue;	
	uint16_t smokeValue;
	uint16_t COValue;	
	uint16_t AQIValue;
	uint16_t hPaValue;
	
}SensorThresholdValue;

extern SensorModules sensorData;			//声明传感器模块的结构体变量
extern SensorThresholdValue Sensorthreshold;	//声明传感器阈值结构体变量



#endif
