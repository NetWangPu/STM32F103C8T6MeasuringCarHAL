/**
 ****************************************************************************************************
 * @file        break.c
 * @author      Mr.wang
 * @version     V1.0
 * @date        2020-08-12
 * @brief       �����ƶ� ��������
 ****************************************************************************************************
 */

#include "./BSP/BREAK/break.h"

/**
 * @brief       ɲ����ʼ��
 * @param       ��
 * @retval      ��
 */
void BREAK_init(void)
{
    GPIO_InitTypeDef gpio_init_struct;
    BREAK_GPIO_CLK_ENABLE();                                 /* LED0ʱ��ʹ�� */

    gpio_init_struct.Pin = BREAK_GPIO_PIN;                   /* LED0���� */
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;            /* ������� */
    gpio_init_struct.Pull = GPIO_PULLUP;                    /* ���� */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;          /* ���� */
    HAL_GPIO_Init(BREAK_GPIO_PORT, &gpio_init_struct);       /* ��ʼ��LED0���� */
}
