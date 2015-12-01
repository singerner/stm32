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


enum LED_TYPE
{
	FRONT = 0x30,
	REAR = 0x31,
	SIDE = 0x32,
};


enum 
{
	LINENAME = 0x00,
	LEFT 		 = 0x01,
	RIGHT    = 0x02,
	BRAKE    = 0x03,
};


#define TURN_OFF				(0x00)
#define TURN_ON				  (0x01)


#define MESSAGE_LINE_NEW		(0x01)
#define MESSAGE_LEFT_NEW		(0x02)
#define MESSAGE_RIGHT_NEW		(0x04)
#define MESSAGE_BRAKE_NEW		(0x08)

/** @defgroup USBH_USR_MAIN
* @brief This file is the MSC demo main file
* @{
*/ 

uint8_t ucDispCMDDataBuff[SIZE] = {0};
//uint8_t disp_temp_state_buff[SIZE] = {0};
uint8_t ucDispCMDLeftDataBuff[SIZE] = {0xA1,0xFB,0xB1,0xE4,0xB5,0xC0};//<-变道
uint8_t ucDispCMDRightDataBuff[SIZE] = {0xB1,0xE4,0xB5,0xC0,0xA1,0xFA};//变道->
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
void refresh_display_type(uint8_t flag);
void filter_words(uint8_t *src, uint16_t code_h, uint16_t code_l);
uint8_t contain_words(const uint8_t *src, uint16_t code_h, uint16_t code_l, uint8_t n_start);
void static_move_disp(const uint8_t *src, uint8_t *dst);
void ascii_to_gbk_code(uint8_t *src);
void key_process(void);
void save_currentline_num(uint8_t num);
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
	cpu_run();
	flash_stop();
	FLASH_ON();
	//uart_init(9600);//notice
	//rs485_recv();
	remote_init();
	SPI_Flash_Init();
	get_cfg_from_flash();
