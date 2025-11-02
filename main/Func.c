#include "Func.h"
#include "stm32f10x.h"                  // Device header
#include "FreeRTOS.h"
#include "task.h"
#include "hw_config.h"
#include "Def_KeyBoard.h"

void Init_Button(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_6;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
}
void Task_Button(void* pvParameters)
{
	uint32_t a = 0;
	while(1)
	{
		vTaskDelay(50);
		if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_5)==Bit_RESET)
		{
			USB_KeyBoard(0,KB_0);
			vTaskDelay(20);
			while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_5)==Bit_RESET)
			{
				vTaskDelay(10);
				if(a++>70)
				{
					USB_KeyBoard(0,KB_Y);
				}
			}
			a=0;
			vTaskDelay(20);
		}
		if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_6)==Bit_RESET)
		{
			USB_KeyBoard(KBH_RIGHT_SHIFT,KB_K);
			USB_KeyBoard(0,KB_E);
			USB_KeyBoard(0,KB_Y);
			USB_KeyBoard(KBH_RIGHT_SHIFT,KB_B);
			USB_KeyBoard(0,KB_O);
			USB_KeyBoard(0,KB_A);
			USB_KeyBoard(0,KB_R);
			USB_KeyBoard(0,KB_D);
			USB_KeyBoard(0,KB_SPACE);
			USB_KeyBoard(0,KB_B);
			USB_KeyBoard(0,KB_Y);
			USB_KeyBoard(0,KB_SPACE);
			USB_KeyBoard(KBH_RIGHT_SHIFT,KB_Q);
			USB_KeyBoard(KBH_RIGHT_SHIFT,KB_Y);
			vTaskDelay(10);
			USB_KeyBoard(0,KB_Y);
			USB_KeyBoard(0,KB_Q);
			USB_KeyBoard(0,KB_Y);
			USB_KeyBoard(0,KB_ENTER);
			vTaskDelay(20);
			while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_6)==Bit_RESET);
			vTaskDelay(20);
		}
	}
}
