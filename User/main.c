/**
 ****************************************************************************************************
 * @file        main.c
 * @author      Mr.wang
 * @version     V1.0
 * @date        2023-08-11
 * @brief       ��೵�������
 * ������   				PAO----A��   PA---B��
 * ��ѹ�ɼ� 				PA4
 * ������   				PA11
 * �������PWM 		PB1
 * �����					PA6
 * ɲ���ƶ���		 	PA2
 * ��ͣ���� 				PA3
 ****************************************************************************************************
 */
#include "stdio.h"
#include "./stm32f1xx_it.h"
#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/LED/led.h"
#include "./BSP/TIMER/encoder.h"
#include "./BSP/TIMER/motor.h"
#include "./BSP/ADC/adc.h"
#include "./BSP/KEY/key.h"
#include "./BSP/BREAK/break.h"

TIM_HandleTypeDef htim3;     /* ��ʱ��3��� ������� */
TIM_HandleTypeDef htim2;     /* ��ʱ��2��� ���������� */

uint16_t g_globalCarSpeed = 150;   /*���� �Լ��涨*/
uint16_t g_globalEmergencyStop = 0;  /*�Ƿ�ͣ*/
uint16_t g_globalLaserIsOpen = 0; /*����� 0�� 1��*/
uint16_t g_globalilluminationIsOpen = 0; /*������ 0�� 1��*/
uint16_t g_globalmeasureOrder = 1; /*ģʽ 0�����㣬1�ǿ����ۼ�(�����Լ�)��2��ֹͣ����*/
uint32_t batteryLevel = 0;  /*��ص��� */
long long xxt = -1;  //��¼�����Լ�¼��� ��ʼֵ������-1 
long long g_globalDistance = 0;  /*��ǰ��þ��� ����xxt*65535 + �Ĵ�����ǰ��ֵ*/
int stop_state = 0; //��������ͣ��������־λ ������״̬��
int count = 0;
int main(void)
{
		HAL_Init();                             /* ��ʼ��HAL�� */
    sys_stm32_clock_init(RCC_PLL_MUL9);     /* ����ʱ��Ϊ72Mhz */
    delay_init(72);                         /* ��ʱ��ʼ�� */
	
	
		/*************����ͨѶ����******************/
    usart_init(115200);                     /* ���ڳ�ʼ��Ϊ115200 */
	
	
		/*************PWM����******************/
		MX_TIM3_Init(2000-1,720-1,150);    /* ��ʼ��PWM  ������150����ɲ�� */
	
	
		/*************Other����******************/
    led_init();                             /* ��ʼ��LED */
		adc_init();															/* ��ʼ��ADC */
		KEY_init();
		BREAK_init();   												/* ��ʼ��ɲ���ƶ� */
		BREAK(1); //�ر�ɲ���ƶ�
	
	
			/*************����������******************/
		MX_TIM2_Init();   
		__HAL_TIM_ENABLE_IT(&htim2,TIM_IT_UPDATE);
		HAL_TIM_Encoder_Start(&htim2,TIM_CHANNEL_ALL);
		__HAL_TIM_CLEAR_IT(&htim2, TIM_IT_UPDATE);
		
    while (1)
    {
			
				//��ͣ�� ˢ���ʹ� �Ͳ����ն��� ��ֹӰ�����
				if(CHECK_KEY())
				{
						g_globalEmergencyStop= !g_globalEmergencyStop;
				}
				if(g_globalEmergencyStop == 1)
				{	
						SET_COMPARE(150);  //150 ɲ��PWM
						//����ɲ���ƶ�
						BREAK(0);
				}else
				{
						BREAK(1);
						//���Ƴ���
						SET_COMPARE(g_globalCarSpeed);
				}
				
				
				//�����������
				if(g_globalmeasureOrder == 0)//����
				{
						if(stop_state)
						{
								HAL_TIM_Encoder_Start(&htim2,TIM_CHANNEL_ALL);
								stop_state  = 0;
						}
						g_globalDistance = 0;
						xxt = 0;
						__HAL_TIM_SET_COUNTER(&htim2,0); //��ռĴ�����ǰֵ
				}
				else if (g_globalmeasureOrder == 1)//����
				{	
						if(stop_state)
						{
								HAL_TIM_Encoder_Start(&htim2,TIM_CHANNEL_ALL);
								stop_state  = 0;
						}
						g_globalDistance = xxt*65535 +  Get_Value();  //��ȡ������ԭʼֵ  ԭʼֵ
				}	
				else if (g_globalmeasureOrder == 2) //ֹͣ����
				{
						HAL_TIM_Encoder_Stop(&htim2,TIM_CHANNEL_ALL);//��������ͣ����
						stop_state = 1;
				}
				
				
				//��ȡ��ǰ��ѹ   ԭʼֵ
				batteryLevel = adc_get_result_average(ADC_ADCX_CHY,2);
				
				
				
				//����LED�뼤���
				if(g_globalLaserIsOpen == 0) 
						LED1(1);
				else if(g_globalLaserIsOpen == 1)
						LED1(0);
				if(g_globalilluminationIsOpen == 0) 
						LED0(1);
				else if(g_globalilluminationIsOpen == 1)
						LED0(0);
				
				
				//���ڷ�������
				count++;
				if(count == 20)
				{
					count = 0;
					//���ػ�����ǰ״̬
					printf("#1#%d#",g_globalCarSpeed);   
					printf("%d#",g_globalEmergencyStop);   
					printf("%d#",g_globalLaserIsOpen);
					printf("%d#",g_globalilluminationIsOpen);
					printf("%lld#",g_globalDistance);
					printf("%d#",batteryLevel);
					printf("%d\r\n",g_globalmeasureOrder);
					
				}
    }
}



