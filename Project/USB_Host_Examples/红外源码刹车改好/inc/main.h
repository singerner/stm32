#ifndef _MAIN_H
#define _MAIN_H

#include "include.h"

#define 	SIZE		(64)
#define   CFGADDR		(0x00)//0K-4K
#define   LINEADDR	(0x1000)//4K-8K
//#define   NOWLINEADDR (0x2000)

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
extern uint8_t check_ok; 
extern uint8_t ucDispCMDDataBuffTemp[SIZE];

extern uint8_t get_left_state(void);
extern uint8_t get_right_state(void);
extern uint8_t get_brake_state(void);
extern void rs485_recv(void);



#endif

