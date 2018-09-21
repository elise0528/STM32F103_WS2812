#ifndef __BSP_DEBUG_USART_H__
#define __BSP_DEBUG_USART_H__

/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_usart.h"

#include <stdio.h>

/* ���Ͷ��� ------------------------------------------------------------------*/
/* �궨�� --------------------------------------------------------------------*/
#define DEBUG_USARTx USART1
#define husart_debug huart1
#define DEBUG_USARTx_BAUDRATE 115200
#define DEBUG_USART_RCC_CLK_ENABLE() __HAL_RCC_USART1_CLK_ENABLE()
#define DEBUG_USART_RCC_CLK_DISABLE() __HAL_RCC_USART1_CLK_DISABLE()

#define DEBUG_USARTx_GPIO_ClK_ENABLE() __HAL_RCC_GPIOA_CLK_ENABLE()
#define DEBUG_USARTx_Tx_GPIO_PIN GPIO_PIN_9
#define DEBUG_USARTx_Tx_GPIO GPIOA
#define DEBUG_USARTx_Rx_GPIO_PIN GPIO_PIN_10
#define DEBUG_USARTx_Rx_GPIO GPIOA

#define DEBUG_USART_IRQn USART1_IRQn
/* ��չ���� ------------------------------------------------------------------*/
extern UART_HandleTypeDef husart_debug;

/* �������� ------------------------------------------------------------------*/
#ifndef STM32_CUBEMX
void DEBUG_UART_MspInit(GPIO_InitTypeDef GPIO_InitStruct);
void DEBUG_USART_Init(void);
#endif

#endif /* __BSP_DEBUG_USART_H__ */

/******************************  *****END OF FILE**** ******************************/
