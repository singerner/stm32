/**
  ******************************************************************************
  * @file    main.c
  * @author  MCD Application Team
  * @version V2.1.0
  * @date    19-March-2012
  * @brief   USB host MSC class demo main file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "usbh_core.h"
#include "usbh_usr.h"
#include "usbh_msc_core.h"
#include <stdlib.h>
#include "include.h"

/** @addtogroup USBH_USER
* @{
*/

/** @defgroup USBH_USR_MAIN
* @brief This file is the MSC demo main file
* @{
*/ 

uint8_t ucDispCMDDataBuff[SIZE] = {0};
uint8_t *welcom = "欢迎光临";
uint8_t ucDispCMDDataBuffTemp[SIZE] = {0};
uint8_t disp_buff[SIZE] = {0};
uint8_t c_MaxColByte = 16;
uint8_t led_type = 0x30;
uint8_t LED_OK = 0;
uint8_t *ucLEDDisplayMemory;
uint8_t *ucColDataBuff;
uint8_t bflag;
uint32_t brightness = 0x4FF;
uint8_t message_state;
uint8_t check_ok = 0;
uint32_t check_ret = 0;
/** @defgroup USBH_USR_MAIN_Private_TypesDefinitions
* @{
*/ 
/**
* @}
*/ 

/** @defgroup USBH_USR_MAIN_Private_Defines
* @{
*/ 
/**
* @}
*/ 


/** @defgroup USBH_USR_MAIN_Private_Macros
* @{
*/ 
/**
* @}
*/ 

/** @defgroup USBH_USR_MAIN_Private_Variables
* @{
*/
#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
  #if defined ( __ICCARM__ ) /*!< IAR Compiler */
    #pragma data_alignment=4   
  #endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */
__ALIGN_BEGIN USB_OTG_CORE_HANDLE      USB_OTG_Core __ALIGN_END;

#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
  #if defined ( __ICCARM__ ) /*!< IAR Compiler */
    #pragma data_alignment=4   
  #endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */
__ALIGN_BEGIN USBH_HOST                USB_Host __ALIGN_END;
/**
* @}
*/ 

/** @defgroup USBH_USR_MAIN_Private_FunctionPrototypes
* @{
*/ 
/**
* @}
*/ 
void gpio_init(void);
void cpu_run(void);
void cpu_stop(void);
void flash_run(void);
void flash_stop(void);
void get_line_from_flash(void);
void get_cfg_from_flash(void);
void alloc_memory(void);
void add_line_info(const uint8_t *src, uint8_t *dst, uint8_t n_insert);
uint8_t get_light_state(void);
uint8_t get_disp_state(void);
void data_process(void);
/** @defgroup USBH_USR_MAIN_Private_Functions
* @{
*/ 

/**
* @brief  Main routine for MSC class application
* @param  None
* @retval int
*/
int main(void)
{
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0xC000);
	/* Init Host Library */
	USBH_Init(&USB_OTG_Core, 
	#ifdef USE_USB_OTG_FS  
		USB_OTG_FS_CORE_ID,
	#else
		USB_OTG_HS_CORE_ID,
	#endif 
	&USB_Host,
	&USBH_MSC_cb, 
	&USR_cb);
	gpio_init();
	Set_Row_Disable();
	cpu_run();
	flash_stop();	
	FLASH_ON();
	delay_ms(1);
	SPI_Flash_Init();
	get_cfg_from_flash();
	get_line_from_flash();	 	
	message_state |= MESSAGE_LINE_NEW;
	alloc_memory();
	while(check_ret < 0xFFFF)
	{
		USBH_Process(&USB_OTG_Core, &USB_Host);
		if(check_ok == 1)
		{
			break;
		}
		else
		{
			check_ret ++;
		}
	}
	wwdg_init(0x7F, 0x5F, WWDG_Prescaler_8); //计数器值为7f,窗口寄存器为5f,分频数为8
	while (1)
	{
		if(check_ok)
		{
			check_ok = 0;
			data_process();
			LED_OK = 1;
		}
		else
		{
			get_disp_state();		
			if(bflag)
			{
				static_16_disp_string(1, 1, disp_buff);  
			}
			else
			{
				move_16_disp_string(1, 1, disp_buff);
			}
		}
	}
}


