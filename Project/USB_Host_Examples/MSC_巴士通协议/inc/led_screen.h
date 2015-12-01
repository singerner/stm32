#ifndef _LED_SCREEN_H
#define _LED_SCREEN_H

#include "include.h"


//ÏÔÊ¾¿ª¹Ø
#define Set_Row_Enable()   GPIO_ResetBits(GPIOC, GPIO_Pin_2)
#define Set_Row_Disable()  GPIO_SetBits(GPIOC, GPIO_Pin_2)

void static_16_disp_string(uint8_t ucDispRow,uint8_t ucRefreshCount,uint8_t *pucDispString);
void refresh_16_static_led_screen(uint8_t ucLoopCount,uint8_t ucShiftCount);


void move_16_disp_string(uint8_t ucDispRow,uint8_t ucRefreshCount,uint8_t *pucDispString);
void refresh_16_move_led_screen(uint8_t ucLoopCount,uint8_t ucShiftCount);


void static_4_disp_string(uint8_t ucDispRow,uint8_t ucRefreshCount,uint8_t *pucDispString);
void move_4_disp_string(uint8_t ucDispRow,uint8_t ucRefreshCount,uint8_t *pucDispString);
void refresh_4_move_led_screen(uint8_t ucLoopCount,uint8_t ucShiftCount);
void refresh_4_static_led_screen(uint8_t ucLoopCount,uint8_t ucShiftCount);

void led_board_16_columndataout(uint8_t Codata);
void led_board_4_columndataout(uint8_t Codata);


void led_board_16_rownumber(uint8_t  ucRowNumber);
void led_board_4_rownumber(uint8_t  ucRowNumber);
#endif
