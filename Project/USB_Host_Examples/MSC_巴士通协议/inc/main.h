#ifndef _MAIN_H
#define _MAIN_H

#include "include.h"

#define 	SIZE		(64)
#define 	c_MaxRowNumber 16

extern uint8_t LED_OK;
extern uint32_t brightness;
extern uint8_t *ucLEDDisplayMemory;
extern uint8_t *ucColDataBuff;
extern uint8_t led_type;
extern uint8_t uart_get_flag;
extern void cpu_run(void);
extern void cpu_stop(void);
extern void flash_run(void);
extern void flash_stop(void);
extern uint8_t c_MaxColByte;
extern uint8_t get_left_state(void);
extern uint8_t get_right_state(void);
extern uint8_t get_brake_state(void);
extern void rs485_send(void);
extern void rs485_recv(void);
#endif

