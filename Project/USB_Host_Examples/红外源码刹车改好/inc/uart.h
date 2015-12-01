#ifndef _UART_H
#define _UART_H

#include "include.h"

void uart_init(uint32_t baud);
void uart_irq(uint8_t irq_num);
void send_data(USART_TypeDef *USARTx, uint8_t data);
void send_str(USART_TypeDef *USARTx, uint8_t *str);

#endif


