#include "uart.h"
#include "crc16.h"

uint8_t uart_data[200] = {0};
uint16_t count = 0, num = 0;
uint8_t head1_flag = 0;
uint8_t head2_flag = 0;
uint8_t head3_flag = 0;
uint8_t wait_head_flag = 1;
uint16_t data_len = 0;
uint8_t data_num[2] = {0};
uint8_t info_cnt = 0;
uint8_t uart_get_flag = 0;
uint8_t crc[13] = {0xA5, 0x5A, 0x0D, 0x00, 0x05, 0x01, 0x01, 0x00, 0x00, 0xFF, 0xFF};

void uart_init(uint32_t baud)
{
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

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
	USART_ClearFlag(USART2, USART_FLAG_TC);
}


void send_data(USART_TypeDef *USARTx, uint8_t data)
{
	USART_SendData(USARTx, (uint8_t) data);
	while(USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);	
}

void send_str(USART_TypeDef *USARTx, uint8_t *buff)
{
	while('\0' != * buff)
	{
		send_data(USARTx, * buff);
		buff ++;
	}
}

	
void uart_irq(uint8_t irq_num)
{
	uint8_t res = 0;
	uint16_t crc_value = 0;
	switch(irq_num)
	{
		case 2:
			if(USART_GetFlagStatus(USART2,USART_FLAG_RXNE) != RESET)
			{
				USART_ClearITPendingBit(USART2,USART_FLAG_RXNE);
				res = USART_ReceiveData(USART2);
			}
			break;
		case 4:
			if(USART_GetFlagStatus(UART4,USART_FLAG_RXNE) != RESET)
			{
				USART_ClearITPendingBit(UART4,USART_FLAG_RXNE);
				res = USART_ReceiveData(UART4);
			}	
			break;
		default:
			break;
	}
	
	if(wait_head_flag)
	{
		if(head1_flag == 0)
		{
			if(0xA5 == res)
			{
				count = 0;
				uart_data[count ++] = res;
				head1_flag = 1;
			}
			else
			{
				count = 0;
				head1_flag = 0;
				wait_head_flag = 1;
			}
		}
		else
		{
			if(head2_flag == 0)
			{
				if(0x5A == res)
				{
					uart_data[count ++] = res;
					wait_head_flag = 0;
					head2_flag = 1;
					info_cnt = 0;
					data_len = 2;	
				}
				else
				{
					count = 0;
					head1_flag = 0;
					head2_flag = 0;
					wait_head_flag = 1;
				}
			}
		}
	}
	else
	{
		info_cnt ++;
		if(info_cnt == 1)
		{
			data_num[info_cnt - 1] = res;
			uart_data[count ++] = res;
		}
		else if(info_cnt == 2)
		{
			data_num[info_cnt - 1] = uart_data[count];
			data_len = (data_num[1] << 8) | data_num[0];
			uart_data[count ++] = res;
		}
		else if(info_cnt == 3)
		{
			if(0x05 == res)
			{
				uart_data[count ++] = res;
			}
			else
			{
				count = 0;
				wait_head_flag = 1;
				head1_flag = 0;
				head2_flag = 0;
			}
		}
		else if(info_cnt == 9)
		{
			if(led_type == res)
			{
				uart_data[count ++] = res;
			}
			else
			{
				count = 0;
				wait_head_flag = 1;
				head1_flag = 0;
				head2_flag = 0;
			}
		}
		else
		{
			if(data_len == (info_cnt + 2))
			{
				uart_data[count ++] = res;
				crc[9] = uart_data[9];
				crc[10] = uart_data[10];
				crc_value = crc_ccitt(crc, 0x0B);
				crc[11] = crc_value & 0xFF;
				crc[12] = (crc_value >> 8) & 0xFF;
				crc_value = 0;
				rs485_send();
				send_str(USART2, crc);
				rs485_recv();
				uart_get_flag = 1;
				wait_head_flag = 1;
				head1_flag = 0;
				head2_flag = 0;
				count = 0;
			}
			else
			{
				uart_data[count ++] = res;
			}
		}
	}
}
