#include "timer.h"
/**-------------------------------------------------------
  * @函数名 NVIC_TIM5Configuration
  * @功能   配置TIM5中断向量参数函数
  * @参数   无
  * @返回值 无
***------------------------------------------------------*/
void nvic_tim5configuration(void)
{ 
    NVIC_InitTypeDef NVIC_InitStructure;

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

    /* Enable the TIM5 gloabal Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

    NVIC_Init(&NVIC_InitStructure);
}

/**-------------------------------------------------------
  * @函数名 NVIC_TIM5Configuration
  * @功能   配置TIM5参数函数，每秒计数器中断一次 
  * @参数   无
  * @返回值 无
***------------------------------------------------------*/
void tim5_init(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    /* TIM5 clock enable */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
    /* 自动重装载寄存器周期的值(计数值) */ 
    TIM_TimeBaseStructure.TIM_Period = (10000 - 1);
	/* 累计 TIM_Period个频率后产生一个更新或者中断 */
    /* 这个就是预分频系数*/
    TIM_TimeBaseStructure.TIM_Prescaler = (7200 - 1);
    /* 采样分频 */
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    /* 向上计数模式 */
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    /* 初始化定时器5 */
    TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);
    /* 清除溢出中断标志 */
    TIM_ClearITPendingBit(TIM5, TIM_IT_Update);
    /* 溢出中断使能 */
    TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);
    /* 计数器使能 */
    TIM_Cmd(TIM5, ENABLE); 
}



void TIM5_IRQHandler(void)
{ 
    static u32 counter = 0;
    if(TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET)
    {		
		if(counter > 1000)
		{
			counter = 0;
		}
		counter ++;
		if(counter % 2 == 0)
		{
			flash_run();
		}
		else
		{
			flash_stop();
		}	
        TIM_ClearITPendingBit(TIM5, TIM_IT_Update);  
    }
}

