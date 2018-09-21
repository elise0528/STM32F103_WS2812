#ifndef __USART_H__
#define __USART_H__

#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_usart.h"

#define USART_DEBUG USART1 //调试打印所使用的串口组

#define USART_TX_TYPE 0

#define USART_RX_TYPE 1

typedef struct
{

    char alterBuf[150];
    unsigned char alterCount;
    _Bool rev_idle;

} ALTER_INFO;

extern ALTER_INFO alterInfo;

extern unsigned char UsartRxBuf[1];

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern DMA_HandleTypeDef hdma_usart1_rx;
extern DMA_HandleTypeDef hdma_usart1_tx;
extern DMA_HandleTypeDef hdma_usart2_rx;
extern DMA_HandleTypeDef hdma_usart2_tx;


void Usart_SendString(USART_TypeDef *USARTx, unsigned char *str, unsigned short len);

void UsartPrintf(USART_TypeDef *USARTx, char *fmt, ...);

// void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);



#endif // __USART_H__
