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

//DO  <- PB6				//为了效率写成这样是真有毛病吧....
#define PIN_WQ_DO()		(((GPIOB->IDR&GPIO_Pin_6)==0)?0:1)

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
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;	//感觉不对，引脚速度太快会不会跟不上
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
	
	U_Printf("W25Q64初始化完成 \r\n");
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
/**@brief  等待W25Q64忙状态结束
  *@param  void
  *@retval void
  */
void WQ_Waiting(void)
{
	WQ_Start();
	WQ_Swap(0x05);
	while( (WQ_Swap(0x05)&0x01) != 0)
	{
//		U_Printf("忙w ");
	}
//	U_Printf("\r\n 忙状态结束 \r\n");
	WQ_Stop();
}
/**@brief  解除芯片的软件写保护
  *@param  void
  *@retval void
  */
void WQ_WriteEnable(void)
{
	WQ_Start();
	WQ_Swap(0x06);
	WQ_Stop();
	WQ_Waiting();
}
/**@brief  对芯片的扇区进行擦除
  *@param  addr 擦除的地址	仅0xFFF三个高十六位
  *@retval void
  *@add	   建议在之后使用WQ_Waiting
  */
void WQ_Erease(uint16_t addr)
{
	WQ_Waiting();
	WQ_WriteEnable();
	WQ_Start();
	WQ_Swap(0x20);//擦除指令
	WQ_Swap((addr>>4));
	WQ_Swap((addr&0xF)<<4);
	WQ_Swap(0x00);
	WQ_Stop();
}

/*	由于这个库只是配合关于stm32f103c8在tft上显示bmp的
 *	以下内容和寻址方式仅配合bmp项目
 *			——2025/5/28-22:57
 */
/*	细想了一下，还是打算把这个写完善一点...
 *	很难啊...
 *			——2025/5/29-18:54
 */
/*	我刚刚上完厕所灵机一动
 *	(我真是个笨蛋)
 *	我可以把index的Frame作为地址映射到整个芯片....
 *	....
 *	把原addr改成size Frame改成addr吧...
 *			——2025/5/29-20:03
 */
/**@brief  写入index
  *@param  addr 		地址 最大值256/4=64
  *@param  size	 		图片存储大小(byte)(像素量*3)
  *@param  delay_time	该帧的持续时间
  *@retval void
  *@add	   建议使用前用WQ_Erease(0x000)擦除
  */
void WQ_WriteIndex(uint8_t addr,uint16_t size,uint16_t delay_time)
{
	WQ_Waiting();
	//格式: 0XXX(图片索引)-XXXX(持续时长) 
	//即每一副图像索引占4位
	WQ_WriteEnable();
	WQ_Start();
	WQ_Swap(0x02);
	//地址
	WQ_Swap(0x00);//地址前两位
	WQ_Swap(0x00);//中间两位
	WQ_Swap(addr*4);//后两位
	//写数据
	WQ_Swap((size>>8));
	WQ_Swap((size&0xFF));
	WQ_Swap((delay_time>>8));
	WQ_Swap((delay_time&0xFF));
	WQ_Stop();
}
/**@brief  读取某一帧的信息
  *@param  同上
  *@retval void
  *@add	   建议使用前用WQ_Erease(0x000)擦除
  */
void WQ_ReadIndex(uint8_t addr,uint16_t* size,uint16_t* delay_time)
{
	WQ_Start();
	WQ_Swap(0x03);
	//地址
	WQ_Swap(0x00);//地址前两位
	WQ_Swap(0x00);//中间两位
	WQ_Swap(addr*4);//后两位
	//读数据
	*size = (WQ_Swap(0xFF)<<8);
	*size |= WQ_Swap(0xFF);
	*delay_time = (WQ_Swap(0xFF)<<8);
	*delay_time |= WQ_Swap(0xFF);
	WQ_Stop();
}
/**@brief  修改index
  *@param  同下(?)
  *@retval void
  *@add    这一段的代码复用率烂的不能再烂了
  *		   真丢人...()
  */
