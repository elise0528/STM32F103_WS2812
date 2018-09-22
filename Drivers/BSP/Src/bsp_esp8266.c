/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "bsp_esp8266.h"

/* ˽�����Ͷ��� --------------------------------------------------------------*/
/* ˽�к궨�� ----------------------------------------------------------------*/
/* ˽�б��� ------------------------------------------------------------------*/
#ifdef STM32_CUBEMX
extern UART_HandleTypeDef husartx_esp8266;
#else
UART_HandleTypeDef husartx_esp8266;
#endif

STRUCT_USARTx_Frame strEsp8266_Frame_Record = {0};
uint8_t esp8266_rxdata;
uint8_t ucSmartConfigFlag = 0;
/* ��չ���� ------------------------------------------------------------------*/
/* ˽�к���ԭ�� --------------------------------------------------------------*/

/**
 * ע����STM32Cubemx���ɳ�ʼ�����̺� 
 * 1.��mina.c����� HAL_UART_RxCpltCallback�ص�����
 * 2.��main����������� HAL_UART_Receive_IT���� ���������ж�
*/
/**
  * @brief  Rx Transfer completed callbacks.
  * @param  huart: pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @retval None
  */
// void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
// {
//   /* Prevent unused argument(s) compilation warning */
//   if (huart->Instance == USART2)
//   {
//     if (strEsp8266_Frame_Record.InfBit.FrameLength < (RX_BUF_MAX_LEN - 1)) //Ԥ��1���ֽ�д������
//     {
//       strEsp8266_Frame_Record.Data_RX_BUF[strEsp8266_Frame_Record.InfBit.FrameLength++] = esp8266_rxdata;
//     }
//     HAL_UART_Receive_IT(&husartx_esp8266, &esp8266_rxdata, 1);
//   }
//   /* NOTE: This function Should not be modified, when the callback is needed,
//            the HAL_UART_RxCpltCallback could be implemented in the user file
//    */
// }


#ifndef STM32_CUBEMX
/* ������ --------------------------------------------------------------------*/
/**
  * ��������: ��ʼ��ESP8266�õ���GPIO����
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  */
void ESP8266_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  ESP8266_RST_GPIO_ClK_ENABLE();

  HAL_GPIO_WritePin(ESP8266_RST_PORT, ESP8266_RST_PIN, GPIO_PIN_RESET);
  GPIO_InitStruct.Pin = ESP8266_RST_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(ESP8266_RST_PORT, &GPIO_InitStruct);
}

