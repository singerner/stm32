#include "spi_flash.h"

/****************************************************************************
* ��    �ƣ�void WREN(void)
* ��    �ܣ�дʹ��
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/ 
void WREN(void)
{
	Select_Flash();
	SPI_Flash_SendByte(0x06);
	NotSelect_Flash();
}

/****************************************************************************
* ��    �ƣ�void Busy_Check(void)
* ��    �ܣ�æ���
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/ 
void Busy_Check(void){
	unsigned char a=1;
	while((a&0x01)==1) a=RDSR();	

}

/****************************************************************************
* ��    �ƣ�void WRDI(void)
* ��    �ܣ�д��ֹ
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/ 
void WRDI(void)
{

	Select_Flash();
	SPI_Flash_SendByte(0x04); 
	NotSelect_Flash();
	Busy_Check();
	
}	
/****************************************************************************
* ��    �ƣ�void WRSR(void)
* ��    �ܣ�д״̬
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷������� 
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
* ��    �ƣ�unsigned char RDSR(void)
* ��    �ܣ���״̬�Ĵ���
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷������� 
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
* ��    �ƣ�void SST25_R_BLOCK(unsigned long addr, unsigned char *readbuff, unsigned int BlockSize)
* ��    �ܣ�ҳ��
* ��ڲ�����unsigned long addr--ҳ   unsigned char *readbuff--����   unsigned int BlockSize--����
* ���ڲ�������
* ˵    ����
* ���÷������� 
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
* ��    �ƣ�void SST25_W_BLOCK(unsigned int addr, u8 *readbuff, unsigned short int BlockSize)
* ��    �ܣ�ҳд
* ��ڲ�����unsigned int addr--ҳ   u8 *readbuff--����   unsigned short int BlockSize--����    
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/  
void SST25_W_BLOCK(unsigned int addr, u8 *readbuff, unsigned short BlockSize)
{
	unsigned int i=0,a2;
	Sector_Erase_4K(addr);   								  //ɾ��ҳ		  
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
* ��    �ƣ�void Sector_Erase_4K(unsigned long a1)
* ��    �ܣ�ҳ����
* ��ڲ�����unsigned long a1--ҳ   
* ���ڲ�������
* ˵    ����
* ���÷������� 
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
/* ����: Block_Erase_32K                      */   
/* ����: Block Erases 32 KByte of the Chip.           */   
/* ����:      Dst:        Ŀ���ַ 000000H - 1FFFFFH  */   
/* ����:      Nothing                         */   
/************************************************************************/   
void Block_Erase_32K(unsigned long Dst)   
{   
 //   WREN_Check();
 	WRSR();
	WREN();    
    Select_Flash();                 
    SPI_Flash_SendByte(0x52);            /* ����32 KByte Block Erase����*/   
    SPI_Flash_SendByte(((Dst & 0xFFFFFF) >> 16));  /* ����3bytes��ַ*/   
    SPI_Flash_SendByte(((Dst & 0xFFFF) >> 8));   
    SPI_Flash_SendByte(Dst & 0xFF);   
    NotSelect_Flash(); 
    Busy_Check();         
}   
/************************************************************************/   
/* ����: Block_Erase_64K                      */   
/* ����: Block Erases 64 KByte    */   
/* ����:      Dst:        Ŀ���ַ000000H - 1FFFFFH   */   
/* ����:      Nothing                         */   
/************************************************************************/   
void Block_Erase_64K(unsigned long Dst)   
{   
//  WREN_Check();
	WRSR();
	WREN();    
    Select_Flash();           
    SPI_Flash_SendByte(0xD8);            /* ����64KByte Block Erase ���� */   
    SPI_Flash_SendByte(((Dst & 0xFFFFFF) >> 16));  /* ����3 bytes��ַ */   
    SPI_Flash_SendByte(((Dst & 0xFFFF) >> 8));   
    SPI_Flash_SendByte(Dst & 0xFF);   
    NotSelect_Flash(); 
    Busy_Check();           
}   
/****************************************************************************
* ��    �ƣ�void FlashReadID(void)
* ��    �ܣ��������뼰�ͺŵĺ���
* ��ڲ�����  
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/ 
uint8_t FlashReadID(uint32_t ad)
{
	uint8_t byte;
	Select_Flash();	
  	SPI_Flash_SendByte(0x90);
	SPI_Flash_SendByte(0x00);
	SPI_Flash_SendByte(0x00);
	SPI_Flash_SendByte(ad);
  	//fac_id= SPI_Flash_ReadByte();		          //BFH: ������SST
	//dev_id= SPI_Flash_ReadByte();	              //41H: �����ͺ�SST25VF016B     
  	byte=SPI_Flash_ReadByte();
	NotSelect_Flash();
	
	return byte;	
}


/* ����: Jedec_ID_Read                        */   
/* ����: ��ȡ����ID(BFh),����������(25h),�豸ID(41h). ʹ��9Fh��ΪJEDEC ID����.     */   
/* ����:      None                            */   
/* ����:      IDs_Read:ID1(����ID = BFh, ������ID(25h), �豸ID(80h)*/   
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
   
  /* ʹ��SPI1 ʱ�� */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1 ,ENABLE);	
  /* ���� SPI1 ����: SCK, MOSI */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Speed =GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  /*���� MISO  ����������ģʽ*/	
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);  

  /*���� NSS */ 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure); 
  
  
 
  /* SPI1���� */ 
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;//ģʽ ��spi
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;//���ݵķ��ͽ��սṹΪ8λ��������8/16��
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;//��ʱ�����գ�Ҳ�����ָ߻��ߵͣ�
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;//�����ĸ�ʱ���أ�Ҳ������1/2��
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;//�ڲ�NSS�ź���SSI����λ�������ڲ������ⲿ��
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;//������Ԥ��Ƶ��2/4��
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;//
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(SPI1, &SPI_InitStructure);
  
  /* ʹ��SPI1  */
  SPI_Cmd(SPI1, ENABLE);  // ʹ�ܻ���ʧ��spi����
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

