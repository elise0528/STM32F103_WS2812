/* 包含头文件 ----------------------------------------------------------------*/
#include "bsp_esp8266.h"

/* 私有类型定义 --------------------------------------------------------------*/
/* 私有宏定义 ----------------------------------------------------------------*/
/* 私有变量 ------------------------------------------------------------------*/
#ifdef STM32_CUBEMX
extern UART_HandleTypeDef husartx_esp8266;
#else
UART_HandleTypeDef husartx_esp8266;
#endif

STRUCT_USARTx_Frame strEsp8266_Frame_Record = {0};
uint8_t esp8266_rxdata;
uint8_t ucSmartConfigFlag = 0;
/* 扩展变量 ------------------------------------------------------------------*/
/* 私有函数原形 --------------------------------------------------------------*/

/**
 * 注意用STM32Cubemx生成初始化工程后 
 * 1.在mina.c中添加 HAL_UART_RxCpltCallback回调函数
 * 2.在main主函数中添加 HAL_UART_Receive_IT函数 开启接收中断
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
//     if (strEsp8266_Frame_Record.InfBit.FrameLength < (RX_BUF_MAX_LEN - 1)) //预留1个字节写结束符
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
/* 函数体 --------------------------------------------------------------------*/
/**
  * 函数功能: 初始化ESP8266用到的GPIO引脚
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
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

  /* 串口外设功能GPIO配置 */
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
  * 函数功能: 初始化ESP8266用到的 USART
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
void ESP8266_UART_Init(void)
{
  /* 串口外设时钟使能 */
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

  /* 使能接收，进入中断回调函数 */
  HAL_UART_Receive_IT(&husartx_esp8266, &esp8266_rxdata, 1);
}

#endif

/**
  * 函数功能: 停止使用ESP8266
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
void ESP8266_Stop(void)
{
  ESP8266_RST_LOW();

  HAL_NVIC_DisableIRQ(ESP8266_USARTx_IRQn);

  ESP8266_USART_RCC_CLK_DISABLE();
}

/**
  * 函数功能: 重启ESP8266模块
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：被ESP8266_AT_Test调用
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
  * 函数功能: 对ESP8266模块发送AT指令
  * 输入参数: cmd，待发送的指令
  *           reply1，reply2，期待的响应，为NULL表不需响应，两者为或逻辑关系
  *           waittime，等待响应的时间
  * 返 回 值: 1，指令发送成功
  *           0，指令发送失败
  * 说    明：无
  */
