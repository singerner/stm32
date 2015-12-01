#include "led_screen.h"
#include "zimo.h"

//控制移动数量计数器
uint8_t ucShiftCount;

//功能介绍:选择行号
//入口参数：行号
void ShT_LEDBoard_RowShiftOut(uint8_t  ucRowNumber)
{
	
 	if((ucRowNumber&0x02)==0x02)
	   GPIO_SetBits(GPIOB, GPIO_Pin_7); 
	else
       GPIO_ResetBits(GPIOB, GPIO_Pin_7); 
 	if((ucRowNumber&0x01)==0x01)
	   GPIO_SetBits(GPIOB, GPIO_Pin_6); 
	else
       GPIO_ResetBits(GPIOB, GPIO_Pin_6); 	 

}

void ShT_LEDBoard_ColShiftOut(uint8_t Codata)
{
	uint8_t i = 0, byte=Codata;
	uint8_t  j;
	for (i = 0; i < 8; i++) 
	{
		for(j=0;j<2;j++);
		GPIO_ResetBits(GPIOC, GPIO_Pin_1); 
		if (byte & 0x01)
			GPIO_SetBits(GPIOC, GPIO_Pin_3);
		else
			GPIO_ResetBits(GPIOC, GPIO_Pin_3); 
		for(j=0;j<2;j++);
		GPIO_SetBits(GPIOC, GPIO_Pin_1); 
		byte = byte >> 1;
	}
}

//滚动显示
void ScrollRefreshLEDScreen(uint8_t ucLoopCount,uint8_t ucShiftCount)
{
	uint8_t ucRowLineCount;
	uint16_t uiRefreshCount;
	int i;

	//以下用于修正系统连接不同数量的单元板时，显示时间的差异性,最小列数为64
	uiRefreshCount = 8;//(ucLoopCount*4)/(c_MaxColNumber/64);//5//64

	while(uiRefreshCount--)
	{
     if(LED_OK)
	  {   
		for(ucRowLineCount=0; ucRowLineCount<4; ucRowLineCount++)   //16????????0??
		{
		  
			uint8_t ucCount;
			uint32_t uiStartAddress;
			uint8_t ucLeftUnit;		//显存中的左单元
			uint8_t ucRightUnit;	

			uint8_t *pDispBuff;	
			uiStartAddress = ucRowLineCount*4*(c_MaxColByte+1);

		    pDispBuff = ucLEDDisplayMemory+uiStartAddress;

			for(ucCount=0; ucCount<4*c_MaxColByte; ucCount++)//6808505
			{
			  if(LED_OK)
			   {
				ucLeftUnit = *pDispBuff++;
				ucRightUnit = *(pDispBuff+3);
				ucColDataBuff[ucCount] = (ucLeftUnit<<ucShiftCount) | (ucRightUnit>>(8-ucShiftCount));
			   }
			   else
			   {
			    goto shexit;
			   }
			}
//先将数据列锁存到HC595
            if(LED_OK)
			{
			////ShT_LEDBoard_ColShiftOut(0,c_MaxColByte);
			  if(ucRowLineCount!=4){
			     for(ucCount=0; ucCount<4*c_MaxColByte; ucCount++)
			     { 
			       //ucColDataBuff[ucCount+(ucRowLineCount*4)]=0x02*(ucRowLineCount+1)*0;
				   ShT_LEDBoard_ColShiftOut(ucColDataBuff[ucCount]);
			     }
			   }
			}
			else
			{
			  goto shexit;
			}

//关闭显示

			Set_Row_Disable();
			for(i = 0; i < brightness; i ++);
//换行    
            if(LED_OK)
			{

              if(ucRowLineCount!=4)
               {
			     ShT_LEDBoard_RowShiftOut(ucRowLineCount);
               }
			}
			else
			{
			  goto shexit;
			}
			
			Set_Row_Enable();

//将HC595锁存的数据输出
            if(ucRowLineCount!=4)
			 {
#if 0
			    c_Col_SOut_High = 0;
			    c_Col_SOut_High = 1;
			    c_Col_SOut_High = 0;
#endif

				GPIO_SetBits(GPIOC, GPIO_Pin_0);
		      //  Set_Row_Enable();
			   	for(i=0;i<0xff;i++);  
                GPIO_ResetBits(GPIOC, GPIO_Pin_0);  
				for(i=0;i<0xf;i++);

								
			   // GPIO_ResetBits(GPIOC, GPIO_Pin_0);        
//开启显示

			 }
			// Set_Row_Enable();
//指向下一行
		}
	  }
         else
              {
                 goto shexit;
              }
      
	}
shexit:	//显示完成后，关闭屏幕显示
	Set_Row_Disable();	
}


