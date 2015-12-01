#include "spi_flash.h"

/****************************************************************************
* 名    称：void WREN(void)
* 功    能：写使能
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/ 
void WREN(void)
{
	Select_Flash();
	SPI_Flash_SendByte(0x06);
	NotSelect_Flash();
}

/****************************************************************************
* 名    称：void Busy_Check(void)
* 功    能：忙检测
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/ 
void Busy_Check(void){
	unsigned char a=1;
	while((a&0x01)==1) a=RDSR();	

}

/****************************************************************************
* 名    称：void WRDI(void)
* 功    能：写禁止
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/ 
void WRDI(void)
{

	Select_Flash();
	SPI_Flash_SendByte(0x04); 
	NotSelect_Flash();
	Busy_Check();
	
}	
/****************************************************************************
* 名    称：void WRSR(void)
* 功    能：写状态
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/ 
void WRSR(void)
{	
	Select_Flash();
	SPI_Flash_SendByte(0x50);
	NotSelect_Flash(); 
	Select_Flash();
	SPI_Flash_SendByte(0x01);
	SPI_Flash_SendByte(0x00); 
	NotSelect_Flash();
    Busy_Check();
}


/****************************************************************************
* 名    称：unsigned char RDSR(void)
* 功    能：读状态寄存器
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/ 
uint8_t RDSR(void){
	uint8_t busy;
	Select_Flash();
	SPI_Flash_SendByte(0x05);
	busy = SPI_Flash_ReadByte();
	NotSelect_Flash();
	return(busy);
	
}

/****************************************************************************
* 名    称：void SST25_R_BLOCK(unsigned long addr, unsigned char *readbuff, unsigned int BlockSize)
* 功    能：页读
* 入口参数：unsigned long addr--页   unsigned char *readbuff--数组   unsigned int BlockSize--长度
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/  
void SST25_R_BLOCK(unsigned long addr, unsigned char *readbuff, unsigned int BlockSize)
{
	unsigned int i=0; 	
	Select_Flash();
	SPI_Flash_SendByte(0x0b);
	SPI_Flash_SendByte((addr&0xffffff)>>16);
	SPI_Flash_SendByte((addr&0xffff)>>8);
	SPI_Flash_SendByte(addr&0xff);
	SPI_Flash_SendByte(0);
	while(i<BlockSize)
	{	
		readbuff[i]=SPI_Flash_ReadByte();	
		i++;
	}
	NotSelect_Flash();	 	
}
/****************************************************************************
* 名    称：void SST25_W_BLOCK(unsigned int addr, u8 *readbuff, unsigned short int BlockSize)
* 功    能：页写
* 入口参数：unsigned int addr--页   u8 *readbuff--数组   unsigned short int BlockSize--长度    
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/  
void SST25_W_BLOCK(unsigned int addr, u8 *readbuff, unsigned short BlockSize)
{
	unsigned int i=0,a2;
	Sector_Erase_4K(addr);   								  //删除页		  
	WRSR();
  	WREN();	
	Select_Flash();    
	SPI_Flash_SendByte(0xad);
	SPI_Flash_SendByte((addr&0xffffff)>>16);
	SPI_Flash_SendByte((addr&0xffff)>>8);
	SPI_Flash_SendByte(addr&0xff);
  	SPI_Flash_SendByte(readbuff[0]);
	SPI_Flash_SendByte(readbuff[1]);
	NotSelect_Flash();
	i=2;
	while(i<BlockSize)
	{
		a2=120;
		while(a2>0) a2--;
		Select_Flash();
		SPI_Flash_SendByte(0xad);
		SPI_Flash_SendByte(readbuff[i++]);
		SPI_Flash_SendByte(readbuff[i++]);
		NotSelect_Flash();
	}
	
	a2=100;
	while(a2>0) a2--;
	WRDI();	
	Select_Flash();	
	Busy_Check();
}
/****************************************************************************
* 名    称：void Sector_Erase_4K(unsigned long a1)
* 功    能：页擦除
* 入口参数：unsigned long a1--页   
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/ 
void Sector_Erase_4K(unsigned long a1)
{
	WRSR();
	WREN();     
	Select_Flash();	  
	SPI_Flash_SendByte(0x20);
	SPI_Flash_SendByte((a1&0xffffff)>>16);          //addh
	SPI_Flash_SendByte((a1&0xffff)>>8);          //addl 
	SPI_Flash_SendByte(a1&0xff);                 //wtt
	NotSelect_Flash();
	Busy_Check();	
}


/************************************************************************/   
/* 名称: Block_Erase_32K                      */   
/* 功能: Block Erases 32 KByte of the Chip.           */   
/* 输入:      Dst:        目标地址 000000H - 1FFFFFH  */   
/* 返回:      Nothing                         */   
/************************************************************************/   
void Block_Erase_32K(unsigned long Dst)   
{   
 //   WREN_Check();
 	WRSR();
	WREN();    
    Select_Flash();                 
    SPI_Flash_SendByte(0x52);            /* 发送32 KByte Block Erase命令*/   
    SPI_Flash_SendByte(((Dst & 0xFFFFFF) >> 16));  /* 发送3bytes地址*/   
    SPI_Flash_SendByte(((Dst & 0xFFFF) >> 8));   
    SPI_Flash_SendByte(Dst & 0xFF);   
    NotSelect_Flash(); 
    Busy_Check();         
}   
/************************************************************************/   
/* 名称: Block_Erase_64K                      */   
/* 功能: Block Erases 64 KByte    */   
/* 输入:      Dst:        目标地址000000H - 1FFFFFH   */   
/* 返回:      Nothing                         */   
/************************************************************************/   
void Block_Erase_64K(unsigned long Dst)   
{   
//  WREN_Check();
	WRSR();
	WREN();    
    Select_Flash();           
    SPI_Flash_SendByte(0xD8);            /* 发送64KByte Block Erase 命令 */   
    SPI_Flash_SendByte(((Dst & 0xFFFFFF) >> 16));  /* 发送3 bytes地址 */   
    SPI_Flash_SendByte(((Dst & 0xFFFF) >> 8));   
    SPI_Flash_SendByte(Dst & 0xFF);   
    NotSelect_Flash(); 
    Busy_Check();           
}   
/****************************************************************************
* 名    称：void FlashReadID(void)
* 功    能：读工厂码及型号的函数
* 入口参数：  
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/ 
uint8_t FlashReadID(uint32_t ad)
{
	uint8_t byte;
	Select_Flash();	
  	SPI_Flash_SendByte(0x90);
	SPI_Flash_SendByte(0x00);
	SPI_Flash_SendByte(0x00);
	SPI_Flash_SendByte(ad);
  	//fac_id= SPI_Flash_ReadByte();		          //BFH: 工程码SST
	//dev_id= SPI_Flash_ReadByte();	              //41H: 器件型号SST25VF016B     
  	byte=SPI_Flash_ReadByte();
	NotSelect_Flash();
	
	return byte;	
}


/* 名称: Jedec_ID_Read                        */   
/* 功能: 读取厂商ID(BFh),记忆体类型(25h),设备ID(41h). 使用9Fh作为JEDEC ID命令.     */   
/* 输入:      None                            */   
/* 返回:      IDs_Read:ID1(厂商ID = BFh, 记忆体ID(25h), 设备ID(80h)*/   
/************************************************************************/   
unsigned long Jedec_ID_Read()    
{   
    unsigned long temp;   
    temp = 0;   
    Select_Flash();           /* enable device */   
    SPI_Flash_SendByte(0x9F);         /* send JEDEC ID command (9Fh) */   
    temp = (temp | SPI_Flash_ReadByte()) << 8; /* receive byte */   
    temp = (temp | SPI_Flash_ReadByte()) << 8;     
    temp = (temp | SPI_Flash_ReadByte());      /* temp value = 0xBF2541 */   
    NotSelect_Flash();           /* disable device */   
    return temp;   
} 

