#include "uart.h"

uint8_t uart_data[200] = {0};
uint16_t count = 0, num = 0;
uint8_t Usart_ucDispCMDDataBuffTemp[126]={0};	//中文站集信息
uint8_t Usart_ucDispCMDDataBuffENGTemp[106]={0};	//英文站集信息
uint8_t Usart_ucDispCMDDataBuffFUWUTemp[166]={0};	//服务用语信息	
uint8_t uart_buff[60] = {0};
uint8_t uart_get_flag = 0;
uint8_t uc_len = 0, uceng_len = 0, ucfuwu_len = 0;


void uart_init(uint32_t baud)
{
	USART_InitTypeDef USART_InitStructure;//初始化usart资源（寄存器）
	GPIO_InitTypeDef GPIO_InitStructure;//初始化读写资源（寄存器）
	NVIC_InitTypeDef NVIC_InitStructure;//初始化中断资源（寄存器）

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);	
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; //抢占优先级0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2 | RCC_APB1Periph_UART4,ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,	ENABLE);

	 /*PA2 USART2_Tx*/ 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //推挽输出（TX） 
    GPIO_Init(GPIOA, &GPIO_InitStructure); 
    /*PA3 USART2_Rx*/ 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入（RX） 
    GPIO_Init(GPIOA, &GPIO_InitStructure);
	
 	/* PC10 UART4_Tx */ 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //推挽输出（TX） 
	GPIO_Init(GPIOC, &GPIO_InitStructure); 
	/* PC11 UART4_Rx  */ 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入（RX） 
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	//uart init
	USART_InitStructure.USART_BaudRate = baud;//初始化波特率 
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//设置数据长度为8bit 
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//停止位为1 
	USART_InitStructure.USART_Parity = USART_Parity_No;//无校验位 
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//数据流控制为none 
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;//接收和发送模式都打开 
	
	/* Configure USART2 */ 
	USART_Init(USART2, &USART_InitStructure);
	/* Configure UART4 */ 
	USART_Init(UART4, &USART_InitStructure);
	
	/* Enable the USART2 */ 
	USART_Cmd(USART2, ENABLE);
	/* Enable the UART4 */ 
	USART_Cmd(UART4, ENABLE);
	
	USART_ITConfig(USART2, USART_IT_RXNE,  ENABLE);
	USART_ITConfig(UART4, USART_IT_RXNE,  ENABLE);
}

void uart_irq(uint8_t irq_num)//（中断）
{
	switch(irq_num)
	{
		case 2:
			if(USART_GetFlagStatus(USART2,USART_FLAG_RXNE) != RESET)
			{
				USART_ClearITPendingBit(USART2,USART_FLAG_RXNE);
				uart_data[count++]=USART_ReceiveData(USART2);
			}
			break;
		case 4:
			if(USART_GetFlagStatus(UART4,USART_FLAG_RXNE) != RESET)
			{
				USART_ClearITPendingBit(UART4,USART_FLAG_RXNE);
				uart_data[count++]=USART_ReceiveData(UART4);
			}	
			break;
		default:
			break;
	}
	
	if(uart_data[0]==0xA5)
	{
		if(count > 3)
		{		
			if((uart_data[1] == 0x98))
			{
				if(uart_data[2] == 0x98)
				{
					Usart_ucDispCMDDataBuffTemp[uc_len++] =uart_data[count-1];
					if((uc_len==122)&&(count==125))
					{
						uc_len=0;
						count=0;
						//uart_get_flag=1;
						goto Rexit;
					}		
				}	
				if(uart_data[2]==0x88)
				{
					Usart_ucDispCMDDataBuffENGTemp[uceng_len++]=uart_data[count-1];
					if((uceng_len==102)&&(count==105))
					{
						uceng_len=0;
						count=0;
						LED_OK=0;
						uart_get_flag=1;
						goto Rexit;
					}
				}	
				if(uart_data[2]==0x78)
				{
					Usart_ucDispCMDDataBuffFUWUTemp[ucfuwu_len++]=uart_data[count-1];	
					if((ucfuwu_len==162)&&(count==165))
					{
						ucfuwu_len=0;
						count=0;
						goto Rexit;
					}
				}	
			}
			else
			{
				uart_buff[num++]=uart_data[count-1];
				if((num==51)&&(count==54))
				{
					num=0;
					count=0;
				}		
			}
		}
	}
	else
	{
		count=0;
	}
Rexit:

}




