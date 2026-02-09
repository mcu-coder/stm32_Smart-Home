#include "stm32f10x.h"                  // Device header
#include "oled.h"
#include "adcx.h"
#include "bmp280.h"
#include "sensormodules.h"
#include "dht11.h"
#include "key.h"
#include "tim2.h"
#include "tim3.h"
#include "flash.h"
#include "motor.h"
#include "led.h"
#include "usart.h"
#include "iwdg.h"

SensorModules sensorData;			//声明传感器模块的结构体变量
SensorThresholdValue Sensorthreshold;	//声明传感器阈值结构体变量

#define KEY_Long1	11

#define KEY_1	1
#define KEY_2	2
#define KEY_3	3
#define KEY_4	4

#define FLASH_START_ADDR	0x0801f000	//写入的起始地址

uint8_t motorFlag = 0;
uint8_t oledPages = 1;	//系统显示页面
uint8_t model;	//系统模式

typedef enum 
{
	DISPLAY_PAGE1 = 1,
	DISPLAY_PAGE2,
	SETTINGS_PAGE
} OLED_PAGES;

/**
  * @brief  显示菜单1的固定内容
  * @param  无
  * @retval 无
  */
void OLED_Menu1(void)
{
	//显示“温度：  C”
	OLED_ShowChinese(1,1,48);
	OLED_ShowChinese(1,2,49);
	OLED_ShowChar(1,5,':');
	OLED_ShowChar(1,8,'C');
	
	//显示“湿度:   %”
	OLED_ShowChinese(1,5,50);
	OLED_ShowChinese(1,6,51);
	OLED_ShowChar(1,13,':');	
	OLED_ShowChar(1,16,'%');
	 
	
	//显示“系统模式：”
	OLED_ShowChinese(4, 1, 28);
	OLED_ShowChinese(4, 2, 29);
	OLED_ShowChinese(4, 3, 30);
	OLED_ShowChinese(4, 4, 31);	
	OLED_ShowChar(4, 9, ':');
}

/**
  * @brief  显示菜单2的固定内容
  * @param  无
  * @retval 无
  */
void OLED_Menu2(void)
{
	//显示“烟雾浓度：  ppm”
	OLED_ShowChinese(1, 1, 20);
	OLED_ShowChinese(1, 2, 21);
	OLED_ShowChinese(1, 3, 22);
	OLED_ShowChinese(1, 4, 23);	
	OLED_ShowChar(1, 9, ':');
	OLED_ShowString(1,13,"ppm");
	 
}

/**
  * @brief  显示菜单1的传感器数据
  * @param  无
  * @retval 无
  */
void SensorDataDisplay1(void)
{
	 
	
	
	//显示系统状态数据
	if (!model)
	{
		OLED_ShowChinese(4, 6, 36);
		OLED_ShowChinese(4, 7, 37);		
	}
	else
	{
		OLED_ShowChinese(4, 6, 32);
		OLED_ShowChinese(4, 7, 33);			
	}
}

/**
  * @brief  显示菜单2的传感器数据
  * @param  无
  * @retval 无
  */
void SensorDataDisplay2(void)
{
	
	//显示烟雾浓度数据
	OLED_ShowNum(1, 10, sensorData.smoke, 3);	
	
	//显示一氧化碳数据
	OLED_ShowNum(2, 10, sensorData.CO, 3);	
	
	//显示空气质量数据
	OLED_ShowNum(3, 10, sensorData.AQI, 3);
}

/**
  * @brief  显示系统阈值设置界面1
  * @param  无
  * @retval 无
  */
void OLED_SetInterfacevoid(void)
{ 
		
	//显示一氧化碳阈值数值
	OLED_ShowNum(3, 12, Sensorthreshold.COValue, 3);
	
	//显示空气质量阈值数值
	OLED_ShowNum(4, 12, Sensorthreshold.AQIValue, 3);
}

/**
  * @brief  记录阈值界面下按KEY1的次数
  * @param  无
  * @retval 返回次数
  */
uint8_t SetSelection(void)
{
	static uint8_t count = 1;
	if(KeyNum == KEY_1)
	{
		KeyNum = 0;
		count++;
		if (count > 4)
		{
			count = 1;
		}
	}
	return count;
}

/**
  * @brief  显示阈值界面的选择符号
  * @param  num 为显示的位置
  * @retval 无
  */
void OLED_Option(uint8_t num)
{
	switch(num)
	{
		case 1:	
			OLED_ShowChar(1,1,'>');
			OLED_ShowChar(2,1,' ');
			OLED_ShowChar(3,1,' ');
			OLED_ShowChar(4,1,' ');
			break;
		case 2:	
			OLED_ShowChar(1,1,' ');
			OLED_ShowChar(2,1,'>');
			OLED_ShowChar(3,1,' ');
			OLED_ShowChar(4,1,' ');
			break;
		case 3:	
			OLED_ShowChar(1,1,' ');
			OLED_ShowChar(2,1,' ');
			OLED_ShowChar(3,1,'>');
			OLED_ShowChar(4,1,' ');
			break;
		case 4:	
			OLED_ShowChar(1,1,' ');
			OLED_ShowChar(2,1,' ');
			OLED_ShowChar(3,1,' ');
			OLED_ShowChar(4,1,'>');
			break;
		default: break;
	}
}

