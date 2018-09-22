#ifndef __BSP_KEY_H__
#define __BSP_KEY_H__

/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* 类型定义 --------------------------------------------------------------*/
typedef enum
{
    KEY_UP = 0,
    KEY_DOWN = 1,
} ENUM_KEYState_TypeDef;

/* 宏定义 --------------------------------------------------------------------*/
#define KEY_RCC_CLK_ENABLE __HAL_RCC_GPIOB_CLK_ENABLE
#define KEY_GPIO_PIN GPIO_PIN_3
#define KEY_GPIO GPIOB
#define KEY_DOWN_LEVEL 1 /* 根据原理图设计，KEY按下时引脚为高电平，所以这里设置为1 */
#define KEY_EXTI_IRQn EXTI0_IRQn
#define KEY_EXTI_IRQHandler EXTI0_IRQHandler

/* 扩展变量 ------------------------------------------------------------------*/
/* 函数声明 ------------------------------------------------------------------*/
#ifndef STM32_CUBEMX
void KEY_GPIO_Init(void);
#endif
ENUM_KEYState_TypeDef KEY_StateRead(void);

#endif // __BSP_KEY_H__

/******************************  *****END OF FILE**** ******************************/
