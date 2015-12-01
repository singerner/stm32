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
#include <includes.h>

/** @addtogroup USBH_USER
* @{
*/
#define LOOP_ON()	(GPIO_ResetBits(GPIOC, GPIO_Pin_12))//供电
#define LOOP_OFF()	(GPIO_SetBits(GPIOC, GPIO_Pin_12))//断电

#define FLASH_ON()	(GPIO_SetBits(GPIOB, GPIO_Pin_10))//启动flash
#define FLASH_OFF()	(GPIO_ResetBits(GPIOB, GPIO_Pin_10))//关闭flash
#define SIZE		(64)//定义常亮
#define CFGADDR		(0x00)//0K-4K  //判断前屏后屏腰屏（缓存的范围）
#define LINEADDR	(0x1000)//4K-8K//显示的缓存范围


#define FRONT		(0x30)//前屏
#define	REAR		(0x31)//后屏
#define SIDE		(0x32)//测屏

#define LINENAME	(0x00)
#define LEFT		(0x01)//左转
#define RIGHT		(0x02)//右转
#define BRAKE		(0x03)//刹车

#define MESSAGE_LINE_NEW	(0x01)
#define MESSAGE_LEFT_NEW	(0x02)
#define MESSAGE_RIGHT_NEW	(0x04)
#define MESSAGE_BRAKE_NEW	(0x08)

/** @defgroup USBH_USR_MAIN
* @brief This file is the MSC demo main file
* @{
*/ 

uint8_t ucDispCMDDataBuff[SIZE] = {0};
uint8_t ucDispCMDLeftDataBuff[SIZE] = {0xA1,0xFB,0xD7,0xF3,0xD7,0xAA,0xCD,0xE4};//左转弯
uint8_t ucDispCMDRightDataBuff[SIZE] = {0xD3,0xD2,0xD7,0xAA,0xCD,0xE4, 0xA1,0xFA};//右转弯
uint8_t ucDispCMDBrakeDataBuff[SIZE] = {0xC9,0xB2,0xB3,0xB5,0xA3,0xA1};//刹车！
uint8_t *welcom = "欢迎乘坐";
uint8_t ucDispCMDDataBuffTemp[SIZE] = {0};
uint8_t disp_buff[SIZE] = {0};
uint8_t c_MaxColByte = 16;
uint8_t led_type = 0x30;
uint8_t Line[20] = {0};
uint8_t LED_OK = 0;
uint8_t *ucLEDDisplayMemory;
uint8_t *ucColDataBuff;
uint8_t bflag;
uint32_t brightness = 0x4FF;
uint8_t message_state;
extern uint8_t uart_get_flag;
extern uint8_t Usart_ucDispCMDDataBuffTemp[126];


uint8_t key_value = 0;
uint8_t on_off_flag = TURN_ON;
uint8_t num_key_press_flag = 0;
uint8_t turn_key_press_flag = 0;
uint8_t key_count = 0;
uint8_t key_count_copy = 0;
uint8_t key_num[4] = {0};
uint8_t line_count[2] = {0};//[0] All line_num   [1] current line_num
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
void data_process(void);
void front_process(uint8_t snowflag);
void rear_process(uint8_t snowflag);
void side_process(uint8_t snowflag);
void refresh_display_type(uint8_t flag);
void six_protocal(uint8_t ct);
void five_protocal(uint8_t ct);

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
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0xC000);//初始化启动项
	gpio_init();//初始化所有定义的gpio寄存器
	cpu_run();
	flash_stop();
//	rs485_recv();//使能485接受数据
	FLASH_ON();
	remote_init();
	LOOP_ON();//使能环流
