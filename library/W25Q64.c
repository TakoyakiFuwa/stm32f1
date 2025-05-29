#include "W25Q64.h"
/*  ST库  */
#include "stm32f10x.h"
/*  外设库  */
#include "U_USART1.h"

/*	画了一个关于F1驱动TFT的板子...
 *	如果想显示图片的话，
 *	需要大点的Flash作为存储介质
 *	F103C8装不了SD卡
 *	所以选择了W25Q64作为存储
 *		——2025/5/28-21:32
 */
/*	关于W25Q64的机制
 *	用6位十六进制寻址
 *	0xFFFFFF
 *		前面三位0xFFF---是最小擦除单元
 *		中间第四位0x---F--是最小页写单元 
 *			    ->即每遇到0x----FF就要换页再继续页写
 *		后面两位0x----FF就是256个字节了
 *	这个驱动库以前三位(即最小擦除单元)作为寻址
 *		——2025/5/28-22:32
 */
/*	其实现在好累，什么也不想干...
 *	但想到如果什么都不干的话...
 *	就不要浪费时间了 快去学习...
 *	突然就有动力写下去了...
 *		——2025/5/28-21:30
 */
/*	引脚关系
 *	PB7	 ->	CS#
 *	PB6	 ->	DO
 *	PB5	 ->	WP#
 *	PA4	 ->	RST#
 *	PA1	 ->	CLK
 *	PA0	 ->	DI
 */
/*	低电平: GPIOx->BRR  = GPIO_Pin
 *	高电平: GPIOx->BSRR = GPIO_Pin
 */
// //CS# <- PB7
// #define PIN_WQ_CS_L() GPIOB->BRR  = GPIO_Pin_7 
// #define PIN_WQ_CS_H() GPIOB->BSRR = GPIO_Pin_7 
// //WP# <- PB5
// #define PIN_WQ_WP_L() GPIOB->BRR  = GPIO_Pin_5
// #define PIN_WQ_WP_H() GPIOB->BSRR = GPIO_Pin_5
// //RST#<- PA4
// #define PIN_WQ_RST_L() GPIOA->BRR  = GPIO_Pin_4 
// #define PIN_WQ_RST_H() GPIOA->BSRR = GPIO_Pin_4
// //CLK <- PA1
// #define PIN_WQ_CLK_L() GPIOA->BRR  = GPIO_Pin_1 
// #define PIN_WQ_CLK_H() GPIOA->BSRR = GPIO_Pin_1
// //DI  <- PA0
// #define PIN_WQ_DI_L() GPIOA->BRR  = GPIO_Pin_0 
// #define PIN_WQ_DI_H() GPIOA->BSRR = GPIO_Pin_0

//CS# <- PB7
#define PIN_WQ_CS_L() GPIO_WriteBit(GPIOB,GPIO_Pin_7,Bit_RESET) 
#define PIN_WQ_CS_H() GPIO_WriteBit(GPIOB,GPIO_Pin_7,Bit_SET) 
//WP# <- PB5
#define PIN_WQ_WP_L() GPIO_WriteBit(GPIOB,GPIO_Pin_5,Bit_RESET)
#define PIN_WQ_WP_H() GPIO_WriteBit(GPIOB,GPIO_Pin_5,Bit_SET)
//RST#<- PA4
#define PIN_WQ_RST_L() GPIOA->BRR  = GPIO_Pin_4 
#define PIN_WQ_RST_H() GPIOA->BSRR = GPIO_Pin_4
//CLK <- PA1
#define PIN_WQ_CLK_L() GPIO_WriteBit(GPIOA,GPIO_Pin_1,Bit_RESET) 
#define PIN_WQ_CLK_H() GPIO_WriteBit(GPIOA,GPIO_Pin_1,Bit_SET)
//DI  <- PA0
#define PIN_WQ_DI_L() GPIO_WriteBit(GPIOA,GPIO_Pin_0,Bit_RESET) 
#define PIN_WQ_DI_H() GPIO_WriteBit(GPIOA,GPIO_Pin_0,Bit_SET)

//DO  <- PB6
//#define PIN_WQ_DO()	GPIOB->IDR&GPIO_Pin_6==0?0:1
uint8_t PIN_WQ_DO(void)
{
	if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_6)==Bit_RESET)
	{
		return 0;
	}
	return 1;	
}

/**@brief  初始化
  *@param  void
  *@retval void
  */
