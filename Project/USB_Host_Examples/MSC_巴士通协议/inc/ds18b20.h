#ifndef __Ds18b20_H
#define __Ds18b20_H

#include "stm32f10x.h"
#include "Delay.h"

#define IO_DS18B20		GPIO_Pin_8
#define GPIO_DS18B20	GPIOC

#define DS18B20_DQ_High	GPIO_SetBits(GPIO_DS18B20,IO_DS18B20)
#define DS18B20_DQ_Low	GPIO_ResetBits(GPIO_DS18B20,IO_DS18B20)

void DS18B20_IO_IN(void);
void DS18B20_IO_OUT(void);
u8 DS18B20_Read_Byte(void);
void DS18B20_Write_Byte(u8 dat);
void DS18B20_Reset(void);
double DS18B20_Get_wd(void);
void DS18B20_Init(void);
  
  
#endif
