#include "Func.h"
#include "stm32f10x.h"                  // Device header
#include "FreeRTOS.h"
#include "task.h"

void Init_Func(void)
{
	//时钟初始化
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	//引脚初始化
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	GPIO_WriteBit(GPIOA,GPIO_Pin_7,Bit_SET);//高电平亮
	//定时器初始化TIM3_CH2
	TIM_InternalClockConfig(TIM3);
	TIM_ARRPreloadConfig(TIM3,ENABLE);
	TIM_TimeBaseInitTypeDef TIM_InitStruct;
	TIM_InitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_InitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_InitStruct.TIM_Period = 4000;
	TIM_InitStruct.TIM_Prescaler = 18;
	TIM_InitStruct.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM3,&TIM_InitStruct);
	//PWM初始化
	TIM_OCInitTypeDef PWM_InitStruct;
	PWM_InitStruct.TIM_OCMode = TIM_OCMode_PWM1;
	PWM_InitStruct.TIM_OCPolarity = TIM_OCPolarity_Low;
	PWM_InitStruct.TIM_OutputState = TIM_OutputState_Enable;
	PWM_InitStruct.TIM_Pulse = 3999;
	TIM_OC2Init(TIM3,&PWM_InitStruct);
	TIM_Cmd(TIM3,ENABLE);
	
}
void Task_Func(void* pvParameters)
{
	uint16_t pulse = 3990;
	int16_t d_p = 1;
	while(1)
	{
		vTaskDelay(100);
		continue;
		pulse += d_p;
		TIM_SetCompare2(TIM3,pulse);
		if(pulse<=3980)
		{
			d_p = 1;
		}
		else if(pulse>=3999)
		{
			d_p = -1;
		}
	}
}
