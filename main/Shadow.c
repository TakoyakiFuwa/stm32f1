#include "Shadow.h"
#include "BaseFunc.h"
/*  ST库  */
#include "stm32f10x.h"
/*  OS库  */
#include "FreeRTOS.h"
#include "task.h"
/*  定义  */
#include "QinYu_define.h"
/*  接口库  */
#include "Func.h"
/*  外设库  */
#include "U_USART1.h"
#include "A_ADC.h"

/**@brief  初始化线程
  */
void Start_MainTask(void* pvParameters)
{
	//启动内容
	Start_Func();
		//初始化函数-格式建议用Init_Xxx
	Init_ADC();
	
	//进入临界区
	taskENTER_CRITICAL();
		//线程函数-格式建议用Task_Xxx
	xTaskCreate(Task_ADC,"ADC",64,NULL,3,NULL);
	
	//退出临界区
	taskEXIT_CRITICAL();
	//删除自身函数
	vTaskDelete(NULL);
}

/**@brief  指令监听
  */
uint8_t Start_CommandFunc(void)
{
	if(Command("Start_CommandFunc"))
	{
		U_Printf("Command(\"COMMAND\")||Command(\"HELP\")\r\n");
	}
	//添加区
	else if(Command("COMMAND")||Command("HELP"))
	{
		U_Printf("这里是stm32f103c6t6的测试程序 \r\n");
		U_Printf("当前测试是USB模拟键盘 \r\n");
	}
	else if(Command("QINYU"))
	{
		U_Printf(QinYu);
	}
	
	//结束
	else
	{
		return 0;
	}
	return 1;
}

