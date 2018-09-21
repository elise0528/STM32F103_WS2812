#include "usart.h"

//C库
#include <stdarg.h>
#include <string.h>
#include <stdio.h>

ALTER_INFO alterInfo;

unsigned char UsartPrintfBuf[128];
unsigned char UsartRxBuf[1];


/*
************************************************************
*	函数名称：	Usart_SendString
*
*	函数功能：	串口数据发送
*
*	入口参数：	USARTx：串口组
*				str：要发送的数据
*				len：数据长度
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
void Usart_SendString(USART_TypeDef *USARTx, unsigned char *str, unsigned short len)
{

    if (USARTx == USART1)
    {
        if (HAL_UART_GetState(&huart1) == HAL_UART_STATE_READY)
        {
            if (HAL_UART_Transmit_DMA(&huart1, str, len) != HAL_OK)
            {
                Error_Handler();
            }
        }
    }
    else if (USARTx == USART2)
    {
        if (HAL_UART_GetState(&huart2) == HAL_UART_STATE_READY)
        {
            if (HAL_UART_Transmit_IT(&huart2, str, len) != HAL_OK)
            {
                Error_Handler();
            }
        }
    }
}

/*
************************************************************
*	函数名称：	UsartPrintf
*
*	函数功能：	格式化打印
*
*	入口参数：	USARTx：串口组
*				fmt：不定长参
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
void UsartPrintf(USART_TypeDef *USARTx, char *fmt, ...)
{

    va_list ap;

    unsigned char *pStr = UsartPrintfBuf;

    memset(UsartPrintfBuf, 0, sizeof(UsartPrintfBuf)); //清空buffer

    va_start(ap, fmt);
    vsprintf((char *)UsartPrintfBuf, fmt, ap); //格式化
    va_end(ap);

    if (USARTx == USART1)
    {
        if (HAL_UART_GetState(&huart1) != HAL_UART_STATE_BUSY_TX)
        {
            if (HAL_UART_Transmit_DMA(&huart1, UsartPrintfBuf, strlen((const char *)pStr)) != HAL_OK)
            {
                Error_Handler();
            }
        }
    }
    else if (USARTx == USART2)
    {
        if (HAL_UART_GetState(&huart2) == HAL_UART_STATE_BUSY_TX)
        {
            if (HAL_UART_Transmit_IT(&huart2, UsartPrintfBuf, strlen((const char *)pStr)) != HAL_OK)
            {
                Error_Handler();
            }
        }
    }
}

/*
************************************************************
*	函数名称：	USART1_IRQHandler
*
*	函数功能：	串口1收发中断
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
// void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
// {
//     HAL_UART_Transmit(&huart1, UsartRxBuf, 1, 0); // DM2发送出去
//     HAL_UART_Receive_IT(&huart1,UsartRxBuf,1); // 重新DMA接收
// }