//	uart_init(9600);//设置波特率为9600  串口初始化
	delay_ms(1);//定时器
	SPI_Flash_Init();//初始化spi flash外围驱动
	get_cfg_from_flash();//得到配置信息 (获取cfg文件的配置)
	get_line_from_flash();	 	
	message_state |= MESSAGE_LINE_NEW;
	message_state |= MESSAGE_LEFT_NEW;
	message_state |= MESSAGE_RIGHT_NEW;
	message_state |= MESSAGE_BRAKE_NEW;
	alloc_memory(); //初始化缓存
	
	wwdg_init(0x7F, 0x5F, WWDG_Prescaler_8); //计数器值为7f,窗口寄存器为5f,分频数为8 (使用窗口看门狗的原因是他可以被中断)
	while (1)
	{
		if(uart_get_flag)
		{
			uart_get_flag = 0;
			data_process();
			LED_OK = 1;
		}
		get_disp_state();		//左转右转还有刹车的判断报站；
		if(bflag)
		{
			LED_DispString(1, 1, disp_buff);    //固定显示
		}
		else
		{
			
			
			LED_ScrollDispHZ(1, 1, disp_buff);//滚动显示
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
		case LINENAME:
			break;
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
	uint8_t len = 0;
	memset(disp_buff, 0x00, SIZE);//清除
	if((get_light_state()) == Bit_RESET)
	{
		brightness = 0xEFF;
	}
	else
	{
		brightness = 0x4FF;
	}
	if((message_state & MESSAGE_LEFT_NEW) == MESSAGE_LEFT_NEW)
	{
		 refresh_display_type(LEFT);
		 message_state &= (~MESSAGE_LEFT_NEW);
	}
	if((get_left_state()) == Bit_RESET)
	{
		for(len = 0; len < SIZE; len ++)
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
		return 0;	
	}
	else
	{
		message_state |= MESSAGE_LINE_NEW;
	}
	
	if((message_state & MESSAGE_RIGHT_NEW) == MESSAGE_RIGHT_NEW)
	{
		refresh_display_type(RIGHT);
		message_state &= (~MESSAGE_RIGHT_NEW);
	}
	if((get_right_state()) == Bit_RESET)
	{
		for(len = 0; len < SIZE; len ++)
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
			bflag = 0;
		}
		else
		{	
			bflag = 1;
		}
		return 0;
	}
	else
	{
		message_state |= MESSAGE_LINE_NEW;
	}
		
	if((message_state & MESSAGE_BRAKE_NEW) == MESSAGE_BRAKE_NEW)
	{
		refresh_display_type(BRAKE);
		message_state &= (~MESSAGE_BRAKE_NEW);
	}
	if((get_brake_state()) == Bit_RESET)
	{
		for(len = 0; len < SIZE; len ++)
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
		return 0;	
	}
	else
	{
		message_state |= MESSAGE_LINE_NEW;
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

void six_protocal(uint8_t ct)
{
	uint8_t index = 0, size = 0;
	uint8_t num = 0;
	uint8_t tempbuff[SIZE] = {0};
	ct ++;
	for(index = 0; Usart_ucDispCMDDataBuffTemp[ct] != 0x26; ct ++)
	{
		if(Usart_ucDispCMDDataBuffTemp[ct]>0x2F && Usart_ucDispCMDDataBuffTemp[ct]<0x3A)//0~9
		{
			ucDispCMDDataBuffTemp[index++]=0xA3;
			ucDispCMDDataBuffTemp[index++]=0xB0+Usart_ucDispCMDDataBuffTemp[ct]-0x30;	
		}
		else if(Usart_ucDispCMDDataBuffTemp[ct]>0x40 && Usart_ucDispCMDDataBuffTemp[ct]<0x5B)//A~Z
		{
			ucDispCMDDataBuffTemp[index++]=0xA3;
			ucDispCMDDataBuffTemp[index++]=0xC1+Usart_ucDispCMDDataBuffTemp[ct]-0x41;		
		}  
		else if(Usart_ucDispCMDDataBuffTemp[ct]>0x60 && Usart_ucDispCMDDataBuffTemp[ct]<0x7B)//a~z
		{
			ucDispCMDDataBuffTemp[index++]=0xA3;
			ucDispCMDDataBuffTemp[index++]=0xE1+Usart_ucDispCMDDataBuffTemp[ct]-0x61;		
		}
		else
		{ 
			ucDispCMDDataBuffTemp[index++]=Usart_ucDispCMDDataBuffTemp[ct];	
		}	
	}
	if(index <= c_MaxColByte)
	{
		size = c_MaxColByte - index;
		for(num = 0; num < size; num++)
		{
			tempbuff[num] = 0x20;
		}
		if(index % 2 == 0)
		{
			add_line_info(ucDispCMDDataBuffTemp, tempbuff, size / 2);
		}
		else
		{
			add_line_info(ucDispCMDDataBuffTemp, tempbuff, size / 2 + 1);
		}
		for(num = 0; num < SIZE; num ++)
		{
			ucDispCMDDataBuff[num] = tempbuff[num];
		}
	}
	else
	{
		for(num = 0; num < SIZE; num ++)
		{
			ucDispCMDDataBuff[num] = ucDispCMDDataBuffTemp[num];
		}
	}
}

void five_protocal(uint8_t ct)
{
	uint8_t num=0;
	uint8_t size=0, index=0;
	uint8_t tempbuff[SIZE] = {0};
	for(index = 0; Usart_ucDispCMDDataBuffTemp[ct] != 0x20; ct ++)
	{
		if(Usart_ucDispCMDDataBuffTemp[ct] > 0x2F && Usart_ucDispCMDDataBuffTemp[ct] < 0x3A)//0~9
		{
			ucDispCMDDataBuffTemp[index ++] = 0xA3;
			ucDispCMDDataBuffTemp[index ++]= 0xB0 + Usart_ucDispCMDDataBuffTemp[ct] - 0x30;	
		}
		else if(Usart_ucDispCMDDataBuffTemp[ct] > 0x40 && Usart_ucDispCMDDataBuffTemp[ct] < 0x5B)//A~Z
		{
			ucDispCMDDataBuffTemp[index ++] = 0xA3;
			ucDispCMDDataBuffTemp[index ++] = 0xC1 + Usart_ucDispCMDDataBuffTemp[ct] - 0x41;		
		}  
		else if(Usart_ucDispCMDDataBuffTemp[ct] > 0x60 && Usart_ucDispCMDDataBuffTemp[ct] < 0x7B)//a~z
		{
			ucDispCMDDataBuffTemp[index ++] = 0xA3;
			ucDispCMDDataBuffTemp[index ++] = 0xE1 + Usart_ucDispCMDDataBuffTemp[ct] - 0x61;		
		}
		else
		{ 
			ucDispCMDDataBuffTemp[index ++] = Usart_ucDispCMDDataBuffTemp[ct];	
		}
	}
	if(index <= c_MaxColByte)
	{
		size = c_MaxColByte - index;
		for(num = 0; num < size; num ++)
		{
			tempbuff[num] = 0x20;
		}
		
		if(index % 2 == 0)
		{
			add_line_info(ucDispCMDDataBuffTemp, tempbuff, size / 2);
		}
		else
		{
			add_line_info(ucDispCMDDataBuffTemp, tempbuff, size / 2 +1);
		}
		for(num = 0; num < SIZE; num ++)
		{
			ucDispCMDDataBuff[num] = tempbuff[num];
		}
	}
	else
	{
		for(num = 0; num < SIZE; num ++)
		{
			ucDispCMDDataBuff[num] = ucDispCMDDataBuffTemp[num];
		}
	}
	
}



void side_process(uint8_t snowflag)
{
	uint8_t ct = 50;

	while(ct ++)
	{
		switch(Usart_ucDispCMDDataBuffTemp[ct])
		{
			case 0x07:
				six_protocal(ct);
				break;
			case 0x3E:
				ct = 50;
				five_protocal(ct);
				break;
			default:
				continue;
		}
		break;
	}
	
}

void rear_process(uint8_t snowflag)
{
	uint8_t ct = 0, index = 0;
	uint8_t size = 0;
	uint8_t compareflag = 0;
	uint8_t  ucharhigh = 0, ucharlow = 0;
	uint16_t code = 0;
	uint8_t tempbuff[SIZE] = {0};
	uint8_t linebuff[20] = {0};
	
	if(snowflag == 0)
	{
		ucDispCMDDataBuffTemp[ct ++] = 0xA3;
		ucDispCMDDataBuffTemp[ct ++] = 0xAA;
		ucDispCMDDataBuffTemp[ct ++] = 0xA1;
		ucDispCMDDataBuffTemp[ct ++] = 0xA1;
	}
	for(index = 50; Usart_ucDispCMDDataBuffTemp[index] != 0x07 && Usart_ucDispCMDDataBuffTemp[index] != 0x2D; index ++)
	{
		ucharhigh = Usart_ucDispCMDDataBuffTemp[index];
		if(ucharhigh > 0x80)
		{
			ucharlow = Usart_ucDispCMDDataBuffTemp[++ index];
			code = (uint16_t)(ucharhigh << 8) + ucharlow;
			if((code == 0xBFD5) || (code == 0xB5F7))
			{
				continue;
			}
			else
			{
				ucDispCMDDataBuffTemp[ct ++] = ucharhigh;
				ucDispCMDDataBuffTemp[ct ++] = ucharlow;
			}
		}
		else
		{
			if(Usart_ucDispCMDDataBuffTemp[index] > 0x2F && Usart_ucDispCMDDataBuffTemp[index] < 0x3A)//0~9
			{
				ucDispCMDDataBuffTemp[ct ++] = 0xA3;
				ucDispCMDDataBuffTemp[ct ++] = 0xB0 + Usart_ucDispCMDDataBuffTemp[index] - 0x30;	
			}
			else if(Usart_ucDispCMDDataBuffTemp[index] > 0x40 && Usart_ucDispCMDDataBuffTemp[index] < 0x5B)//A~Z
			{
				ucDispCMDDataBuffTemp[ct ++] = 0xA3;
				ucDispCMDDataBuffTemp[ct ++] = 0xC1 + Usart_ucDispCMDDataBuffTemp[index] - 0x41;		
			}  
			else if(Usart_ucDispCMDDataBuffTemp[index] > 0x60 && Usart_ucDispCMDDataBuffTemp[index] < 0x7B)//a~z
			{
				ucDispCMDDataBuffTemp[ct ++] = 0xA3;
				ucDispCMDDataBuffTemp[ct ++] = 0xE1 + Usart_ucDispCMDDataBuffTemp[index] - 0x61;		
			}
			else if(Usart_ucDispCMDDataBuffTemp[index] == 0x5E)
			{
				ucDispCMDDataBuffTemp[ct++] = 0xA3;
				ucDispCMDDataBuffTemp[ct++] = 0xAA;
			}
			else
			{ 
				ucDispCMDDataBuffTemp[ct ++] = Usart_ucDispCMDDataBuffTemp[index];	
			}
		}
	}
	get_line_name(ucDispCMDDataBuffTemp, linebuff);
	for(index = 0 ; index < 20; index ++)
	{
		if(Line[index] != linebuff[index])
		{
			Line[index] = linebuff[index];
			compareflag = 1;
		}
	}
	if(compareflag)
	{
		message_state |= MESSAGE_LEFT_NEW;
		message_state |= MESSAGE_RIGHT_NEW;
		message_state |= MESSAGE_BRAKE_NEW;
	}
	if(ct <= c_MaxColByte)
	{
		size= c_MaxColByte - ct;
		for(index = 0; index < size; index ++)
		{
			tempbuff[index] = 0x20;
		}
		if(ct % 2 == 0)
		{
			add_line_info(ucDispCMDDataBuffTemp, tempbuff, size / 2);
		}
		else
		{
			add_line_info(ucDispCMDDataBuffTemp, tempbuff, size / 2 + 1);
		}
		for(index = 0; index < SIZE; index ++)
		{
			ucDispCMDDataBuff[index] = tempbuff[index];
		}	
	}
	else
	{
		for(index = 0; index < SIZE; index ++)
		{
			ucDispCMDDataBuff[index] = ucDispCMDDataBuffTemp[index];
		}
	}
}

void front_process(uint8_t snowflag)
{
	uint8_t ct = 0, index = 0;
	uint8_t size = 0;
	uint8_t tempbuff[SIZE] = {0};
	
	if(snowflag == 0)
	{
		ucDispCMDDataBuffTemp[ct ++] = 0xA3;
		ucDispCMDDataBuffTemp[ct ++] = 0xAA;
		ucDispCMDDataBuffTemp[ct ++] = 0xA1;
		ucDispCMDDataBuffTemp[ct ++] = 0xA1;
	}
	for(index = 50; Usart_ucDispCMDDataBuffTemp[index] != 0x07 && Usart_ucDispCMDDataBuffTemp[index] != 0x2D; index ++)
	{
		if(Usart_ucDispCMDDataBuffTemp[index] > 0x2F && Usart_ucDispCMDDataBuffTemp[index] < 0x3A)//0~9
		{
			ucDispCMDDataBuffTemp[ct ++] = 0xA3;
			ucDispCMDDataBuffTemp[ct ++] = 0xB0 + Usart_ucDispCMDDataBuffTemp[index] - 0x30;	
		}
		else if(Usart_ucDispCMDDataBuffTemp[index] > 0x40 && Usart_ucDispCMDDataBuffTemp[index] < 0x5B)//A~Z
		{
			ucDispCMDDataBuffTemp[ct ++] = 0xA3;
			ucDispCMDDataBuffTemp[ct ++] = 0xC1 + Usart_ucDispCMDDataBuffTemp[index] - 0x41;		
		}  
		else if(Usart_ucDispCMDDataBuffTemp[index] > 0x60 && Usart_ucDispCMDDataBuffTemp[index] < 0x7B)//a~z
		{
			ucDispCMDDataBuffTemp[ct ++] = 0xA3;
			ucDispCMDDataBuffTemp[ct ++] = 0xE1 + Usart_ucDispCMDDataBuffTemp[index] - 0x61;		
		}
		else if(Usart_ucDispCMDDataBuffTemp[index] == 0x5E)
		{
			ucDispCMDDataBuffTemp[ct++] = 0xA3;
			ucDispCMDDataBuffTemp[ct++] = 0xAA;
		}
		else
		{ 
			ucDispCMDDataBuffTemp[ct ++] = Usart_ucDispCMDDataBuffTemp[index];	
		}
	}
	if(ct <= c_MaxColByte)
	{
		size= c_MaxColByte - ct;
		for(index = 0; index < size; index ++)
		{
			tempbuff[index] = 0x20;
		}
		if(ct % 2 == 0)
		{
			add_line_info(ucDispCMDDataBuffTemp, tempbuff, size / 2);
		}
		else
		{
			add_line_info(ucDispCMDDataBuffTemp, tempbuff, size / 2 + 1);
		}
		for(index = 0; index < SIZE; index ++)
		{
			ucDispCMDDataBuff[index] = tempbuff[index];
		}	
	}
	else
	{
		for(index = 0; index < SIZE; index ++)
		{
			ucDispCMDDataBuff[index] = ucDispCMDDataBuffTemp[index];
		}
	}
}

void data_process(void)
{
	uint8_t checksum = 0xD5;
	uint8_t index= 0;
	uint8_t num = 0;
	uint8_t snowflag = 0;
	
	for(num = 0; num < 120; num ++)//判断校验位
	{
		checksum = checksum +  Usart_ucDispCMDDataBuffTemp [num];
	}
	if(checksum == Usart_ucDispCMDDataBuffTemp[++ num])
	{
		index = 50;
		if(Usart_ucDispCMDDataBuffTemp[index] == 0x5E)
		{
			snowflag = 1;
		}
		else
		{
			snowflag = 0;
		}
		switch (led_type)
		{
			case FRONT:
				front_process(snowflag);//前屏
				break;
			case REAR:
				rear_process(snowflag);//后屏
				break;
			case SIDE:
				side_process(snowflag);//侧屏
				break;
			default:
				break;
		}
		message_state |= MESSAGE_LINE_NEW;
		SST25_W_BLOCK(LINEADDR, ucDispCMDDataBuff, SIZE);		//写数据到flash里面
	}
	memset(Usart_ucDispCMDDataBuffTemp, 0x00, sizeof(Usart_ucDispCMDDataBuffTemp) / sizeof(uint8_t));
	memset(ucDispCMDDataBuffTemp, 0x00, SIZE);
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
		if((0xA3AA == Code) || (0xA1A1 == Code) || (0xA1F9 == Code))//A3AA是雪花  A1A1
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
	ucLEDDisplayMemory = (uint8_t *)malloc((c_MaxColByte * 4 + 4) *4);//缓存
	ucColDataBuff = (uint8_t *)malloc(4 * c_MaxColByte);//一行数据缓冲区
}

void get_cfg_from_flash(void)
{
	uint8_t cfg_buf[64] = {0};
	uint8_t cfg[2] = {0};
	SST25_R_BLOCK(CFGADDR, cfg_buf, 64);//从flash里吧数据读到cfg-buf里面
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
				led_type = FRONT;//前屏
				break;
			case REAR:
				led_type = REAR;//后屏
				break;
			case SIDE:
				led_type = SIDE;//腰屏
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
	
 	SST25_R_BLOCK(LINEADDR, ucDispCMDDataBuff, SIZE);//读数据到buff里面
	if((ucDispCMDDataBuff[0] == 0xFF) || (ucDispCMDDataBuff[1] == 0xFF) || (ucDispCMDDataBuff[SIZE - 1] == 0xFF))//行驶中
	{
		memset(ucDispCMDDataBuff, 0x00, SIZE);//清除buff里面的数据
		num = c_MaxColByte - 8;
		for(ct = 0; ct < num; ct ++)
		{
			ucDispCMDDataBuff[ct] = 0x20;
		}
		switch(led_type) 
		{
			case FRONT:
			case REAR:
				add_line_info(welcom, ucDispCMDDataBuff, num / 2);//前屏显示欢迎光乘坐
				get_line_name(ucDispCMDDataBuff, Line);//
				break;
			case SIDE:
				add_line_info(welcom, ucDispCMDDataBuff, num / 2);
				break;
			default:
				break;
		}
		bflag = 1;
	}
	else//到站
	{
		switch(led_type)
		{
			case FRONT:
				bflag = 1;
				break;
			case REAR:
				get_line_name(ucDispCMDDataBuff, Line);
				bflag = 1;
				break;
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
	
	/*1:485 control 8:flash light flash灯是gpio_pin_8*/
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/*5: cpu light  10:flash on/off  cpu灯是gpio_pin_10*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* PC12 loop on/off 环流供电 环流供电是gpio_pin_12*/ 
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
