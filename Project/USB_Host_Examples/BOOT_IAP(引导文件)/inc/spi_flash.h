#ifndef __Spi_Flash_H
#define __Spi_Flash_H

#include "stm32f10x.h"

#define Dummy_Byte 0

#define ZIKU_ADDR   (0x2000)//8K
#define CFG_ADDR    (0x00)//0K-4k
#define SST25_PAGE	(0x10000)//64k

/* Select SPI FLASH: ChipSelect pin low  */
#define Select_Flash()     GPIO_ResetBits(GPIOA, GPIO_Pin_4)
/* Deselect SPI FLASH: ChipSelect pin high */
#define NotSelect_Flash()    GPIO_SetBits(GPIOA, GPIO_Pin_4)

uint8_t SPI_Flash_ReadByte(void);		//flash操作基本函数，读一个字节
uint8_t SPI_Flash_SendByte(uint8_t byte);		//	FLASH操作基本函数，发送一个字节
void SPI_Flash_Init(void);
uint8_t FlashReadID(uint32_t ad); //读取flashID四个字节
unsigned long Jedec_ID_Read(void);
void Busy_Check(void);
void WREN(void);
void WRDI(void);
void WRSR(void);
unsigned char RDSR(void);
void Sector_Erase_4K(unsigned long a1);
void Block_Erase_32K(unsigned long Dst); 
void Block_Erase_64K(unsigned long Dst); 
void SST25_R_BLOCK(unsigned long addr, unsigned char *readbuff, unsigned int BlockSize);
void SST25_W_BLOCK(unsigned int addr, u8 *readbuff, unsigned short int BlockSize);
		        

#endif

