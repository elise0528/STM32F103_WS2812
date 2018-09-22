/* 包含头文件 ----------------------------------------------------------------*/
#include "bsp_key.h"

/* 私有类型定义 --------------------------------------------------------------*/
/* 私有宏定义 ----------------------------------------------------------------*/
/* 私有变量 ------------------------------------------------------------------*/
/* 扩展变量 ------------------------------------------------------------------*/
/* 私有函数原形 --------------------------------------------------------------*/
/* 函数体 --------------------------------------------------------------------*/

#ifndef STM32_CUBEMX
/**
  * 函数功能: 板载按键IO引脚初始化.
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：使用宏定义方法代替具体引脚号，方便程序移植，只要简单修改bsp_key.h
  *           文件相关宏定义就可以方便修改引脚。
  */
void KEY_GPIO_Init(void)
{
    /* 定义IO硬件初始化结构体变量 */
    GPIO_InitTypeDef GPIO_InitStruct;

    /* 使能(开启)KEY引脚对应IO端口时钟 */
    KEY_RCC_CLK_ENABLE();
    KEY2_RCC_CLK_ENABLE();

    /* 配置KEY GPIO:输入下拉模式 */
    GPIO_InitStruct.Pin = KEY_GPIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    HAL_GPIO_Init(KEY_GPIO, &GPIO_InitStruct);

    /* EXTI interrupt init*/
    HAL_NVIC_SetPriority(KEY_EXTI_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(KEY_EXTI_IRQn);
}
#endif

/**
  * 函数功能: 读取按键KEY的状态
  * 输入参数：无
  * 返 回 值: KEY_DOWN：按键被按下；
  *           KEY_UP  ：按键没被按下
  * 说    明：轮询扫描式。
  */
ENUM_KEYState_TypeDef KEY_StateRead(void)
{
    /* 读取此时按键值并判断是否是被按下状态，如果是被按下状态进入函数内 */
    if (HAL_GPIO_ReadPin(KEY_GPIO, KEY_GPIO_PIN) == KEY_DOWN_LEVEL)
    {
        /* 延时一小段时间，消除抖动 */
        HAL_Delay(10);
        /* 延时时间后再来判断按键状态，如果还是按下状态说明按键确实被按下 */
        if (HAL_GPIO_ReadPin(KEY_GPIO, KEY_GPIO_PIN) == KEY_DOWN_LEVEL)
        {
            /* 等待按键弹开才退出按键扫描函数 */
            while (HAL_GPIO_ReadPin(KEY_GPIO, KEY_GPIO_PIN) == KEY_DOWN_LEVEL)
                ;
            /* 按键扫描完毕，确定按键被按下，返回按键被按下状态 */
            return KEY_DOWN;
        }
    }
    /* 按键没被按下，返回没被按下状态 */
    return KEY_UP;
}

/**
  * 函数功能: 按键外部中断服务函数
  * 输入参数: GPIO_Pin：中断引脚
  * 返 回 值: 无
  * 说    明: 中断触发式。
  */
/*
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if(GPIO_Pin==KEY1_GPIO_PIN)
  {
    HAL_Delay(20);  //延时一小段时间，消除抖动
    if(HAL_GPIO_ReadPin(KEY1_GPIO,KEY1_GPIO_PIN)==KEY1_DOWN_LEVEL)
    {
      BEEP_TOGGLE;
      LED1_ON;
      LED2_ON;
      LED3_ON;
    }
    __HAL_GPIO_EXTI_CLEAR_IT(KEY1_GPIO_PIN);
  }

}
*/

/******************************  *****END OF FILE**** ******************************/
