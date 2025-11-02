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
/*  USB库  */
#include "hw_config.h"
#include "Def_KeyBoard.h"

/**@brief  初始化线程
  */
void Start_MainTask(void* pvParameters)
{
	//启动内容
	Start_Func();
		//初始化函数-格式建议用Init_Xxx
	Init_USB();
	Init_Button();
	
	//进入临界区
	taskENTER_CRITICAL();
		//线程函数-格式建议用Task_Xxx
	xTaskCreate(Task_Button,"Button",64,NULL,3,NULL);
	
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
	else if(Command("TEST"))
	{
		
		U_Printf("测试\r\n");
	}
	else if(Command("HELLO"))
	{
		U_Printf("Hello! New R disk.\r\n");
	}
	else if(Command("USB0"))
	{
		USB_KeyBoard(KBH_LEFT_WIN,0);
		U_Printf("已模拟键盘发送字符 \r\n");
	}
	
	//结束
	else
	{
		return 0;
	}
	return 1;
}

