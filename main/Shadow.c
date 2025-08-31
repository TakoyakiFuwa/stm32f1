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
#include "WQ_BMP.h"

void BMP(uint8_t addr);
/**@brief  初始化线程
  */
void Start_MainTask(void* pvParameters)
{
	//启动内容
	Start_Func();
		//初始化函数-格式建议用Init_Xxx
//	Init_TFT();
//	Init_WQ();
	Init_WBMP();
	
	//进入临界区
	taskENTER_CRITICAL();
		//线程函数-格式建议用Task_Xxx
	
	
	//退出临界区
	taskEXIT_CRITICAL();
	BMP(1);
	//删除自身函数
	vTaskDelete(NULL);
}
wq_memory wq;
void BMP(uint8_t addr)
{		
	//画蓝白粉()
	uint8_t width = 133/3 +1;
	uint16_t blue  = TFT_RGB888To565(0x5FCDE4);
	uint16_t white = TFT_RGB888To565(0xFFFFFF); 
	uint16_t pink  = TFT_RGB888To565(0xFFB6C1);
	
	TFT_SetRect(0,0,168,width);
	for(int i=0;i<width*168;i++)
	{
		TFT_WriteData16(pink);
	}
	TFT_SetRect(0,width,168,width);
	for(int i=0;i<width*168;i++)
	{
		TFT_WriteData16(white);
	}
	TFT_SetRect(0,width*2,168,width);
	for(int i=0;i<width*168;i++)
	{
		TFT_WriteData16(blue);
	}
	//打开文件画图
	wq = WQ_Open(addr,wq_state_read);
	uint8_t data;
	uint16_t rgb565;
	uint8_t height;
	uint8_t words[10];
	for(int i=0;i<9;i++)
	{
		WQ_Read(&wq,&words[i],1);
	}
	if(words[0]=='T'||words[0]=='t')
	{//写出这种程序.....罢了罢了...
		if(words[1]=='R'||words[1]=='r')
		{
			if(words[2]=='E'||words[2]=='e')
			{
				if(words[3]=='E'||words[3]=='e')
				{TFT_Clear(2179);}
			}
		}
	}
	WQ_Read(&wq,&width,1);
	WQ_Read(&wq,&height,1);
	TFT_SetRect((160-width)/2,(128-height)/2+1,width,height);
	U_Printf("size:%d*%d \r\n",width,height);
	for(int i=0;i<width*height;i++)
	{
		WQ_Read(&wq,(uint8_t*)&rgb565,1);
		rgb565<<=8;
		WQ_Read(&wq,&data,1);
		rgb565|=data;
		TFT_WriteData16(rgb565);
	}
}
/**@brief  指令监听
  */
extern uint8_t usart1_buff[];
extern USART_InitTypeDef USART_InitStruct;
extern int8_t cmdORdata;
uint8_t addr_;
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
	else if(Command("WBMP"))
	{//图片显示
		Cmd_WBMP();
	}
	else if(Command("CONFIG"))
	{
		addr_ = usart1_buff[6]-'0';
		if(usart1_buff[7]<='9' && usart1_buff[7]>='0')
		{
			addr_*=10;
			addr_+=usart1_buff[7]-'0';
		}
		wq = WQ_Open(addr_,wq_state_write);
		U_Printf("已经重新配置[%d] \r\n",addr_);
		//配置数据接收
			//关闭旧DMA配置
		USART_DMACmd(USART1,USART_DMAReq_Rx,DISABLE);
		DMA_Cmd(DMA1_Channel5,DISABLE);
			//提前申请数据传输
		vTaskDelay(100);
		U_Printf("DataIn");
		vTaskDelay(100);
			//配置中断
		USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
			//更改USART1模式
		cmdORdata = 1;	
		while(cmdORdata!=0)
		{
			vTaskDelay(100);
		}
		vTaskDelay(100);
		BMP(addr_);
	}
	else if(Command("Read"))
	{
		addr_ = usart1_buff[4]-'0';
		if(usart1_buff[5]<='9' && usart1_buff[5]>='0')
		{
			addr_*=10;
			addr_+=usart1_buff[5]-'0';
		}
		BMP(addr_);
	}
	
	//结束
	else
	{
		return 0;
	}
	return 1;
}

