#include "key.h"
#include "Delay.h"

 uint8_t key_cnt = 0;
 uint8_t bkey_flag = 0;

void key_init()
{
		NVIC_InitTypeDef 	NVIC_InitStructure;
		GPIO_InitTypeDef  GPIO_InitStruct;   
		EXTI_InitTypeDef 	EXTI_InitStructure;
		
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
	
		GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_0 | GPIO_Pin_1;
		//GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;	
		GPIO_Init(GPIOB, &GPIO_InitStruct);

		GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource0);
	
		EXTI_InitStructure.EXTI_Line = EXTI_Line0;
		EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
		EXTI_InitStructure.EXTI_LineCmd = ENABLE;
		EXTI_Init(&EXTI_InitStructure);
	
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	
	  NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;          //指定中断源
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;        //指定响应优先级别1
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	        //使能外部中断通道
		NVIC_Init(&NVIC_InitStructure);
		
}

void EXTI0_IRQHandler(void)
{
		delay_ms(5);
		if(KEY_0 == 1)
		{
				bkey_flag = 1;
				key_cnt ++;
				if(key_cnt > 3)
				{
					key_cnt = 0;
				}
		}
		EXTI_ClearITPendingBit(EXTI_Line0);
}