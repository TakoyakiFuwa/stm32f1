#include "WQ_BMP.h"
/*  ST库  */
#include "stm32f10x.h"
/*  驱动库  */
#include "W25Q64.h"
#include "TFT_ST7735.h"
/*  Printf库  */
#include "U_USART1.h"

/*	值得一提的是...
 *	这个不是BMP解码库...
 *	只是把W25Q64的数据读取出来
 *	通过屏幕输出而已...
 *			——2025/5/31-22:30
 */

void Init_WBMP(void)
{
	Init_TFT();
	Init_WQ();
	
	U_Printf("BMP库(TFT/W25Q64)初始化完成 \r\n");
}

void WBMP_TFTShow(uint8_t addr)
{
	
}

void Cmd_WBMP(void)
{
	wq_memory wq_w = WQ_Open(1,wq_state_read);
	uint8_t data = 0;
	uint8_t width =0,height=0;
	WQ_Read(&wq_w,&width,1);
	WQ_Read(&wq_w,&height,1);
	TFT_SetRect(((160-width)/2),((132-height)/2),width,height);
//	while(WQ_Read(&wq_w,&data,1)!=-1)
//	{
//		TFT_WriteData(data);
//	}
	
	WQ_Start();
	WQ_Swap(0x03);
	//地址
	WQ_Swap((wq_w.ptr>>16));//地址前两位
	WQ_Swap((wq_w.ptr>>8));//中间两位
	WQ_Swap(wq_w.ptr);//后两位
	//读数据
	int a=width*height*2;
	for(int i=0;i<a;i++)
	{
		data = WQ_Swap(0xFF);
		TFT_WriteData(data);
	}
	
	WQ_Stop();
	

	U_Printf("这里是WBMP的测试接口... \r\n");
}


















