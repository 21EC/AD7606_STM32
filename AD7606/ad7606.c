/*
 * ad7606.c
 *
 *  Created on: Jul 16, 2021
 *      Author: headcrab
 */

// SPI 16-bit mode, single data lane, hw nSS, nSSP Enabled, CPOL high, CPHA 2 edge

#include "ad7606.h"

/* Extern variables declaration */

#ifdef AD7606_USE_TIM_CONV
void AD7606_Init(AD7606_Handler *device, SPI_HandleTypeDef *spi_handler, TIM_HandleTypeDef *conva_tim_handler, uint32_t conva_channel,
    TIM_HandleTypeDef *convb_tim_handler, uint32_t convb_channel, GPIO_TypeDef *busy_gpio_port, uint16_t busy_gpio_pin, GPIO_TypeDef *reset_gpio_port, uint16_t reset_gpio_pin, AD7606_InputRange volt_range,
    float volt_ref)
{
  device->device_spi = spi_handler;
  device->conva_tim = conva_tim_handler;
  device->conva_tim_channel = conva_channel;
  device->convb_tim = convb_tim_handler;
  device->convb_tim_channel = convb_channel;
  device->busy_port = busy_gpio_port;
  device->busy_pin = busy_gpio_pin;
  device->reset_port = reset_gpio_port;
  device->reset_pin = reset_gpio_pin;
  device->range = volt_range;
  device->vref = volt_ref;
  AD7606_Reset(device);
  return;
}
#else
void AD7606_Init(AD7606_Handler *device, SPI_HandleTypeDef *spi_handler, GPIO_TypeDef *conva_gpio_port, uint16_t conva_gpio_pin, GPIO_TypeDef *convb_gpio_port,
    uint16_t convb_gpio_pin, GPIO_TypeDef *busy_gpio_port, uint16_t busy_gpio_pin, GPIO_TypeDef *reset_gpio_port, uint16_t reset_gpio_pin,
    AD7606_InputRange volt_range, float volt_ref)
{
  device->device_spi = spi_handler;
  device->conva_port = conva_gpio_port;
  device->conva_pin = conva_gpio_pin;
  device->convb_port = convb_gpio_port;
  device->convb_pin = convb_gpio_pin;
  device->busy_port = busy_gpio_port;
  device->busy_pin = busy_gpio_pin;
  device->reset_port = reset_gpio_port;
  device->reset_pin = reset_gpio_pin;
  device->range = volt_range;
  device->vref = volt_ref;
  AD7606_Reset(device);
}
#endif

void AD7606_Reset(AD7606_Handler *device)
{
  HAL_GPIO_WritePin(device->conva_port, device->conva_pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(device->convb_port, device->convb_pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(device->reset_port, device->reset_pin, GPIO_PIN_SET);
  AD7606_Delay(device, 1);
  HAL_GPIO_WritePin(device->reset_port, device->reset_pin, GPIO_PIN_RESET);
}

inline void AD7606_Delay(AD7606_Handler *device, uint32_t delay)
{
  uint32_t i;
  for (i = 0; i < delay; ++i)
  {
    __NOP();
  }
  return;
}

#ifdef AD7606_USE_TIM_CONV
void AD7606_StartConv(AD7606_Handler *device, AD7606_Convst conv)
{
  // Use timer PWM output for convst output;
  switch(conv)
  {
  case Convst_A: HAL_TIM_PWM_Start(device->conva_tim, device->conva_tim_channel); break;
  case Convst_B: HAL_TIM_PWM_Start(device->convb_tim, device->convb_tim_channel); break;
  case Convst_All: HAL_TIM_PWM_Start(device->conva_tim, device->conva_tim_channel); HAL_TIM_PWM_Start(device->convb_tim, device->convb_tim_channel); break;
  }
  return;
}

void AD7606_StopConv(AD7606_Handler *device, AD7606_Convst conv)
{
  switch(conv)
  {
  case Convst_A: HAL_TIM_PWM_Stop(device->conva_tim, device->conva_tim_channel); break;
  case Convst_B: HAL_TIM_PWM_Stop(device->convb_tim, device->convb_tim_channel); break;
  case Convst_All: HAL_TIM_PWM_Stop(device->conva_tim, device->conva_tim_channel); HAL_TIM_PWM_Stop(device->convb_tim, device->convb_tim_channel); break;
  }
  return;
}
#else
void AD7606_PulseConv(AD7606_Handler *device)
{
  HAL_GPIO_WritePin(device->conva_port, device->conva_pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(device->convb_port, device->convb_pin, GPIO_PIN_RESET);
  AD7606_Delay(device, 1);
  HAL_GPIO_WritePin(device->conva_port, device->conva_pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(device->convb_port, device->convb_pin, GPIO_PIN_SET);
}
#endif

void AD7606_StartReadBytes(AD7606_Handler *device, int16_t *pDst, uint16_t length)
{
#ifndef AD7606_USE_TIM_CONV
  AD7606_PulseConv(device);
#endif
  while(HAL_GPIO_ReadPin(device->busy_port, device->busy_pin) == GPIO_PIN_SET);
  HAL_SPI_Receive_DMA(device->device_spi, (uint8_t *)pDst, length);
  return;
}

void AD7606_ConvertToVoltage(AD7606_Handler *device, int16_t *pSrc, float *pDst, uint16_t length)
{
  uint16_t i;
  switch (device->range)
  {
  case Range_10VPP:
    for (i = 0; i < length; i++)
    {
      pDst[i] = (float)pSrc[i] * 10.0 * 2.5 / (device->vref) / 32768.0;
    }
    break;
  case Range_5VPP:
    for (i = 0; i < length; i++)
    {
      pDst[i] = (float)pSrc[i] * 5.0 * 2.5 / (device->vref) / 32768.0;
    }
    break;
  }
  return;
}
