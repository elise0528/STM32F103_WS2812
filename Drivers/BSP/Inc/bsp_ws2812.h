#ifndef __BSP_WS2812_H
#define __BSP_WS2812_H

#include "stm32f1xx_hal.h"

#define LED_NUMBER 60

#define SPI_WS2812

#ifdef PWM_WS2812

extern TIM_HandleTypeDef htim4;
extern DMA_HandleTypeDef hdma_tim4_ch3;

#endif

#ifdef SPI_WS2812

extern SPI_HandleTypeDef hspi1;
extern DMA_HandleTypeDef hdma_spi1_tx;

#endif

void WS2812_DataInit(void);
void WS2812_setAllPixelColor(uint8_t r, uint8_t g, uint8_t b);
void WS2812_setPixelColor(uint8_t n, uint8_t r, uint8_t g, uint8_t b);
void WS2812_SetPixelColor(uint8_t n, uint32_t c);
void WS2812_PixelUpdate(void);
uint32_t WS2812_Color(uint8_t r, uint8_t g, uint8_t b);
uint32_t WS2812_Wheel(uint8_t WheelPos);
void WS2812_rainbow(uint32_t wait);
void WS2812_rainbowCycle(uint8_t wait);
void WS2812_theaterChase(uint32_t c, uint8_t wait);
void WS2812_theaterChaseRainbow(uint8_t wait);
void WS2812_colorWipe(uint32_t c, uint8_t wait);
void WS2812_breath(uint32_t wait);



#endif 
