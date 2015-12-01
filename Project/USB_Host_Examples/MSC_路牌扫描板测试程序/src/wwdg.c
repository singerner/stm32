#include "wwdg.h"

 /*保存WWDG计数器的设置值,默认为最大.*/ 
uint8_t WWDG_CNT=0x7f; 

void wwdg_init(u8 tr,u8 wr,u32 fprer)
{ 	
	NVIC_InitTypeDef NVIC_InitStructure;
 	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG,ENABLE); //使能窗口看门狗时钟
	
	WWDG_CNT = tr & WWDG_CNT;   //初始化WWDG_CNT. 
	WWDG_SetPrescaler(fprer); //设置分频值
	WWDG_SetWindowValue(wr); //设置窗口值
	WWDG_SetCounter(WWDG_CNT);
	WWDG_Enable(WWDG_CNT);  //开启看门狗
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel=WWDG_IRQn;  //窗口看门狗中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x02;  //抢占优先级为2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03;					//子优先级为3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;  //使能窗口看门狗
	NVIC_Init(&NVIC_InitStructure);
	
	WWDG_ClearFlag();//清除提前唤醒中断标志位
    WWDG_EnableIT();//开启提前唤醒中断
}


//窗口看门狗中断服务程序
void WWDG_IRQHandler(void)
{
	uint8_t wr, tr;
	static uint8_t flag = 0;
	flag ++;
	if(flag > 200)
	{
		flag = 0;
	}
	wr = WWDG->CFR & 0x7F;//计数值
	tr = WWDG->CR & 0x7F;//计数值
	if(tr < wr)
	{
		WWDG_SetCounter(WWDG_CNT); //重设窗口看门狗值
		WWDG_ClearFlag();//清除提前唤醒中断标志位	
		if(flag % 2 == 0)
		{
			cpu_run();
		}
		else
		{
			cpu_stop();
		}
	}
}

