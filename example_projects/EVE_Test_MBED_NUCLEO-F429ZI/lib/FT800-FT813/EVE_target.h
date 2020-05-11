/*
@file    EVE_target.h
@brief   target specific includes, definitions and functions
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
*/

#ifndef EVE_TARGET_H_
#define EVE_TARGET_H_

#include <EVE.h>
#include <vector>

//Use thread waiting or not
#if defined(PIO_FRAMEWORK_MBED_RTOS_PRESENT) \
    && (MBED_CONF_RTOS_API_PRESENT)          \
    && (MBED_CONF_RTOS_PRESENT)
    #define DELAY_MS(ms) ThisThread::sleep_for(ms);
#else
    #define DELAY_MS(ms) wait_us(ms * 1000);
#endif

namespace EVE
{
/* EVE Memory Commands - used with EVE_memWritexx and EVE_memReadxx */
#define MEM_WRITE 0x80 /* EVE Host Memory Write */
#define MEM_READ  0x00 /* EVE Host Memory Read */

class EVE_HAL
{
public:
    enum SPIFrequency : int32_t
    {
        F_10K  = 10000,
        F_100K = 100000,
        F_1M   = 1000000,
        F_5M   = 5000000,
        F_10M  = 10000000,
        F_20M  = 20000000,
        F_30M  = 30000000
    };
    inline void csSet() { m_ssel.write(0); }
    inline void csClear() { m_ssel.write(1); }
    inline void transmit(uint8_t data) { m_spi.write(data); }
    inline int  receive(uint8_t data) { return m_spi.write(data); }
    inline void pdnSet() { m_pd.write(0); }
    inline void pdnClear() { m_pd.write(1); }

    //*********Basic communication functions
    void     cmdWrite(uint8_t command, uint8_t parameter);
    uint8_t  rd8(uint32_t address);
    uint16_t rd16(uint32_t address);
    uint32_t rd32(uint32_t address);

    void wr8(uint32_t address, uint8_t data);
    void wr16(uint32_t address, uint16_t data);
    void wr32(uint32_t address, uint32_t data);

    void wrByteBuffer(uint32_t address, const std::vector<uint8_t> & buffer);
    void wrByteBuffer(uint32_t address, const uint8_t * buffer, uint16_t len);

    /*!
     * \brief Raw write w/o select/deselect
     * \param data to send over SPI
     * \return read result
     */
    inline uint8_t write(uint8_t data)
    {
        return static_cast<uint8_t>(m_spi.write(data));
    }

    static EVE_HAL * instance();
    static EVE_HAL * instance(PinName mosi,
                              PinName miso,
                              PinName sclk,
                              PinName ssel,
                              PinName pd);

    ~EVE_HAL() { _ptr = nullptr; }

    void setSPIfrequency(SPIFrequency frequency);

private:
    EVE_HAL();
    EVE_HAL(PinName mosi = EVE_SPI_MOSI,
            PinName miso = EVE_SPI_MISO,
            PinName sclk = EVE_SPI_CLK,
            PinName ssel = EVE_SPI_SSEL,
            PinName pd   = EVE_PD);

    EVE_HAL(const EVE_HAL & other) = delete;
    EVE_HAL(EVE_HAL && other)      = delete;

    SPI              m_spi;
    DigitalOut       m_ssel;
    DigitalOut       m_pd;
    static EVE_HAL * _ptr;
};
}    // namespace EVE
extern "C" void    EVE_cs_set();
extern "C" void    EVE_cs_clear();
extern "C" void    spi_transmit(uint8_t data);
extern "C" int     spi_receive(uint8_t data);
extern "C" uint8_t fetch_flash_byte(const uint8_t * data);
extern "C" void    EVE_pdn_set();
extern "C" void    EVE_pdn_clear();
extern "C" void    spi_transmit_async(uint8_t data);

#endif /* EVE_TARGET_H_ */
