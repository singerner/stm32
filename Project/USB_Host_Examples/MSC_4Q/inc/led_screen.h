#ifndef _LED_SCREEN_H
#define _LED_SCREEN_H

#include "includes.h"


//ÏÔÊ¾¿ª¹Ø
#define Set_Row_Enable()   GPIO_ResetBits(GPIOC, GPIO_Pin_2)
#define Set_Row_Disable()  GPIO_SetBits(GPIOC, GPIO_Pin_2)

void ScrollRefreshLEDScreen1(uint8_t ucLoopCount,uint8_t ucShiftCount);
void LED_DispString(uint8_t ucDispRow,uint8_t ucRefreshCount,uint8_t *pucDispString);

void LED_ScrollDispHZ(uint8_t ucDispRow,uint8_t ucRefreshCount,uint8_t *pucDispString);
void ScrollRefreshLEDScreen(uint8_t ucLoopCount,uint8_t ucShiftCount);

void ShT_LEDBoard_ColShiftOut(uint8_t Codata);
void ShT_LEDBoard_RowShiftOut(uint8_t  ucRowNumber);
#endif