/**
  * @brief  对阈值界面的传感器阈值进行修改
  * @param  num 为当前用户需要更改的传感器阈值位置
  * @retval 无
  */
void ThresholdModification(uint8_t num)
{
	switch (num)
	{
		case 1:
			if (KeyNum == KEY_3)
			{
				KeyNum = 0;
				Sensorthreshold.luxValue += 10;
				if (Sensorthreshold.luxValue > 2000)
				{
					Sensorthreshold.luxValue = 0;
				}
			}
			else if (KeyNum == KEY_4)
			{
				KeyNum = 0;
				Sensorthreshold.luxValue -= 10;
				if (Sensorthreshold.luxValue > 2000)
				{
					Sensorthreshold.luxValue = 2000;
				}				
			}
			break;
			
		case 2:
			if (KeyNum == KEY_3)
			{
				KeyNum = 0;
				Sensorthreshold.smokeValue += 10;
				if (Sensorthreshold.smokeValue > 500)
				{
					Sensorthreshold.smokeValue = 0;
				}
			}
			else if (KeyNum == KEY_4)
			{
				KeyNum = 0;
				Sensorthreshold.smokeValue -= 10;
				if (Sensorthreshold.smokeValue > 500)
				{
					Sensorthreshold.smokeValue = 500;
				}				
			}	
			break;
		case 3:
			if (KeyNum == KEY_3)
			{
				KeyNum = 0;
				Sensorthreshold.COValue += 10;
				if (Sensorthreshold.COValue > 500)
				{
					Sensorthreshold.COValue = 0;
				}
			}
			else if (KeyNum == KEY_4)
			{
				KeyNum = 0;
				Sensorthreshold.COValue -= 10;
				if (Sensorthreshold.COValue > 500)
				{
					Sensorthreshold.COValue = 500;
				}				
			}
			break;
		case 4:
			if (KeyNum == KEY_3)
			{
				KeyNum = 0;
				Sensorthreshold.AQIValue += 10;
				if (Sensorthreshold.AQIValue > 500)
				{
					Sensorthreshold.AQIValue = 0;
				}
			}
			else if (KeyNum == KEY_4)
			{
				KeyNum = 0;
				Sensorthreshold.AQIValue -= 10;
				if (Sensorthreshold.AQIValue > 500)
				{
					Sensorthreshold.AQIValue = 500;
				}				
			}
			break;
		default: break;		
	}
}

/**
  * @brief  根据标志位控制步进电机的运行
  * @param  无
  * @retval 无
  */
void MotorOperation(void)
{
	if (motorFlag == 1)
	{
		MOTOR_Direction_Angle(1, 0, 90, 1);
		MOTOR_STOP();
		motorFlag = 0;
	}
 

}

/**
  * @brief  传感器数据扫描
  * @param  无
  * @retval 无
  */
void SensorScan(void)
{
	DHT11_Read_Data(&sensorData.humi, &sensorData.temp);
	Get_Average_LDR_LUX(&sensorData.lux); 
	sensorData.hPa = (uint32_t)BMP280_Get_Pressure();
}


int main(void)
{
	ADCX_Init();
    Timer2_Init(9,14398);
	Uart2_Init(9600);
	Uart1_Init(115200); 
	
	OLED_Init();
	 
	
	Sensorthreshold.luxValue = FLASH_R(FLASH_START_ADDR);	//从指定页的地址读FLASH
	Sensorthreshold.smokeValue = FLASH_R(FLASH_START_ADDR+2);	//从指定页的地址读FLASH 
	
	GENERAL_TIM_Init(); 
	while (1)
	{
			 
		SensorScan();	//获取传感器数据
		
		
		switch (oledPages)
		{
			case DISPLAY_PAGE1:
				OLED_Menu1();	//显示主页面1固定信息
				SensorDataDisplay1();//显示传感器1数据
				MotorOperation();
			
				/*按键按下时进入主页面2*/
				if (KeyNum == KEY_2)
				{
					KeyNum = 0;
					oledPages = DISPLAY_PAGE2;
					OLED_Clear();
				}
				
			  break;
				
			case DISPLAY_PAGE2:
				OLED_Menu2();
				SensorDataDisplay2();
				MotorOperation();
			
				/*按键按下时进入主页面2*/
				if (KeyNum == KEY_2)
				{
					KeyNum = 0;
					oledPages = DISPLAY_PAGE1;
					OLED_Clear();
				}				
				break;
			 
			default: break;
		}
		 			
	}
}
