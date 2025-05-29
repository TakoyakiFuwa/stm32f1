#include "Shadow.h"
#include "BaseFunc.h"
/*  ST库  */
#include "stm32f10x.h"
/*  OS库  */
#include "FreeRTOS.h"
#include "task.h"
/*  接口库  */
#include "Func.h"
/*  外设库  */
#include "U_USART1.h"
/*  驱动库  */
#include "TFT_ST7735.h"
#include "W25Q64.h"

/**@brief  初始化线程
  */
void Start_MainTask(void* pvParameters)
{
	//启动内容
	Start_Func();
		//初始化函数-格式建议用Init_Xxx
//	Init_TFT();
	Init_WQ();
	
	
	//进入临界区
	taskENTER_CRITICAL();
		//线程函数-格式建议用Task_Xxx
	
	
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
	else if(Command("TEST"))
	{
		
		U_Printf("测试\r\n");
	}
	else if(Command("HELLO"))
	{
		U_Printf("Hello! New R disk.\r\n");
	}
	else if(Command("WQ"))
	{
		Cmd_WQ();
	}
	
	//结束
	else
	{
		return 0;
	}
	return 1;
}