uint8_t judget_extern_signal()
{
		uint8_t leftflag = 0, rightflag = 0, brakeflag = 0;
		uint8_t flag = 0;
		if(get_brake_state() == Bit_RESET)
		{
				brakeflag = 1;
		}
		if((1 == brakeflag) && (Bit_SET == get_brake_state() ))
		{
				brakeflag = 0;
				flag = 1;
		}				
		if(Bit_RESET == get_left_state())
		{
				leftflag = 1;
		}
		if((1 == leftflag) && (Bit_SET == get_left_state()))
		{
				leftflag = 0;
				flag = 1;
		}
		if(Bit_RESET == get_right_state())
		{
				rightflag = 1;
		}
		if((1 == rightflag) && (Bit_SET == get_right_state()))
		{
				rightflag = 0;
				flag = 1;
		}
		if(1 == keyflag)
		{
				keyflag = 0;
				flag = 1;
		}

		return flag;
}
	


void LED_ScrollDispHZ(uint8_t ucDispRow,uint8_t ucRefreshCount,uint8_t *pucDispString)
{
	uint16_t uiIndex;
	uint8_t ucZMData;
	uint8_t uci;
	uint8_t  ucHZDataBuff[32];
	uint16_t uiHZCode;
	uint8_t *pDispString_Head;		//头指针
	uint8_t *pDispString_Tail;
	uint8_t ucRowLineCount;
	uint8_t bResultFlag;
	uint8_t signalFlag;
	
	pDispString_Head = pucDispString;
	pDispString_Tail = pucDispString;
	memset(ucLEDDisplayMemory, 0x00, (c_MaxColByte*4+4)*4);
	while(1)
	{ 	
  	   if(LED_OK)
			 {
					uint8_t ucCharHigh;
					uint8_t ucCharLow;
					signalFlag = judget_extern_signal();
					if(1 == signalFlag)
					{
							break;
					}
					
		   
					if(*pDispString_Head == '\0')
					{
						  pDispString_Head = pDispString_Tail;
					}
					ucCharHigh = *pDispString_Head++;

					if(ucCharHigh > 0x80)
					{
				if(LED_OK)
				{
					//中文字符的处理
					ucCharLow = *pDispString_Head++;
					uiHZCode =(((uint16_t)ucCharHigh)<<8)+ucCharLow;					
					//GetHZDot(uiHZCode,ucHZDataBuff);
					uiIndex=0;

					while(1)
					{
						if(ZK_Data[uiIndex].uiHZcode == 0xFFFF)
						{
							bResultFlag = 0;
							break;
						}

						if(ZK_Data[uiIndex].uiHZcode == uiHZCode)
						{
							//-找到
							bResultFlag = 1;
							break;
						}
						else
						{
							uiIndex++;
						}
					}

					if(bResultFlag)
						memcpy(ucHZDataBuff,ZK_Data[uiIndex].model,32);		 
					else
						GetHZDot(uiHZCode,ucHZDataBuff);

					for(uci=1; uci<5; uci++)
					{
						if(LED_OK)
						{
							ucZMData = ucHZDataBuff[30-(16-uci)*2];
							ucLEDDisplayMemory[uci*(c_MaxColByte+1)*4-4] =ucZMData;

							ucZMData = ucHZDataBuff[30-(16-uci)*2+8];
							ucLEDDisplayMemory[uci*(c_MaxColByte+1)*4-3] =ucZMData;

							ucZMData = ucHZDataBuff[30-(16-uci)*2+16];
							ucLEDDisplayMemory[uci*(c_MaxColByte+1)*4-2] =ucZMData;

							ucZMData = ucHZDataBuff[30-(16-uci)*2+24];
							ucLEDDisplayMemory[uci*(c_MaxColByte+1)*4-1] =ucZMData;
						}
						else
						{
							goto rexit;
						}
					}

					for(ucShiftCount=0; ucShiftCount<8; ucShiftCount++)
					{
						if(LED_OK)
						{			   				   				   				   				   				   				   				   				   				   
							ScrollRefreshLEDScreen(ucRefreshCount,ucShiftCount);
						}
						else
						{
							goto rexit;
						}
					}
					//整屏移动一字节
					for(ucRowLineCount=0; ucRowLineCount<4; ucRowLineCount++)
					{
						if(LED_OK)
						{
							memcpy(ucLEDDisplayMemory+ucRowLineCount*4*(c_MaxColByte+1),ucLEDDisplayMemory+ucRowLineCount*4*(c_MaxColByte+1)+4,4*c_MaxColByte);
						}
						else
						{
							goto rexit;
						}
					}

					//处理右半个汉字,卷入屏幕最右端
					for(uci=1; uci<5; uci++)
					{
						if(LED_OK)
						{
							ucZMData = ucHZDataBuff[32-(16-uci)*2-1];
							ucLEDDisplayMemory[uci*(c_MaxColByte+1)*4-4] =ucZMData;

							ucZMData = ucHZDataBuff[32-(16-uci)*2-1+8];
							ucLEDDisplayMemory[uci*(c_MaxColByte+1)*4-3] =ucZMData;

							ucZMData = ucHZDataBuff[32-(16-uci)*2-1+16];
							ucLEDDisplayMemory[uci*(c_MaxColByte+1)*4-2] =ucZMData;

							ucZMData = ucHZDataBuff[32-(16-uci)*2-1+24];
							ucLEDDisplayMemory[uci*(c_MaxColByte+1)*4-1] =ucZMData;	 				
						}
						else
						{
							goto rexit;
						}
					}

					for(ucShiftCount=0; ucShiftCount<8; ucShiftCount++)
					{
						if(LED_OK)
						{
							ScrollRefreshLEDScreen(ucRefreshCount,ucShiftCount);
						}
						else
						{
							goto rexit;
						}
					}
					//整屏移动一字节
					for(ucRowLineCount=0; ucRowLineCount<4; ucRowLineCount++)
					{
						if(LED_OK)
						{
							memcpy(ucLEDDisplayMemory+ucRowLineCount*4*(c_MaxColByte+1),ucLEDDisplayMemory+ucRowLineCount*4*(c_MaxColByte+1)+4,4*c_MaxColByte);
						}
						else
						{
							goto rexit;
						}
					}
				}
				else
				{
					goto rexit;
				}		
			}
			else
			{
				//西文字符的处理
				if(LED_OK)
				{
					for(uci=1; uci<5; uci++)
					{
						if(LED_OK)
						{										 
							ucZMData = ASCII816[ucCharHigh - 0x20][uci-1];
							ucLEDDisplayMemory[uci*(c_MaxColByte+1)*4-4] =ucZMData;

							ucZMData = ASCII816[ucCharHigh - 0x20][uci-1+4];
							ucLEDDisplayMemory[uci*(c_MaxColByte+1)*4-3] =ucZMData;

							ucZMData = ASCII816[ucCharHigh - 0x20][uci-1+8];
							ucLEDDisplayMemory[uci*(c_MaxColByte+1)*4-2] =ucZMData;

							ucZMData = ASCII816[ucCharHigh - 0x20][uci-1+12];
							ucLEDDisplayMemory[uci*(c_MaxColByte+1)*4-1] =ucZMData;						
						}
						else
						{
							goto rexit;
						}
					}

					for(ucShiftCount=0; ucShiftCount<8; ucShiftCount++)
					{   
						if(LED_OK)
						{
							ScrollRefreshLEDScreen(ucRefreshCount,ucShiftCount);
						}
						else
						{
							goto rexit;
						}
					}
					for(ucRowLineCount=0; ucRowLineCount<4; ucRowLineCount++)
					{
						if(LED_OK)
						{
							memcpy(ucLEDDisplayMemory+ucRowLineCount*4*(c_MaxColByte+1),ucLEDDisplayMemory+ucRowLineCount*4*(c_MaxColByte+1)+4,4*c_MaxColByte);
						}
						else
						{
							goto rexit;
						}
					}
				}
				else
				{
					goto rexit;
				}
			}	
		}
		else
		{
			goto rexit;
		}
	}
rexit:
uci=1;
}


