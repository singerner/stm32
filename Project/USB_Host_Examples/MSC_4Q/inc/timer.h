#ifndef _TIMER_H
#define _TIMER_H

#include "includes.h"

void nvic_tim5configuration(void);
void tim5_init(void);
void TIM5_IRQHandler(void);

#endif
