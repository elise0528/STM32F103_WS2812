#include "bsp_ws2812.h"

#ifdef PWM_WS2812	

TIM_HandleTypeDef htim4;
DMA_HandleTypeDef hdma_tim4_ch3;


uint16_t RGB_buffer[3 + 24 * LED_NUMBER + 240] = {0};


void WS2812_DataInit(void)
{
    u8 i = 0;
    u8 j = 0;
    u32 rgb_value = 0;
    for (j = 0; j < LED_NUMBER; ++j)
    {
        for (i = 0; i < 24; ++i)
        {
            if ((rgb_value << i) & 0x800000) //高位先发，此时高位为1时
            {
                RGB_buffer[24 * j + i + 3] = 59; //68%占空比
            }
            else
            {
                RGB_buffer[24 * j + i + 3] = 32; //32%占空比
            }
        }
    }
}

//WS2812发送24位数据
void WS2812_Send24Bit(u8 red, u8 green, u8 blue, u8 n)
{
    u8 i = 0;
    u32 rgb_value = green << 16 | red << 8 | blue;
    for (i = 0; i < 24; ++i)
    {
        if ((rgb_value << i) & 0x800000) //高位先发，此时高位为1时
        {
            RGB_buffer[24 * n + i + 3] = 67; //68%占空比
        }
        else
        {
            RGB_buffer[24 * n + i + 3] = 22; //32%占空比
        }
    }
}

void WS2812_setAllPixelColor(uint8_t r, uint8_t g, uint8_t b)
{
    u8 i = 0;
    while (HAL_DMA_GetState(&hdma_tim4_ch3) == HAL_DMA_STATE_BUSY);
    __HAL_DMA_CLEAR_FLAG(&hdma_tim4_ch3, DMA_FLAG_TC5);
    __HAL_DMA_DISABLE(&hdma_tim4_ch3);
    for (i = 0; i < LED_NUMBER; ++i)
    {
        WS2812_Send24Bit(r, g, b, i);
    }
    HAL_TIM_PWM_Start_DMA(&htim4, TIM_CHANNEL_3, (uint32_t *)&RGB_buffer, sizeof(RGB_buffer) / sizeof(RGB_buffer[0]));
}

void WS2812_setPixelColor(uint8_t n, uint8_t r, uint8_t g, uint8_t b)
{
    u8 i = 0;
    while (HAL_DMA_GetState(&hdma_tim4_ch3) == HAL_DMA_STATE_BUSY);
    __HAL_DMA_CLEAR_FLAG(&hdma_tim4_ch3, DMA_FLAG_TC5);
    __HAL_DMA_DISABLE(&hdma_tim4_ch3);
    for (i = 0; i < LED_NUMBER; i++)
    {
        if (i != n)
        {
            //WS2812_Send24Bit(0, 0, 0, i);
        }
        else
        {
            WS2812_Send24Bit(r, g, b, n);
        }
    }
    HAL_TIM_PWM_Start_DMA(&htim4, TIM_CHANNEL_3, (uint32_t *)&RGB_buffer, sizeof(RGB_buffer) / sizeof(RGB_buffer[0]));
}

#endif

#ifdef SPI_WS2812



#endif

void WS2812_SetPixelColor(uint8_t n, uint32_t c)
{
    uint8_t r, b, g;
    r = (uint8_t)(c >> 16);
    b = (uint8_t)(c >> 8);
    g = (uint8_t)c;
    WS2812_setPixelColor(n, r, g, b);
}


uint32_t WS2812_Color(uint8_t r, uint8_t g, uint8_t b)
{
    return ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
}

uint32_t WS2812_Wheel(uint8_t WheelPos)
{
    WheelPos = 255 - WheelPos;
    if (WheelPos < 85)
    {
        return WS2812_Color(255 - WheelPos * 3, 0, WheelPos * 3);
    }
    if (WheelPos < 170)
    {
        WheelPos -= 85;
        return WS2812_Color(0, WheelPos * 3, 255 - WheelPos * 3);
    }
    WheelPos -= 170;
    return WS2812_Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

void WS2812_rainbow(uint32_t wait)
{
    uint16_t i, j;

    for (j = 0; j < 256; j++)
    {
        for (i = 0; i < LED_NUMBER; i++)
        {
            WS2812_SetPixelColor(i, WS2812_Wheel((i + j) & 255));
        }
        HAL_Delay(wait);
    }
}

// Slightly different, this makes the rainbow equally distributed throughout
void WS2812_rainbowCycle(uint8_t wait)
{
    uint16_t i, j;

    for (j = 0; j < 256 * 5; j++)
    {
        // 5 cycles of all colors on wheel
        for (i = 0; i < LED_NUMBER; i++)
        {
            WS2812_SetPixelColor(i, WS2812_Wheel(((i * 256 / LED_NUMBER) + j) & 255));
        }

        HAL_Delay(wait);
    }
}
//Theatre-style crawling lights.o??üμ?
void WS2812_theaterChase(uint32_t c, uint8_t wait)
{
    for (int j = 0; j < 10; j++)
    {
        //do 10 cycles of chasing
        for (int q = 0; q < 3; q++)
        {
            for (uint16_t i = 0; i < LED_NUMBER; i = i + 3)
            {
                WS2812_SetPixelColor(i + q, c); //turn every third pixel on
            }

            HAL_Delay(wait);

            for (uint16_t i = 0; i < LED_NUMBER; i = i + 3)
            {
                WS2812_SetPixelColor(i + q, 0); //turn every third pixel off
            }

        }
    }
}

//Theatre-style crawling lights with rainbow effect
void WS2812_theaterChaseRainbow(uint8_t wait)
{
    for (int j = 0; j < 256; j++)
    {
        // cycle all 256 colors in the wheel
        for (int q = 0; q < 3; q++)
        {
            for (uint16_t i = 0; i < LED_NUMBER; i = i + 3)
            {
                WS2812_SetPixelColor(i + q, WS2812_Wheel((i + j) % 255)); //turn every third pixel on
            }


            HAL_Delay(wait);

            for (uint16_t i = 0; i < LED_NUMBER; i = i + 3)
            {
                WS2812_SetPixelColor(i + q, 0); //turn every third pixel off
            }
        }
    }
}
// Fill the dots one after the other with a color
void WS2812_colorWipe(uint32_t c, uint8_t wait)
{
    uint16_t i = 0;
    for (i = 0; i < LED_NUMBER; i++)
    {
        WS2812_SetPixelColor(i, c);
        HAL_Delay(wait);
    }
}

void WS2812_breath(uint32_t wait)
{
    for (int j=4; j < 256; j = j + 1)
    {
		if(j>100&&j<256-5)j=j+5;
        for (int i = 0; i < LED_NUMBER; i++)
        {
            WS2812_SetPixelColor(i, WS2812_Color(j, 0, 0)); // lighting  in turn
            HAL_Delay(wait);
        }
    }
    for (int j = 255; j > 4; j = j - 1)
    {
		if(j>100)j=j-5;
        for (int i = 0; i <LED_NUMBER; i++)
        {
            WS2812_SetPixelColor(i, WS2812_Color(j, 0, 0));
            HAL_Delay(wait);
        }
    }
}

