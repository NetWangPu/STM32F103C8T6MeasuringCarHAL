#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "string.h"
#include "stdlib.h"

/* ���ʹ��os,����������ͷ�ļ�����. */
#if SYS_SUPPORT_OS
#include "includes.h" /* os ʹ�� */
#endif

/******************************************************************************************/
/* �������´���, ֧��printf����, ������Ҫѡ��use MicroLIB */

#if 1

#if (__ARMCC_VERSION >= 6010050)            /* ʹ��AC6������ʱ */
__asm(".global __use_no_semihosting\n\t");  /* ������ʹ�ð�����ģʽ */
__asm(".global __ARM_use_no_argv \n\t");    /* AC6����Ҫ����main����Ϊ�޲�����ʽ�����򲿷����̿��ܳ��ְ�����ģʽ */

#else
/* ʹ��AC5������ʱ, Ҫ�����ﶨ��__FILE �� ��ʹ�ð�����ģʽ */
#pragma import(__use_no_semihosting)

struct __FILE
{
    int handle;
    /* Whatever you require here. If the only file you are using is */
    /* standard output using printf() for debugging, no file handling */
    /* is required. */
};

#endif

/* ��ʹ�ð�����ģʽ��������Ҫ�ض���_ttywrch\_sys_exit\_sys_command_string����,��ͬʱ����AC6��AC5ģʽ */
int _ttywrch(int ch)
{
    ch = ch;
    return ch;
}

/* ����_sys_exit()�Ա���ʹ�ð�����ģʽ */
void _sys_exit(int x)
{
    x = x;
}

char *_sys_command_string(char *cmd, int len)
{
    return NULL;
}


/* FILE �� stdio.h���涨��. */
FILE __stdout;

/* MDK����Ҫ�ض���fputc����, printf�������ջ�ͨ������fputc����ַ��������� */
int fputc(int ch, FILE *f)
{
    while ((USART_UX->SR & 0X40) == 0);     /* �ȴ���һ���ַ�������� */

    USART_UX->DR = (uint8_t)ch;             /* ��Ҫ���͵��ַ� ch д�뵽DR�Ĵ��� */
    return ch;
}
#endif
/******************************************************************************************/

#if USART_EN_RX /*���ʹ���˽���*/

/* ���ջ���, ���USART_REC_LEN���ֽ�. */
uint8_t g_usart_rx_buf[USART_REC_LEN];
char g_usart_rx_buf_char[USART_REC_LEN];
/*  ����״̬
 *  bit15��      ������ɱ�־
 *  bit14��      ���յ�0x0d
 *  bit13~0��    ���յ�����Ч�ֽ���Ŀ
*/
uint16_t g_usart_rx_sta = 0;

uint8_t g_rx_buffer[RXBUFFERSIZE];  /* HAL��ʹ�õĴ��ڽ��ջ��� */

UART_HandleTypeDef g_uart1_handle;  /* UART��� */

/**
 * @brief       ����X��ʼ������
 * @param       baudrate: ������, �����Լ���Ҫ���ò�����ֵ
 * @note        ע��: ����������ȷ��ʱ��Դ, ���򴮿ڲ����ʾͻ������쳣.
 *              �����USART��ʱ��Դ��sys_stm32_clock_init()�������Ѿ����ù���.
 * @retval      ��
 */
void usart_init(uint32_t baudrate)
{
    /*UART ��ʼ������*/
    g_uart1_handle.Instance = USART_UX;                                       /* USART_UX */
    g_uart1_handle.Init.BaudRate = baudrate;                                  /* ������ */
    g_uart1_handle.Init.WordLength = UART_WORDLENGTH_8B;                      /* �ֳ�Ϊ8λ���ݸ�ʽ */
    g_uart1_handle.Init.StopBits = UART_STOPBITS_1;                           /* һ��ֹͣλ */
    g_uart1_handle.Init.Parity = UART_PARITY_NONE;                            /* ����żУ��λ */
    g_uart1_handle.Init.HwFlowCtl = UART_HWCONTROL_NONE;                      /* ��Ӳ������ */
    g_uart1_handle.Init.Mode = UART_MODE_TX_RX;                               /* �շ�ģʽ */
    HAL_UART_Init(&g_uart1_handle);                                           /* HAL_UART_Init()��ʹ��UART1 */

    /* �ú����Ὺ�������жϣ���־λUART_IT_RXNE���������ý��ջ����Լ����ջ��������������� */
    HAL_UART_Receive_IT(&g_uart1_handle, (uint8_t *)g_rx_buffer, RXBUFFERSIZE); 
}

/**
 * @brief       UART�ײ��ʼ������
 * @param       huart: UART�������ָ��
 * @note        �˺����ᱻHAL_UART_Init()����
 *              ���ʱ��ʹ�ܣ��������ã��ж�����
 * @retval      ��
 */
void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    GPIO_InitTypeDef gpio_init_struct;

    if (huart->Instance == USART_UX)                            /* ����Ǵ���1�����д���1 MSP��ʼ�� */
    {
        USART_TX_GPIO_CLK_ENABLE();                             /* ʹ�ܴ���TX��ʱ�� */
        USART_RX_GPIO_CLK_ENABLE();                             /* ʹ�ܴ���RX��ʱ�� */
        USART_UX_CLK_ENABLE();                                  /* ʹ�ܴ���ʱ�� */

        gpio_init_struct.Pin = USART_TX_GPIO_PIN;               /* ���ڷ������ź� */
        gpio_init_struct.Mode = GPIO_MODE_AF_PP;                /* ����������� */
        gpio_init_struct.Pull = GPIO_PULLUP;                    /* ���� */
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;          /* IO�ٶ�����Ϊ���� */
        HAL_GPIO_Init(USART_TX_GPIO_PORT, &gpio_init_struct);
                
        gpio_init_struct.Pin = USART_RX_GPIO_PIN;               /* ����RX�� ģʽ���� */
        gpio_init_struct.Mode = GPIO_MODE_AF_INPUT;    
        HAL_GPIO_Init(USART_RX_GPIO_PORT, &gpio_init_struct);   /* ����RX�� �������ó�����ģʽ */
        
#if USART_EN_RX
        HAL_NVIC_EnableIRQ(USART_UX_IRQn);                      /* ʹ��USART1�ж�ͨ�� */
        HAL_NVIC_SetPriority(USART_UX_IRQn, 3, 3);              /* ��2��������ȼ�:��ռ���ȼ�3�������ȼ�3 */
#endif
    }
}