bool ESP8266_Cmd(char *cmd, char *reply1, char *reply2, uint32_t waittime)
{
  strEsp8266_Frame_Record.InfBit.FrameLength = 0; //从新开始接收新的数据包

  ESP8266_Usart("%s\r\n", cmd);

  if ((reply1 == 0) && (reply2 == 0)) //不需要接收数据
    return true;

  HAL_Delay(waittime); //延时

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
  strEsp8266_Frame_Record.InfBit.FrameLength = 0; //从新开始接收新的数据包

  if ((reply1 == 0) && (reply2 == 0)) //不需要接收数据
    return true;

  HAL_Delay(waittime); //延时

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
  * 函数功能: 格式化输出，类似于C库中的printf，但这里没有用到C库
  * 输入参数: USARTx 串口通道，这里只用到了串口2，即USART2
  *		        Data   要发送到串口的内容的指针
  *			      ...    其他参数
  * 返 回 值: 无
  * 说    明：典型应用 USART2_printf( USART2, "\r\n this is a demo \r\n" );
  *            		     USART2_printf( USART2, "\r\n %d \r\n", i );
  *            		     USART2_printf( USART2, "\r\n %s \r\n", j );
  */
void ESP8266_USART_printf(USART_TypeDef *USARTx, char *Data, ...)
{
  unsigned char UsartPrintfBuf[296];
  va_list ap;
  unsigned char *pStr = UsartPrintfBuf;

  va_start(ap, Data);
  vsnprintf((char *)UsartPrintfBuf, sizeof(UsartPrintfBuf), Data, ap); //格式化
  va_end(ap);

  while (*pStr != 0) // 判断是否到达字符串结束符
  {
    HAL_UART_Transmit(&husartx_esp8266, (uint8_t *)pStr++, 1, 0xFF);
    while (__HAL_UART_GET_FLAG(&husartx_esp8266, USART_FLAG_TXE) == 0)
      ;
  }
}

/**
  * 函数功能: 对ESP8266模块进行AT测试启动
  * 输入参数: 无
  * 返 回 值: 1，选择成功
  *           0，选择失败
  * 说    明：无
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
  * 函数功能: 选择ESP8266模块的工作模式
  * 输入参数: enumMode，工作模式
  * 返 回 值: 1，选择成功
  *           0，选择失败
  * 说    明：无
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
  * 函数功能: ESP8266模块连接外部WiFi
  * 输入参数: pSSID，WiFi名称字符串
  *           pPassWord，WiFi密码字符串
  * 返 回 值: 1，连接成功
  *           0，连接失败
  * 说    明：无
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
  * 函数功能: ESP8266模块创建WiFi热点
  * 输入参数: pSSID，WiFi名称字符串
  *           pPassWord，WiFi密码字符串
  *           enunPsdMode，WiFi加密方式代号字符串
  * 返 回 值: 1，创建成功
  *           0，创建失败
  * 说    明：无
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
  * 函数功能: ESP8266模块启动多连接
  * 输入参数: enumEnUnvarnishTx，配置是否多连接
  * 返 回 值: 1，配置成功
  *           0，配置失败
  * 说    明：无
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
  * 函数功能: ESP8266模块连接外部服务器
  * 输入参数: enumE，网络协议
  *           ip，服务器IP字符串
  *           ComNum，服务器端口字符串
  *           id，模块连接服务器的ID
  * 返 回 值: 1，连接成功
  *           0，连接失败
  * 说    明：无
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
  * 函数功能: ESP8266模块开启或关闭服务器模式
  * 输入参数: enumMode，开启/关闭
  *           pPortNum，服务器端口号字符串
  *           pTimeOver，服务器超时时间字符串，单位：秒
  * 返 回 值: 1，操作成功
  *           0，操作失败
  * 说    明：无
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
  * 函数功能: 获取ESP8266 的连接状态，较适合单端口时使用
  * 输入参数: 无
  * 返 回 值: 2，获得ip
  *           3，建立连接
  *           4，失去连接
  *           0，获取状态失败
  * 说    明：无
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
  * 函数功能: 获取ESP8266 的端口（Id）连接状态，较适合多端口时使用
  * 输入参数: 无
  * 返 回 值: 端口（Id）的连接状态，低5位为有效位，分别对应Id5~0，某位若置1表该Id建立了连接，若被清0表该Id未建立连接
  * 说    明：无
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
  * 函数功能: 获取ESP8266 的 AP IP
  * 输入参数: pApIp，存放 AP IP 的数组的首地址
  *           ucArrayLength，存放 AP IP 的数组的长度
  * 返 回 值: 1，获取成功
  *           0，获取失败
  * 说    明：无
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
  * 函数功能: 配置ESP8266模块进入透传发送
  * 输入参数: 无
  * 返 回 值: 1，配置成功
  *           0，配置失败
  * 说    明：无
  */
bool ESP8266_UnvarnishSend(void)
{

  if (!ESP8266_Cmd("AT+CIPMODE=1", "OK", 0, 1000))
    return false;
  return ESP8266_Cmd("AT+CIPSEND", "OK", ">", 1000);
}

/**
  * 函数功能: 配置ESP8266模块退出透传模式
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
void ESP8266_ExitUnvarnishSend(void)
{
  HAL_Delay(1000);
  ESP8266_Usart("+++");
  HAL_Delay(500);
}

/**
  * 函数功能: ESP8266模块发送字符串
  * 输入参数: enumEnUnvarnishTx，声明是否已使能了透传模式
  *           pStr，要发送的字符串
  *           ucId，哪个ID发送的字符串
  *           ulStrLength，要发送的字符串的字节数
  * 返 回 值: 1，发送成功
  *           0，发送失败
  * 说    明：无
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
  * 函数功能: ESP8266模块接收字符串
  * 输入参数: enumEnUnvarnishTx，声明是否已使能了透传模式
  * 返 回 值: 接收到的字符串首地址
  * 说    明：无
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
  * 函数功能: ESP8266模块智能配网
  * 输入参数: 无
  * 返 回 值: 1，发送成功
  *          0，发送失败
  * 说    明：无
  */
bool ESP8266_SmartConfig(void)
{
  uint8_t cnt = 0;
  printf("\n正在智能配网 ESP8266 ......\n");

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
  printf("\n智能配网 ESP8266 完毕\n");
  return 1;
}
