#include "timer.h"
/**-------------------------------------------------------
  * @������ NVIC_TIM5Configuration
  * @����   ����TIM5�ж�������������
  * @����   ��
  * @����ֵ ��
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
  * @������ NVIC_TIM5Configuration
  * @����   ����TIM5����������ÿ��������ж�һ�� 
  * @����   ��
  * @����ֵ ��
***------------------------------------------------------*/
void tim5_init(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    /* TIM5 clock enable */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
    /* �Զ���װ�ؼĴ������ڵ�ֵ(����ֵ) */ 
    TIM_TimeBaseStructure.TIM_Period = (10000 - 1);
	/* �ۼ� TIM_Period��Ƶ�ʺ����һ�����»����ж� */
    /* �������Ԥ��Ƶϵ��*/
    TIM_TimeBaseStructure.TIM_Prescaler = (7200 - 1);
    /* ������Ƶ */
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    /* ���ϼ���ģʽ */
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    /* ��ʼ����ʱ��5 */
    TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);
    /* �������жϱ�־ */
    TIM_ClearITPendingBit(TIM5, TIM_IT_Update);
    /* ����ж�ʹ�� */
    TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);
    /* ������ʹ�� */
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

