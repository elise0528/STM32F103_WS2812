 /* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "bsp_debug_usart.h"

/* ˽�����Ͷ��� --------------------------------------------------------------*/
/* ˽�к궨�� ----------------------------------------------------------------*/
/* ˽�б��� ------------------------------------------------------------------*/
#ifdef STM32_CUBEMX
extern UART_HandleTypeDef husart_debug;
#else
UART_HandleTypeDef husart_debug;
#endif
/* ��չ���� ------------------------------------------------------------------*/
/* ˽�к���ԭ�� --------------------------------------------------------------*/
/* ������ --------------------------------------------------------------------*/

#ifndef STM32_CUBEMX
/**
  * ��������: ����Ӳ����ʼ������
  * �������: huart�����ھ������ָ��
  * �� �� ֵ: ��
  * ˵    ��: �ú�����HAL���ڲ�����
  */
void DEBUG_UART_MspInit(GPIO_InitTypeDef GPIO_InitStruct)
{

  /* ��������ʱ��ʹ�� */
  DEBUG_USART_RCC_CLK_ENABLE();

  /* �������蹦��GPIO���� */
  GPIO_InitStruct.Pin = DEBUG_USARTx_Tx_GPIO_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(DEBUG_USARTx_Tx_GPIO, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = DEBUG_USARTx_Rx_GPIO_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(DEBUG_USARTx_Rx_GPIO, &GPIO_InitStruct);
}

/**
  * ��������: ���ڲ�������.
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  */
void DEBUG_USART_Init(void)
{

  husart_debug.Instance = DEBUG_USARTx;
  husart_debug.Init.BaudRate = DEBUG_USARTx_BAUDRATE;
  husart_debug.Init.WordLength = UART_WORDLENGTH_8B;
  husart_debug.Init.StopBits = UART_STOPBITS_1;
  husart_debug.Init.Parity = UART_PARITY_NONE;
  husart_debug.Init.Mode = UART_MODE_TX_RX;
  husart_debug.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  husart_debug.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&husart_debug) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
}
#endif

/**
  * ��������: �ض���c�⺯��printf��DEBUG_USARTx
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  */
int fputc(int ch, FILE *f)
{
  HAL_UART_Transmit(&husart_debug, (uint8_t *)&ch, 1, 0xffff);
  return ch;
}

/**
  * ��������: �ض���c�⺯��getchar,scanf��DEBUG_USARTx
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  */
int fgetc(FILE *f)
{
  uint8_t ch = 0;
  HAL_UART_Receive(&husart_debug, &ch, 1, 0xffff);
  return ch;
}

/******************************  *****END OF FILE**** ******************************/
