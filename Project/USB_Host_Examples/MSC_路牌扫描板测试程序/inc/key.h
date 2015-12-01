#ifndef _KEY_H_
#define _KEY_H_
#include "stm32f10x.h"

#define KEY_0 	(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0))

void key_init(void);
void EXTI0_IRQHandler(void);


extern  uint8_t key_cnt;
extern uint8_t bkey_flag;
#endif
