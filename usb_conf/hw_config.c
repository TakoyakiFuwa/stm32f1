/**
  ******************************************************************************
  * @file    hw_config.c
  * @author  MCD Application Team
  * @version V4.1.0
  * @date    26-May-2017
  * @brief   Hardware Configuration & Setup
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2017 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "hw_config.h"
#include "stdint.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void IntToUnicode (uint32_t value , uint8_t *pbuf , uint8_t len);
/* Private functions ---------------------------------------------------------*/

/**@brief USB初始化
  */
void Init_USB(void)
{
	//引脚初始化
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//USB中断配置
	NVIC_SetPriorityGrouping(NVIC_PriorityGroup_2);
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	//USB时钟配置
	RCC_USBCLKConfig(RCC_USBCLKSource_PLLCLK_1Div5);
	//Enable the USB clock
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USB, ENABLE);

	//USB初始化
	USB_Init();
}
/*  USB中断向量配置  */
void USB_LP_CAN1_RX0_IRQHandler(void)
{
	USB_Istr();
}
#include "FreeRTOS.h"
#include "task.h"
/**
  * Function Name : QYHID_Send.
  * Description   : prepares buffer to be sent containing QYHID event infos.
  * Input         : Keys: keys received from terminal.
  * Output        : None.
  * Return value  : None.
  */
void USB_KeyBoard(uint8_t KBH_xxx,uint8_t KB_xxx)
{
	// 定义键盘报告（8 字节）
	uint8_t Keyboard_Report[8] = {0,0,0,0,0,0,0,0};

    // 1️⃣ 清空报告
	for(int i=0;i<8;i++)
	{
		Keyboard_Report[i] = 0;
	}
	
	//辅助键
	Keyboard_Report[0] = KBH_xxx;
	//一直等于零
//	Keyboard_Report[1] = 0;
	//普通按键
    Keyboard_Report[2] = KB_xxx;
//	Keyboard_Report[2] = 0x04;
//	Keyboard_Report[2] = 0x04;
	
	
    USB_SIL_Write(EP1_IN, Keyboard_Report, sizeof(Keyboard_Report));
	SetEPTxValid(ENDP1);
	
	vTaskDelay(10);
	
	for(int i=0;i<8;i++)
	{
		Keyboard_Report[i] = 0;
	}
	// 5️⃣ 发送“释放”报告（所有键松开）
    USB_SIL_Write(EP1_IN, Keyboard_Report, sizeof(Keyboard_Report));
    SetEPTxValid(ENDP1);
}
void USB_KeyBoard_OFF(void)
{
    // 等待主机读取后，再发送“释放键”报告
//	for(int i=0;i<10;i++)
//	{
//		for(int j=0;j<60000;j++);
//	}
	
	uint8_t Keyboard_Report[8] = {0,0,0,0,0,0,0,0};
	// 5️⃣ 发送“释放”报告（所有键松开）
    USB_SIL_Write(EP1_IN, Keyboard_Report, sizeof(Keyboard_Report));
    SetEPTxValid(ENDP1);
	
	return;
}

/**
  * Function Name  : Get_SerialNum.
  * Description    : Create the serial number string descriptor.
  * Input          : None.
  * Output         : None.
  * Return         : None.
  */
#define         ID1          (0x1FFFF7E8)
#define         ID2          (0x1FFFF7EC)
#define         ID3          (0x1FFFF7F0)
void Get_SerialNum(void)
{
  uint32_t Device_Serial0, Device_Serial1, Device_Serial2;

  Device_Serial0 = *(uint32_t*)ID1;
  Device_Serial1 = *(uint32_t*)ID2;
  Device_Serial2 = *(uint32_t*)ID3;
  
  Device_Serial0 += Device_Serial2;

  if (Device_Serial0 != 0)
  {
    IntToUnicode (Device_Serial0, &QYHID_StringSerial[2] , 8);
    IntToUnicode (Device_Serial1, &QYHID_StringSerial[18], 4);
  }
}

/**
  * Function Name  : HexToChar.
  * Description    : Convert Hex 32Bits value into char.
  * Input          : None.
  * Output         : None.
  * Return         : None.
  */
static void IntToUnicode (uint32_t value , uint8_t *pbuf , uint8_t len)
{
  uint8_t idx = 0;
  
  for( idx = 0 ; idx < len ; idx ++)
  {
    if( ((value >> 28)) < 0xA )
    {
      pbuf[ 2* idx] = (value >> 28) + '0';
    }
    else
    {
      pbuf[2* idx] = (value >> 28) + 'A' - 10; 
    }
    
    value = value << 4;
    
    pbuf[ 2* idx + 1] = 0;
  }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
