#ifndef _UART_H
#define _UART_H

#include "include.h"

#define COMMAND_HARDWARE_INFO		(0x00)
#define COMMAND_UPDATE_OUTLINE		(0x01)
#define COMMAND_UPDATE_INSTATION	(0x02)
#define COMMAND_UPDATE_INSERVICE	(0x03)
#define COMMAND_SET_INSTATION		(0x04)
#define COMMAND_TERIMNAL_TOINSIDE	(0x05)

void uart_init(uint32_t baud);
void uart_irq(uint8_t irq_num);
void send_str(USART_TypeDef *USARTx, uint8_t *buff);
void send_data(USART_TypeDef *USARTx, uint8_t data);


extern uint8_t uart_data[200];
extern uint16_t data_len;
extern uint8_t uart_get_flag;

#endif


