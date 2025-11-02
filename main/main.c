//  /*  启动函数库  */
//  #include "Shadow.h"
//  /*  OS库  */
//  #include "FreeRTOS.h"
//  #include "task.h"
//  
//  int main(void)
//  {
//  	//创建开始线程
//  	TaskHandle_t START_MAINTASK_Handler;
//  	xTaskCreate(Start_MainTask,"Start_Main",128,NULL,8,&START_MAINTASK_Handler);
//  	//开启任务调度器
//  	vTaskStartScheduler();
//  	while(1)
//  	{
//  		//其实这个while疑似没用
//  	}
//  }


#include "hw_config.h"
#include "U_USART1.h"

int main(void)
{
	U_USART1_Init();
	
	//Set_System
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
//  USB_Interrupts_Config();
	NVIC_SetPriorityGrouping(NVIC_PriorityGroup_2);
	NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

// Set_USBClock();
  RCC_USBCLKConfig(RCC_USBCLKSource_PLLCLK_1Div5);
  /* Enable the USB clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USB, ENABLE);

  
  USB_Init();
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
  GPIO_InitTypeDef GPIO_InitStruct;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(GPIOC,&GPIO_InitStruct);
  GPIO_WriteBit(GPIOC,GPIO_Pin_13,Bit_RESET);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_6;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	

  while (1)
  {
//    if (bDeviceState == CONFIGURED && a++<=5)
//    {
//        Joystick_Send(JoyState());
//    }
	if(bDeviceState==CONFIGURED)
	{
		if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_5)==Bit_RESET)
		{
			Joystick_Send(5);
		}
		if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_6)==Bit_RESET)
		{
			Joystick_Send(-5);
		}
		for(int i=0;i<1000;i++)
		{
		}
	}
	
	
  }
}

void USB_LP_CAN1_RX0_IRQHandler(void)
{
	USB_Istr();
}