void WQ_ChangeIndex(uint8_t addr,uint16_t size,uint16_t delay_time)
{
	uint16_t temp_size=0,temp_delay=0;
	//写到临时存储器中
	for(int i=0;i<64;i++)
	{
		WQ_Waiting();
		//从0x000读出
		WQ_ReadIndex(i,&temp_size,&temp_delay);
		if(temp_size==0xFFFF)
		{
			break;
		}
		//写入到0x001
		WQ_Waiting();
			//即每一副图像索引占4位
		WQ_WriteEnable();
		WQ_Start();
		WQ_Swap(0x02);
		//地址
		WQ_Swap(0x00);//地址前两位
		WQ_Swap(0x10);//中间两位
		WQ_Swap(i*4);//后两位
		//写数据
		WQ_Swap((temp_size>>8));
		WQ_Swap((temp_size&0xFF));
		WQ_Swap((temp_delay>>8));
		WQ_Swap((temp_delay&0xFF));
		WQ_Stop();
	}
	WQ_Erease(0x000);
	//从临时存储器写回到原存储器
	WQ_Waiting();
	for(int i=0;i<64;i++)
	{
		WQ_Waiting();
		//从0x001读出
		WQ_Start();
		WQ_Swap(0x03);
		//地址
		WQ_Swap(0x00);//地址前两位
		WQ_Swap(0x10);//中间两位
		WQ_Swap(i*4);//后两位
		//读数据
		temp_size = (WQ_Swap(0xFF)<<8);
		temp_size |= WQ_Swap(0xFF);
		temp_delay = (WQ_Swap(0xFF)<<8);
		temp_delay |= WQ_Swap(0xFF);
		WQ_Stop();
		if(temp_size==0xFFFF)
		{
			break;
		}
		//写入到0x000
			//遇到修改位置的特殊处理
		if(i==addr)
		{
//			U_Printf("不兑！：%d %d \r\n",size,delay_time);
			WQ_WriteIndex(i,size,delay_time);
			continue;
		}
		WQ_WriteIndex(i,temp_size,temp_delay);
	}
	WQ_Erease(0x001);
}
/**@brief  打印index的信息
  */
void WQ_PrintfIndex(void)
{
	U_Printf("尺寸应该小于64*256即0x4000");
	uint16_t size = 0,delay_time = 0;
	for(int i=0;i<64;i++)
	{
		WQ_ReadIndex(i,&size,&delay_time);
		if(size==0xFFFF)
		{
			break;
		}
		U_Printf("地址:%d(%h) 尺寸:%h byte 持续时长:%d \r\n",i,((i*64)<<12),size,delay_time);
	}
	U_Printf("打印结束 \r\n");
}



/*	以下是关于从index映射到具体内存区的部分 index的0位置用来指示当前有几个图片
 *	0xFFF-1->4096-1处可以分配
 *	index的每处地址(共64)可以*64后映射到内存区
 *			——2025/5/29-20:12
 */
