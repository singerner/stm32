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
#define LOOP_ON()	(GPIO_ResetBits(GPIOC, GPIO_Pin_12))
#define LOOP_OFF()	(GPIO_SetBits(GPIOC, GPIO_Pin_12))

#define FLASH_ON()	(GPIO_SetBits(GPIOB, GPIO_Pin_10))
#define FLASH_OFF()	(GPIO_ResetBits(GPIOB, GPIO_Pin_10))
#define SIZE		(64)
#define CFGADDR		(0x00)//0K-4K
#define LINEADDR	(0x1000)//4K-8K


#define FRONT		(0x30)
#define	REAR		(0x31)
#define SIDE		(0x32)
#define INSIDE		(0x33)


#define DEVICE_INSIDE	(0x01)
#define DEVICE_FRONT	(0X02)
#define DEVICE_SIDE		(0x03)
#define DEVICE_REAR		(0x04)	

#define LEFT		(0x01)
#define RIGHT		(0x02)
#define BRAKE		(0x03)

#define MESSAGE_LINE_NEW	(0x01)
#define MESSAGE_LEFT_NEW	(0x02)
#define MESSAGE_RIGHT_NEW	(0x04)
#define MESSAGE_BRAKE_NEW	(0x08)
#define MESSAGE_CN_NEW		(0x10)
#define MESSAGE_ENG_NEW		(0x20)
#define MESSAGE_TEMP_NEW	(0x40)
#define MESSAGE_FUWU_NEW	(0x80)

/** @defgroup USBH_USR_MAIN
* @brief This file is the MSC demo main file
* @{
*/ 

uint8_t ucDispCMDDataBuff[SIZE] = {0};//Line
uint8_t ucDispCMDDataBuffENG[SIZE * 2] = {0};//ENG
uint8_t ucDispCMDDataBuffTEMP[20] = "当前温度 00℃";
uint8_t ucDispCMDDataBuffFUWU[SIZE * 3] = {0};//FUWU
uint8_t ucDispCMDLeftDataBuff[SIZE] = {0xA1,0xFB,0xD7,0xF3,0xD7,0xAA,0xCD,0xE4};//左转弯
uint8_t ucDispCMDRightDataBuff[SIZE] = {0xD3,0xD2,0xD7,0xAA,0xCD,0xE4, 0xA1,0xFA};//右转弯
uint8_t ucDispCMDBrakeDataBuff[SIZE] = {0xC9,0xB2,0xB3,0xB5,0xA3,0xA1};//刹车！
uint8_t *welcom = "欢迎乘坐";
uint8_t ucDispCMDDataBuffTemp[SIZE] = {0};
uint8_t disp_buff[SIZE * 3] = {0};
uint8_t c_MaxColByte = 16;
uint8_t led_type = DEVICE_INSIDE;
uint8_t Line[20] = {0};
uint8_t LED_OK = 0;
uint8_t *ucLEDDisplayMemory;
uint8_t *ucColDataBuff;
uint8_t bflag;
uint32_t brightness = 0x4FF;
uint16_t message_state = 0;

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
void rs485_recv(void);
void rs485_send(void);
void get_line_from_flash(void);
void get_cfg_from_flash(void);
void alloc_memory(void);
void add_line_info(const uint8_t *src, uint8_t *dst, uint8_t n_insert);
void get_line_name(const uint8_t *src, uint8_t *dst);
uint8_t get_light_state(void);
uint8_t get_left_state(void);
uint8_t get_right_state(void);
uint8_t get_brake_state(void);
uint8_t get_disp_state(void);
void check_data(void);
void detail_lupai_info(void);
void detail_inside_info(void);
void head_process(void);
void side_process(void);
void rear_process(void);
void refresh_display_type(uint8_t flag);
void read_tempature(void);

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
	gpio_init();
	cpu_run();
	flash_stop();
	rs485_recv();
	FLASH_ON();
	LOOP_ON();
	uart_init(9600);
	delay_ms(1);
	SPI_Flash_Init();
	get_cfg_from_flash();
	get_line_from_flash();	 
	if(led_type != DEVICE_INSIDE)
	{
		message_state |= MESSAGE_LINE_NEW;
		message_state |= MESSAGE_LEFT_NEW;
		message_state |= MESSAGE_RIGHT_NEW;
		message_state |= MESSAGE_BRAKE_NEW;
	}
	alloc_memory();
	DS18B20_Init();
	wwdg_init(0x7F, 0x4F, WWDG_Prescaler_8); //计数器值为7f,窗口寄存器为4f,分频数为8
	while (1)
	{
		if(uart_get_flag)
		{
			uart_get_flag = 0;
			check_data();
			LED_OK = 1;
		}
		get_disp_state();
		switch(led_type)
		{
			case DEVICE_FRONT:
			case DEVICE_SIDE:
			case DEVICE_REAR:
				if(bflag)
				{
					static_4_disp_string(1, 1, disp_buff);    
				}
				else
				{
					move_4_disp_string(1, 1, disp_buff);
				}
				break;
			case DEVICE_INSIDE:
				if(bflag)
				{
					static_16_disp_string(1, 1, disp_buff);
				}
				else
				{
					move_16_disp_string(1, 1, disp_buff);
				}
				break;
		}
	}
}

