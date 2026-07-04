#include "Func.h"
#include "stm32f10x.h"                  // Device header
#include "FreeRTOS.h"
#include "task.h"

void Init_Func(void)
{//TIM1_CH3
	//时钟初始化
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);
	//引脚初始化
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	//时基单元初始化
	TIM_ARRPreloadConfig(TIM1,ENABLE);
	TIM_TimeBaseInitTypeDef TIM_InitStruct;
	TIM_InitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_InitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_InitStruct.TIM_Period = 1000 -1;
	TIM_InitStruct.TIM_Prescaler = 18 -1;
	TIM_InitStruct.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM1,&TIM_InitStruct);
	//PWM初始化
	TIM_CtrlPWMOutputs(TIM1,ENABLE);
	TIM_OCInitTypeDef TIM_OCInitStruct;
	TIM_OCInitStruct.TIM_OCIdleState = TIM_OCIdleState_Reset;
	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStruct.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
	TIM_OCInitStruct.TIM_OCNPolarity = TIM_OCNPolarity_High;
	TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStruct.TIM_OutputNState = TIM_OutputNState_Enable;
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStruct.TIM_Pulse = 0;
	TIM_OC3Init(TIM1,&TIM_OCInitStruct);
	//定时器启动
	TIM_Cmd(TIM1,ENABLE);
	U_Printf("Func:设置TIM1_CH3初始化 \r\n");
}
void Task_Func(void* pvParameters)
{
	int8_t position = 4;
	int32_t ref = 0;
	while(1)
	{
		vTaskDelay(10);
		if(ref>=1000)
		{
			vTaskDelay(200);
			position = -4;
		}
		else if(ref<=0)
		{
			vTaskDelay(200);
			position = 4;
		}
		ref+=position;
		TIM_SetCompare3(TIM1,ref);
	}
}







