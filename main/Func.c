#include "Func.h"
#include "stm32f10x.h"                  // Device header
#include "FreeRTOS.h"
#include "task.h"

void Init_Func(void)
{

}
void Task_Func(void* pvParameters)
{
	while(1)
	{
		vTaskDelay(100);
		
	}
}