void refresh_display_type(uint8_t type)
{
	uint8_t *dst = NULL;
	uint8_t n_insert = 0;
	uint8_t len = 0, ct = 0, num = 0;
	uint8_t blankbytes = 0;
	uint8_t tempbuff[32] = {0};
	uint8_t left_buf[SIZE] = {0xA1,0xFB,0xD7,0xF3,0xD7,0xAA,0xCD,0xE4};
	uint8_t right_buf[SIZE] = {0xD3,0xD2,0xD7,0xAA,0xCD,0xE4,0xA1,0xFA,};
	uint8_t brake_buf[SIZE] = {0xC9,0xB2,0xB3,0xB5,0xA3,0xA1};
	
	while(Line[len ++] != '\0');
	len --;
	
	switch (type)
	{
		case LEFT:		
			dst = left_buf;
			blankbytes = 8;
			n_insert = 2;
			break;
		case RIGHT:
			dst = right_buf;
			blankbytes = 8;
			n_insert = 6;
			break;
		case BRAKE:
			dst = brake_buf;
			blankbytes = 6;
			n_insert = 0;
			break;
		default:
			break;
	}
	
	if(len > (c_MaxColByte - blankbytes))
	{
		 add_line_info(Line, dst, n_insert); 
	}
	else
	{
		num = ((c_MaxColByte - blankbytes) - len);
		for(ct = 0; ct < num; ct ++)
		{
			tempbuff[ct] = 0x20;
		}
		add_line_info(Line, tempbuff, num / 2);
		add_line_info(tempbuff, dst, n_insert);
	}
	
	for(ct = 0; ct < SIZE; ct ++)
	{
		if(type == LEFT)
		{
			ucDispCMDLeftDataBuff[ct] = left_buf[ct];
		}
		else if(type == RIGHT)
		{
			ucDispCMDRightDataBuff[ct] = right_buf[ct];
		}
		else if(type == BRAKE)
		{
			ucDispCMDBrakeDataBuff[ct] = brake_buf[ct];
		}
	}
}

