#include "stm32f10x.h"                  // Device header
#include "USART_WQ.h"
#include "W25Q64.h"
#include "U_USART1.h"

uint8_t uwq_buff[256];
uint16_t uwq_count = 0;
int8_t uwq_isbuff = 0;

wq_memory U_wq;

void UWQ_Init(void)
{
	//时钟初始化
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	GPIO_InitTypeDef GPIO_InitStruct;
	//引脚初始化
		//PA2->USART2_TX
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
		//PA3->USART2_RX
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	//外设初始化
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	USART_InitTypeDef USART_InitStruct;
	USART_InitStruct.USART_BaudRate = 4800;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStruct.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;
	USART_InitStruct.USART_Parity = USART_Parity_No;
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART2,&USART_InitStruct);
	//启用USART2
	USART_Cmd(USART2,ENABLE);
	//中断初始化
	NVIC_SetPriorityGrouping(NVIC_PriorityGroup_4);
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 6;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 6;
	NVIC_Init(&NVIC_InitStruct);
	//设置空闲中断
	USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);
	USART_ITConfig(USART2,USART_IT_IDLE,ENABLE);
	
	UWQ_Words("Hello!!!这里是USART2 \r\n");
}

void UWQ_ConfigWQ(uint8_t addr)
{
	U_wq = WQ_Open(addr,wq_state_write);
}

void USART2_IRQHandler(void)
{
	if(USART_GetITStatus(USART2,USART_IT_RXNE)==SET)
	{
		uint8_t data = USART_ReceiveData(USART2);
		//处理得到的数据
		WQ_Write(&U_wq,&data,1);
		
		
		USART_ClearITPendingBit(USART2,USART_IT_RXNE);
	}
	else if(USART_GetITStatus(USART2,USART_IT_IDLE)==SET)
	{
		//置标志位
		uwq_isbuff = 1;
		//清除IDLE标志位
		USART2->SR;
		USART2->DR;
		
//		UWQ_Words("a \r\n");
		U_Printf("UWQ处理完成 \r\n");
//		WQ_Close(U_wq);
	}
}
void UWQ_EndCFG(void)
{
	WQ_Close(U_wq);
}
void UWQ_Putchar(uint8_t _char)
{
	while(USART_GetFlagStatus(USART2,USART_FLAG_TXE)!=SET);
	USART_SendData(USART2,(uint16_t)_char);
}

void UWQ_Words(const char* words)
{
	for(int i=0;words[i]!='\0';i++)
	{
		UWQ_Putchar(words[i]);
	}
}


