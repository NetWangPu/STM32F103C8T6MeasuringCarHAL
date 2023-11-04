/**
 ****************************************************************************************************
 * @file        key.c
 * @author      Mr.wang
 * @version     V1.0
 * @date        2020-08-12
 * @brief       ��ͣ���� ��������
 ****************************************************************************************************
 */

#include "./BSP/KEY/key.h"
#include "./SYSTEM/delay/delay.h"


/**
 * @brief       ������ʼ������
 * @param       ��
 * @retval      ��
 */
void KEY_init(void)
{
    GPIO_InitTypeDef gpio_init_struct;
    KEY0_GPIO_CLK_ENABLE();                                     /* KEY0ʱ��ʹ�� */

    gpio_init_struct.Pin = KEY0_GPIO_PIN;                       /* KEY0���� */
    gpio_init_struct.Mode = GPIO_MODE_INPUT;                    /* ���� */
    gpio_init_struct.Pull = GPIO_PULLUP;                        /* ���� */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;              /* ���� */
    HAL_GPIO_Init(KEY0_GPIO_PORT, &gpio_init_struct);           /* KEY0����ģʽ����,�������� */

}
uint8_t previousKeyState = 0;
/**
 * @brief       ����ɨ�躯�� ������ ����Ͳ�ʹ���ж��� û��Ҫ
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




