uint8_t get_disp_state(void)
{
	uint8_t len = 0, ct = 0;
	
	if((get_light_state()) == Bit_RESET)
	{
		brightness = 0xEFF;
	}
	else
	{
		brightness = 0x4FF;
	}
	if(led_type != DEVICE_INSIDE)
	{
		if(led_type == DEVICE_REAR)
		{
			if((message_state & MESSAGE_LEFT_NEW) == MESSAGE_LEFT_NEW)
			{
				refresh_display_type(LEFT);
				message_state &= (~MESSAGE_LEFT_NEW);
			}
			if((get_left_state()) == Bit_RESET)
			{
				memset(disp_buff, 0x00, SIZE * 3);
				for(len = 0; len < (SIZE * 3); len ++)
				{
					if(ucDispCMDLeftDataBuff[len] == '\0')
					{
						break;
					}
					disp_buff[len] = ucDispCMDLeftDataBuff[len];
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
				message_state |= MESSAGE_LINE_NEW;
				return 0;	
			}
		
			if((message_state & MESSAGE_RIGHT_NEW) == MESSAGE_RIGHT_NEW)
			{
				refresh_display_type(RIGHT);
				message_state &= (~MESSAGE_RIGHT_NEW);
			}
			if((get_right_state()) == Bit_RESET)
			{
				memset(disp_buff, 0x00, SIZE * 3);
				for(len = 0; len < (SIZE * 3); len ++)
				{
					if(ucDispCMDRightDataBuff[len] == '\0')
					{
						break;
					}
					disp_buff[len] = ucDispCMDRightDataBuff[len];
				}
				if(len > c_MaxColByte)
				{
					disp_buff[len] = 0x20;
					disp_buff[len + 1] = 0x20;
					disp_buff[len + 2] = 0x20;
					disp_buff[len + 3] = 0x20;
					disp_buff[len + 4] = 0x20;
					bflag = 0;
				}
				else
				{	
					bflag = 1;
				}
				message_state |= MESSAGE_LINE_NEW;
				return 0;
			}
			
			if((message_state & MESSAGE_BRAKE_NEW) == MESSAGE_BRAKE_NEW)
			{
				refresh_display_type(BRAKE);
				message_state &= (~MESSAGE_BRAKE_NEW);
			}
			if((get_brake_state()) == Bit_RESET)
			{
				memset(disp_buff, 0x00, SIZE * 3);  
				for(len = 0; len < (SIZE * 3); len ++)
				{
					if(ucDispCMDBrakeDataBuff[len] == '\0')
					{
						break;
					}
					disp_buff[len] = ucDispCMDBrakeDataBuff[len];
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
				message_state |= MESSAGE_LINE_NEW;
				return 0;	
			}
		}
		if((message_state & MESSAGE_LINE_NEW) == MESSAGE_LINE_NEW)
		{
			memset(disp_buff, 0x00, SIZE * 3);
			message_state &= (~MESSAGE_LINE_NEW);
			for(len = 0; len < (SIZE * 3); len ++)
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
	}
	else
	{
		if((message_state & MESSAGE_CN_NEW) == MESSAGE_CN_NEW)
		{
			memset(disp_buff, 0x00, SIZE * 3);
			
			for(len = 0; len < (SIZE * 3); len ++)
			{
				if(ucDispCMDDataBuff[len] == '\0')
				{
					break;
				}
				disp_buff[len] = ucDispCMDDataBuff[len];
			}
			if(len > c_MaxColByte)
			{
				for(ct = 0; ct < c_MaxColByte; ct ++)
				{
					disp_buff[ct + len] = 0x20;
				}
				bflag = 0;
			}
			else
			{
				bflag = 1;
			}
			message_state &= (~MESSAGE_CN_NEW);			
			if(ucDispCMDDataBuffENG[0] == 0x00)
			{
				message_state &= (~MESSAGE_ENG_NEW);
				message_state |= MESSAGE_TEMP_NEW;
				message_state &= (~MESSAGE_FUWU_NEW);			
			}
			else
			{
				message_state |= MESSAGE_ENG_NEW;	
				message_state &= (~MESSAGE_TEMP_NEW);
				message_state &= (~MESSAGE_FUWU_NEW);			
			}
			
			return 0;
		}
		
		if((message_state & MESSAGE_ENG_NEW) == MESSAGE_ENG_NEW)
		{
			memset(disp_buff, 0x00, SIZE * 3);
			
			for(len = 0; len < (SIZE * 3); len ++)
			{
				if(ucDispCMDDataBuffENG[len] == '\0')
				{
					break;
				}
				disp_buff[len] = ucDispCMDDataBuffENG[len];
			}
			for(ct = 0; ct < c_MaxColByte; ct ++)
			{
				disp_buff[ct + len] = 0x20;
			}
			bflag = 0;
			message_state &= (~MESSAGE_CN_NEW);	
			message_state &= (~MESSAGE_ENG_NEW);
			message_state |= (MESSAGE_TEMP_NEW);
			message_state &= (~MESSAGE_FUWU_NEW);
			return 0;
		}
		
		if((message_state & MESSAGE_TEMP_NEW) == MESSAGE_TEMP_NEW)
		{
			memset(disp_buff, 0x00, SIZE * 3);

			read_tempature();
			len = c_MaxColByte - 14;
			for(ct = 0; ct < len; ct ++)
			{
				disp_buff[ct] = 0x20;
			}
			add_line_info(ucDispCMDDataBuffTEMP, disp_buff, len / 2 + 1);
			bflag = 1;
			message_state &= (~MESSAGE_TEMP_NEW);
			if(ucDispCMDDataBuffFUWU[0] == 0)
			{
				message_state |= MESSAGE_CN_NEW;
				message_state &= (~MESSAGE_ENG_NEW);
				message_state &= (~MESSAGE_FUWU_NEW);
			}
			else
			{
				message_state &= (~MESSAGE_CN_NEW);
				message_state &= (~MESSAGE_ENG_NEW);
				message_state |= MESSAGE_FUWU_NEW;
			}

			return 0;
		}
		
		if((message_state & MESSAGE_FUWU_NEW) == MESSAGE_FUWU_NEW)
		{
			memset(disp_buff, 0x00, SIZE * 3);
			
			for(len = 0; len < (SIZE * 3); len ++)
			{
				if(ucDispCMDDataBuffFUWU[len] == '\0')
				{
					break;
				}
				disp_buff[len] = ucDispCMDDataBuffFUWU[len];
			}
			for(ct = 0; ct < c_MaxColByte; ct ++)
			{
				disp_buff[ct + len] = 0x20;
			}
			bflag = 0;
			message_state &= MESSAGE_FUWU_NEW;
			if(ucDispCMDDataBuff[0] == 0)
			{
				message_state &= (~MESSAGE_CN_NEW);
				message_state &= (~MESSAGE_ENG_NEW);
				message_state |= MESSAGE_TEMP_NEW;
			}
			else
			{
				message_state |= MESSAGE_CN_NEW;
				message_state &= (~MESSAGE_ENG_NEW);
				message_state &= (~MESSAGE_TEMP_NEW);
			}
			return 0;
		}
	}
	return 0;
}

void read_tempature(void)
{
	double tempature;
	uint8_t temp;
	uint8_t tmp[2] = {0};
	uint8_t ct = 2;
	while(ct --)
	{
		tempature = DS18B20_Get_wd();
	}
	temp = (uint8_t)tempature;
	if(temp >= 35)
	{
		tmp[0] = 0x30;
		tmp[1] = 0x35;
	}
	else if(temp <= 0)
	{
		tmp[0] = 0x30;
		tmp[1] = 0x30;
	}
	else
	{
		tmp[0] = temp / 10 + 0x30;
		tmp[1] = temp % 10 + 0x30;
	}
	ucDispCMDDataBuffTEMP[9] = tmp[0];
	ucDispCMDDataBuffTEMP[10] = tmp[1];
}


void check_data(void)
{
	uint8_t ct = 6;
	uint16_t crc_value = 0;
	crc_value = crc_ccitt(uart_data, data_len - 2);
	if(((crc_value & 0xFF) == uart_data[data_len - 2]) && (((crc_value >> 8) & 0xFF) == uart_data[data_len - 1]))
	{
		if(led_type != DEVICE_INSIDE)
		{
			if(uart_data[ct] == COMMAND_UPDATE_OUTLINE)//update_line_info
			{
				detail_lupai_info();
				SST25_W_BLOCK(LINEADDR, ucDispCMDDataBuff, SIZE);
			}
		}
		else
		{
			detail_inside_info();
		}
	}
	memset(uart_data, 0x00, sizeof(uart_data) / sizeof(uint8_t));
	memset(ucDispCMDDataBuffTemp, 0x00, sizeof(ucDispCMDDataBuffTemp) / sizeof(uint8_t));
}

void detail_inside_info(void)
{
	uint8_t ct = 6;
	uint16_t len = 0, count = 0, ret = 0;
	uint8_t size = 0;
	uint8_t tempbuff[SIZE] = {0};

	if(uart_data[ct] == COMMAND_UPDATE_INSTATION)//update station info
	{
		memset(ucDispCMDDataBuff, 0x00, SIZE);
		memset(ucDispCMDDataBuffENG, 0x00, SIZE * 2);
		for(len = 15; uart_data[len] != '\0'; len ++, count ++)
		{
			tempbuff[count] = uart_data[len];
		}
		if(count <= c_MaxColByte)
		{
			size = c_MaxColByte - count;
			for(ret = 0; ret < size; ret ++)
			{
				ucDispCMDDataBuff[ret] = 0x20;
			}
			if((count % 2) == 0)
			{
				add_line_info(tempbuff, ucDispCMDDataBuff, size / 2);
			}
			else
			{
				add_line_info(tempbuff, ucDispCMDDataBuff, size / 2 + 1);
			}
		}
		else
		{
			for(count = 0; count < SIZE; count ++)
			{
				ucDispCMDDataBuff[count] = disp_buff[count];
			}
		}
		for(len ++, count = 0; uart_data[len] != '\0'; len ++, count ++)
		{
			ucDispCMDDataBuffENG[count] = uart_data[len];
		}	
	}
	else if(uart_data[ct] == COMMAND_UPDATE_INSERVICE)//update service info
	{
		memset(ucDispCMDDataBuffFUWU, 0x00, SIZE * 3);
		for(len = 13, count = 0; uart_data[len] != '\0'; len ++, count ++)
		{
			ucDispCMDDataBuffFUWU[count] = uart_data[len];
		}
	}
	message_state |= MESSAGE_CN_NEW;
	message_state &= (~MESSAGE_ENG_NEW);
	message_state &= (~MESSAGE_TEMP_NEW);
	message_state &= (~MESSAGE_FUWU_NEW);
}

void detail_lupai_info(void)
{
	uint8_t ct = 0, len = 0;
	uint8_t size = 0;
	uint8_t tempbuff[SIZE] = {0};
	
	memset(ucDispCMDDataBuff, 0x00, SIZE);
	for(len = 11; uart_data[len] != '\0'; len ++)
	{
		if((uart_data[len] > 0x2F) && (uart_data[len] < 0x3A))//0~9
		{
			ucDispCMDDataBuffTemp[ct ++] = 0xA3;
			ucDispCMDDataBuffTemp[ct ++] = 0xB0 + uart_data[len] - 0x30;
		}
		else if((uart_data[len] > 0x60) && (uart_data[len] < 0x7B))//a~z
		{
			ucDispCMDDataBuffTemp[ct ++] = 0xA3;
			ucDispCMDDataBuffTemp[ct ++] = 0xE1 + uart_data[len] - 0x61;
		}
		else if((uart_data[len] > 0x40) && (uart_data[len] < 0x5B))//A~Z
		{
			ucDispCMDDataBuffTemp[ct ++] = 0xA3;
			ucDispCMDDataBuffTemp[ct ++] = 0xC1 + uart_data[len] - 0x41;
		}
		else
		{
			ucDispCMDDataBuffTemp[ct ++] = uart_data[len];
		}
	}
	switch (led_type)
	{
		case DEVICE_FRONT:
			break;
		case DEVICE_SIDE:
			break;
		case DEVICE_REAR:
			rear_process();
			for(ct = 0; ucDispCMDDataBuffTemp[ct] != '\0'; ct ++);
			break;
		default:
			break;	
	}
	if(ct <= c_MaxColByte)
	{
		size = c_MaxColByte - ct;
		for(len = 0; len < size; len ++)
		{
			tempbuff[len] = 0x20;
		}
		if(ct % 2 == 0)
		{
			add_line_info(ucDispCMDDataBuffTemp, tempbuff, size / 2);
		}
		else
		{
			add_line_info(ucDispCMDDataBuffTemp, tempbuff, size / 2 + 1);
		}
		for(len = 0; len < SIZE; len ++)
		{
			ucDispCMDDataBuff[len] = tempbuff[len];
		}
	}
	else
	{
		for(len = 0; len < SIZE; len ++)
		{
			ucDispCMDDataBuff[len] = ucDispCMDDataBuffTemp[len];
		}
	}
	message_state |= MESSAGE_LINE_NEW;
}


void rear_process(void)
{
	uint8_t ucharhigh, ucharlow;
	uint16_t code;
	uint8_t ct = 0, len = 0;
	uint8_t linebuff[20] = {0};
	for(ct = 0; ucDispCMDDataBuffTemp[ct] != '\0'; ct ++, len ++)
	{
		ucharhigh = ucDispCMDDataBuffTemp[ct];
		if(ucharhigh == 0x20)
		{
			ucDispCMDDataBuff[len] = ucharhigh;
		}
		else
		{
			ucharlow = ucDispCMDDataBuffTemp[++ ct];
			code = (uint16_t)(ucharhigh << 8) | ucharlow;
			if((code == 0xBFD5) || (code == 0xB5F7))
			{
				continue;
			}
			else
			{
				ucDispCMDDataBuff[len ++] = ucharhigh;
				ucDispCMDDataBuff[len] = ucharlow;
			}
		}
	}
	for(ct = 0; ct < SIZE; ct ++)
	{
		ucDispCMDDataBuffTemp[ct] = ucDispCMDDataBuff[ct];
	}
	get_line_name(ucDispCMDDataBuffTemp, linebuff);
	for(ct = 0; ct < 20; ct ++)
	{
		if(linebuff[ct] != Line[ct])
		{
			message_state |= MESSAGE_LEFT_NEW;
			message_state |= MESSAGE_RIGHT_NEW;
			message_state |= MESSAGE_BRAKE_NEW;
		}
		Line[ct] = linebuff[ct];
	}
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

void get_line_name(const uint8_t *src, uint8_t *dst)
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
		if(Code == 0xA3AA || Code == 0xA1A1 | Code == 0xA1F9)
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
	if(led_type != DEVICE_INSIDE)
	{
		ucLEDDisplayMemory = (uint8_t *)malloc((c_MaxColByte * 4 + 4) *4);//缓存
		ucColDataBuff = (uint8_t *)malloc(4 * c_MaxColByte);//一行数据缓冲区
	}
	else
	{
		ucLEDDisplayMemory = (uint8_t *)malloc((c_MaxColByte * 4 + 4) *4);//缓存
		ucColDataBuff = (uint8_t *)malloc(c_MaxColByte);//一行数据缓冲区
	}
}


void get_cfg_from_flash(void)
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
			else if((0x20 <= cfg_buf[0]) && (cfg_buf[0] <= 0x29))
			{
				cfg[0] = cfg_buf[0] - 0x0C;
			}
			c_MaxColByte = cfg[0] * 2;
			cfg[1] = cfg_buf[1];
			switch(cfg[1])
			{
			case INSIDE:
				led_type = DEVICE_INSIDE;
				break;
			case FRONT:
				led_type = DEVICE_FRONT;
				break;
			case REAR:
				led_type = DEVICE_REAR;
				break;
			case SIDE:
				led_type = DEVICE_SIDE;
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
	switch(led_type)
	{
		case DEVICE_INSIDE:
			for(ct = 0; ct < c_MaxColByte; ct ++)
			{
				disp_buff[ct] = 0x20;
			}
			add_line_info(welcom, disp_buff, 0);
			bflag = 0;
			break;
		case DEVICE_FRONT:
		case DEVICE_REAR:
		case DEVICE_SIDE:
			SST25_R_BLOCK(LINEADDR, ucDispCMDDataBuff, SIZE);
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
					case 0x02:
					case 0x04:
						add_line_info(welcom, ucDispCMDDataBuff, num / 2);
						get_line_name(ucDispCMDDataBuff, Line);
						break;
					case 0x03:
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
					case 0x02:
						bflag = 1;
						break;
					case 0x04:
						get_line_name(ucDispCMDDataBuff, Line);
						bflag = 1;
						break;
					case 0x03:
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
			break;
		default:
			break;
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