void ESP8266__UART_MspInit(GPIO_InitTypeDef GPIO_InitStruct)
{
  ESP8266_USARTx_GPIO_ClK_ENABLE();

  /* �������蹦��GPIO���� */
  GPIO_InitStruct.Pin = ESP8266_USARTx_Tx_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(ESP8266_USARTx_Port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = ESP8266_USARTx_Rx_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(ESP8266_USARTx_Port, &GPIO_InitStruct);

  /* USART1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(ESP8266_USARTx_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(ESP8266_USARTx_IRQn);
}

/**
  * ��������: ��ʼ��ESP8266�õ��� USART
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  */
void ESP8266_UART_Init(void)
{
  /* ��������ʱ��ʹ�� */
  ESP8266_USART_RCC_CLK_ENABLE();

  husartx_esp8266.Instance = ESP8266_USARTx;
  husartx_esp8266.Init.BaudRate = ESP8266_USARTx_BAUDRATE;
  husartx_esp8266.Init.WordLength = UART_WORDLENGTH_8B;
  husartx_esp8266.Init.StopBits = UART_STOPBITS_1;
  husartx_esp8266.Init.Parity = UART_PARITY_NONE;
  husartx_esp8266.Init.Mode = UART_MODE_TX_RX;
  husartx_esp8266.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  husartx_esp8266.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&husartx_esp8266) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  /* ʹ�ܽ��գ������жϻص����� */
  HAL_UART_Receive_IT(&husartx_esp8266, &esp8266_rxdata, 1);
}

#endif

/**
  * ��������: ֹͣʹ��ESP8266
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  */
void ESP8266_Stop(void)
{
  ESP8266_RST_LOW();

  HAL_NVIC_DisableIRQ(ESP8266_USARTx_IRQn);

  ESP8266_USART_RCC_CLK_DISABLE();
}

/**
  * ��������: ����ESP8266ģ��
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������ESP8266_AT_Test����
  */
void ESP8266_Rst(void)
{
#if 0
	 ESP8266_Cmd ( "AT+RST", "OK", "ready", 2500 );
#else
  ESP8266_RST_LOW();
  HAL_Delay(500);
  ESP8266_RST_HIGH();
#endif
}

/**
  * ��������: ��ESP8266ģ�鷢��ATָ��
  * �������: cmd�������͵�ָ��
  *           reply1��reply2���ڴ�����Ӧ��ΪNULL������Ӧ������Ϊ���߼���ϵ
  *           waittime���ȴ���Ӧ��ʱ��
  * �� �� ֵ: 1��ָ��ͳɹ�
  *           0��ָ���ʧ��
  * ˵    ������
  */
bool ESP8266_Cmd(char *cmd, char *reply1, char *reply2, uint32_t waittime)
{
  strEsp8266_Frame_Record.InfBit.FrameLength = 0; //���¿�ʼ�����µ����ݰ�

  ESP8266_Usart("%s\r\n", cmd);

  if ((reply1 == 0) && (reply2 == 0)) //����Ҫ��������
    return true;

  HAL_Delay(waittime); //��ʱ

  strEsp8266_Frame_Record.Data_RX_BUF[strEsp8266_Frame_Record.InfBit.FrameLength] = '\0';

  PC_Usart("\n%s", strEsp8266_Frame_Record.Data_RX_BUF);
  //  printf("%s->%s\n",cmd,strEsp8266_Frame_Record .Data_RX_BUF);
  if ((reply1 != 0) && (reply2 != 0))
    return ((bool)strstr(strEsp8266_Frame_Record.Data_RX_BUF, reply1) ||
            (bool)strstr(strEsp8266_Frame_Record.Data_RX_BUF, reply2));

  else if (reply1 != 0)
    return ((bool)strstr(strEsp8266_Frame_Record.Data_RX_BUF, reply1));

  else
    return ((bool)strstr(strEsp8266_Frame_Record.Data_RX_BUF, reply2));
}

bool ESP8266_NoCmd(char *reply1, char *reply2, uint32_t waittime)
{
  strEsp8266_Frame_Record.InfBit.FrameLength = 0; //���¿�ʼ�����µ����ݰ�

  if ((reply1 == 0) && (reply2 == 0)) //����Ҫ��������
    return true;

  HAL_Delay(waittime); //��ʱ

  strEsp8266_Frame_Record.Data_RX_BUF[strEsp8266_Frame_Record.InfBit.FrameLength] = '\0';

  if (!strEsp8266_Frame_Record.Data_RX_BUF[0] == '\0')
  {
    PC_Usart("\n%s", strEsp8266_Frame_Record.Data_RX_BUF);
  }

  //  printf("%s->%s\n",cmd,strEsp8266_Frame_Record .Data_RX_BUF);
  if ((reply1 != 0) && (reply2 != 0))
    return ((bool)strstr(strEsp8266_Frame_Record.Data_RX_BUF, reply1) ||
            (bool)strstr(strEsp8266_Frame_Record.Data_RX_BUF, reply2));

  else if (reply1 != 0)
    return ((bool)strstr(strEsp8266_Frame_Record.Data_RX_BUF, reply1));

  else
    return ((bool)strstr(strEsp8266_Frame_Record.Data_RX_BUF, reply2));
}

/**
  * ��������: ��ʽ�������������C���е�printf��������û���õ�C��
  * �������: USARTx ����ͨ��������ֻ�õ��˴���2����USART2
  *		        Data   Ҫ���͵����ڵ����ݵ�ָ��
  *			      ...    ��������
  * �� �� ֵ: ��
  * ˵    ��������Ӧ�� USART2_printf( USART2, "\r\n this is a demo \r\n" );
  *            		     USART2_printf( USART2, "\r\n %d \r\n", i );
  *            		     USART2_printf( USART2, "\r\n %s \r\n", j );
  */
void ESP8266_USART_printf(USART_TypeDef *USARTx, char *Data, ...)
{
  unsigned char UsartPrintfBuf[296];
  va_list ap;
  unsigned char *pStr = UsartPrintfBuf;

  va_start(ap, Data);
  vsnprintf((char *)UsartPrintfBuf, sizeof(UsartPrintfBuf), Data, ap); //��ʽ��
  va_end(ap);

  while (*pStr != 0) // �ж��Ƿ񵽴��ַ���������
  {
    HAL_UART_Transmit(&husartx_esp8266, (uint8_t *)pStr++, 1, 0xFF);
    while (__HAL_UART_GET_FLAG(&husartx_esp8266, USART_FLAG_TXE) == 0)
      ;
  }
}

/**
  * ��������: ��ESP8266ģ�����AT��������
  * �������: ��
  * �� �� ֵ: 1��ѡ��ɹ�
  *           0��ѡ��ʧ��
  * ˵    ������
  */
bool ESP8266_AT_Test(void)
{
  char count = 0;

  ESP8266_RST_HIGH();
  HAL_Delay(1000);
  while (count < 10)
  {
    if (ESP8266_Cmd("AT", "OK", NULL, 1000))
      return 1;
    ESP8266_Rst();
    HAL_Delay(1000);
    ++count;
  }
  return 0;
}

/**
  * ��������: ѡ��ESP8266ģ��Ĺ���ģʽ
  * �������: enumMode������ģʽ
  * �� �� ֵ: 1��ѡ��ɹ�
  *           0��ѡ��ʧ��
  * ˵    ������
  */
bool ESP8266_Net_Mode_Choose(ENUM_Net_ModeTypeDef enumMode, ENUM_NetDef_TypeDef enumDef)
{
  bool result = 0;
  char count = 0;
  while (count < 10)
  {
    switch (enumMode)
    {
    case STA:
      if (enumDef)
      {
        result = ESP8266_Cmd("AT+CWMODE_DEF=1", "OK", "no change", 2500);
      }
      else
      {
        result = ESP8266_Cmd("AT+CWMODE=1", "OK", "no change", 2500);
      }
      break;
    case AP:
      if (enumDef)
      {
        result = ESP8266_Cmd("AT+CWMODE_DEF=2", "OK", "no change", 2500);
      }
      else
      {
        result = ESP8266_Cmd("AT+CWMODE=2", "OK", "no change", 2500);
      }
      break;
    case STA_AP:
      if (enumDef)
      {
        result = ESP8266_Cmd("AT+CWMODE_DEF=3", "OK", "no change", 2500);
      }
      else
      {
        result = ESP8266_Cmd("AT+CWMODE=3", "OK", "no change", 2500);
      }
      break;
    default:
      result = false;
      break;
    }
    if (result)
      return result;
    ++count;
  }
  return 0;
}

/**
  * ��������: ESP8266ģ�������ⲿWiFi
  * �������: pSSID��WiFi�����ַ���
  *           pPassWord��WiFi�����ַ���
  * �� �� ֵ: 1�����ӳɹ�
  *           0������ʧ��
  * ˵    ������
  */
bool ESP8266_JoinAP(char *pSSID, char *pPassWord)
{
  char cCmd[120];
  char count = 0;
  sprintf(cCmd, "AT+CWJAP=\"%s\",\"%s\"", pSSID, pPassWord);
  while (count < 10)
  {
    if (ESP8266_Cmd(cCmd, "OK", NULL, 5000))
      return 1;
    ++count;
  }
  return 0;
}

/**
  * ��������: ESP8266ģ�鴴��WiFi�ȵ�
  * �������: pSSID��WiFi�����ַ���
  *           pPassWord��WiFi�����ַ���
  *           enunPsdMode��WiFi���ܷ�ʽ�����ַ���
  * �� �� ֵ: 1�������ɹ�
  *           0������ʧ��
  * ˵    ������
  */
bool ESP8266_BuildAP(char *pSSID, char *pPassWord, ENUM_AP_PsdMode_TypeDef enunPsdMode)
{
  char cCmd[120];
  char count = 0;
  sprintf(cCmd, "AT+CWSAP=\"%s\",\"%s\",1,%d", pSSID, pPassWord, enunPsdMode);
  while (count < 10)
  {
    if (ESP8266_Cmd(cCmd, "OK", 0, 1000))
      return 1;
    ++count;
  }
  return 0;
}

/**
  * ��������: ESP8266ģ������������
  * �������: enumEnUnvarnishTx�������Ƿ������
  * �� �� ֵ: 1�����óɹ�
  *           0������ʧ��
  * ˵    ������
  */
bool ESP8266_Enable_MultipleId(FunctionalState enumEnUnvarnishTx)
{
  char cStr[20];
  char count = 0;
  sprintf(cStr, "AT+CIPMUX=%d", (enumEnUnvarnishTx ? 1 : 0));
  while (count < 10)
  {
    if (ESP8266_Cmd(cStr, "OK", 0, 500))
      return 1;
    ++count;
  }
  return 0;
}

/**
  * ��������: ESP8266ģ�������ⲿ������
  * �������: enumE������Э��
  *           ip��������IP�ַ���
  *           ComNum���������˿��ַ���
  *           id��ģ�����ӷ�������ID
  * �� �� ֵ: 1�����ӳɹ�
  *           0������ʧ��
  * ˵    ������
  */
bool ESP8266_Link_Server(ENUM_NetPro_TypeDef enumE, char *ip, char *ComNum, ENUM_ID_NO_TypeDef id)
{
  char cStr[100] = {0}, cCmd[120];

  switch (enumE)
  {
  case enumTCP:
    sprintf(cStr, "\"%s\",\"%s\",%s", "TCP", ip, ComNum);
    break;

  case enumUDP:
    sprintf(cStr, "\"%s\",\"%s\",%s", "UDP", ip, ComNum);
    break;

  default:
    break;
  }

  if (id < 5)
    sprintf(cCmd, "AT+CIPSTART=%d,%s", id, cStr);

  else
    sprintf(cCmd, "AT+CIPSTART=%s", cStr);

  return ESP8266_Cmd(cCmd, "OK", "ALREAY CONNECT", 4000);
}

/**
  * ��������: ESP8266ģ�鿪����رշ�����ģʽ
  * �������: enumMode������/�ر�
  *           pPortNum���������˿ں��ַ���
  *           pTimeOver����������ʱʱ���ַ�������λ����
  * �� �� ֵ: 1�������ɹ�
  *           0������ʧ��
  * ˵    ������
  */
bool ESP8266_StartOrShutServer(FunctionalState enumMode, char *pPortNum, char *pTimeOver)
{
  char cCmd1[120], cCmd2[120];

  if (enumMode)
  {
    sprintf(cCmd1, "AT+CIPSERVER=%d,%s", 1, pPortNum);

    sprintf(cCmd2, "AT+CIPSTO=%s", pTimeOver);

    return (ESP8266_Cmd(cCmd1, "OK", 0, 500) &&
            ESP8266_Cmd(cCmd2, "OK", 0, 500));
  }

  else
  {
    sprintf(cCmd1, "AT+CIPSERVER=%d,%s", 0, pPortNum);

    return ESP8266_Cmd(cCmd1, "OK", 0, 500);
  }
}

/**
  * ��������: ��ȡESP8266 ������״̬�����ʺϵ��˿�ʱʹ��
  * �������: ��
  * �� �� ֵ: 2�����ip
  *           3����������
  *           4��ʧȥ����
  *           0����ȡ״̬ʧ��
  * ˵    ������
  */
uint8_t ESP8266_Get_LinkStatus(void)
{
  if (ESP8266_Cmd("AT+CIPSTATUS", "OK", 0, 500))
  {
    if (strstr(strEsp8266_Frame_Record.Data_RX_BUF, "STATUS:2\r\n"))
      return 2;

    else if (strstr(strEsp8266_Frame_Record.Data_RX_BUF, "STATUS:3\r\n"))
      return 3;

    else if (strstr(strEsp8266_Frame_Record.Data_RX_BUF, "STATUS:4\r\n"))
      return 4;
  }
  return 0;
}

/**
  * ��������: ��ȡESP8266 �Ķ˿ڣ�Id������״̬�����ʺ϶�˿�ʱʹ��
  * �������: ��
  * �� �� ֵ: �˿ڣ�Id��������״̬����5λΪ��Чλ���ֱ��ӦId5~0��ĳλ����1���Id���������ӣ�������0���Idδ��������
  * ˵    ������
  */
uint8_t ESP8266_Get_IdLinkStatus(void)
{
  uint8_t ucIdLinkStatus = 0x00;

  if (ESP8266_Cmd("AT+CIPSTATUS", "OK", 0, 500))
  {
    if (strstr(strEsp8266_Frame_Record.Data_RX_BUF, "+CIPSTATUS:0,"))
      ucIdLinkStatus |= 0x01;
    else
      ucIdLinkStatus &= ~0x01;

    if (strstr(strEsp8266_Frame_Record.Data_RX_BUF, "+CIPSTATUS:1,"))
      ucIdLinkStatus |= 0x02;
    else
      ucIdLinkStatus &= ~0x02;

    if (strstr(strEsp8266_Frame_Record.Data_RX_BUF, "+CIPSTATUS:2,"))
      ucIdLinkStatus |= 0x04;
    else
      ucIdLinkStatus &= ~0x04;

    if (strstr(strEsp8266_Frame_Record.Data_RX_BUF, "+CIPSTATUS:3,"))
      ucIdLinkStatus |= 0x08;
    else
      ucIdLinkStatus &= ~0x08;

    if (strstr(strEsp8266_Frame_Record.Data_RX_BUF, "+CIPSTATUS:4,"))
      ucIdLinkStatus |= 0x10;
    else
      ucIdLinkStatus &= ~0x10;
  }
  return ucIdLinkStatus;
}

/**
  * ��������: ��ȡESP8266 �� AP IP
  * �������: pApIp����� AP IP ��������׵�ַ
  *           ucArrayLength����� AP IP ������ĳ���
  * �� �� ֵ: 1����ȡ�ɹ�
  *           0����ȡʧ��
  * ˵    ������
  */
uint8_t ESP8266_Inquire_ApIp(char *pApIp, uint8_t ucArrayLength)
{
  char uc;

  char *pCh;

  ESP8266_Cmd("AT+CIFSR", "OK", 0, 500);

  pCh = strstr(strEsp8266_Frame_Record.Data_RX_BUF, "APIP,\"");

  if (pCh)
    pCh += 6;

  else
    return 0;

  for (uc = 0; uc < ucArrayLength; uc++)
  {
    pApIp[uc] = *(pCh + uc);

    if (pApIp[uc] == '\"')
    {
      pApIp[uc] = '\0';
      break;
    }
  }
  return 1;
}

/**
  * ��������: ����ESP8266ģ�����͸������
  * �������: ��
  * �� �� ֵ: 1�����óɹ�
  *           0������ʧ��
  * ˵    ������
  */
bool ESP8266_UnvarnishSend(void)
{

  if (!ESP8266_Cmd("AT+CIPMODE=1", "OK", 0, 1000))
    return false;
  return ESP8266_Cmd("AT+CIPSEND", "OK", ">", 1000);
}

/**
  * ��������: ����ESP8266ģ���˳�͸��ģʽ
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  */
void ESP8266_ExitUnvarnishSend(void)
{
  HAL_Delay(1000);
  ESP8266_Usart("+++");
  HAL_Delay(500);
}

/**
  * ��������: ESP8266ģ�鷢���ַ���
  * �������: enumEnUnvarnishTx�������Ƿ���ʹ����͸��ģʽ
  *           pStr��Ҫ���͵��ַ���
  *           ucId���ĸ�ID���͵��ַ���
  *           ulStrLength��Ҫ���͵��ַ������ֽ���
  * �� �� ֵ: 1�����ͳɹ�
  *           0������ʧ��
  * ˵    ������
  */
bool ESP8266_SendString(FunctionalState enumEnUnvarnishTx, char *pStr, uint32_t ulStrLength, ENUM_ID_NO_TypeDef ucId)
{
  char cStr[20];
  bool bRet = false;

  if (enumEnUnvarnishTx)
  {
    ESP8266_Usart("%s", pStr);

    bRet = true;
  }

  else
  {
    if (ucId < 5)
      sprintf(cStr, "AT+CIPSEND=%d,%d", ucId, ulStrLength + 2);

    else
      sprintf(cStr, "AT+CIPSEND=%d", ulStrLength + 2);

    ESP8266_Cmd(cStr, "> ", 0, 1000);

    bRet = ESP8266_Cmd(pStr, "SEND OK", 0, 1000);
  }

  return bRet;
}

/**
  * ��������: ESP8266ģ������ַ���
  * �������: enumEnUnvarnishTx�������Ƿ���ʹ����͸��ģʽ
  * �� �� ֵ: ���յ����ַ����׵�ַ
  * ˵    ������
  */
char *ESP8266_ReceiveString(FunctionalState enumEnUnvarnishTx)
{
  char *pRecStr = 0;

  strEsp8266_Frame_Record.InfBit.FrameLength = 0;
  strEsp8266_Frame_Record.InfBit.FrameFinishFlag = 0;

  while (!strEsp8266_Frame_Record.InfBit.FrameFinishFlag)
    ;
  strEsp8266_Frame_Record.Data_RX_BUF[strEsp8266_Frame_Record.InfBit.FrameLength] = '\0';

  if (enumEnUnvarnishTx)
    pRecStr = strEsp8266_Frame_Record.Data_RX_BUF;
  else
  {
    if (strstr(strEsp8266_Frame_Record.Data_RX_BUF, "+IPD"))
      pRecStr = strEsp8266_Frame_Record.Data_RX_BUF;
  }
  return pRecStr;
}

bool ESP8266_AutoConnect_STA(void)
{
  char cCmd[20];
  char count = 0;
  sprintf(cCmd, "AT+CWAUTOCONN=1");
  while (count < 10)
  {
    if (ESP8266_Cmd(cCmd, "OK", NULL, 5000))
      return 1;
    ++count;
  }
  return 0;
}

bool ESP8266_SmartConfig_Start(void)
{
  char cCmd[25];
  char count = 0;
  sprintf(cCmd, "AT+CWSTARTSMART=3");
  while (count < 10)
  {
    if (ESP8266_Cmd(cCmd, "OK", NULL, 3000))
      return 1;
    ++count;
  }
  return 0;
}

bool ESP8266_SmartConfig_Stop(void)
{
  char cCmd[25];
  char count = 0;
  sprintf(cCmd, "AT+CWSTOPSMART");
  while (count < 10)
  {
    if (ESP8266_Cmd(cCmd, "OK", NULL, 3000))
      return 1;
    ++count;
  }
  return 0;
}

/**
  * ��������: ESP8266ģ����������
  * �������: ��
  * �� �� ֵ: 1�����ͳɹ�
  *          0������ʧ��
  * ˵    ������
  */
bool ESP8266_SmartConfig(void)
{
  uint8_t cnt = 0;
  printf("\n������������ ESP8266 ......\n");

  if (ESP8266_AT_Test())
  {
    printf("\nAT test OK\n");
  }
  printf("\n< 1 >\n");
  if (ESP8266_Net_Mode_Choose(STA, DEF))
  {
    printf("\nESP8266_Net_Mode_Choose OK\n");
  }
  printf("\n< 2 >\n");
  if (ESP8266_AutoConnect_STA())
  {
    printf("\nESP8266_AutoConnect_STA OK\n");
  }
  printf("\n< 3 >\n");
  ESP8266_Enable_MultipleId(DISABLE);
  while (!ESP8266_SmartConfig_Start())
    ;
  printf("\nESP8266_SmartConfig_Start \n");
  printf("\n< 4 >\n");
  for (cnt = 0; cnt < 120; cnt++)
  {
    if (ESP8266_NoCmd("connected", NULL, 1000))
      break;
  }
  if (cnt == 120)
  {
    printf("\nESP8266_SmartConfig TimeOut \n");
    while (!ESP8266_SmartConfig_Stop())
    ;
    return 0;
  }
  // ucSmartConfigStartFlag =1;
  while (!ESP8266_SmartConfig_Stop())
    ;

  while (!ESP8266_Get_LinkStatus())
    ;
  printf("\n< 5 >\n");
  printf("\n�������� ESP8266 ���\n");
  return 1;
}
