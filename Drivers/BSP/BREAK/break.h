/**
 ****************************************************************************************************
 * @file        break.h
 * @author      Mr.wang
 * @version     V1.0
 * @date        2020-08-12
 * @brief       �����ƶ� ��������
 ****************************************************************************************************
 */

#ifndef _BREAK_H
#define _BREAK_H
#include "./SYSTEM/sys/sys.h"



#define BREAK_GPIO_PORT                  GPIOA
#define BREAK_GPIO_PIN                   GPIO_PIN_2
#define BREAK_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)             /* PB��ʱ��ʹ�� */


#define BREAK(x)   do{ x ? \
                      HAL_GPIO_WritePin(BREAK_GPIO_PORT, BREAK_GPIO_PIN, GPIO_PIN_SET) : \
                      HAL_GPIO_WritePin(BREAK_GPIO_PORT, BREAK_GPIO_PIN, GPIO_PIN_RESET); \
                  }while(0)      

void BREAK_init(void);                                                                            /* ��ʼ�� */

#endif