void data_process(void)//把数据写到指定的缓存
{
	uint8_t ct = 0, len = 0;
	uint8_t size = 0;
	uint8_t tempbuff[SIZE] = {0};
	memset(ucDispCMDDataBuff, 0x00, SIZE);
	for(ct = 0, len = 0; ucDispCMDDataBuffTemp[len] != '\0'; len ++, ct ++)
	{
		if((ucDispCMDDataBuffTemp[len] == 0x0D) || (ucDispCMDDataBuffTemp[len] == 0x0A))
		{
			continue;
		}
		else
		{
			ucDispCMDDataBuff[ct] = ucDispCMDDataBuffTemp[len];
		}
	}
	if(ct <= c_MaxColByte)
	{
		size = c_MaxColByte - ct;
		for(len = 0; len < size; len ++)
		{
			tempbuff[len] = 0x20;
		}
		if((ct % 2) == 0)
		{
			add_line_info(ucDispCMDDataBuff, tempbuff, size / 2);
		}
		else
		{
			add_line_info(ucDispCMDDataBuff, tempbuff, size / 2 + 1);
		}
		for(len = 0; len < SIZE; len ++)
		{
			ucDispCMDDataBuff[len] = tempbuff[len];
		}
	}
	
	SST25_W_BLOCK(ADADDR, ucDispCMDDataBuff, SIZE);//把数据写到指定的地址里面
	message_state |= MESSAGE_LINE_NEW;
}



uint8_t get_disp_state(void)//清除多余的字节
{
	uint8_t len = 0;
	memset(disp_buff, 0x00, SIZE);
	if((get_light_state()) == Bit_RESET)
	{
		brightness = 0xEFF;
	}
	else
	{
		brightness = 0x4FF;
	}
	if((message_state & MESSAGE_LINE_NEW) == MESSAGE_LINE_NEW)
	{
		for(len = 0; len < SIZE; len ++)
		{
			if(ucDispCMDDataBuff[len] == '\0')
			{
				break;
			}
			disp_buff[len] = ucDispCMDDataBuff[len];
		}
		if(len > c_MaxColByte)
		{
			disp_buff[len] = 0x20;
			disp_buff[len + 1] = 0x20;
			disp_buff[len + 2] = 0x20;
			disp_buff[len + 3] = 0x20;
			bflag = 0;
		}
		else
		{
			bflag = 1;
		}
		return 0;
	}
	return 0;
}

uint8_t get_light_state(void)
{
	return ((GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_7)) ? Bit_SET : Bit_RESET);
}

uint8_t get_left_state(void)
{
	return ((GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_5)) ? Bit_SET : Bit_RESET);
}

uint8_t get_right_state(void)
{
	return ((GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_4)) ? Bit_SET : Bit_RESET);
}

uint8_t get_brake_state(void)
{
	return ((GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12)) ? Bit_SET : Bit_RESET);
}

void get_line_name(const uint8_t *src, uint8_t *dst)//合并汉字
{
	uint8_t ct = 0, len = 0;
	uint8_t uCharLow = 0, uCharHigh = 0;
	uint16_t Code = 0;
	for(ct = 0; src[ct] != '\0'; ct ++)
	{
		if(src[ct] == 0x20)
		{
			continue;
		}
		uCharHigh = src[ct];
		uCharLow = src[++ ct];
		Code = (uint16_t)(uCharHigh << 8) + uCharLow;
		if(Code == 0xA3AA || Code == 0xA1A1)
		{
			continue;
		}
		else
		{
			dst[len ++] = uCharHigh;
			dst[len ++] = uCharLow;
		}
	}
}

void alloc_memory(void)
{
	ucLEDDisplayMemory = (uint8_t *)malloc((c_MaxColByte * 4) *4);//缓存
	ucColDataBuff = (uint8_t *)malloc(c_MaxColByte);//一行数据缓冲区
}

