# AD7606_STM32

## How to use

Connection:

AD7606|Connection|Comment
---|---|---
SCK|STM32-HSPI_SCK
CS|STM32-HSPI-NSS
CONVST-A|GPIO-OUT-CONVST-A|could be tied with CONVST-B
CONVST-B|GPIO-OUT-CONVST-B|could be tied with CONVST-A
BUSY|GPIO-IN-BUSY
RST|GPIO-OUT-RST|GPIO speed should be low
DB7/DOUTA|STM32-HSPI-MISO
DB8/DOUTB|Float|Not support dual line mode now
Other data pins|Float|Not support parallel mode now
OS[2:0]|GND|We recommend using jumpers to set the over-sampling pins to reduce GPIO consumption

Implementation:

```cpp
AD7606_Handler ad7606;
int16_t bufferi[8];
float bufferf[8];

setup() 
{
  AD7606_Init(&ad7606, &hspi1, AD7606_CONV_GPIO_Port, AD7606_CONV_Pin, AD7606_CONV_GPIO_Port, AD7606_CONV_Pin, AD7606_BUSY_GPIO_Port, AD7606_BUSY_Pin, AD7606_RST_GPIO_Port, AD7606_RST_Pin, Range_10VPP, 2.5);
}

while (1)
{
  AD7606_StartReadBytes(&ad7606, bufferi, 8);
  AD7606_ConvertToVoltage(&ad7606, bufferi, bufferf, 8);
}

```

Troubleshooting:

If the AD7606 returns the wrong value, try to lower the GPIO speed.
