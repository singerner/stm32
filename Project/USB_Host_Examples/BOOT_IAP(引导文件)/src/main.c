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
#include "iap.h"

/** @addtogroup USBH_USER
* @{
*/

/** @defgroup USBH_USR_MAIN
* @brief This file is the MSC demo main file
* @{
*/ 

/** @defgroup USBH_USR_MAIN_Private_TypesDefinitions
* @{
*/ 
/**
* @}
*/ 
void flash_run(void);
void flash_stop(void);
void gpio_init(void);
void flash_on(void);
void flash_off(void);
void ledscreen_on(void);
void ledscreen_off(void);
extern void USB_OTG_BSP_DisEnableInterrupt(USB_OTG_CORE_HANDLE *pdev);
int update_ok = 0;
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


/** @defgroup USBH_USR_MAIN_Private_Functions
* @{
*/ 


/**
* @brief  Main routine for MSC class application
* @param  None
* @retval int
*/
int main(void)
{	__IO uint32_t i = 1000;
	int check_cnt = 0;
	int key_ret = 0;
	/*!< At this stage the microcontroller clock setting is already configured, 
	this is done through SystemInit() function which is called from startup
	file (startup_stm32fxxx_xx.s) before to branch to application main.
	To reconfigure the default setting of SystemInit() function, refer to
	system_stm32fxxx.c file
	*/ 
	gpio_init();
	flash_stop();
	ledscreen_off();
	if(key_ret == 0x00)
	{
		FLASH_Unlock();
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
		
		while(1)
		{
			if(update_ok == 0x00)
			{
				/* Host Task handler */
				USBH_Process(&USB_OTG_Core, &USB_Host);
				check_cnt ++;
			}
			else
			{
				check_cnt = 0xFFFF + 10;
			}
			if(check_cnt > 0xFFFF)
			{	
				USB_OTG_BSP_DisEnableInterrupt(&USB_OTG_Core);
				check_cnt = 0;
				if (((*(__IO uint32_t*)ApplicationAddress) & 0x2FFE0000 ) == 0x20000000)  
				{  
					JumpAddress = *(__IO uint32_t*) (ApplicationAddress + 4);  
					Jump_To_Application = (pFunction) JumpAddress;  
					//__set_MSP(*(__IO uint32_t*) ApplicationAddress);  		
					Jump_To_Application();  
				}
			}
		}
	}
}

void gpio_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}


void ledscreen_on(void)
{
	GPIO_ResetBits(GPIOC, GPIO_Pin_2);
}

void ledscreen_off(void)
{
	GPIO_SetBits(GPIOC, GPIO_Pin_2);
}

void flash_on(void)
{
	GPIO_SetBits(GPIOB, GPIO_Pin_10);
}


void flash_off(void)
{
	GPIO_ResetBits(GPIOB, GPIO_Pin_10);
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
void assert_failed(uint8_t* file, uint32_t line)
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
