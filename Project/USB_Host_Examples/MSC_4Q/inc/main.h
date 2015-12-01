#ifndef _MAIN_H
#define _MAIN_H

#include "includes.h"

#define 	SIZE		(64)

#define 	c_MaxRowNumber 16

extern uint8_t LED_OK;
extern uint32_t brightness;
extern uint8_t *ucLEDDisplayMemory;
extern uint8_t *ucColDataBuff;
extern void cpu_run(void);
extern void cpu_stop(void);
extern void flash_run(void);
extern void flash_stop(void);
extern uint8_t c_MaxColByte;
extern uint8_t get_left_state(void);
extern uint8_t get_right_state(void);
extern uint8_t get_brake_state(void);

#endif

