#include "TFT_font.h"
#include "stdint.h"

/*	打算脱离屏幕驱动和单片机底层 
 *	来实现一个完全在上层的取模实现库
 *	
 *	通过实现以下接口后即可使用
 *	注意... >>>屏幕显示方向<<< 和字符取模方向需要调整为以下方向
 *	填充方向:+------------>
 *			|			 x
 *			|	1  2  3
 *			|	|  |  |
 *			|	|  |  |
 *			|	V  V  V
 *			v y
 *	（即img2lcd中的 扫描模式:	垂直扫描 字节内像素数据反序）
 *	（以及PCtoLCD2002中 设置  阴码 逐列式 逆向(低位在前) ）
 *	注：以下提到坐标相关都是指 左上角
 *				2025/7/31-9:39
 */
/*	原本tft_font是在这个(TFT_font.h)中定义的
 *	这个文件也是基于tft_font结构体对屏幕函数进行一些显示实现
 *	文件关系重新调整后把tft_font移动到TFT_UI中
 *	但这个文件仍然叫tft_font.h/c
 *			——2025/8/16-15:01.秦羽
 */

tft_font FONT[10];
uint16_t COLOR[16];

/*  接口部分  */
	//屏幕驱动库
#include "TFT_ST7735.h"
	//主要实现的两个内部用函数
/**@brief  设定显示位置
  *@add    这里宽和高和X/Yend大概是Xend=width+x-1，Yend=height+y-1
  */
static void TFTF_SetRect(uint16_t x,uint16_t y,uint16_t width,uint16_t height)
{	
	TFT_SetRect(x,y,width,height);
}
/**@brief  发送单个像素
  */
static void TFTF_Pixel(uint16_t rgb565)
{
	TFT_WriteData16(rgb565);
}
/**@brief  字体初始化
  */
#include "qy_ascii_font.h"
const char font_ASCII_PIXEL_2412[][36];
const char font_ASCII_PIXEL_3216[][64];
const char font_ASCII_NI7SEG_2412[][36];
const char font_ASCII_NI7SEG_3216[][64];
static void Init_Font(uint8_t font_index,uint32_t font_lib,uint8_t height,uint8_t width)
{
	FONT[font_index].font_lib = (const char*)font_lib;
	FONT[font_index].width = width;
	FONT[font_index].height = height;
}
void Init_TFTF(void)
{
	Init_Font(0,(uint32_t)font_ASCII_PIXEL_2412,24,12);
	Init_Font(1,(uint32_t)font_ASCII_PIXEL_3216,32,16);
	Init_Font(2,(uint32_t)font_ASCII_NI7SEG_2412,24,12);
	Init_Font(3,(uint32_t)font_ASCII_NI7SEG_3216,32,16);
	uint32_t colors[16] = {
		0x000000	
		,0x3b2e7e	//藏蓝
		,0x9d2933	//胭脂红
		,0xd9b611	//秋香色
		,0x40de5a	//草绿
		,0x88ada6	//水色
		,0x725e82	//乌色
		,0xf47983	//桃红
		,0xff8936	//橘黄
		,0x0eb83a	//葱青
		,0x70f3ff	//蔚蓝
		,0xff2121	//大红
		,0xeacd76	//金
		,0x75664d	//黎明
		,0xd6ecf0	//月白
	    ,0xFFFFFF	
	};
	for(int i=0;i<16;i++)
	{
		COLOR[i] = TFTF_RGB(colors[i]);
	}
}


/*  以下不是接口...  */



/*  关于形状的部分  */

/**@brief  填充矩形
  *@param  -
  *@param  color 颜色
  *@retval void
  */
void TFTF_DrawRect(uint16_t x,uint16_t y,uint16_t width,uint16_t height,uint16_t color)
{
	TFTF_SetRect(x,y,width,height);
	for(int i=0;i<width*height;i++)
	{
		TFTF_Pixel(color);
	}
}
/**@brief  绘制矩形框
  *@param  x y width height
  *@param  color 框颜色
  *@param  thick 框线宽
  *@retval void
  *@add    注：框架线是向内收缩的 x y width height是最大外边框
  */
