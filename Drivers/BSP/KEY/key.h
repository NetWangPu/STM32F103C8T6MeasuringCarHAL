/**
 ****************************************************************************************************
 * @file        key.h
 * @author      Mr.wang
 * @version     V1.0
 * @date        2020-08-12
 * @brief       急停按键 驱动代码
 ****************************************************************************************************
 */

#ifndef __KEY_H
#define __KEY_H

#include "./SYSTEM/sys/sys.h"


#define KEY0_GPIO_PORT                  GPIOA
#define KEY0_GPIO_PIN                   GPIO_PIN_3
#define KEY0_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0) 

void KEY_init(void);                /* 按键初始化函数 */
uint8_t CHECK_KEY(void);

#endif


















