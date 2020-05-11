/*
@file    EVE_target.c
@brief   target specific functions
@version 4.1
@date    2020-04-19
@author  Rudolph Riedel
@author  Mikhail Ivanov

@section LICENSE

MIT License

Copyright (c) 2016-2020 Rudolph Riedel
Copyright (c) 2020 Mikhail Ivanov

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

@section History

4.0
- added support for MSP432
- moved the two include lines out of reach for Arduino to increase compatibility with Arduino
- removed preceding "__" from two CMSIS functions that were not necessary and maybe even wrong
- moved the very basic DELAY_MS() function for ATSAM to EVE_target.c and therefore removed the unneceesary inlining for this function
- added DMA support for ATSAME51
- started to implement DMA support for STM32

4.1
    - added initial MBED  support
 */
#include <EVE_target.h>
#include <stm32f4xx_ll_gpio.h>
using namespace EVE;
EVE_HAL * EVE_HAL::_ptr = nullptr;
EVE_HAL * EVE_HAL::instance()
{
    if(!_ptr)
        error("EVE_HAL is not initialized");
    return _ptr;
}

EVE_HAL * EVE_HAL::instance(PinName mosi, PinName miso, PinName sclk, PinName ssel, PinName pd)
{
    if(!_ptr)
        _ptr = new EVE_HAL(mosi, miso, sclk, ssel, pd);
    return _ptr;
}

void EVE_HAL::setSPIfrequency(EVE_HAL::SPIFrequency frequency)
{
    m_spi.frequency(static_cast<int>(frequency));
}

EVE_HAL::EVE_HAL() :
    EVE_HAL(EVE_SPI_MOSI,
            EVE_SPI_MISO,
            EVE_SPI_CLK,
            EVE_SPI_SSEL,
            EVE_PD) {}

EVE_HAL::EVE_HAL(PinName mosi,
                 PinName miso,
                 PinName sclk,
                 PinName ssel,
                 PinName pd) :
    m_spi(mosi, miso, sclk),
    m_ssel(ssel),
    m_pd(pd)
{
    pdnSet();
    csSet();
    ThisThread::sleep_for(100);
    m_spi.format(8, 0);
    this->setSPIfrequency(F_1M);
    LL_GPIO_SetPinSpeed(GPIOF, LL_GPIO_PIN_9, LL_GPIO_SPEED_FREQ_LOW);
    LL_GPIO_SetPinSpeed(GPIOF, LL_GPIO_PIN_8, LL_GPIO_SPEED_FREQ_LOW);
    LL_GPIO_SetPinSpeed(GPIOF, LL_GPIO_PIN_7, LL_GPIO_SPEED_FREQ_LOW);
    LL_GPIO_SetPinSpeed(GPIOF, LL_GPIO_PIN_12, LL_GPIO_SPEED_FREQ_LOW);
    _ptr = this;
}

//*********Basic communication functions
void EVE_HAL::cmdWrite(uint8_t cmd, uint8_t parameter)
{
    csSet();
    m_spi.write(cmd);
    m_spi.write(parameter);
    m_spi.write(0x00);
    csClear();
}

uint8_t EVE_HAL::rd8(uint32_t address)
{
    uint8_t data;
    csSet();
    m_spi.write(static_cast<uint8_t>((address >> 16) | MEM_READ));
    m_spi.write(static_cast<uint8_t>(address >> 8));
    m_spi.write(static_cast<uint8_t>(address));
    m_spi.write(0x00);
    data = static_cast<uint8_t>(m_spi.write(0x00));
    csClear();
    return data;
}

uint16_t EVE_HAL::rd16(uint32_t address)
{
    uint16_t data;

    csSet();
    m_spi.write(static_cast<uint8_t>((address >> 16) | MEM_READ));
    m_spi.write(static_cast<uint8_t>(address >> 8));
    m_spi.write(static_cast<uint8_t>(address));
    m_spi.write(0x00);
    data = static_cast<uint16_t>(m_spi.write(0x00));
    data = static_cast<uint16_t>(m_spi.write(0x00) << 8) | data;
    csClear();
    return data;
}

