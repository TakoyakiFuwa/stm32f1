#ifndef __USART_WQ_H__
#define __USART_WQ_H__
#include <stdint.h>

void UWQ_Init(void);
void UWQ_Putchar(uint8_t _char);
void UWQ_Words(const char* words);
void UWQ_ConfigWQ(uint8_t addr);
void UWQ_EndCFG(void);

#endif
