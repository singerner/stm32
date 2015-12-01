#include "uart.h"

void uart_init(uint32_t baud)
{
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);	
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; //��ռ���ȼ�0
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
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //���������TX�� 
	GPIO_Init(GPIOA, &GPIO_InitStructure); 
	/*PA3 USART2_Rx*/ 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//�������루RX�� 
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
 	/* PC10 UART4_Tx */ 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //���������TX�� 
	GPIO_Init(GPIOC, &GPIO_InitStructure); 
	/* PC11 UART4_Rx  */ 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//�������루RX�� 
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	//uart init
	USART_InitStructure.USART_BaudRate = baud;//��ʼ�������� 
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�������ݳ���Ϊ8bit 
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//ֹͣλΪ1 
	USART_InitStructure.USART_Parity = USART_Parity_No;//��У��λ 
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//����������Ϊnone 
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;//���պͷ���ģʽ���� 
	
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
	USART_ClearFlag(USART2, USART_FLAG_TC);// ���־
}


void send_data(USART_TypeDef *USARTx, uint8_t data)
{
	rs485_send();
	USART_SendData(USARTx, (uint8_t) data);
	while(USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);	
	rs485_recv();
}

void send_str(USART_TypeDef *USARTx, uint8_t *str)
{
	rs485_send();
	while('\0' !=*str)
	{
		send_data(USARTx, *str);
		str ++;
	}
	rs485_recv();
}

void uart_irq(uint8_t irq_num)
{
	
}






