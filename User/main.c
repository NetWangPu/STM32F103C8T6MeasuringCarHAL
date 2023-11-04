/**
 ****************************************************************************************************
 * @file        main.c
 * @author      Mr.wang
 * @version     V1.0
 * @date        2023-08-11
 * @brief       测距车程序设计
 * 编码器   				PAO----A相   PA---B相
 * 电压采集 				PA4
 * 照明灯   				PA11
 * 电机驱动PWM 		PB1
 * 激光灯					PA6
 * 刹车制动器		 	PA2
 * 急停按键 				PA3
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

TIM_HandleTypeDef htim3;     /* 定时器3句柄 驱动电机 */
TIM_HandleTypeDef htim2;     /* 定时器2句柄 驱动编码器 */

uint16_t g_globalCarSpeed = 150;   /*车速 自己规定*/
uint16_t g_globalEmergencyStop = 0;  /*是否急停*/
uint16_t g_globalLaserIsOpen = 0; /*激光灯 0开 1关*/
uint16_t g_globalilluminationIsOpen = 0; /*照明灯 0开 1关*/
uint16_t g_globalmeasureOrder = 1; /*模式 0是清零，1是开启累加(后退自减)，2是停止计数*/
uint32_t batteryLevel = 0;  /*电池电量 */
long long xxt = -1;  //记录方向性记录溢出 初始值必须是-1 
long long g_globalDistance = 0;  /*当前测得距离 等于xxt*65535 + 寄存器当前的值*/
int stop_state = 0; //编码器暂停后启动标志位 类似于状态机
int count = 0;
int main(void)
{
		HAL_Init();                             /* 初始化HAL库 */
    sys_stm32_clock_init(RCC_PLL_MUL9);     /* 设置时钟为72Mhz */
    delay_init(72);                         /* 延时初始化 */
	
	
		/*************串口通讯工作******************/
    usart_init(115200);                     /* 串口初始化为115200 */
	
	
		/*************PWM工作******************/
		MX_TIM3_Init(2000-1,720-1,150);    /* 初始化PWM  第三个150正好刹车 */
	
	
		/*************Other工作******************/
    led_init();                             /* 初始化LED */
		adc_init();															/* 初始化ADC */
		KEY_init();
		BREAK_init();   												/* 初始化刹车制动 */
		BREAK(1); //关闭刹车制动
	
	
			/*************编码器工作******************/
		MX_TIM2_Init();   
		__HAL_TIM_ENABLE_IT(&htim2,TIM_IT_UPDATE);
		HAL_TIM_Encoder_Start(&htim2,TIM_CHANNEL_ALL);
		__HAL_TIM_CLEAR_IT(&htim2, TIM_IT_UPDATE);
		
    while (1)
    {
			
				//急停？ 刷新率够 就不用终端了 防止影响测量
				if(CHECK_KEY())
				{
						g_globalEmergencyStop= !g_globalEmergencyStop;
				}
				if(g_globalEmergencyStop == 1)
				{	
						SET_COMPARE(150);  //150 刹车PWM
						//开启刹车制动
						BREAK(0);
				}else
				{
						BREAK(1);
						//控制车速
						SET_COMPARE(g_globalCarSpeed);
				}
				
				
				//编码器测距离
				if(g_globalmeasureOrder == 0)//清零
				{
						if(stop_state)
						{
								HAL_TIM_Encoder_Start(&htim2,TIM_CHANNEL_ALL);
								stop_state  = 0;
						}
						g_globalDistance = 0;
						xxt = 0;
						__HAL_TIM_SET_COUNTER(&htim2,0); //清空寄存器当前值
				}
				else if (g_globalmeasureOrder == 1)//计数
				{	
						if(stop_state)
						{
								HAL_TIM_Encoder_Start(&htim2,TIM_CHANNEL_ALL);
								stop_state  = 0;
						}
						g_globalDistance = xxt*65535 +  Get_Value();  //获取编码器原始值  原始值
				}	
				else if (g_globalmeasureOrder == 2) //停止计数
				{
						HAL_TIM_Encoder_Stop(&htim2,TIM_CHANNEL_ALL);//编码器暂停工作
						stop_state = 1;
				}
				
				
				//获取当前电压   原始值
				batteryLevel = adc_get_result_average(ADC_ADCX_CHY,2);
				
				
				
				//控制LED与激光灯
				if(g_globalLaserIsOpen == 0) 
						LED1(1);
				else if(g_globalLaserIsOpen == 1)
						LED1(0);
				if(g_globalilluminationIsOpen == 0) 
						LED0(1);
				else if(g_globalilluminationIsOpen == 1)
						LED0(0);
				
				
				//串口发送数据
				count++;
				if(count == 20)
				{
					count = 0;
					//返回机器当前状态
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



