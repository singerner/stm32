#include "remote.h"

void remote_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_ICInitTypeDef  TIM_ICInitStructure;  
 
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE); //使能PORTB时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);	//TIM3 时钟使能 
	
	//GPIO_AFIODeInit();
	GPIO_PinRemapConfig(GPIO_FullRemap_TIM3, ENABLE);//全映射
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;				 //PC8 输入 
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; 		//上拉输入 
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_SetBits(GPIOC, GPIO_Pin_8);	//初始化GPIOC.9
						  
 	TIM_TimeBaseStructure.TIM_Period = 10000; //设定计数器自动重装值 最大10ms溢出  
	TIM_TimeBaseStructure.TIM_Prescaler =(72-1); 	//预分频器,1M的计数频率,1us加1.	   
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx

	TIM_ICInitStructure.TIM_Channel = TIM_Channel_3;  // 选择输入端 IC3映射到TI3上
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//上升沿捕获
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //配置输入分频,不分频 
	TIM_ICInitStructure.TIM_ICFilter = 0x03;//IC3F=0011 配置输入滤波器 8个定时器时钟周期滤波
	TIM_ICInit(TIM3, &TIM_ICInitStructure);//初始化定时器输入捕获通道

	TIM_Cmd(TIM3, ENABLE); 	//使能定时器3

	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器	

	TIM_ITConfig(TIM3, TIM_IT_Update | TIM_IT_CC3,ENABLE);//允许更新中断 ,允许CC3IE捕获中断							
}




uint8_t  RmtSta = 0;	//0000 0000  7位是引导码是否完成标志位，3位是高电平是否捕获标志位
uint16_t Dval;		//下降沿时计数器的值
uint32_t RmtRec = 0;	//红外接收到的数据   		    
uint8_t  RmtCnt = 0;	//按键按下的次数  
uint8_t  keyflag = 0;

//定时器中断服务程序	 
void TIM3_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
		{
				if(RmtSta & 0x80)//上次有数据被接收到了
				{	
						RmtSta	&=	(~0x10);						//取消上升沿已经被捕获标记
						if((RmtSta & 0x0F) == 0x00)
								RmtSta |= 1 << 6;//标记已经完成一次按键的键值信息采集
						if((RmtSta & 0x0F) < 14)
								RmtSta ++;
						else
						{
								RmtSta &= ~(1 << 7);//清空引导标识
								RmtSta &= 0xF0;	//清空计数器	
						}						 	   	
				}							    
		}
		if(TIM_GetITStatus(TIM3, TIM_IT_CC3) != RESET)
		{	  
			if(RDATA)//上升沿捕获
			{
					TIM_OC3PolarityConfig(TIM3,	TIM_ICPolarity_Falling);		//CC1P=1 设置为下降沿捕获				
					TIM_SetCounter(TIM3, 0);	   	//清空定时器值
					RmtSta |= 0x10;					//标记上升沿已经被捕获
			}
			else //下降沿捕获
			{			
					 Dval = TIM_GetCapture3(TIM3);//读取CCR1也可以清CC1IF标志位
					 TIM_OC3PolarityConfig(TIM3, TIM_ICPolarity_Rising); //CC3P=0	设置为上升沿捕获
					 if(RmtSta & 0x10)			//完成一次高电平捕获 
					 {
								if(RmtSta & 0x80)//接收到了引导码
								{
									 if(Dval > 300 && Dval < 800)			//560为标准值,560us
									 {
												RmtRec <<= 1;	//左移一位.
												RmtRec |= 0;	//接收到0	   
									 }
									else if(Dval > 1400 && Dval < 1800)	//1680为标准值,1680us
									{
												RmtRec <<= 1;	//左移一位.
												RmtRec |= 1;	//接收到1
									}
								 else if(Dval > 2200 && Dval < 2600)	//得到按键键值增加的信息 2500为标准值2.5ms
								 {
											RmtCnt ++; 		//按键次数增加1次
											RmtSta &= 0xF0;	//清空计时器	
											keyflag = 1;
										
								}
							}
							else if(Dval > 4200 && Dval < 4700)		//4500为标准值4.5ms
							{
									RmtSta |= 1 << 7;	//标记成功接收到了引导码
									RmtCnt = 0;		//清除按键次数计数器
							}						 
					}
					RmtSta &= ~(1 << 4);//清除高电平捕获标志位
			}				 		     	    					   
		}
		TIM_ClearFlag(TIM3, TIM_IT_Update | TIM_IT_CC3);	    
}

u8 Remote_Scan(void)
{        
	 uint8_t sta=0, temp =  0;       
   uint8_t key_value = 0, num ;
	 uint8_t t1, t2;
	 if(RmtSta&(1 << 6))//得到一个按键的所有信息了
	 { 
			t1=RmtRec >> 24;			//得到地址码
	    t2=(RmtRec >> 16)&0xff;	//得到地址反码 
 	    if(0xFD == t2)
	    { 
	        t1=RmtRec >> 8;
	        t2 = RmtRec; 	
	        if(t1 == (uint8_t)~t2)
					{
							sta = t1;//键值正确
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
			if((RmtSta & 0x80) == 0)//按键数据错误/遥控已经没有按下了
			{
				RmtSta &= ~(1 << 6);//清除接收到有效按键标识
				RmtCnt = 0;		//清除按键次数计数器
			}			
	 }  
   return key_value;
}

