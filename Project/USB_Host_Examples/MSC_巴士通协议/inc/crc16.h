#ifndef _CRC16_H
#define _CRC16_H

#include "stm32f10x.h"

uint16_t crc_ccitt(uint8_t *q,uint16_t len);

#endif