#if 0
void LED_DispString(uint8_t ucDispRow,uint8_t ucRefreshCount,uint8_t *pucDispString)
{
		if()
}
#else
//固定显示
void LED_DispString(uint8_t ucDispRow,uint8_t ucRefreshCount,uint8_t *pucDispString)
{
	uint8_t bResultFlag;
	uint8_t ucZMData;
	uint8_t uci;
	uint8_t  ucHZDataBuff[32];
	uint16_t uiIndex;
	uint16_t uiHZCode;
	uint8_t *pDispString_Head;		//头指针
	uint8_t ucRowLineCount;

	pDispString_Head = pucDispString;
	
	memset(ucLEDDisplayMemory,0x00,(c_MaxColByte*4)*4);
	
	while(*pDispString_Head != '\0')
	{
		if(LED_OK)
		{
			uint8_t ucCharHigh;
			uint8_t ucCharLow;
			ucCharHigh = *pDispString_Head++;
			if(ucCharHigh>0x80)	
			{
				//中文字符的处理
				ucCharLow = *pDispString_Head++;
				uiHZCode =(((uint16_t)ucCharHigh)<<8)+ucCharLow;
			
				uiIndex=0;
				
				while(1)
				{
					if(ZK_Data[uiIndex].uiHZcode == 0xFFFF)
					{
						bResultFlag = 0;
						break;
					}
					
					if(ZK_Data[uiIndex].uiHZcode == uiHZCode)
					{
						//-找到
						bResultFlag = 1;
						break;
					}
					else
					{
						uiIndex++;
					}
				}

				if(bResultFlag)
					memcpy(ucHZDataBuff,ZK_Data[uiIndex].model,32);		 
				else
					GetHZDot(uiHZCode,ucHZDataBuff);
				
				for(uci=1; uci<5; uci++)
				{
					if(LED_OK)
					{
						ucZMData = ucHZDataBuff[30-(16-uci)*2];
						ucLEDDisplayMemory[uci*(c_MaxColByte+1)*4-4] =ucZMData; //uci*
						
						ucZMData = ucHZDataBuff[30-(16-uci)*2+8];
						ucLEDDisplayMemory[uci*(c_MaxColByte+1)*4-3] =ucZMData;
						
						ucZMData = ucHZDataBuff[30-(16-uci)*2+16];
						ucLEDDisplayMemory[uci*(c_MaxColByte+1)*4-2] =ucZMData;
						
						ucZMData = ucHZDataBuff[30-(16-uci)*2+24];
						ucLEDDisplayMemory[uci*(c_MaxColByte+1)*4-1] =ucZMData;
					}
					else
					{
						goto rexit;
					}
				}

				for(ucRowLineCount=0; ucRowLineCount<4; ucRowLineCount++)
				{
					if(LED_OK)
					{
							memcpy(ucLEDDisplayMemory+ucRowLineCount*4*(c_MaxColByte+1),ucLEDDisplayMemory+ucRowLineCount*4*(c_MaxColByte+1)+4,4*c_MaxColByte);
					}
					else
					{
						goto rexit;
					}
				}				//处理右半个汉字,卷入屏幕最右端
				for(uci=1; uci<5; uci++)
				{
					if(LED_OK)
					{
						ucZMData = ucHZDataBuff[32-(16-uci)*2-1];
						ucLEDDisplayMemory[uci*(c_MaxColByte+1)*4-4] =ucZMData;
						
						ucZMData = ucHZDataBuff[32-(16-uci)*2-1+8];
						ucLEDDisplayMemory[uci*(c_MaxColByte+1)*4-3] =ucZMData;
						
						ucZMData = ucHZDataBuff[32-(16-uci)*2-1+16];
						ucLEDDisplayMemory[uci*(c_MaxColByte+1)*4-2] =ucZMData;
						
						ucZMData = ucHZDataBuff[32-(16-uci)*2-1+24];
						ucLEDDisplayMemory[uci*(c_MaxColByte+1)*4-1] =ucZMData;
					}
					else
					{
						goto rexit;
					}
				}
				for(ucRowLineCount=0; ucRowLineCount<4; ucRowLineCount++)
				{
					if(LED_OK)
					{
							memcpy(ucLEDDisplayMemory+ucRowLineCount*4*(c_MaxColByte+1),ucLEDDisplayMemory+ucRowLineCount*4*(c_MaxColByte+1)+4,4*c_MaxColByte);
					}
					else
					{
						goto rexit;
					}
				}			
			}
			else //xiwen
			{
				if(LED_OK)
				{
					for(uci=1; uci<5; uci++)
					{		 
							ucZMData = ASCII816[ucCharHigh - 0x20][uci-1];
							ucLEDDisplayMemory[uci*(c_MaxColByte+1)*4-4] =ucZMData;
							
							ucZMData = ASCII816[ucCharHigh - 0x20][uci-1+4];
							ucLEDDisplayMemory[uci*(c_MaxColByte+1)*4-3] =ucZMData;
							
							ucZMData = ASCII816[ucCharHigh - 0x20][uci-1+8];
							ucLEDDisplayMemory[uci*(c_MaxColByte+1)*4-2] =ucZMData;
							
							ucZMData = ASCII816[ucCharHigh - 0x20][uci-1+12];
							ucLEDDisplayMemory[uci*(c_MaxColByte+1)*4-1] =ucZMData;
					}
					for(ucRowLineCount=0; ucRowLineCount<4; ucRowLineCount++)
					{
						if(LED_OK)
						{
							memcpy(ucLEDDisplayMemory+ucRowLineCount*4*(c_MaxColByte+1),ucLEDDisplayMemory+ucRowLineCount*4*(c_MaxColByte+1)+4,4*c_MaxColByte);
						}
						else
						{
							goto rexit;
						}
					}
				}
				else
				{
					goto rexit;
				}		
			}
		}
		else
		{
			goto rexit;
		}
	}
	if(LED_OK)
	{		   	   			   				   				   				   				   				   				   				   				   
		ScrollRefreshLEDScreen1(ucRefreshCount,0);
	}
	else
	{
		goto rexit;
	}
rexit:
	uci=1;
}
#endif


