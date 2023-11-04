/**
 ****************************************************************************************************
 * @file        gtim.c
 * @author      Mr.wang
 * @version     V1.1
 * @date        2023-08-12
 * @brief       电机驱动代码  输出PWM 
 ****************************************************************************************************
 */
#include "./BSP/TIMER/motor.h"
#include "./SYSTEM/usart/usart.h"

extern TIM_HandleTypeDef htim3; /* 定时器x句柄 */


void MX_TIM3_Init(uint16_t arr,uint16_t psc,uint16_t defarr)
{
		TIM_OC_InitTypeDef sConfigOC = {0};
		htim3.Instance = TIM3;
		htim3.Init.Prescaler = psc;
		htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
		htim3.Init.Period = arr;
		if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
		{
				printf("HAL_TIM_PWM_Init error");
		}
		sConfigOC.OCMode = TIM_OCMODE_PWM1;
		sConfigOC.Pulse = defarr;
		sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH; //TIM_OCPOLARITY_LOW //TIM_OCPOLARITY_HIGH
		if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
		{
				printf("HAL_TIM_PWM_ConfigChannel error");
		}
		HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_4);
}

void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef* timHandle)
{
		GPIO_InitTypeDef GPIO_InitStruct = {0};
		if(timHandle->Instance==TIM3)
		{		
				__HAL_RCC_TIM3_CLK_ENABLE();
				__HAL_RCC_GPIOB_CLK_ENABLE();
				/**TIM3 GPIO Configuration
				PB1     ------> TIM3_CH4
				*/
				GPIO_InitStruct.Pin = GPIO_PIN_1;
				GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
				GPIO_InitStruct.Pull = GPIO_PULLUP;               /* 下拉 */
				GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
				HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
		}
}

void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* tim_baseHandle)
{
		if(tim_baseHandle->Instance==TIM3)
		{
				__HAL_RCC_TIM3_CLK_DISABLE();
		}
}


void SET_COMPARE(uint16_t arr)
{
		/* 修改比较值控制占空比 */
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, arr);
}




