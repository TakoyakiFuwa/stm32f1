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
#include "A_ADC.h"
#include "TFT_ST7735.h"
#include "UI_DEF.h"
#include "TFT_font.h"
#include "P_PWM.h"
#include "R_RTC.h"
#include "TFT_DMA.h"

/**@brief  初始化线程
  */
void Start_MainTask(void* pvParameters)
{
	//启动内容
	Start_Func();
		//初始化函数-格式建议用Init_Xxx
	Init_Func();
	Init_ADC();
//	Init_TFT();
//	Init_UI();
	Init_TFTD();
//	UI_Write_Num(10,20,1234,FONT_NI7SEG_2412,COLOR_LIGHT_GREEN,COLOR_YELLOW,5);
	UI_Write_String(10,70,"Hello",FONT_PIXEL_2412,COLOR_BLUE,COLOR_GOLD,6);
	
	//进入临界区
	taskENTER_CRITICAL();
		//线程函数-格式建议用Task_Xxx
	xTaskCreate(Task_Func,"Func",64,NULL,1,NULL);
//	xTaskCreate(Task_PWM,"PWM",64,NULL,1,NULL);
//	xTaskCreate(Task_ADC,"ADC",64,NULL,2,NULL);
//	xTaskCreate(Task_RTC,"RTC",32,NULL,1,NULL);
	xTaskCreate(Task_TFTD,"TFT_DMA",128,NULL,2,NULL);
	
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
		U_Printf("现在在写hub相关驱动 \r\n");
	}
	else if(Command("TFTD"))
	{
		U_Printf("这里是TFT with DMA的命令程序: \r\n");
		Cmd_TFTD();
		U_Printf("命令程序结束 \r\n");
	}
	
	//结束
	else
	{
		return 0;
	}
	return 1;
}

