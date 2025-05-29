#ifndef __W25Q64_H__
#define __W25Q64_H__
#include "stm32f10x.h"

void Init_WQ(void);
void Cmd_WQ(void);


typedef struct{
	uint8_t addr;
	uint16_t size;
	uint16_t delay_time;
	uint32_t ptr;		//当前指针位置
	uint8_t open_status;
}wq_memory;
enum {
	wq_state_read = 0,
	wq_state_write,
	wq_state_overread//读完了
};
void WQ_PrintfIndex(void);
wq_memory WQ_Open(uint8_t addr,uint8_t wq_state);
void WQ_Write(wq_memory* wq,uint8_t* datas,uint8_t count);
int8_t WQ_Read(wq_memory* wq,uint8_t* datas,uint8_t count);
void WQ_Close(wq_memory ptr);

#endif
