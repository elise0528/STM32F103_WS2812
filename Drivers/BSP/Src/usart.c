#include "usart.h"

//C��
#include <stdarg.h>
#include <string.h>
#include <stdio.h>

ALTER_INFO alterInfo;

unsigned char UsartPrintfBuf[128];
unsigned char UsartRxBuf[1];


/*
************************************************************
*	�������ƣ�	Usart_SendString
*
*	�������ܣ�	�������ݷ���
*
*	��ڲ�����	USARTx��������
*				str��Ҫ���͵�����
*				len�����ݳ���
*
*	���ز�����	��
*
*	˵����		
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
*	�������ƣ�	UsartPrintf
*
*	�������ܣ�	��ʽ����ӡ
*
*	��ڲ�����	USARTx��������
*				fmt����������
*
*	���ز�����	��
*
*	˵����		
************************************************************
*/
void UsartPrintf(USART_TypeDef *USARTx, char *fmt, ...)
{

    va_list ap;

    unsigned char *pStr = UsartPrintfBuf;

    memset(UsartPrintfBuf, 0, sizeof(UsartPrintfBuf)); //���buffer

    va_start(ap, fmt);
    vsprintf((char *)UsartPrintfBuf, fmt, ap); //��ʽ��
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
*	�������ƣ�	USART1_IRQHandler
*
*	�������ܣ�	����1�շ��ж�
*
*	��ڲ�����	��
*
*	���ز�����	��
*
*	˵����		
************************************************************
*/
// void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
// {
//     HAL_UART_Transmit(&huart1, UsartRxBuf, 1, 0); // DM2���ͳ�ȥ
//     HAL_UART_Receive_IT(&huart1,UsartRxBuf,1); // ����DMA����
// }

