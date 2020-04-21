/**
@file    ft8xx.h
@brief   EVE - HMI solution library for based on FT-BT8xx chip
@version 0.1
@date    2020-04-19
@author  Mikhail Ivanov

@section LICENSE

    MIT License

        Copyright (c) 2020 Mikhail Ivanov

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
                                                              copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

       THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
        SOFTWARE.
            */

#ifndef FT8XX_H
#define FT8XX_H

#include <EVE_commands.h>
#include <type_traits>

        extern Serial pc;

class FT8xx : private NonCopyable<FT8xx>
{
public:
#if (MBED_VERSION >= MBED_ENCODE_VERSION(5,8,0)) && MBED_CONF_EVENTS_PRESENT
    struct TouchCalibrationResult
    {
        uint32_t touch_a{}, touch_b{}, touch_c{}, touch_d{}, touch_e{}, touch_f{};
    };

    FT8xx(PinName mosi,
          PinName miso,
          PinName sclk,
          PinName ssel,
          PinName pd,
          PinName interrupt,
          EVE_HAL::SPIFrequency spiFrequency = EVE_HAL::F_20M,
          bool sharedEventQueue = false,
          uint32_t threadStackSize = (3*512),
          const char * threadName = "FT8xxThrd"
          );
#elif
    FT8xx(PinName mosi,
          PinName miso,
          PinName sclk,
          PinName ssel,
          PinName pd,
          EVE_HAL::SPIFrequency spiFrequency = EVE_HAL::F_20M
          );
#endif
    ~FT8xx();

    /*!
     * \brief touchCalibrate - function for calibrate touchscreen
     * \param factory - if true - load factory calibration, else - start new calibration
     */
    const FT8xx::TouchCalibrationResult &touchCalibrate(bool factory = true);

#if (MBED_VERSION >= MBED_ENCODE_VERSION(5,8,0)) && MBED_CONF_EVENTS_PRESENT
    /*!
     * \brief attach one callback to any number of interrupt flags
     * \param f - callback function void(uint8_t flag) f;
     * \param flag - one of the possible interrupt mas bit
     * for more information see EVE.h Interrupt bits and BT81X (815/6) datasheet pg 19
     */
    void attach(mbed::Callback<void(uint8_t)> f, uint8_t flag);

    inline void attachPageSwapCallback(mbed::Callback<void(uint8_t)> f){attach(f, EVE_INT_SWAP);}
    inline void attachTouchDetectedCallback(mbed::Callback<void(uint8_t)> f){attach(f, EVE_INT_TOUCH);}
    inline void attachTouchTagCallback(mbed::Callback<void(uint8_t)> f){attach(f, EVE_INT_TAG);}
    inline void attachTouchConversionsCallback(mbed::Callback<void(uint8_t)> f){attach(f, EVE_INT_CONVCOMPLETE);}

    /*!
     * \brief attachToTag. attachet callback to specific tag and pass tag number as parameter
     * \param f - callback function attached to specific tag (1-254)
     */
    void attachToTag(mbed::Callback<void(uint8_t)> f);

#endif

private:
    EVE_HAL * m_hal;
#if (MBED_VERSION >= MBED_ENCODE_VERSION(5,8,0)) && MBED_CONF_EVENTS_PRESENT
    void interruptFound();

    InterruptIn m_interrupt;
    Thread * m_eventThread{nullptr};
    EventQueue * m_queue{nullptr};
    //Calbacks for interrupt events
    mbed::Callback<void(uint8_t)> m_pageSwapCallback{nullptr};
    mbed::Callback<void(uint8_t)> m_touchDetectedCallback{nullptr};
    mbed::Callback<void(uint8_t)> m_touchTagCallback{nullptr};
    mbed::Callback<void(uint8_t)> m_touchConvCompCallback{nullptr};

    mbed::Callback<void(uint8_t)> m_tagNumberCallback{nullptr};
#endif
};

#endif // FT8XX_H