void Init_WQ(void)
{
	//引脚初始化
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_4;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;	//感觉不对，引脚速度太快会不会跟不上
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_7;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	//初始电平
	PIN_WQ_CS_H();
	PIN_WQ_CLK_L();
	PIN_WQ_WP_H();	//写保护建议一直给高就行
	PIN_WQ_RST_H();	//复位也一直给高
		//因为引脚不够用，这里测试时专门整个引脚做GND
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_14;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOC,&GPIO_InitStruct);
	GPIO_WriteBit(GPIOC,GPIO_Pin_14,Bit_RESET);
}
/**@brief  SPI开始通信
  */
void WQ_Start(void)
{
	PIN_WQ_CS_L();
}

/**@brief  SPI结束通信
  */
void WQ_Stop(void)
{
	PIN_WQ_CS_H();
}
/**@brief  SPI交换一字节数据
  *@param  data 要发送的数据
  *@retval 获得的数据
  *@add	   高位在前
  */
uint8_t WQ_Swap(uint8_t data)
{
	uint8_t retval =0;
	for(int i=0;i<8;i++)
	{
		if( ((0x80>>i)&data) != 0)
		{
			PIN_WQ_DI_H();
		}
		else 
		{
			PIN_WQ_DI_L();
		}
		//这里应该先读取再低电平
		PIN_WQ_CLK_H();
		if(PIN_WQ_DO()!=0)
		{
			retval |= (0x80>>i);
		}
		PIN_WQ_CLK_L();
	}
	return retval;
}
void WQ_Waiting(void)
{
	WQ_Start();
	WQ_Swap(0x05);
	while( (WQ_Swap(0x05)&0x01) != 0)
	{
		U_Printf("忙w ");
	}
	U_Printf("\r\n 忙状态结束 \r\n");
	WQ_Stop();
}
void WQ_WriteEnable(void)
{
	WQ_Start();
	WQ_Swap(0x06);
	WQ_Stop();
	WQ_Waiting();
}

/*	由于这个库只是配合关于stm32f103c8在tft上显示bmp的
 *	以下内容和寻址方式仅配合bmp项目
 *			——2025/5/28-22:57
 */
void WQ_WriteIndex(uint8_t Frame,uint16_t index,uint16_t delay_time)
{
	//格式: 0XXX(图片索引)-XXXX(持续时长) 
	//即每一副图像索引占4位
	WQ_WriteEnable();
	WQ_Start();
	WQ_Swap(0x02);
	//地址
	WQ_Swap(0x00);//地址前两位
	WQ_Swap(0x00);//中间两位
	WQ_Swap(Frame*4);//后两位
	//写数据
	WQ_Swap((index>>8));
	WQ_Swap((index&0xFF));
	WQ_Swap((delay_time>>8));
	WQ_Swap((delay_time&0xFF));
	WQ_Stop();
}
void WQ_ReadIndex(uint8_t Frame,uint16_t* index,uint16_t* delay_time)
{
	WQ_Start();
	WQ_Swap(0x03);
	//地址
	WQ_Swap(0x00);//地址前两位
	WQ_Swap(0x00);//中间两位
	WQ_Swap(Frame*4);//后两位
	//读数据
	*index = (WQ_Swap(0xFF)<<8);
	*index |= WQ_Swap(0xFF);
	*delay_time = (WQ_Swap(0xFF)<<8);
	*delay_time |= WQ_Swap(0xFF);
	WQ_Stop();
}
void WQ_Erease(uint16_t index)
{
	WQ_WriteEnable();
	WQ_Start();
	WQ_Swap(0x20);
	WQ_Swap(0x00);
	WQ_Swap(0x00);
	WQ_Swap(0x00);
	WQ_Stop();
}

/**@brief  测试接口 可用9f读取ID:-> EF 4017
  */
void Cmd_WQ(void)
{
	WQ_Erease(0);
	uint16_t data[2];
	WQ_Waiting();
	WQ_WriteIndex(0,0xCCB,120);
	WQ_Waiting();
	WQ_ReadIndex(0,&data[0],&data[1]);
	U_Printf("读出数据:%h  /  %d \r\n",data[0],data[1]);
	
//	WQ_Start();
//	WQ_Swap(0x9F);//1101 1110 1000 0000 0010 1110
//	uint8_t ID[4];
//	for(int i=0;i<4;i++)
//	{
//		ID[i] = WQ_Swap(0xFF);
//	}
//	U_Printf("输出?:%h %h %h %h \r\n",ID[0],ID[1],ID[2],ID[3]);
//	WQ_Stop();
	U_Printf("这里是W25Q64测试接口呢w \r\n");
}



