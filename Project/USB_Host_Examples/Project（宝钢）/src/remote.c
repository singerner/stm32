#include "remote.h"

void remote_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_ICInitTypeDef  TIM_ICInitStructure;  
 
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE); //ʹ��PORTBʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);	//TIM3 ʱ��ʹ�� 
	
	//GPIO_AFIODeInit();
	GPIO_PinRemapConfig(GPIO_FullRemap_TIM3, ENABLE);//ȫӳ��
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;				 //PC8 ���� 
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; 		//�������� 
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_SetBits(GPIOC, GPIO_Pin_8);	//��ʼ��GPIOC.9
						  
 	TIM_TimeBaseStructure.TIM_Period = 10000; //�趨�������Զ���װֵ ���10ms���  
	TIM_TimeBaseStructure.TIM_Prescaler =(72-1); 	//Ԥ��Ƶ��,1M�ļ���Ƶ��,1us��1.	   
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx

	TIM_ICInitStructure.TIM_Channel = TIM_Channel_3;  // ѡ������� IC3ӳ�䵽TI3��
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//�����ز���
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //���������Ƶ,����Ƶ 
	TIM_ICInitStructure.TIM_ICFilter = 0x03;//IC3F=0011 ���������˲��� 8����ʱ��ʱ�������˲�
	TIM_ICInit(TIM3, &TIM_ICInitStructure);//��ʼ����ʱ�����벶��ͨ��

	TIM_Cmd(TIM3, ENABLE); 	//ʹ�ܶ�ʱ��3

	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���	

	TIM_ITConfig(TIM3, TIM_IT_Update | TIM_IT_CC3,ENABLE);//��������ж� ,����CC3IE�����ж�							
}

uint8_t  RmtSta = 0;	//0000 0000  7λ���������Ƿ���ɱ�־λ��3λ�Ǹߵ�ƽ�Ƿ񲶻��־λ
uint16_t Dval;		//�½���ʱ��������ֵ
uint32_t RmtRec = 0;	//������յ�������   		    
uint8_t  RmtCnt = 0;	//�������µĴ���  
uint8_t  keyflag = 0;


void TIM3_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
		{
				if(RmtSta & 0x80)//�ϴ������ݱ����յ���
				{	
						RmtSta	&=	(~0x10);						//ȡ���������Ѿ���������
						if((RmtSta & 0x0F) == 0x00)
								RmtSta |= 1 << 6;//����Ѿ����һ�ΰ����ļ�ֵ��Ϣ�ɼ�
						if((RmtSta & 0x0F) < 14)
								RmtSta ++;
						else
						{
								RmtSta &= ~(1 << 7);//���������ʶ
								RmtSta &= 0xF0;	//��ռ�����	
						}						 	   	
				}							    
		}
		if(TIM_GetITStatus(TIM3, TIM_IT_CC3) != RESET)
		{	  
			if(RDATA)//�����ز���
			{
					TIM_OC3PolarityConfig(TIM3,	TIM_ICPolarity_Falling);		//CC1P=1 ����Ϊ�½��ز���				
					TIM_SetCounter(TIM3, 0);	   	//��ն�ʱ��ֵ
					RmtSta |= 0x10;					//����������Ѿ�������
			}
			else //�½��ز���
			{			
					 Dval = TIM_GetCapture3(TIM3);//��ȡCCR1Ҳ������CC1IF��־λ
					 TIM_OC3PolarityConfig(TIM3, TIM_ICPolarity_Rising); //CC3P=0	����Ϊ�����ز���
					 if(RmtSta & 0x10)			//���һ�θߵ�ƽ���� 
					 {
								if(RmtSta & 0x80)//���յ���������
								{
									 if(Dval > 300 && Dval < 800)			//560Ϊ��׼ֵ,560us
									 {
												RmtRec <<= 1;	//����һλ.
												RmtRec |= 0;	//���յ�0	   
									 }
									else if(Dval > 1400 && Dval < 1800)	//1680Ϊ��׼ֵ,1680us
									{
												RmtRec <<= 1;	//����һλ.
												RmtRec |= 1;	//���յ�1
									}
								 else if(Dval > 2200 && Dval < 2600)	//�õ�������ֵ���ӵ���Ϣ 2500Ϊ��׼ֵ2.5ms
								 {
											RmtCnt ++; 		//������������1��
											RmtSta &= 0xF0;	//��ռ�ʱ��	
											keyflag = 1;
										
								}
							}
							else if(Dval > 4200 && Dval < 4700)		//4500Ϊ��׼ֵ4.5ms
							{
									RmtSta |= 1 << 7;	//��ǳɹ����յ���������
									RmtCnt = 0;		//�����������������
							}						 
					}
					RmtSta &= ~(1 << 4);//����ߵ�ƽ�����־λ
			}				 		     	    					   
		}
		TIM_ClearFlag(TIM3, TIM_IT_Update | TIM_IT_CC3);	    
}

u8 Remote_Scan(void)
{        
	 uint8_t sta=0, temp =  0;       
   uint8_t key_value = 0, num ;
	 uint8_t t1, t2;
	 if(RmtSta&(1 << 6))//�õ�һ��������������Ϣ��
	 { 
			t1=RmtRec >> 24;			//�õ���ַ��
	    t2=(RmtRec >> 16)&0xff;	//�õ���ַ���� 
 	    if(0xFD == t2)
	    { 
	        t1=RmtRec >> 8;
	        t2 = RmtRec; 	
	        if(t1 == (uint8_t)~t2)
					{
							sta = t1;//��ֵ��ȷ
							for(num = 0; num <= 7; num ++)
							{
									temp = (sta >> num) & 0x01;					
									key_value = key_value | (temp << (7 - num));
							}
					}
					if(0 == key_value)
					{
							key_value ++;
					}
					RmtRec = 0;
			} 
			if((RmtSta & 0x80) == 0)//�������ݴ���/ң���Ѿ�û�а�����
			{
				RmtSta &= ~(1 << 6);//������յ���Ч������ʶ
				RmtCnt = 0;		//�����������������
			}			
	 }  
   return key_value;
}