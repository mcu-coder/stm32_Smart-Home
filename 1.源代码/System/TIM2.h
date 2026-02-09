#ifndef __TIM2_H__
#define __TIM2_H__

#include "stm32f10x.h"                  // Device header
#include "key.h"   
#include "sensormodules.h"
#include "buzzer.h"
#include "delay.h"
#include "gizwits_product.h"
#include "led.h"

extern uint8_t model;
extern uint8_t oledPages;

void Timer2_Init(u16 Prescaler, u16 Period);


#endif