/*******************************************************************************
* Function Name  : SPI_FLASH_Init
* Description    : Initializes the peripherals used by the SPI FLASH driver.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_Flash_Init(void)
{
  SPI_InitTypeDef  SPI_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
   
  /* 使能SPI1 时钟 */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1 ,ENABLE);	
  /* 配置 SPI1 引脚: SCK, MOSI */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Speed =GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  /*配置 MISO  主输入从输出模式*/	
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);  

  /*配置 NSS */ 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure); 
  
  
 
  /* SPI1配置 */ 
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;//模式 主spi
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;//数据的发送接收结构为8位（有两种8/16）
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;//低时钟悬空（也有两种高或者低）
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;//捕获哪个时钟沿（也有两种1/2）
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;//内部NSS信号有SSI控制位（两种内部或者外部）
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;//波特率预分频（2/4）
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;//
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(SPI1, &SPI_InitStructure);
  
  /* 使能SPI1  */
  SPI_Cmd(SPI1, ENABLE);  // 使能或者失能spi外设
  NotSelect_Flash();

}

/*******************************************************************************
* Function Name  : SPI_FLASH_ReadByte
* Description    : Reads a byte from the SPI Flash.
*                  This function must be used only if the Start_Read_Sequence
*                  function has been previously called.
* Input          : None
* Output         : None
* Return         : Byte Read from the SPI Flash.
*******************************************************************************/
uint8_t SPI_Flash_ReadByte(void)
{
  return (SPI_Flash_SendByte(Dummy_Byte));
}

/*******************************************************************************
* Function Name  : SPI_FLASH_SendByte
* Description    : Sends a byte through the SPI interface and return the byte 
*                  received from the SPI bus.
* Input          : byte : byte to send.
* Output         : None
* Return         : The value of the received byte.
*******************************************************************************/
uint8_t SPI_Flash_SendByte(uint8_t byte)
{
  /* Loop while DR register in not emplty */
  while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
  //NotSelect_Flash();  while(1);
  /* Send byte through the SPI2 peripheral */
  SPI_I2S_SendData(SPI1, byte);

  /* Wait to receive a byte */
  while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);

  /* Return the byte read from the SPI bus */
  return SPI_I2S_ReceiveData(SPI1);
}

