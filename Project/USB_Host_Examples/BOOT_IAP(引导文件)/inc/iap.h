#ifndef _IAP_H
#define _IAP_H
#include "stm32f10x.h"

#define ApplicationAddress	  (0x0800C000)
#define PAGE_SIZE             (0x800)//2K

typedef  void (*pFunction)(void);

extern unsigned long JumpAddress;
extern pFunction Jump_To_Application;
extern unsigned long RamSource;
#endif