void TFTF_DrawFrame(uint16_t x,uint16_t y,uint16_t width,uint16_t height,uint16_t color,int8_t thick)
{
	if(thick<=0)
	{
		return;
	}
	TFTF_DrawRect(x,y,width,thick,color);
	TFTF_DrawRect(x,y+height-thick,width,thick,color);
	TFTF_DrawRect(x,y+thick,thick,height-thick*2,color);
	TFTF_DrawRect(x+width-thick,y+thick,thick,height-thick*2,color);
}

/*  关于取模显示的部分  */
/**@brief  显示单张单色图片
  *@param  x,y 坐标(左上角)
  *@param  pic01 图片内容
  */
void TFTF_Single_01Pic(uint16_t x,uint16_t y,uint8_t pic01,uint8_t color_index)
{
	tft_font f_pic01 = FONT[pic01];
	
	TFTF_SetRect(x,y,f_pic01.width,f_pic01.height);
	uint32_t a=f_pic01.width*f_pic01.height/8;
	for(uint32_t i=0;i<a;i++)
	{
		for(int j=0;j<8;j++)
		{
			if( (f_pic01.font_lib[i]&(0x01<<j)) != 0 )
			{
				TFTF_Pixel(COLOR[(color_index>>4)]);
			}
			else
			{
				TFTF_Pixel(COLOR[(color_index&0x0F)]);
			}
		}
	}
}
/**@brief  显示单个字符
  *@param  	x,y 	位置
  *@param	_char 	要显示的字符
  *@param  font		字体
  */
void TFTF_Single_Char(uint16_t x,uint16_t y,char _char,uint8_t font,uint8_t color_index)
{
	tft_font f_font = FONT[font];
	TFTF_SetRect(x,y,f_font.width,f_font.height);
	uint32_t a=f_font.width*f_font.height/8;
	uint16_t index = (_char-' ')*f_font.height*f_font.width/8;
	for(uint32_t i=0;i<a;i++)
	{
		for(int j=0;j<8;j++)
		{
			if( (f_font.font_lib[index+i]&(0x01<<j)) != 0 )
			{
				TFTF_Pixel(COLOR[(color_index>>4)]);
			}
			else
			{
				TFTF_Pixel(COLOR[(color_index&0x0F)]);
			}
		}
	}
}

/*  关于文本的部分  */

/**@brief  显示一个数字
  *@param  -
  *@param  digits  显示的位数，超过位数会吞掉低位
  */
void TFTF_ShowNum(uint16_t x,uint16_t y,uint32_t num,uint8_t font,uint8_t color_index,int8_t digits)
{
	tft_font f_font = FONT[font];
	uint32_t num_length = 1;
	for(;digits>0;digits--)
	{	
		num_length*=10;
	}
	//从高位开始显示
	int8_t i=0;
	for(num_length/=10;num_length>=1;num_length/=10)
	{
		TFTF_Single_Char(x+f_font.width*(i++),y,num/num_length+'0',font,color_index);
		//减去最高位
		num -= (num - (num%num_length));
	}
}
/**@brief  字符串
  *@param  -
  *@param  NumOfChar  显示的数量
  */
void TFTF_ShowString(uint16_t x,uint16_t y,const char* text,uint8_t font,uint8_t color_index,int8_t NumOfChar)
{
	tft_font f_font=FONT[font];
	int i=0;
	for(;text[i]!='\0';i++)
	{
		if(--NumOfChar<0)
		{
			TFTF_Single_Char(x+f_font.width*(--i),y,'-',font,color_index);
			return;
		}
		TFTF_Single_Char(x+f_font.width*(i),y,text[i],font,color_index);
	}
	for(;NumOfChar>0;NumOfChar--)
	{
		TFTF_Single_Char(x+f_font.width*(i++),y,' ',font,color_index);
	}
}
/**@brief  从RGB888变为RGB565
  *@param  RGB888
  *@retval RGB565
  */
uint16_t TFTF_RGB(uint32_t rgb888)
{
	uint16_t rgb565 = 0;
	rgb565 = rgb888>>19;
	rgb565 = rgb565<<6;
	rgb565 |= ((rgb888>>10)&0x3F);
	rgb565 = rgb565<<5;
	rgb565 |= ((rgb888>>3)&0x1F);
	
	return rgb565;
}







