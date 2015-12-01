#ifndef _UART_H
#define _UART_H

#include "includes.h"

void uart_init(uint32_t baud);
void uart_irq(uint8_t irq_num);

#endif


