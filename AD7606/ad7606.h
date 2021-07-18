/*
 * ad7606.h
 *
 *  Created on: Jul 16, 2021
 *      Author: headcrab
 */

#ifndef AD7606_AD7606_H_
#define AD7606_AD7606_H_

#include "main.h"

//#define AD7606_USE_TIM_CONV

typedef enum
{
  Range_10VPP,
  Range_5VPP
} AD7606_InputRange;

typedef enum
{
  Convst_A,
  Convst_B,
  Convst_All
} AD7606_Convst;

typedef struct
{
    SPI_HandleTypeDef *device_spi;
#ifdef AD7606_USE_TIM_CONV
    TIM_HandleTypeDef *conva_tim;
    uint32_t conva_tim_channel;
    TIM_HandleTypeDef *convb_tim;
    uint32_t convb_tim_channel;
#else
    GPIO_TypeDef *conva_port;
    uint16_t conva_pin;
    GPIO_TypeDef *convb_port;
    uint16_t convb_pin;
#endif
    GPIO_TypeDef *busy_port;
    uint16_t busy_pin;
    GPIO_TypeDef *reset_port;
    uint16_t reset_pin;
    AD7606_InputRange range;
    float vref;

} AD7606_Handler;

#ifdef AD7606_USE_TIM_CONV
void AD7606_Init(AD7606_Handler *device, SPI_HandleTypeDef *spi_handler, TIM_HandleTypeDef *conva_tim_handler, uint32_t conva_channel,
    TIM_HandleTypeDef *convb_tim_handler, uint32_t convb_channel, GPIO_TypeDef *busy_gpio_port, uint16_t busy_gpio_pin, GPIO_TypeDef *reset_gpio_port,
    uint16_t reset_gpio_pin, AD7606_InputRange volt_range, float volt_ref);
#else
void AD7606_Init(AD7606_Handler *device, SPI_HandleTypeDef *spi_handler, GPIO_TypeDef *conva_gpio_port, uint16_t conva_gpio_pin, GPIO_TypeDef *convb_gpio_port,
    uint16_t convb_gpio_pin, GPIO_TypeDef *busy_gpio_port, uint16_t busy_gpio_pin, GPIO_TypeDef *reset_gpio_port, uint16_t reset_gpio_pin,
    AD7606_InputRange volt_range, float volt_ref);
#endif
void AD7606_Reset(AD7606_Handler *device);
void AD7606_Delay(AD7606_Handler *device, uint32_t delay);
#ifdef AD7606_USE_TIM_CONV
void AD7606_StartConv(AD7606_Handler *device, AD7606_Convst conv);
void AD7606_StopConv(AD7606_Handler *device, AD7606_Convst conv);
#else
void AD7606_PulseConv(AD7606_Handler *device);
#endif
void AD7606_StartReadBytes(AD7606_Handler *device, int16_t *pDst, uint16_t length);
void AD7606_ConvertToVoltage(AD7606_Handler *device, int16_t *pSrc, float *pDst, uint16_t length);

#endif /* AD7606_AD7606_H_ */