uint32_t EVE_HAL::rd32(uint32_t address)
{
    uint32_t data;
    csSet();
    m_spi.write(static_cast<uint8_t>((address >> 16) | MEM_READ));
    m_spi.write(static_cast<uint8_t>(address >> 8));
    m_spi.write(static_cast<uint8_t>(address));
    m_spi.write(0x00);
    data = static_cast<uint32_t>(m_spi.write(0x00));
    data = static_cast<uint32_t>(m_spi.write(0x00) << 8) | data;
    data = static_cast<uint32_t>(m_spi.write(0x00) << 16) | data;
    data = static_cast<uint32_t>(m_spi.write(0x00) << 24) | data;
    csClear();
    return data;
}

void EVE_HAL::wr8(uint32_t address, uint8_t data)
{
    csSet();
    m_spi.write(static_cast<uint8_t>((address >> 16) | MEM_WRITE));
    m_spi.write(static_cast<uint8_t>(address >> 8));
    m_spi.write(static_cast<uint8_t>(address));
    m_spi.write(data);
    csClear();
}

void EVE_HAL::wr16(uint32_t address, uint16_t data)
{
    csSet();
    m_spi.write(static_cast<uint8_t>((address >> 16) | MEM_WRITE));
    m_spi.write(static_cast<uint8_t>(address >> 8));
    m_spi.write(static_cast<uint8_t>(address));
    m_spi.write(static_cast<uint8_t>(data));
    m_spi.write(static_cast<uint8_t>(data >> 8));
    csClear();
}

void EVE_HAL::wr32(uint32_t address, uint32_t data)
{
    csSet();
    m_spi.write(static_cast<uint8_t>((address >> 16) | MEM_WRITE));
    m_spi.write(static_cast<uint8_t>(address >> 8));
    m_spi.write(static_cast<uint8_t>(address));
    m_spi.write(static_cast<uint8_t>(data));
    m_spi.write(static_cast<uint8_t>(data >> 8));
    m_spi.write(static_cast<uint8_t>(data >> 16));
    m_spi.write(static_cast<uint8_t>(data >> 24));
    csClear();
}

void EVE_HAL::wrByteBuffer(uint32_t address, const std::vector<uint8_t> & buffer)
{
    csSet();
    m_spi.write(static_cast<uint8_t>((address >> 16) | MEM_WRITE));
    m_spi.write(static_cast<uint8_t>(address >> 8));
    m_spi.write(static_cast<uint8_t>(address));
    for(const auto & b : buffer)
        write(b);
    csClear();
}

void EVE_HAL::wrByteBuffer(uint32_t address, const uint8_t * buffer, uint16_t len)
{
    uint16_t count;
    len = (len + 3) & (~3);

    csSet();
    m_spi.write(static_cast<uint8_t>((address >> 16) | MEM_WRITE));
    m_spi.write(static_cast<uint8_t>(address >> 8));
    m_spi.write(static_cast<uint8_t>(address));
    for(count = 0; count < len; count++)
    {
        m_spi.write(static_cast<uint8_t>(*buffer + count));
    }
    csClear();
}

//********************************************************************
void EVE_cs_set()
{
    EVE_HAL::instance()->csSet();
}

void EVE_cs_clear()
{
    EVE_HAL::instance()->csClear();
}

void spi_transmit(uint8_t data)
{
    EVE_HAL::instance()->transmit(data);
}

int spi_receive(uint8_t data)
{
    return EVE_HAL::instance()->receive(data);
}

uint8_t fetch_flash_byte(const uint8_t * data)
{
    return *data;
}

void EVE_pdn_set()
{
    return EVE_HAL::instance()->pdnSet();
}

void EVE_pdn_clear()
{
    return EVE_HAL::instance()->pdnClear();
}

void spi_transmit_async(uint8_t data)
{
#if defined(EVE_DMA)
    //TODO: Implement DMA
#else
    EVE_HAL::instance()->transmit(data);
#endif
}
