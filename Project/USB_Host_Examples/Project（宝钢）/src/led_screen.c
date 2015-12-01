#include "led_screen.h"
#include "zimo.h"

void static_16_disp_string(uint8_t ucDispRow,uint8_t ucRefreshCount,uint8_t *pucDispString)
{
	uint8_t bResultFlag;
	uint8_t ucZMData;
	uint8_t uci;
	uint8_t  ucHZDataBuff[32];
	uint16_t uiIndex;
	uint16_t uiHZCode;
	uint8_t *pDispString_Head;		//头指针
	uint8_t ucRowLineCount;
	
	ucDispRow = ucDispRow;
	pDispString_Head = pucDispString;
	memset(ucLEDDisplayMemory,0x00,(c_MaxColByte*4)*4);
	
	while(*pDispString_Head != '\0')
	{ 
	   if(LED_OK)
	   {	 	   
		    uint8_t ucCharHigh;
	    	uint8_t ucCharLow;
	       	ucCharHigh = *pDispString_Head++;
			if(ucCharHigh > 0x80 || ucCharHigh == 0x5E)
			{
				if(LED_OK)
				{
					if(ucCharHigh!=0x5E)
					{
						//中文字符的处理
						ucCharLow = *pDispString_Head++;
						uiHZCode =(((uint16_t)ucCharHigh)<<8)+ucCharLow;
					}
					else
					{
						uiHZCode=0xA3AA;
					}
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
					
					for(uci=1; uci<17; uci++)
					{
						if(LED_OK)
						{
							ucZMData = ucHZDataBuff[30-(16-uci)*2];
							ucLEDDisplayMemory[uci*(c_MaxColByte+1)-1] =ucZMData;
						}
						else
						{
							goto rexit;
						}
					}
					for(ucRowLineCount=0; ucRowLineCount<16; ucRowLineCount++)
					{
						if(LED_OK)
						{
							memcpy(ucLEDDisplayMemory+ucRowLineCount*(c_MaxColByte+1),ucLEDDisplayMemory+ucRowLineCount*(c_MaxColByte+1)+1,c_MaxColByte);
						}
						else
						{
							goto rexit;
						}
					}	
								
					//处理右半个汉字,卷入屏幕最右端
					for(uci=1; uci<17; uci++)
					{
						if(LED_OK)
						{
							ucZMData = ucHZDataBuff[32-(16-uci)*2-1];
							ucLEDDisplayMemory[uci*(c_MaxColByte+1)-1] =ucZMData;		 
						}
						else 
						{
						 	goto rexit;
						}
					}
					//整屏移动一字节
					for(ucRowLineCount=0; ucRowLineCount<16; ucRowLineCount++)
					{
						if(LED_OK)
						{
							memcpy(ucLEDDisplayMemory+ucRowLineCount*(c_MaxColByte+1),ucLEDDisplayMemory+ucRowLineCount*(c_MaxColByte+1)+1,c_MaxColByte);
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
					for(uci=1; uci<17; uci++)
					{
						if(LED_OK)
						{
							ucZMData = ASCII816[ucCharHigh - 0x20][uci-1];
							ucLEDDisplayMemory[uci*(c_MaxColByte+1)-1] =ucZMData;
						}
						else
						{
							goto rexit;
						}
					}
				
					for(ucRowLineCount=0; ucRowLineCount<16; ucRowLineCount++)
					{
						if(LED_OK)
						{
							memcpy(ucLEDDisplayMemory+ucRowLineCount*(c_MaxColByte+1),ucLEDDisplayMemory+ucRowLineCount*(c_MaxColByte+1)+1,c_MaxColByte);
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
		static_16_refresh_led_screen(ucRefreshCount,0);
	}
	else
	{
		goto rexit;
	}
	
	rexit:
	uci=1;
}

void static_16_refresh_led_screen(uint8_t ucLoopCount,uint8_t ucShiftCount)
{
	uint8_t ucRowLineCount;
	uint16_t uiRefreshCount;
	//以下用于修正系统连接不同数量的单元板时，显示时间的差异性,最小列数为64
	uiRefreshCount = 1;//(ucLoopCount*4)/(c_MaxColNumber/64);//5//64

	while(uiRefreshCount--)
	{
     if(LED_OK)
	  {   
		for(ucRowLineCount=0; ucRowLineCount<16; ucRowLineCount++)   //16????????0??
		{
			uint8_t ucCount;
			uint16_t uiStartAddress;
			uint8_t ucLeftUnit;		//显存中的左单元	

			uint8_t *pDispBuff;	
			uiStartAddress = ucRowLineCount*(c_MaxColByte+1);


		    pDispBuff = ucLEDDisplayMemory+uiStartAddress;

			for(ucCount=0; ucCount<c_MaxColByte; ucCount++)//6808505
			{
				if(LED_OK)
				{
					ucLeftUnit = *pDispBuff++;
					ucColDataBuff[ucCount] = ucLeftUnit;
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
			   for(ucCount=0; ucCount<c_MaxColByte; ucCount++)
			   {
					led_board_16_coldata_out(ucColDataBuff[ucCount]);
			   }
			}
			else
			{
			  goto shexit;
			}
//关闭显示

			Set_Row_Disable();
		//	for(i=0;i<0xff;i++);
//换行    
            if(LED_OK)
			{

              if(ucRowLineCount!=16)
               {
			     led_board_16_row_number(ucRowLineCount);
               }
			}
			else
			{
			  goto shexit;
			}
			
			Set_Row_Enable();

//将HC595锁存的数据输出
           if(ucRowLineCount!=16)
			 {
#if 0
			    c_Col_SOut_High = 0;
			    c_Col_SOut_High = 1;
			    c_Col_SOut_High = 0;
#endif
                GPIO_ResetBits(GPIOC, GPIO_Pin_0);  
				//for(i=0;i<0xf;i++);
			    GPIO_SetBits(GPIOC, GPIO_Pin_0);
			   	//for(i=0;i<0xf;i++);
			    GPIO_ResetBits(GPIOC, GPIO_Pin_0);        
//开启显示
			   //Set_Row_Enable();
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

void move_16_disp_string(uint8_t ucDispRow,uint8_t ucRefreshCount,uint8_t *pucDispString)	
{
	uint16_t uiIndex;
	uint8_t ucZMData;
	uint8_t uci;
	uint16_t i;
	uint8_t  ucHZDataBuff[32];
	uint16_t uiHZCode;
	uint8_t *pDispString_Head;		//头指针
	uint8_t ucRowLineCount;
	uint8_t bResultFlag;	
	uint8_t ucShiftCount;
	ucDispRow = ucDispRow;

	//启动滚动显示

	pDispString_Head = pucDispString;

	memset(ucLEDDisplayMemory,0x00,(c_MaxColByte+1)*c_MaxRowNumber);

	while(1)
	{ 
		
  	   if(LED_OK)
	   {	 	   
		    uint8_t ucCharHigh;
	    	uint8_t ucCharLow;
			if(*pDispString_Head == '\0')
			{
				pDispString_Head = pucDispString;
			}
	       	ucCharHigh = *pDispString_Head++;
				
			if((ucCharHigh > 0x80) || (ucCharHigh == 0x5E))
			{
              if(LED_OK)
			  {
			  	if(ucCharHigh != 0x5E)
				{
					//中文字符的处理
		       		ucCharLow = *pDispString_Head++;
					uiHZCode =(((uint16_t)ucCharHigh)<<8)+ucCharLow;
				}
				else
				{
					uiHZCode=0xA3AA;
				}
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
			
				for(uci=1; uci<17; uci++)
				{
				   if(LED_OK)
				   {
				   	 	ucZMData = ucHZDataBuff[30-(16-uci)*2];
						ucLEDDisplayMemory[uci*(c_MaxColByte+1)-1] =ucZMData;
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
						 move_16_refresh_led_screen(ucRefreshCount,ucShiftCount);
						 for(i=0;i<0x5ff;i++);
					}
					else
					{
						goto rexit;
					}
				}
				//整屏移动一字节
				for(ucRowLineCount=0; ucRowLineCount<16; ucRowLineCount++)
				{
					 if(LED_OK)
				  	 {
				 		 memcpy(ucLEDDisplayMemory+ucRowLineCount*(c_MaxColByte+1),ucLEDDisplayMemory+ucRowLineCount*(c_MaxColByte+1)+1,c_MaxColByte);
  				 	 }
				  	 else
				  	 {
				 	  	goto rexit;
				  	  }
				}
				
				//处理右半个汉字,卷入屏幕最右端
				for(uci=1; uci<17; uci++)
				{
				   if(LED_OK)
				   {
						ucZMData = ucHZDataBuff[32-(16-uci)*2-1];
						ucLEDDisplayMemory[uci*(c_MaxColByte+1)-1] =ucZMData; 				
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
						move_16_refresh_led_screen(ucRefreshCount,ucShiftCount);
						for(i=0;i<0x5ff;i++);
				  	}
				  	else
				  	{
				    	goto rexit;
				  	}
				}
				//整屏移动一字节
				for(ucRowLineCount=0; ucRowLineCount<16; ucRowLineCount++)
				{
				  	if(LED_OK)
				   	{
						memcpy(ucLEDDisplayMemory+ucRowLineCount*(c_MaxColByte+1),ucLEDDisplayMemory+ucRowLineCount*(c_MaxColByte+1)+1,c_MaxColByte);
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
					for(uci=1; uci<17; uci++)
					{
				   		if(LED_OK)
				   		{
							ucZMData = ASCII816[ucCharHigh - 0x20][uci-1];
							ucLEDDisplayMemory[uci*(c_MaxColByte+1)-1] =ucZMData;
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
							move_16_refresh_led_screen(ucRefreshCount,ucShiftCount);
							for(i=0;i<0x5ff;i++);
				  		}
				  		else
				  		{
				    		goto rexit;
				  		}
				   }
				   for(ucRowLineCount=0; ucRowLineCount<16; ucRowLineCount++)
				   {
				  		if(LED_OK)
				   		{
							memcpy(ucLEDDisplayMemory+ucRowLineCount*(c_MaxColByte+1),ucLEDDisplayMemory+ucRowLineCount*(c_MaxColByte+1)+1,c_MaxColByte);
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

void move_16_refresh_led_screen(uint8_t ucLoopCount,uint8_t ucShiftCount)
{
	uint8_t ucRowLineCount;
	uint8_t uiRefreshCount;
	//以下用于修正系统连接不同数量的单元板时，显示时间的差异性,最小列数为64
	uiRefreshCount = 5;//(ucLoopCount*4)/(c_MaxColNumber/64);//5//64

	while(uiRefreshCount--)
	{
     	if(LED_OK)
	  {   
		for(ucRowLineCount=0; ucRowLineCount<16; ucRowLineCount++)   //16????????0??
		{
		  
			uint8_t ucCount;
			uint32_t uiStartAddress;
			uint8_t ucLeftUnit;		//显存中的左单元
			uint8_t ucRightUnit;	

			uint8_t *pDispBuff;	
			uiStartAddress = ucRowLineCount*(c_MaxColByte+1);


		    pDispBuff = ucLEDDisplayMemory+uiStartAddress;


			for(ucCount=0; ucCount<c_MaxColByte; ucCount++)//6808505
			{
			  if(LED_OK)
			   {
				ucLeftUnit = *pDispBuff++;
				ucRightUnit = *pDispBuff;
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
			   for(ucCount=0; ucCount<c_MaxColByte; ucCount++)
			   {
				  led_board_16_coldata_out(ucColDataBuff[ucCount]);
			   }
			}
			else
			{
			  goto shexit;
			}
//关闭显示

			Set_Row_Disable();
//换行    
            if(LED_OK)
			{

              if(ucRowLineCount!=16)
               {
			     led_board_16_row_number(ucRowLineCount);
               }
			}
			else
			{
			  goto shexit;
			}
			
			Set_Row_Enable();

//将HC595锁存的数据输出
           if(ucRowLineCount!=16)
			 {
                GPIO_ResetBits(GPIOC, GPIO_Pin_0);  
				//for(i=0;i<0xf;i++);
			    GPIO_SetBits(GPIOC, GPIO_Pin_0);
			   	//for(i=0;i<0xf;i++);
			    GPIO_ResetBits(GPIOC, GPIO_Pin_0);        
//开启显示
			   //Set_Row_Enable();
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

//功能介绍:选择行号
//入口参数：行号
void led_board_16_row_number(uint8_t  ucRowNumber)
{
	if((ucRowNumber&0x08)==0x08)
	   GPIO_SetBits(GPIOB, GPIO_Pin_9); 	
	else
	   GPIO_ResetBits(GPIOB, GPIO_Pin_9); 
 	if((ucRowNumber&0x04)==0x04)
	   GPIO_SetBits(GPIOB, GPIO_Pin_8); 
	else
       GPIO_ResetBits(GPIOB, GPIO_Pin_8); 
 	if((ucRowNumber&0x02)==0x02)
	   GPIO_SetBits(GPIOB, GPIO_Pin_7); 
	else
       GPIO_ResetBits(GPIOB, GPIO_Pin_7); 
 	if((ucRowNumber&0x01)==0x01)
	   GPIO_SetBits(GPIOB, GPIO_Pin_6); 
	else
       GPIO_ResetBits(GPIOB, GPIO_Pin_6); 	   	   
}

void led_board_16_coldata_out(uint8_t Codata)
{
	uint8_t i=0, byte=Codata;
	uint8_t j;
	for (i=0;i<8;i++) 
	{
		for(j=0;j<2;j++);
		GPIO_ResetBits(GPIOC, GPIO_Pin_1); 
		if (byte&0x01)
		GPIO_ResetBits(GPIOC, GPIO_Pin_3); 
		else
		GPIO_SetBits(GPIOC, GPIO_Pin_3); 
		for(j=0;j<2;j++);
		GPIO_ResetBits(GPIOC, GPIO_Pin_1); 
		for(j=0;j<2;j++);
		GPIO_SetBits(GPIOC, GPIO_Pin_1); 
		//for(j=0;j<10;j++);
		byte=byte>>1;
	} 
}

