/**
 ****************************************************************************************************
 * @file        break.c
 * @author      Mr.wang
 * @version     V1.0
 * @date        2020-08-12
 * @brief       紧急制动 驱动代码
 ****************************************************************************************************
 */

#include "./BSP/BREAK/break.h"

/**
 * @brief       刹车初始化
 * @param       无
 * @retval      无
 */
void BREAK_init(void)
{
    GPIO_InitTypeDef gpio_init_struct;
    BREAK_GPIO_CLK_ENABLE();                                 /* LED0时钟使能 */

    gpio_init_struct.Pin = BREAK_GPIO_PIN;                   /* LED0引脚 */
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;            /* 推挽输出 */
    gpio_init_struct.Pull = GPIO_PULLUP;                    /* 上拉 */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;          /* 高速 */
    HAL_GPIO_Init(BREAK_GPIO_PORT, &gpio_init_struct);       /* 初始化LED0引脚 */
}