//	get_line_from_flash();	
	message_state |= MESSAGE_LINE_NEW;
	message_state |= MESSAGE_LEFT_NEW;
	message_state |= MESSAGE_RIGHT_NEW;
	message_state |= MESSAGE_BRAKE_NEW;
	alloc_memory();
	
	while(check_ret < 0xFFFF)
	{
			USBH_Process(&USB_OTG_Core, &USB_Host);
			if(1 == check_ok)
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
		
			if(1 == check_ok)//check_ok 是usb下载的标志位 下载完成之后他会被至1
			{
					check_ok = 0;
					SST25_R_BLOCK(LINEADDR + 1, ucDispCMDDataBuffTemp, SIZE - 1);
					ascii_to_gbk_code(ucDispCMDDataBuffTemp);
					static_move_disp(ucDispCMDDataBuffTemp, ucDispCMDDataBuff);
					LED_OK = 1;
			}
			else
			{
					key_value = Remote_Scan();
					key_process();
					//memset(Line,0x00,20);
					//get_line_name(ucDispCMDDataBuff,Line);//得到线路名储存在Line数组里面
					get_disp_state();		
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
}


void key_process(void)
{
	uint32_t lineaddr = 0;
	uint8_t key = 0;
	

	switch(key_value)
	{
		case ON_OFF_KEY:
			if(TURN_ON == on_off_flag)
			{
					on_off_flag = TURN_OFF;
					LED_OK = 0;
			}
			else 
			{
					on_off_flag = TURN_ON;
					LED_OK = 1;
			}
			break;
		case OK_KEY:
			if(0x00 != key_num[0])
			{
					num_key_press_flag = 1;
					key_count_copy = key_count;
					key_count = 0;
			}
			break;
		case UP_KEY:
			SST25_R_BLOCK(LINEADDR, line_count, 1);
			if(0x00 != line_count[0])
			{
					 if(0x00 == line_count[1] || 0x01 == line_count[1])//0x01 == line_count[1] bug
					 {
							line_count[1] = line_count[0];
					 }
					 else
					 {
							line_count[1] --;	
					 }
					turn_key_press_flag = 1;
			}
			break;
		case DOWN_KEY:
			SST25_R_BLOCK(LINEADDR, line_count, 1);
			if(0 != line_count[0])
			{
					 if(line_count[1] >= line_count[0])
					 {
							line_count[1] = 0;
					 }
					 line_count[1] ++;	
					 turn_key_press_flag = 1;
			}
			break;
		case ZERO_KEY:
			if(0x00 != key_num[0])
			{
				 key_num[key_count] = 0x00;
				 key_count ++;
			}
			break;
		case ONE_KEY:
			key_num[key_count] = 0x01;
			key_count ++;
			break;
		case TWO_KEY:
			key_num[key_count] = 0x02;
			key_count ++;
			break;
		case THREE_KEY:
			key_num[key_count] = 0x03;
			key_count ++;
			break;
		case FOUR_KEY:
			key_num[key_count] = 0x04;
			key_count ++;
			break;
		case FIVE_KEY:
			key_num[key_count] = 0x05;
			key_count ++;
			break;
		case SIX_KEY:
			key_num[key_count] = 0x06;
			key_count ++;
			break;
		case SEVEN_KEY:
			key_num[key_count] = 0x07;
			key_count ++;
			break;
		case EIGHT_KEY:
			key_num[key_count] = 0x08;
			key_count ++;
			break;
		case NINE_KEY:
			key_num[key_count] = 0x09;
			key_count ++;
			break;
	}
	if(1 == num_key_press_flag)
	{
			num_key_press_flag = 0;
			if(key_count_copy <= 2)
			{
					if(1 == key_count_copy)
					{
							line_count[1] = key_num[0];
					}	
					else if(2 == key_count_copy)
					{
							key = ((uint8_t)(key_num[0] << 4)) | key_num[1];
							line_count[1] =  key - ((key & 0xF0) >> 4) * 6;
					}
					SST25_R_BLOCK(LINEADDR, line_count, 1);
					if((line_count[0] <= 0x14) && (line_count[1] <= line_count[0]) && line_count[0] != 0x00)
					{
							memset(key_num, 0x00, 4);
							if(1 == line_count[1])
							{
									lineaddr = LINEADDR + 1;
						//			SST25_R_BLOCK(lineaddr, ucDispCMDDataBuffTemp, SIZE - 1);
							}
							else
							{
									lineaddr = LINEADDR + SIZE * (line_count[1] - 1);
							//		SST25_R_BLOCK(lineaddr, ucDispCMDDataBuffTemp, SIZE);
							}
					//		ascii_to_gbk_code(ucDispCMDDataBuffTemp);
					//		static_move_disp(ucDispCMDDataBuffTemp, ucDispCMDDataBuff);
							save_currentline_num(line_count[1]);
					}
			}
			else
			{
					memset(key_num, 0x00, 4);
			}
	}
	if(1 == turn_key_press_flag)
	{
		  turn_key_press_flag = 0;
			SST25_R_BLOCK(LINEADDR, line_count, 1);
			if((line_count[0] <= 0x14) && (line_count[1] <= line_count[0]) && line_count[0] != 0x00)
			{
					memset(key_num, 0x00, 4);
					if(1 == line_count[1])
					{
							lineaddr = LINEADDR + 1;
						//	SST25_R_BLOCK(lineaddr, ucDispCMDDataBuffTemp, SIZE - 1);
					}
				  else
				  {
						 lineaddr = LINEADDR + SIZE * (line_count[1] - 1);
						// SST25_R_BLOCK(lineaddr, ucDispCMDDataBuffTemp, SIZE);
					 
				  }
				 // ascii_to_gbk_code(ucDispCMDDataBuffTemp);
				//  static_move_disp(ucDispCMDDataBuffTemp, ucDispCMDDataBuff);
					save_currentline_num(line_count[1]);
			}
	 }
	// get_line_name(ucDispCMDDataBuff,Line);
	  memset(ucDispCMDDataBuffTemp, 0x00, SIZE);
}


void save_currentline_num(uint8_t num)
{
		uint8_t buff[10] = {0};
		if((int)num < 0 && (int)num > 0x14)
		{
				return ;
		}
		SST25_R_BLOCK(CFGADDR, buff, 10);
		if(buff[0] == 0x00  || buff[0] == 0xFF)
		{
				return ;
		}
		buff[2] = num;
		SST25_W_BLOCK(CFGADDR, buff, 10);
		return ;
}



void refresh_display_type(uint8_t type)//问题出在这个函数   第二次的时候没有写进去
{
	uint8_t *dst = NULL;
	uint8_t n_insert = 0;
	uint8_t len = 0, ct = 0, num = 0;
	uint8_t blankbytes = 0;
	uint8_t tempbuff[32] = {0};
	uint8_t left_buf[SIZE] = {0x3C,0x2D,0xD7,0xF3,0xD7,0xAA,0xCD,0xE4};  //左转弯
	uint8_t right_buf[SIZE] ={0xD3,0xD2,0xD7,0xAA,0xCD,0xE4,0xA1,0xFA,};//右转弯
	uint8_t brake_buf[SIZE] = {0xC9,0xB2,0xB3,0xB5,0xA3,0xA1};//刹车
	
	
	while(Line[len ++] != '\0');
	len --;
	
	switch (type)    //这里有问题左右转弯的基础都是6  
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
	
	if(len > (c_MaxColByte - blankbytes))//很有可能问题出在这里   家线路名的时候出的问题
	{
		 add_line_info(Line, dst, n_insert); //把线路名加到中间
	}
	else
	{
		num = ((c_MaxColByte - blankbytes) - len);
		for(ct = 0; ct < num; ct ++)
		{
			tempbuff[ct] = 0x20;
		}
		add_line_info(Line, tempbuff, num / 2);//把线路名加到中间
		add_line_info(tempbuff, dst, n_insert);//把tempbuff加到中间
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

uint8_t get_disp_state(void)//区分显示方式    左右转弯刹车还是正常显示
{
	uint8_t len = 0;
	memset(disp_buff, 0x00, SIZE);
	
//	memset(Line,0x00,20);
	get_line_from_flash();   //从flash得到线路

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
		 message_state &= (~MESSAGE_LEFT_NEW);//还原上面的 操作
	}
	if((get_left_state()) == Bit_RESET)//来了电信号就直接执行这一步
	{
		for(len = 0; len < SIZE; len ++)
		{
			if(ucDispCMDLeftDataBuff[len] == '\0')
			{
				break;
			}
			disp_buff[len] = ucDispCMDLeftDataBuff[len];//把左转弯数据写到显示数组
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
		message_state |= MESSAGE_LINE_NEW;//正常显示
	}
//	memset(ucDispCMDLeftDataBuff,0x00,SIZE);//清除左转弯数据  然后添加新的数据
	
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


void filter_words(uint8_t *src, uint16_t code_h, uint16_t code_l)
{
		uint8_t ct = 0, src_len = 0;
		uint8_t buff[SIZE] = {0};
		uint8_t ucharhigh = 0, ucharlow = 0;
		uint16_t tempcode = 0;
		uint8_t flag_1 = 0, flag_2 = 0;
		uint16_t code = 0xB3B5;
	
		for(src_len = 0, ct = 0; src[src_len] != '\0'; src_len ++)
		{
				ucharhigh = src[src_len];
				if(ucharhigh > 0x80)
				{
						ucharlow = src[++ src_len];
						tempcode = (uint16_t)((ucharhigh << 8)| ucharlow);
						if(0 == flag_1)
						{
								if(tempcode == code_h)
								{
										flag_1 = 1;
										continue;
								}
								else
								{
										if(1 == flag_2)
										{
												flag_2 = 0;
												if(tempcode == code)
												{
														continue;
												}
										}
										buff[ct ++] = ucharhigh;
										buff[ct ++] = ucharlow;
								}
						}
						else
						{
								if(tempcode == code_l)
								{
										flag_1 = 0;
										flag_2 = 1;
										continue;
								}
								else
								{
										flag_1 = 0;
										buff[ct ++] = (code_h >> 8) & 0xFF;
										buff[ct ++] = (code_h) & 0xFF;
										buff[ct ++] = ucharhigh;
										buff[ct ++] = ucharlow;
										
								}
						}
				}	
				else
				{
						buff[ct ++] = ucharhigh;
				}
		}
		for(ct = 0; ct < SIZE;  ct ++)
		{
				src[ct] = buff[ct];
		}
}

void static_move_disp(const uint8_t *src, uint8_t *dst)
{
	uint8_t index = 0, num = 0, size = 0;
	uint8_t tempbuff[SIZE] = {0};
	
	while(ucDispCMDDataBuffTemp[index ++] != '\0');
	index --;
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





uint8_t contain_words(const uint8_t *src, uint16_t code_h, uint16_t code_l, uint8_t n_start)
{
		uint8_t ucharhigh = 0, ucharlow = 0;
		uint16_t uihzcode = 0;
		uint8_t flag = 0;
		uint8_t bresultflag = 0;
	
		for(; (src[n_start] != 0x07) && (src[n_start] != 0x2D); n_start ++)
		{
				ucharhigh = src[n_start];
				if(ucharhigh > 0x80)
				{
							ucharlow = src[++ n_start];
							uihzcode = (uint16_t)((ucharhigh << 8) | ucharlow);
							if(0 == flag)
							{
									if(uihzcode == code_h)
									{
											flag = 1;
									}
							}
							else
							{
									if(uihzcode == code_l)
									{
											bresultflag = 1;
											flag = 0;
											break;
									}
							}
				}
		}
		return bresultflag;
}



void ascii_to_gbk_code(uint8_t *src)
{
		uint8_t ct = 0, src_len = 0;
		uint8_t tempbuff[SIZE] = {0};
		for(ct = 0, src_len = 0; src[src_len] != '\0'; src_len ++)
		{
				if(src[src_len] > 0x2F && src[src_len] < 0x3A)//0~9
				{
						tempbuff[ct ++] = 0xA3;
						tempbuff[ct ++] = 0xB0 + src[src_len] - 0x30;	
				}
				else if(src[src_len] > 0x40 && src[src_len] < 0x5B)//A~Z
				{
						tempbuff[ct ++] = 0xA3;
						tempbuff[ct ++] = 0xC1 + src[src_len] - 0x41;		
				}  
				else if(src[src_len] > 0x60 && src[src_len] < 0x7B)//a~z
				{
						tempbuff[ct ++] = 0xA3;
						tempbuff[ct ++] = 0xE1 + src[src_len] - 0x61;		
				}
				else
				{ 
						tempbuff[ct ++] = src[src_len];	
				}
		}
		for(ct = 0; ct < SIZE; ct ++)
		{
				ucDispCMDDataBuffTemp[ct] = tempbuff[ct];
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
	
	memset(Line,0x00,20);
	
	for(ct = 0; src[ct] != '\0'; ct ++)
	{
		if(src[ct] == 0x20)
		{
			continue;
		}
		uCharHigh = src[ct];
		uCharLow = src[++ ct];
		Code = (uint16_t)(uCharHigh << 8) + uCharLow;
		if(Code == 0xA3AA || Code == 0xA1A1 || Code == 0xA1F9 || Code ==0x2020)//这一步的作用是除去空格和雪花
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
	SST25_R_BLOCK(CFGADDR, cfg_buf, 64);
	if((cfg_buf[0] != 0x00) && (cfg_buf[1] != 0x00) || (cfg_buf[0] != 0xFF) && (cfg_buf[1] != 0xFF))
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
	SST25_R_BLOCK(CFGADDR + 2, &num, 1);//读出当前是第几
	if((int)num <= 0 || (int)num > 0x14 || 1 == num)
	{
			SST25_R_BLOCK(LINEADDR + 1, ucDispCMDDataBuffTemp, SIZE - 1);
			static_move_disp(ucDispCMDDataBuffTemp, ucDispCMDDataBuff);
			get_line_name(ucDispCMDDataBuff,Line);
			LED_OK = 1;
			return ;
	}
	else
	{
			SST25_R_BLOCK(LINEADDR + SIZE * (num - 1), ucDispCMDDataBuffTemp, SIZE);
			
			ascii_to_gbk_code(ucDispCMDDataBuffTemp);
			static_move_disp(ucDispCMDDataBuffTemp, ucDispCMDDataBuff);
			get_line_name(ucDispCMDDataBuff,Line);
			LED_OK = 1;
			return ;
	}
 
	#if 0
FRIST_LINE:
	SST25_R_BLOCK(LINEADDR + 1, ucDispCMDDataBuffTemp, SIZE - 1);
	
	if((ucDispCMDDataBuffTemp[0] == 0xFF) || (ucDispCMDDataBuffTemp[1] == 0xFF) || (ucDispCMDDataBuffTemp[SIZE - 1] == 0xFF))
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
				get_line_name(ucDispCMDDataBuff, Line);
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
			ascii_to_gbk_code(ucDispCMDDataBuffTemp);
			switch(led_type)
			{
				case FRONT:
						static_move_disp(ucDispCMDDataBuffTemp, ucDispCMDDataBuff);
						bflag = 1;
					break;
				case REAR:
					
					static_move_disp(ucDispCMDDataBuffTemp, ucDispCMDDataBuff);
					get_line_name(ucDispCMDDataBuff, Line);
					bflag = 1;
					break;
				case SIDE:
					ct = 0;
					while(ucDispCMDDataBuffTemp[ct ++] != '\0')
						ucDispCMDDataBuff[ct] = ucDispCMDDataBuffTemp[ct];
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
	
			static_move_disp(ucDispCMDDataBuffTemp, ucDispCMDDataBuff);
			get_line_name(ucDispCMDDataBuff,Line);
	
	LED_OK = 1;
	#endif
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
