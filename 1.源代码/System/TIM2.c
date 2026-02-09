#include "tim2.h"                  // Device header

extern uint8_t motor_Flag_lock = 0;
uint8_t buzzerFlag = 0;
uint8_t keyDelay_50ms = 150;

void Timer2_Init(u16 Prescaler, u16 Period)	//0-65535
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = Period;
	TIM_TimeBaseInitStructure.TIM_Prescaler = Prescaler;
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);
	
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_Cmd(TIM2, ENABLE);
}

void TIM2_IRQHandler(void)	//2ms定时中断
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET ) 
	{
		
		Key_scan();	//按键扫描函数
		
		if (keyDelay_50ms)
		{
			keyDelay_50ms--;
			KeyNum = 0;	
		}		
		
		if (model)
		{
			if (sensorData.lux < Sensorthreshold.luxValue)
			{
				LED_ON();
			}
			else
			{
				LED_OFF();
			}
			
			if (((sensorData.smoke > Sensorthreshold.smokeValue) || (sensorData.CO > Sensorthreshold.COValue) || 
				(sensorData.AQI > Sensorthreshold.AQIValue)) && motor_Flag_lock == 0)
			{
				motorFlag = 1;
				motor_Flag_lock = 1;
			}
			else if ((sensorData.smoke < Sensorthreshold.smokeValue) && (sensorData.CO < Sensorthreshold.COValue) && 
				(sensorData.AQI < Sensorthreshold.AQIValue) && motor_Flag_lock == 1)
			{
				motorFlag = 2;
				motor_Flag_lock = 0;
			}
			
			if (motor_Flag_lock == 1)
			{
				Buzzer_ON();
			}
			else
			{
				Buzzer_OFF();
			}
			
		}
		
		
		//机智云配网按键，可以重新进行配网。	
		if(KeyNum == 44 && (oledPages == 1 || oledPages == 2)) 
		{
			KeyNum = 0;
			gizwitsSetMode(2);
			Buzzer_ON();
			Delay_ms(300);
			Buzzer_OFF();
		}		
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);				
	}	
}