extern uint16_t g_globalCarSpeed, g_globalLaserIsOpen,g_globalilluminationIsOpen,g_globalmeasureOrder,g_globalEmergencyStop;
extern long long g_globalDistance;
char *instruction1;
char *instruction2;
char *instruction3;
char *instruction4;
char *instruction5;
char *instruction6;

/**
 * @brief       �������ݽ��ս���ָ���
                ���ݽ������������
 * @param       ��
 * @retval      ��
 */
void parseCommand(void)
{
    //����ָ�� �ո�ָ�
		for (size_t i = 0; i < USART_REC_LEN; i++) {
			g_usart_rx_buf_char[i] = (char)g_usart_rx_buf[i];
		}
    instruction1 = strtok(g_usart_rx_buf_char, "#"); //�ӵ��շ���־λ
		instruction1 = strtok(NULL, "#");
    instruction2 = strtok(NULL, "#");
	  instruction3 = strtok(NULL, "#");
    instruction4 = strtok(NULL, "#");
    instruction5 = strtok(NULL, "#");
		instruction6 = strtok(NULL, "#");
    //ת��Ϊ����
    g_globalCarSpeed = atoi(instruction1);
		//ɲ��
		if(atoi(instruction2))
		{
						g_globalEmergencyStop= !g_globalEmergencyStop;
		}
    g_globalLaserIsOpen = atoi(instruction3);
		g_globalilluminationIsOpen = atoi(instruction4);
		g_globalmeasureOrder = atoi(instruction5);
    //�����жϷ�ֹû�õĲ��� ��
    //��ӡ������Ϣ
#ifdef OPENDEBUG		
    printf("g_globalCarSpeed:%d\r\n",g_globalCarSpeed);
		printf("g_globalEmergencyStop:%d\r\n",g_globalEmergencyStop);
		printf("g_globalLaserIsOpen:%d \r\n",g_globalLaserIsOpen);
		printf("g_globalilluminationIsOpen:%d\r\n",g_globalilluminationIsOpen);
		printf("g_globalmeasureOrder:%d\r\n",g_globalmeasureOrder);
#endif
}

/**
 * @brief       �������ݽ��ջص�����
                ���ݴ������������
 * @param       huart:���ھ��
 * @retval      ��
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART_UX)                    /* ����Ǵ���1 */
    {
        if ((g_usart_rx_sta & 0x8000) == 0)             /* ����δ��� */
        {
            if (g_usart_rx_sta & 0x4000)                /* ���յ���0x0d�����س����� */
            {
                if (g_rx_buffer[0] != 0x0a)             /* ���յ��Ĳ���0x0a�������ǻ��м��� */
                {
                    g_usart_rx_sta = 0;                 /* ���մ���,���¿�ʼ */
                }
                else                                    /* ���յ�����0x0a�������м��� */
                {
                    g_usart_rx_sta |= 0x8000;           /* ��������� */
										//���ָ��Ŀ�ͷ��#�����ʾ�ǿ���ָ�� ����
										//if (g_usart_rx_buf[0] == '#'||g_usart_rx_buf[1] == '#')
										if (g_usart_rx_buf[0] == '#'&& g_usart_rx_buf [1] == '0')
										{
												parseCommand();
										}
										else
										{
												printf("error received:%s\r\n", g_usart_rx_buf);
												//���received_command[0]
												//printf("received_command[0]:%c\r\n", g_usart_rx_buf[0]);
												//printf("received_command[1]:%c\r\n", g_usart_rx_buf[1]);
										}
										g_usart_rx_sta = 0;
										memset(g_usart_rx_buf, 0, sizeof(g_usart_rx_buf));
                }
            }
            else                                        /* ��û�յ�0X0d�����س����� */
            {
                if (g_rx_buffer[0] == 0x0d)
                    g_usart_rx_sta |= 0x4000;
                else
                {
                    g_usart_rx_buf[g_usart_rx_sta & 0X3FFF] = g_rx_buffer[0];
                    g_usart_rx_sta++;

                    if (g_usart_rx_sta > (USART_REC_LEN - 1))
                    {
                        g_usart_rx_sta = 0;             /* �������ݴ���,���¿�ʼ���� */
                    }
                }
            }
        }
    }
}

/**
 * @brief       ����X�жϷ�����
                ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���
 * @param       ��
 * @retval      ��
 */
void USART_UX_IRQHandler(void)
{
#if SYS_SUPPORT_OS                                                   /* ʹ��OS */
    OSIntEnter();
#endif
    HAL_UART_IRQHandler(&g_uart1_handle);                               /* ����HAL���жϴ����ú��� */

    while (HAL_UART_Receive_IT(&g_uart1_handle, (uint8_t *)g_rx_buffer, RXBUFFERSIZE) != HAL_OK)     /* ���¿����жϲ��������� */
    {
        /* �������Ῠ�������� */
    }

#if SYS_SUPPORT_OS                                                   /* ʹ��OS */
    OSIntExit();
#endif
}
#endif
