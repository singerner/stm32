#ifndef _MAIN_H
#define _MAIN_H

#include "include.h"

#define 	SIZE		(120)
#define 	c_MaxRowNumber 16

#define FLASH_ON()	(GPIO_SetBits(GPIOB, GPIO_Pin_10))
#define FLASH_OFF()	(GPIO_ResetBits(GPIOB, GPIO_Pin_10))

#define CFGADDR		(0x00)//0k-4k
#define ADADDR		(0x1000)//4K-8K

#define FRONT		(0x30)
#define	REAR		(0x31)
#define SIDE		(0x32)

#define MESSAGE_LINE_NEW	(0x01)

extern uint8_t LED_OK;
extern uint8_t check_ok;
extern uint32_t brightness;
extern uint8_t *ucLEDDisplayMemory;
extern uint8_t *ucColDataBuff;
extern uint8_t ucDispCMDDataBuffTemp[SIZE];
extern void cpu_run(void);
extern void cpu_stop(void);
extern void flash_run(void);
extern void flash_stop(void);
extern uint8_t c_MaxColByte;
extern uint8_t get_left_state(void);
#endif

