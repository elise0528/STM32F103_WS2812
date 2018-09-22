/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "bsp_key.h"

/* ˽�����Ͷ��� --------------------------------------------------------------*/
/* ˽�к궨�� ----------------------------------------------------------------*/
/* ˽�б��� ------------------------------------------------------------------*/
/* ��չ���� ------------------------------------------------------------------*/
/* ˽�к���ԭ�� --------------------------------------------------------------*/
/* ������ --------------------------------------------------------------------*/

#ifndef STM32_CUBEMX
/**
  * ��������: ���ذ���IO���ų�ʼ��.
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ����ʹ�ú궨�巽������������źţ����������ֲ��ֻҪ���޸�bsp_key.h
  *           �ļ���غ궨��Ϳ��Է����޸����š�
  */
void KEY_GPIO_Init(void)
{
    /* ����IOӲ����ʼ���ṹ����� */
    GPIO_InitTypeDef GPIO_InitStruct;

    /* ʹ��(����)KEY���Ŷ�ӦIO�˿�ʱ�� */
    KEY_RCC_CLK_ENABLE();
    KEY2_RCC_CLK_ENABLE();

    /* ����KEY GPIO:��������ģʽ */
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
  * ��������: ��ȡ����KEY��״̬
  * �����������
  * �� �� ֵ: KEY_DOWN�����������£�
  *           KEY_UP  ������û������
  * ˵    ������ѯɨ��ʽ��
  */
ENUM_KEYState_TypeDef KEY_StateRead(void)
{
    /* ��ȡ��ʱ����ֵ���ж��Ƿ��Ǳ�����״̬������Ǳ�����״̬���뺯���� */
    if (HAL_GPIO_ReadPin(KEY_GPIO, KEY_GPIO_PIN) == KEY_DOWN_LEVEL)
    {
        /* ��ʱһС��ʱ�䣬�������� */
        HAL_Delay(10);
        /* ��ʱʱ��������жϰ���״̬��������ǰ���״̬˵������ȷʵ������ */
        if (HAL_GPIO_ReadPin(KEY_GPIO, KEY_GPIO_PIN) == KEY_DOWN_LEVEL)
        {
            /* �ȴ������������˳�����ɨ�躯�� */
            while (HAL_GPIO_ReadPin(KEY_GPIO, KEY_GPIO_PIN) == KEY_DOWN_LEVEL)
                ;
            /* ����ɨ����ϣ�ȷ�����������£����ذ���������״̬ */
            return KEY_DOWN;
        }
    }
    /* ����û�����£�����û������״̬ */
    return KEY_UP;
}

/**
  * ��������: �����ⲿ�жϷ�����
  * �������: GPIO_Pin���ж�����
  * �� �� ֵ: ��
  * ˵    ��: �жϴ���ʽ��
  */
/*
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if(GPIO_Pin==KEY1_GPIO_PIN)
  {
    HAL_Delay(20);  //��ʱһС��ʱ�䣬��������
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
