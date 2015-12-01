#include "ziku.h"


void GetHZDot(uint16_t uiHZCode,uint8_t *ucHZDotData)
{
	uint32_t ulAddress;
    uint16_t  ultmp;
    uint8_t ultmp2;
    uint8_t ultmp1;
//	uint16_t uci;
	
	ultmp=uiHZCode;
	ultmp1=(uint8_t)(ultmp >> 8);
	ultmp2=(uint8_t)(ultmp);

	ulAddress = ((ultmp1-0xA1)*94+(ultmp2-0xA1))*32 + 0x2000;//161

	SST25_R_BLOCK(ulAddress, ucHZDotData, 32);
};