wq_memory WQ_Open(uint8_t addr,uint8_t wq_state)
{
	wq_memory ptr;
	ptr.addr = 65;
	//打开方式不对
	if(wq_state>=3||addr==0)
	{
		U_Printf("wq_open wrong \r\n");
		return ptr;
	}
	ptr.addr = addr;
	if(wq_state==wq_state_write)
	{
		WQ_Waiting();
		for(int i=0;i<64;i++)
		{
			if(i%5==0)
			{
				U_Printf("擦除中%d \r\n",i);
			}
			WQ_Erease(addr*64+i);
		}
		ptr.delay_time = 0;
		ptr.size = 0;
		ptr.ptr = (ptr.addr*64)<<12;
	}
	else
	{
		WQ_ReadIndex(addr,&ptr.size,&ptr.delay_time);
		ptr.ptr = (ptr.addr*64)<<12;
	}
	ptr.open_status = wq_state;
	return ptr;
}
void WQ_PrintfPTR(wq_memory wq)
{
	U_Printf("addr:%d(%h) ptr:%h delay_time:%d size:%d \r\n",wq.addr,((wq.addr*64)<<12),wq.ptr,wq.delay_time,wq.size);
}
void WQ_Write(wq_memory* wq,uint8_t* datas,uint8_t count)
{
	WQ_Waiting();
	WQ_WriteEnable();
	WQ_Start();
	WQ_Swap(0x02);
	//地址
	WQ_Swap((wq->ptr>>16));//地址前两位
	WQ_Swap((wq->ptr>>8));//中间两位
	WQ_Swap(wq->ptr);//后两位
//	//写数据
//	for(int i=0;i<count;i++)
//	{	
//		WQ_Swap(datas[i]);
////		if((wq->ptr&0xFF)==0xFF)
////		{
////			U_Printf("换页:(这里应该有FF)%h wq_size:%d \r\n",wq->ptr,wq->size);		
////			if(wq->size>=0x7FFF)
////			{
////				U_Printf("越界了... \r\n");
////				return;
////			}
////			WQ_Stop();
////			WQ_Waiting();
////			WQ_Start();
////			WQ_Swap(0x02);
////			uint32_t temp_addr = wq->ptr+1;
////			WQ_Swap((temp_addr>>16));//地址前两位
////			WQ_Swap((temp_addr>>8));//中间两位
////			WQ_Swap(temp_addr);//后两位
////		}
//		wq->ptr++;
//		wq->size++;
//	}
	WQ_Swap(datas[0]);
	wq->ptr++;
	wq->size++;
	WQ_Stop();
}
int8_t WQ_Read(wq_memory* wq,uint8_t* datas,uint8_t count)
{
	uint16_t temp_size = wq->size;
	WQ_Waiting();
	WQ_Start();
	WQ_Swap(0x03);
	//地址
	WQ_Swap((wq->ptr>>16));//地址前两位
	WQ_Swap((wq->ptr>>8));//中间两位
	WQ_Swap(wq->ptr);//后两位
	//读数据
	for(int i=0;i<count;i++)
	{	
		datas[i] = WQ_Swap(0xFF);
		wq->ptr++;
		wq->size--;
	}
	WQ_Stop();
	if(wq->size>temp_size||wq->open_status==wq_state_overread)
	{
		wq->open_status=wq_state_overread;
		return -1;
	}
	return 1;
}
void WQ_Close(wq_memory ptr)
{
	if(ptr.open_status==wq_state_read||ptr.open_status==wq_state_overread)
	{
		return;
	}
	WQ_PrintfPTR(ptr);
	WQ_ChangeIndex(ptr.addr,ptr.size,ptr.delay_time);
}

/**@brief  测试接口 可用9f读取ID:-> EF 4017
  */
void Cmd_WQ(void)
{
	WQ_Erease(0x000);
	WQ_Waiting();
	for(int i=0;i<16;i++)
	{
		WQ_WriteIndex(i,0x010,0);
	}
	WQ_Waiting();
	WQ_PrintfIndex();

//	uint8_t data[5] = {72,3,12,32,66};
//	wq_memory ptr = WQ_Open(5,wq_state_write);
//	WQ_Write(&ptr,data,5);
//	WQ_Write(&ptr,data,3);
//	WQ_Close(ptr);
//	ptr = WQ_Open(5,wq_state_read);
//	WQ_PrintfPTR(ptr);
//	WQ_PrintfIndex();

//	int8_t status = 0;
//	uint8_t data;
//	wq_memory ptr = WQ_Open(5,wq_state_read);
//	WQ_PrintfPTR(ptr);
//	for(int i=0;i<10;i++)
//	{
//		status = WQ_Read(&ptr,&data,1);
//		U_Printf("第%d次:%d ,\t状态:",i,data);
//		if(status>0)
//		{
//			U_Printf("1");
//		}
//		else
//		{
//			U_Printf("n");
//		}
//		U_Printf("\r\n");
//	}
//	U_Printf("\r\n");
	
	
	U_Printf("这里是W25Q64测试接口呢w \r\n");
}



