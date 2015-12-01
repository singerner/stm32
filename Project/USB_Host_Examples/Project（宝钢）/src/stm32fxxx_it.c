/**
  ******************************************************************************
  * @file    stm32fxxx_it.c
  * @author  MCD Application Team
  * @version V2.1.0
  * @date    19-March-2012
  * @brief   This file includes the interrupt handlers for the application
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
#include "usb_bsp.h"
#include "usb_hcd_int.h"
#include "usbh_core.h"
#include "stm32fxxx_it.h"
#include "main.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

extern USB_OTG_CORE_HANDLE          USB_OTG_Core;
extern USBH_HOST                    USB_Host;
 
/* Private function prototypes -----------------------------------------------*/
extern void USB_OTG_BSP_TimerIRQ (void);
extern void irq_handler(uint8_t num);

/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*             Cortex-M Processor Exceptions Handlers                         */
/******************************************************************************/
/**
  * @brief  NMI_Handler
  *         This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  HardFault_Handler
  *         This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  MemManage_Handler
  *         This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  BusFault_Handler
  *         This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  UsageFault_Handler
  *         This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  SVC_Handler
  *         This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  DebugMon_Handler
  *         This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}


/**
  * @brief  PendSV_Handler
  *         This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  EXTI1_IRQHandler
  *         This function handles External line 1 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI1_IRQHandler(void)
{
  if(EXTI_GetITStatus(EXTI_Line1) != RESET)
  {
      USB_Host.usr_cb->OverCurrentDetected();
      EXTI_ClearITPendingBit(EXTI_Line1);
  }
}
/**
  * @brief  TIM2_IRQHandler
  *         This function handles Timer2 Handler.
  * @param  None
  * @retval None
  */
void TIM2_IRQHandler(void)
{
  USB_OTG_BSP_TimerIRQ();
}


#if 0
uint8_t uart_data[200] = {0};
uint16_t count = 0, num = 0;
uint8_t Usart_ucDispCMDDataBuffTemp[126]={0};	//中文站集信息
uint8_t Usart_ucDispCMDDataBuffENGTemp[106]={0};	//英文站集信息
uint8_t Usart_ucDispCMDDataBuffFUWUTemp[166]={0};	//服务用语信息	
uint8_t uart_buff[60] = {0};
uint8_t uart_get_flag = 0;
uint8_t uc_len = 0, uceng_len = 0, ucfuwu_len = 0;

void USART2_IRQHandler(void)
{
   if(USART_GetFlagStatus(USART2,USART_FLAG_RXNE)!=RESET)
	{
		USART_ClearITPendingBit(USART2,USART_FLAG_RXNE);
		uart_data[count++]=USART_ReceiveData(USART2);
	}
	
	if(uart_data[0]==0xA5)
	{
		if(count > 3)
		{		
			if((uart_data[1] == 0x98))
			{
				if(uart_data[2] == 0x98)
				{
					Usart_ucDispCMDDataBuffTemp[uc_len++]=uart_data[count-1];
					if((uc_len==122)&&(count==125))
					{
						uc_len=0;
						count=0;
						//uart_get_flag=1;
						goto Rexit;
					}		
				}	
				if(uart_data[2]==0x88)
				{
					Usart_ucDispCMDDataBuffENGTemp[uceng_len++]=uart_data[count-1];
					if((uceng_len==102)&&(count==105))
					{
						uceng_len=0;
						count=0;
						LED_OK=0;
						uart_get_flag=1;
						goto Rexit;
					}
				}	
				if(uart_data[2]==0x78)
				{
					Usart_ucDispCMDDataBuffFUWUTemp[ucfuwu_len++]=uart_data[count-1];	
					if((ucfuwu_len==162)&&(count==165))
					{
						ucfuwu_len=0;
						count=0;
						goto Rexit;
					}
				}	
			}
			else
			{
				uart_buff[num++]=uart_data[count-1];
				if((num==51)&&(count==54))
				{
					num=0;
					count=0;
				}		
			}
		}
	}
	else
	{
		count=0;
	}
Rexit:
	
}


void UART4_IRQHandler(void)
{

}
#endif


/**
  * @brief  OTG_FS_IRQHandler
  *          This function handles USB-On-The-Go FS global interrupt request.
  *          requests.
  * @param  None
  * @retval None
  */
#ifdef USE_USB_OTG_FS  
void OTG_FS_IRQHandler(void)
#else
void OTG_HS_IRQHandler(void)
#endif
{
  USBH_OTG_ISR_Handler(&USB_OTG_Core);
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