//固定显示
void ScrollRefreshLEDScreen1(uint8_t ucLoopCount,uint8_t ucShiftCount)
{
	uint8_t ucRowLineCount;
	uint32_t uiRefreshCount;
	int i;
	
	//以下用于修正系统连接不同数量的单元板时，显示时间的差异性,最小列数为64
	uiRefreshCount = 2;//(ucLoopCount*4)/(c_MaxColNumber/64);//5//64
					 
	while(uiRefreshCount--)
	{
     if(LED_OK)
	  {   
		for(ucRowLineCount=0; ucRowLineCount<4; ucRowLineCount++)   //16????????0??
		{
			uint8_t ucCount;
			uint32_t uiStartAddress;
			uint8_t ucLeftUnit;		//显存中的左单元
			uint8_t *pDispBuff;	
			uiStartAddress = ucRowLineCount*4*(c_MaxColByte+1);//


		    pDispBuff = ucLEDDisplayMemory+uiStartAddress;
			for(ucCount=0; ucCount<4*c_MaxColByte; ucCount++)//6808505
			{
			  if(LED_OK)
			   {
					ucLeftUnit = *pDispBuff++;
					ucColDataBuff[ucCount] = ucLeftUnit;//send data
			   }
			   else
			   {
			    goto shexit;
			   }
			}
			//先将数据列锁存到HC595
            if(LED_OK)
			{
			////ShT_LEDBoard_ColShiftOut(0,c_MaxColByte);
			  if(ucRowLineCount!=4){
			     for(ucCount=0; ucCount<4*c_MaxColByte; ucCount++)
			     { 
				   ShT_LEDBoard_ColShiftOut(ucColDataBuff[ucCount]);
			     }
			   }
			}
			else
			{
			  goto shexit;
			}

//关闭显示
			Set_Row_Disable();
			for(i=0;i<brightness;i++);
//换行    
            if(LED_OK)
			{

              if(ucRowLineCount!=4)
               {
			     	ShT_LEDBoard_RowShiftOut(ucRowLineCount);//选择行号
               }
			}
			else
			{
			  goto shexit;
			}
			
			Set_Row_Enable();

//将HC595锁存的数据输出
            if(ucRowLineCount!=4)
			 {
#if 0
			    c_Col_SOut_High = 0;
			    c_Col_SOut_High = 1;
			    c_Col_SOut_High = 0;
#endif

				GPIO_SetBits(GPIOC, GPIO_Pin_0);
		      //  Set_Row_Enable();
			   	for(i=0;i<0xff;i++);  
                GPIO_ResetBits(GPIOC, GPIO_Pin_0);  
				for(i=0;i<0xf;i++);

								
			   // GPIO_ResetBits(GPIOC, GPIO_Pin_0);        
//开启显示

			 }
			// Set_Row_Enable();
//指向下一行
		}
	  }
         else
              {
                 goto shexit;
              }
      
	}
shexit:	//显示完成后，关闭屏幕显示
	Set_Row_Disable();	
}