void get_cfg_from_flash(void)//区分是前后侧屏
{
	uint8_t cfg_buf[50] = {0};
	uint8_t cfg[2] = {0};
	SST25_R_BLOCK(CFGADDR, cfg_buf, 50);
	if((cfg_buf[0] != 0x00) && (cfg_buf[1] != 0x00) && (cfg_buf[0] != 0xFF) && (cfg_buf[1] != 0xFF))
	{
		if((cfg_buf[2] == 0x0A) && (cfg_buf[3] == 0x0D))
		{
			if((0x01 <= cfg_buf[0]) && (cfg_buf[0] <= 0x09))
			{
				cfg[0] = cfg_buf[0];		
			}
			else if((0x10 <= cfg_buf[0]) && (cfg_buf[0] <= 0x19))
			{
				cfg[0] = cfg_buf[0] - 0x06;
			}
			c_MaxColByte = cfg[0] * 2;
			cfg[1] = cfg_buf[1];
			switch(cfg[1])
			{
			case FRONT:
				led_type = FRONT;
				break;
			case REAR:
				led_type = REAR;
				break;
			case SIDE:
				led_type = SIDE;
				break;
			default:
				break;
			}
		}
	}
}

void get_line_from_flash(void)
{
	uint8_t ct = 0, num = 0;
	
 	SST25_R_BLOCK(ADADDR, ucDispCMDDataBuff, SIZE);
	if((ucDispCMDDataBuff[0] == 0xFF) || (ucDispCMDDataBuff[1] == 0xFF) || (ucDispCMDDataBuff[SIZE - 1] == 0xFF))
	{
		memset(ucDispCMDDataBuff, 0x00, SIZE);
		num = c_MaxColByte - 8;
		for(ct = 0; ct < num; ct ++)
		{
			ucDispCMDDataBuff[ct] = 0x20;
		}
		switch(led_type) 
		{
			case FRONT:
			case REAR:
				add_line_info(welcom, ucDispCMDDataBuff, num / 2);
				break;
			case SIDE:
				add_line_info(welcom, ucDispCMDDataBuff, num / 2);
				break;
			default:
				break;
		}
		bflag = 1;
	}
	else
	{
		switch(led_type)
		{
			case FRONT:
			case REAR:
			case SIDE:
				ct = 0;
				while(ucDispCMDDataBuff[ct ++] != '\0');
				if(ct <= c_MaxColByte)
				{
					bflag = 1;
				}
				else
				{
					bflag = 0;
				}
				break;
			default:
				break;
		}
	}
	LED_OK = 1;
}

void add_line_info(const uint8_t *src, uint8_t *dst, uint8_t n_insert)
{
	uint8_t len = 0;
	uint8_t dst_len = 0, src_len = 0;
	
	while(dst[dst_len ++] != '\0');
	while(src[src_len ++] != '\0');	
	
	while(dst_len > n_insert)
	{
		dst[(dst_len - 1) + (src_len - 1)] = dst[dst_len - 1];
		dst[dst_len - 1] = '\0';
		dst_len --;
	}
	for(len = 0; len < (src_len - 1); len ++)
	{
		dst[n_insert + len] = src[len];
	}
}


void gpio_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE);	
	
	/*1:485 control 8:flash light*/
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/*5: cpu light  10:flash on/off*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* PC12 loop on/off */ 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	/*0:A  1:B  2:C  3:D*/
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	/*0:load  1:clk  2:blank  3:data*/
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	/*Light*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}

void rs485_recv(void)
{
	GPIO_ResetBits(GPIOA, GPIO_Pin_1);
}

void rs485_send(void)
{
	GPIO_SetBits(GPIOA, GPIO_Pin_1);
}

void cpu_run(void)
{
	GPIO_ResetBits(GPIOB, GPIO_Pin_5);
}

void cpu_stop(void)
{
	GPIO_SetBits(GPIOB, GPIO_Pin_5);
}


void flash_run(void)
{
	GPIO_ResetBits(GPIOA, GPIO_Pin_8);
}

void flash_stop(void)
{
	GPIO_SetBits(GPIOA, GPIO_Pin_8);
}


#ifdef USE_FULL_ASSERT
/**
* @brief  assert_failed
*         Reports the name of the source file and the source line number
*         where the assert_param error has occurred.
* @param  File: pointer to the source file name
* @param  Line: assert_param error line source number
* @retval None
*/
void assert_failed(uint8_t* file, uint16_t32_t line)
{
  /* User can add his own implementation to report the file name and line number,
  ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  
  /* Infinite loop */
  while (1)
  {}
}
#endif


/**
* @}
*/ 

/**
* @}
*/ 

/**
* @}
*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
