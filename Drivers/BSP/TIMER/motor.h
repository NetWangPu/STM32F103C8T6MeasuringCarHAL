#ifndef __MOTOR_H
#define __MOTOR_H

#include "./SYSTEM/sys/sys.h"

void MX_TIM3_Init(uint16_t arr,uint16_t psc,uint16_t defarr);
void HAL_TIM_MspPostInit(TIM_HandleTypeDef* timHandle);
void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* tim_baseHandle);
void SET_COMPARE(uint16_t arr);

#endif

















