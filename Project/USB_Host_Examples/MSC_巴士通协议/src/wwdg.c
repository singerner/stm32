#include "wwdg.h"

 /*����WWDG������������ֵ,Ĭ��Ϊ���.*/ 
uint8_t WWDG_CNT=0x7f; 

void wwdg_init(u8 tr,u8 wr,u32 fprer)
{ 	
	NVIC_InitTypeDef NVIC_InitStructure;
 	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG,ENABLE); //ʹ�ܴ��ڿ��Ź�ʱ��
	
	WWDG_CNT = tr & WWDG_CNT;   //��ʼ��WWDG_CNT. 
	WWDG_SetPrescaler(fprer); //���÷�Ƶֵ
	WWDG_SetWindowValue(wr); //���ô���ֵ
	WWDG_SetCounter(WWDG_CNT);
	WWDG_Enable(WWDG_CNT);  //�������Ź�
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel=WWDG_IRQn;  //���ڿ��Ź��ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x02;  //��ռ���ȼ�Ϊ2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03;					//�����ȼ�Ϊ3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;  //ʹ�ܴ��ڿ��Ź�
	NVIC_Init(&NVIC_InitStructure);
	
	WWDG_ClearFlag();//�����ǰ�����жϱ�־λ
    WWDG_EnableIT();//������ǰ�����ж�
}


//���ڿ��Ź��жϷ������
void WWDG_IRQHandler(void)
{
	uint8_t wr, tr;
	static uint8_t flag = 0;
	flag ++;
	if(flag > 200)
	{
		flag = 0;
	}
	wr = WWDG->CFR & 0x7F;//����ֵ
	tr = WWDG->CR & 0x7F;//����ֵ
	if(tr < wr)
	{
		WWDG_SetCounter(WWDG_CNT); //���贰�ڿ��Ź�ֵ
		WWDG_ClearFlag();//�����ǰ�����жϱ�־λ	
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

