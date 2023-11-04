/**
 ****************************************************************************************************
 * @file        key.c
 * @author      Mr.wang
 * @version     V1.0
 * @date        2020-08-12
 * @brief       急停按键 驱动代码
 ****************************************************************************************************
 */

#include "./BSP/KEY/key.h"
#include "./SYSTEM/delay/delay.h"


/**
 * @brief       按键初始化函数
 * @param       无
 * @retval      无
 */
void KEY_init(void)
{
    GPIO_InitTypeDef gpio_init_struct;
    KEY0_GPIO_CLK_ENABLE();                                     /* KEY0时钟使能 */

    gpio_init_struct.Pin = KEY0_GPIO_PIN;                       /* KEY0引脚 */
    gpio_init_struct.Mode = GPIO_MODE_INPUT;                    /* 输入 */
    gpio_init_struct.Pull = GPIO_PULLUP;                        /* 上拉 */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;              /* 高速 */
    HAL_GPIO_Init(KEY0_GPIO_PORT, &gpio_init_struct);           /* KEY0引脚模式设置,上拉输入 */

}
uint8_t previousKeyState = 0;
/**
 * @brief       按键扫描函数 带消抖 这里就不使用中断了 没必要
 */
uint8_t CHECK_KEY(void)
{
    uint8_t currentKeyState = HAL_GPIO_ReadPin(KEY0_GPIO_PORT, KEY0_GPIO_PIN); 
    if (currentKeyState != previousKeyState)
    {
        HAL_Delay(50);
        currentKeyState = HAL_GPIO_ReadPin(KEY0_GPIO_PORT, KEY0_GPIO_PIN);
        if (currentKeyState != previousKeyState)
        {
            previousKeyState = currentKeyState; 
            if (currentKeyState == GPIO_PIN_RESET)
            {
                return 1; 
            }
        }
    }
    return 0;
}




















