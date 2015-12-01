#ifndef _REMOTE_H
#define _REMOTE_H

#include "include.h"


#define ON_OFF_KEY 	(0x01)
#define MENU_KEY		(0x02)

#define ADD_KEY			(0x04)
#define UP_KEY			(0x05)
#define RETURN_KEY	(0x06)

#define LEFT_KEY		(0x08)
#define OK_KEY			(0x09)
#define RIGHT_KEY		(0x0A)

#define REDUCE_KEY	(0x0C)
#define DOWN_KEY		(0x0D)
#define ZERO_KEY		(0x0E)

#define ONE_KEY			(0x10)
#define TWO_KEY			(0x11)
#define THREE_KEY		(0x12)

#define FOUR_KEY		(0x14)
#define FIVE_KEY		(0x15)
#define SIX_KEY			(0x16)

#define SEVEN_KEY		(0x18)
#define EIGHT_KEY		(0x19)
#define NINE_KEY		(0x1A)


#define Periph_BASE         0x40000000  // 外设基地址 Peripheral 
#define Periph_BB_BASE      0x42000000  // 外设别名区基地址 Peripheral bitband

#define Periph_BB(PeriphAddr, BitNumber)    \
					*(__IO uint32_t *) (Periph_BB_BASE | ((PeriphAddr - Periph_BASE) << 5) | ((BitNumber) << 2))					
					
#define PCInBit(n)      Periph_BB((uint32_t)&GPIOC->IDR,n)  //输入

#define RDATA 	PCInBit(8)	 //红外数据输入

void remote_init(void);
u8 Remote_Scan(void);
extern uint8_t keyflag;

#endif
