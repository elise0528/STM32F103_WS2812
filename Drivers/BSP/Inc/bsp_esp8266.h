#ifndef __BSP_ESP8266_H__
#define __BSP_ESP8266_H__

/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_usart.h"

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>

#if defined(__CC_ARM)
#pragma anon_unions //启用对匿名结构和联合的支持
#endif
/* 类型定义 ------------------------------------------------------------------*/
/******************************* ESP8266 数据类型定义 ***************************/
typedef enum
{
    STA,
    AP,
    STA_AP
} ENUM_Net_ModeTypeDef;

typedef enum
{
    enumTCP,
    enumUDP,
} ENUM_NetPro_TypeDef;

typedef enum
{
    NO_DEF,
    DEF
} ENUM_NetDef_TypeDef;

typedef enum
{
    NO_CMD,
    CMD
} ENUM_Cmd_TypeDef;

typedef enum
{
    Multiple_ID_0 = 0,
    Multiple_ID_1 = 1,
    Multiple_ID_2 = 2,
    Multiple_ID_3 = 3,
    Multiple_ID_4 = 4,
    Single_ID_0 = 5
} ENUM_ID_NO_TypeDef;

typedef enum
{
    OPEN = 0,
    WEP = 1,
    WPA_PSK = 2,
    WPA2_PSK = 3,
    WPA_WPA2_PSK = 4
} ENUM_AP_PsdMode_TypeDef;

#define RX_BUF_MAX_LEN 1024 //最大接收缓存字节数

typedef struct //串口数据帧的处理结构体
{
    char Data_RX_BUF[RX_BUF_MAX_LEN];
    union {
        __IO uint16_t InfAll;
        struct
        {
            __IO uint16_t FrameLength : 15;    // 14:0
            __IO uint16_t FrameFinishFlag : 1; // 15
        } InfBit;
    };
} STRUCT_USARTx_Frame;

/* 宏定义 --------------------------------------------------------------------*/

/******************************** ESP8266 连接引脚定义 ***********************************/
#define ESP8266_USARTx USART2
#define husartx_esp8266 huart2
#define ESP8266_USARTx_BAUDRATE 115200
#define ESP8266_USART_RCC_CLK_ENABLE() __HAL_RCC_USART2_CLK_ENABLE()
#define ESP8266_USART_RCC_CLK_DISABLE() __HAL_RCC_USART2_CLK_DISABLE()

#define ESP8266_USARTx_GPIO_ClK_ENABLE() __HAL_RCC_GPIOA_CLK_ENABLE()
#define ESP8266_USARTx_Port GPIOA
#define ESP8266_USARTx_Tx_PIN GPIO_PIN_2
#define ESP8266_USARTx_Rx_PIN GPIO_PIN_3

#define ESP8266_USARTx_IRQHANDLER USART2_IRQHandler
#define ESP8266_USARTx_IRQn USART2_IRQn

#define ESP8266_RST_GPIO_ClK_ENABLE() __HAL_RCC_GPIOD_CLK_ENABLE()
#define ESP8266_RST_PORT GPIOD
#define ESP8266_RST_PIN GPIO_PIN_6
#define ESP8266_RST_HIGH() HAL_GPIO_WritePin(ESP8266_RST_PORT, ESP8266_RST_PIN, GPIO_PIN_SET)
#define ESP8266_RST_LOW() HAL_GPIO_WritePin(ESP8266_RST_PORT, ESP8266_RST_PIN, GPIO_PIN_RESET)

/*********************************************** ESP8266 函数宏定义 *******************************************/
#define ESP8266_Usart(fmt, ...) ESP8266_USART_printf(ESP8266_USARTx, fmt, ##__VA_ARGS__)
#define PC_Usart(fmt, ...) printf(fmt, ##__VA_ARGS__)
//#define PC_Usart( fmt, ... )

/* 扩展变量 ------------------------------------------------------------------*/
extern UART_HandleTypeDef husartx_esp8266;
extern STRUCT_USARTx_Frame strEsp8266_Frame_Record;
extern uint8_t esp8266_rxdata;
extern uint8_t ucSmartConfigFlag;

/* 函数声明 ------------------------------------------------------------------*/
#ifndef STM32_CUBEMX
void ESP8266_GPIO_Init(void);
void ESP8266_UART_MspInit(GPIO_InitTypeDef GPIO_InitStruct);
void ESP8266_UART_Init(void);
#endif 
void ESP8266_Stop(void);
void ESP8266_Rst(void);
bool ESP8266_Cmd(char *cmd, char *reply1, char *reply2, uint32_t waittime);
bool  ESP8266_AT_Test( void );
bool ESP8266_Net_Mode_Choose(ENUM_Net_ModeTypeDef enumMode, ENUM_NetDef_TypeDef enumDef);
bool  ESP8266_JoinAP( char * pSSID, char * pPassWord );
bool  ESP8266_BuildAP( char * pSSID, char * pPassWord, ENUM_AP_PsdMode_TypeDef enunPsdMode );
bool  ESP8266_Enable_MultipleId( FunctionalState enumEnUnvarnishTx );
bool  ESP8266_Link_Server( ENUM_NetPro_TypeDef enumE, char * ip, char * ComNum, ENUM_ID_NO_TypeDef id);
bool  ESP8266_StartOrShutServer( FunctionalState enumMode, char * pPortNum, char * pTimeOver );
uint8_t ESP8266_Get_LinkStatus( void );
uint8_t ESP8266_Get_IdLinkStatus( void );
uint8_t ESP8266_Inquire_ApIp( char * pApIp, uint8_t ucArrayLength );
bool ESP8266_UnvarnishSend( void );
void ESP8266_ExitUnvarnishSend( void );
bool ESP8266_SendString( FunctionalState enumEnUnvarnishTx, char * pStr, uint32_t ulStrLength, ENUM_ID_NO_TypeDef ucId );
char * ESP8266_ReceiveString( FunctionalState enumEnUnvarnishTx );

bool ESP8266_SmartConfig(void);

void ESP8266_USART_printf(USART_TypeDef *USARTx, char *Data, ...);

#endif

/******************************  *****END OF FILE**** ******************************/
