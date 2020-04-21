/**
@file    ft8xx.cpp
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
#include "ft8xx.h"

#if (MBED_VERSION >= MBED_ENCODE_VERSION(5,8,0)) && MBED_CONF_EVENTS_PRESENT
        /*!
 * \brief FT8xx::FT8xx driver for FTDI FT8xx-BT8xx
 */
        FT8xx::FT8xx(PinName mosi,
                     PinName miso,
                     PinName sclk,
                     PinName ssel,
                     PinName pd,
                     PinName interrupt,
                     EVE_HAL::SPIFrequency spiFrequency,
                     bool sharedEventQueue,
                     uint32_t threadStackSize,
                     const char *threadName
                     )
    : m_interrupt(interrupt),
    m_eventThread(new Thread(osPriorityNormal, threadStackSize, nullptr, threadName))
{
    m_interrupt.mode(PullUp);
    m_hal = EVE_HAL::instance(mosi, miso, sclk, ssel, pd);
    //Initialize Screen
    EVE_init();
    //Clear interrupt flags for prevent fake interrupt calling after restart
    EVE_memRead8(REG_INT_FLAGS);
    //Disable interrupts
    EVE_memWrite8(REG_INT_EN, 0x0);
    //Clear interrupt mask
    EVE_memWrite8(REG_INT_MASK, 0x0);

    if(sharedEventQueue == true)
    {
        m_queue = mbed_event_queue();
    }
    else
    {
        m_queue = new EventQueue(5*EVENTS_EVENT_SIZE);
        m_eventThread->start(callback(m_queue, &EventQueue::dispatch_forever));
    }
    m_interrupt.fall(m_queue->event(callback(this, &FT8xx::interruptFound)));
    m_hal->setSPIfrequency(spiFrequency);
}
#elif
        /*!
 * \brief FT8xx::FT8xx driver for FTDI FT8xx-BT8xx
 */
        FT8xx(PinName mosi,
              PinName miso,
              PinName sclk,
              PinName ssel,
              PinName pd,
              EVE_HAL::SPIFrequency spiFrequency
              )
{
    EVE_init();
    m_hal->setSPIfrequency(EVE_HAL::F_20M);
}
#endif

FT8xx::~FT8xx()
{
    delete m_hal;
}

const FT8xx::TouchCalibrationResult & FT8xx::touchCalibrate(bool factory)
{
    /* send pre-recorded touch calibration values, depending on the display the code is compiled for */
    if(factory == true)
    {
#if defined (EVE_CFAF240400C1_030SC)
        EVE_memWrite32(REG_TOUCH_TRANSFORM_A, 0x0000ed11);
        EVE_memWrite32(REG_TOUCH_TRANSFORM_B, 0x00001139);
        EVE_memWrite32(REG_TOUCH_TRANSFORM_C, 0xfff76809);
        EVE_memWrite32(REG_TOUCH_TRANSFORM_D, 0x00000000);
        EVE_memWrite32(REG_TOUCH_TRANSFORM_E, 0x00010690);
        EVE_memWrite32(REG_TOUCH_TRANSFORM_F, 0xfffadf2e);
#endif

#if defined (EVE_CFAF320240F_035T)
        EVE_memWrite32(REG_TOUCH_TRANSFORM_A, 0x00005614);
        EVE_memWrite32(REG_TOUCH_TRANSFORM_B, 0x0000009e);
        EVE_memWrite32(REG_TOUCH_TRANSFORM_C, 0xfff43422);
        EVE_memWrite32(REG_TOUCH_TRANSFORM_D, 0x0000001d);
        EVE_memWrite32(REG_TOUCH_TRANSFORM_E, 0xffffbda4);
        EVE_memWrite32(REG_TOUCH_TRANSFORM_F, 0x00f8f2ef);
#endif

#if defined (EVE_CFAF480128A0_039TC)
        EVE_memWrite32(REG_TOUCH_TRANSFORM_A, 0x00010485);
        EVE_memWrite32(REG_TOUCH_TRANSFORM_B, 0x0000017f);
        EVE_memWrite32(REG_TOUCH_TRANSFORM_C, 0xfffb0bd3);
        EVE_memWrite32(REG_TOUCH_TRANSFORM_D, 0x00000073);
        EVE_memWrite32(REG_TOUCH_TRANSFORM_E, 0x0000e293);
        EVE_memWrite32(REG_TOUCH_TRANSFORM_F, 0x00069904);
#endif

#if defined (EVE_CFAF800480E0_050SC)
        EVE_memWrite32(REG_TOUCH_TRANSFORM_A, 0x000107f9);
        EVE_memWrite32(REG_TOUCH_TRANSFORM_B, 0xffffff8c);
        EVE_memWrite32(REG_TOUCH_TRANSFORM_C, 0xfff451ae);
        EVE_memWrite32(REG_TOUCH_TRANSFORM_D, 0x000000d2);
        EVE_memWrite32(REG_TOUCH_TRANSFORM_E, 0x0000feac);
        EVE_memWrite32(REG_TOUCH_TRANSFORM_F, 0xfffcfaaf);
#endif

#if defined (EVE_PAF90)
        EVE_memWrite32(REG_TOUCH_TRANSFORM_A, 0x00000159);
        EVE_memWrite32(REG_TOUCH_TRANSFORM_B, 0x0001019c);
        EVE_memWrite32(REG_TOUCH_TRANSFORM_C, 0xfff93625);
        EVE_memWrite32(REG_TOUCH_TRANSFORM_D, 0x00010157);
        EVE_memWrite32(REG_TOUCH_TRANSFORM_E, 0x00000000);
        EVE_memWrite32(REG_TOUCH_TRANSFORM_F, 0x0000c101);
#endif

#if defined (EVE_RiTFT43)
        EVE_memWrite32(REG_TOUCH_TRANSFORM_A, 0x000062cd);
        EVE_memWrite32(REG_TOUCH_TRANSFORM_B, 0xfffffe45);
        EVE_memWrite32(REG_TOUCH_TRANSFORM_C, 0xfff45e0a);
        EVE_memWrite32(REG_TOUCH_TRANSFORM_D, 0x000001a3);
        EVE_memWrite32(REG_TOUCH_TRANSFORM_E, 0x00005b33);
        EVE_memWrite32(REG_TOUCH_TRANSFORM_F, 0xFFFbb870);
#endif

#if defined (EVE_EVE2_38)
        EVE_memWrite32(REG_TOUCH_TRANSFORM_A, 0x00007bed);
        EVE_memWrite32(REG_TOUCH_TRANSFORM_B, 0x000001b0);
        EVE_memWrite32(REG_TOUCH_TRANSFORM_C, 0xfff60aa5);
        EVE_memWrite32(REG_TOUCH_TRANSFORM_D, 0x00000095);
        EVE_memWrite32(REG_TOUCH_TRANSFORM_E, 0xffffdcda);
        EVE_memWrite32(REG_TOUCH_TRANSFORM_F, 0x00829c08);
#endif

#if defined (EVE_EVE2_35G)
        EVE_memWrite32(REG_TOUCH_TRANSFORM_A, 0x000109E4);
        EVE_memWrite32(REG_TOUCH_TRANSFORM_B, 0x000007A6);
        EVE_memWrite32(REG_TOUCH_TRANSFORM_C, 0xFFEC1EBA);
        EVE_memWrite32(REG_TOUCH_TRANSFORM_D, 0x0000072C);
        EVE_memWrite32(REG_TOUCH_TRANSFORM_E, 0x0001096A);
        EVE_memWrite32(REG_TOUCH_TRANSFORM_F, 0xFFF469CF);
#endif

#if defined (EVE_EVE2_43G)
        EVE_memWrite32(REG_TOUCH_TRANSFORM_A, 0x0000a1ff);
        EVE_memWrite32(REG_TOUCH_TRANSFORM_B, 0x00000680);
        EVE_memWrite32(REG_TOUCH_TRANSFORM_C, 0xffe54cc2);
        EVE_memWrite32(REG_TOUCH_TRANSFORM_D, 0xffffff53);
        EVE_memWrite32(REG_TOUCH_TRANSFORM_E, 0x0000912c);
        EVE_memWrite32(REG_TOUCH_TRANSFORM_F, 0xfffe628d);
#endif

#if defined (EVE_EVE2_50G)
        EVE_memWrite32(REG_TOUCH_TRANSFORM_A, 0x000109E4);
        EVE_memWrite32(REG_TOUCH_TRANSFORM_B, 0x000007A6);
        EVE_memWrite32(REG_TOUCH_TRANSFORM_C, 0xFFEC1EBA);
        EVE_memWrite32(REG_TOUCH_TRANSFORM_D, 0x0000072C);
        EVE_memWrite32(REG_TOUCH_TRANSFORM_E, 0x0001096A);
        EVE_memWrite32(REG_TOUCH_TRANSFORM_F, 0xFFF469CF);
#endif

#if defined (EVE_EVE2_70G)
        EVE_memWrite32(REG_TOUCH_TRANSFORM_A, 0x000105BC);
        EVE_memWrite32(REG_TOUCH_TRANSFORM_B, 0xFFFFFA8A);
        EVE_memWrite32(REG_TOUCH_TRANSFORM_C, 0x00004670);
        EVE_memWrite32(REG_TOUCH_TRANSFORM_D, 0xFFFFFF75);
        EVE_memWrite32(REG_TOUCH_TRANSFORM_E, 0x00010074);
        EVE_memWrite32(REG_TOUCH_TRANSFORM_F, 0xFFFF14C8);
#endif

#if defined (EVE_NHD_35)
        EVE_memWrite32(REG_TOUCH_TRANSFORM_A, 0x0000f78b);
        EVE_memWrite32(REG_TOUCH_TRANSFORM_B, 0x00000427);
        EVE_memWrite32(REG_TOUCH_TRANSFORM_C, 0xfffcedf8);
        EVE_memWrite32(REG_TOUCH_TRANSFORM_D, 0xfffffba4);
        EVE_memWrite32(REG_TOUCH_TRANSFORM_E, 0x0000f756);
        EVE_memWrite32(REG_TOUCH_TRANSFORM_F, 0x0009279e);
#endif

#if defined (EVE_RVT70)
        EVE_memWrite32(REG_TOUCH_TRANSFORM_A, 0x000074df);
        EVE_memWrite32(REG_TOUCH_TRANSFORM_B, 0x000000e6);
        EVE_memWrite32(REG_TOUCH_TRANSFORM_C, 0xfffd5474);
        EVE_memWrite32(REG_TOUCH_TRANSFORM_D, 0x000001af);
        EVE_memWrite32(REG_TOUCH_TRANSFORM_E, 0x00007e79);
        EVE_memWrite32(REG_TOUCH_TRANSFORM_F, 0xffe9a63c);
#endif

#if defined (EVE_FT811CB_HY50HD)
        EVE_memWrite32(REG_TOUCH_TRANSFORM_A, 66353);
        EVE_memWrite32(REG_TOUCH_TRANSFORM_B, 712);
        EVE_memWrite32(REG_TOUCH_TRANSFORM_C, 4293876677);
        EVE_memWrite32(REG_TOUCH_TRANSFORM_D, 4294966157);
        EVE_memWrite32(REG_TOUCH_TRANSFORM_E, 67516);
        EVE_memWrite32(REG_TOUCH_TRANSFORM_F, 418276);
#endif

#if defined (EVE_ADAM101)
        EVE_memWrite32(REG_TOUCH_TRANSFORM_A, 0x000101E3);
        EVE_memWrite32(REG_TOUCH_TRANSFORM_B, 0x00000114);
        EVE_memWrite32(REG_TOUCH_TRANSFORM_C, 0xFFF5EEBA);
        EVE_memWrite32(REG_TOUCH_TRANSFORM_D, 0xFFFFFF5E);
        EVE_memWrite32(REG_TOUCH_TRANSFORM_E, 0x00010226);
        EVE_memWrite32(REG_TOUCH_TRANSFORM_F, 0x0000C783);
#endif
        FT8xx::TouchCalibrationResult res;
        res.touch_a = EVE_memRead32(REG_TOUCH_TRANSFORM_A);
        res.touch_b = EVE_memRead32(REG_TOUCH_TRANSFORM_B);
        res.touch_c = EVE_memRead32(REG_TOUCH_TRANSFORM_C);
        res.touch_d = EVE_memRead32(REG_TOUCH_TRANSFORM_D);
        res.touch_e = EVE_memRead32(REG_TOUCH_TRANSFORM_E);
        res.touch_f = EVE_memRead32(REG_TOUCH_TRANSFORM_F);
        return std::move(res);
    }
    else
    {
        /* write down the numbers on the screen and either place them in one of the pre-defined blocks above or make a new block */
        /* calibrate touch and displays values to screen */
        EVE_cmd_dl(CMD_DLSTART);
        EVE_cmd_dl(DL_CLEAR_RGB | COLOR_RGB(0, 0, 0));
        EVE_cmd_dl(DL_CLEAR | CLR_COL | CLR_STN | CLR_TAG);
        EVE_cmd_text((EVE_HSIZE/2), 50, 26, EVE_OPT_CENTER, "Please tap on the dot.");
        EVE_cmd_calibrate();
        EVE_cmd_dl(DL_DISPLAY);
        EVE_cmd_dl(CMD_SWAP);
        EVE_cmd_execute();

        FT8xx::TouchCalibrationResult res;
        res.touch_a = EVE_memRead32(REG_TOUCH_TRANSFORM_A);
        res.touch_b = EVE_memRead32(REG_TOUCH_TRANSFORM_B);
        res.touch_c = EVE_memRead32(REG_TOUCH_TRANSFORM_C);
        res.touch_d = EVE_memRead32(REG_TOUCH_TRANSFORM_D);
        res.touch_e = EVE_memRead32(REG_TOUCH_TRANSFORM_E);
        res.touch_f = EVE_memRead32(REG_TOUCH_TRANSFORM_F);
        return std::move(res);
    }
}

#if (MBED_VERSION >= MBED_ENCODE_VERSION(5,8,0)) && MBED_CONF_EVENTS_PRESENT
void FT8xx::interruptFound()
{
    uint8_t flag = EVE_memRead8(REG_INT_FLAGS);
    if((flag & EVE_INT_SWAP) != 0
        && m_pageSwapCallback)
    {
        m_pageSwapCallback(flag);
    }

    if((flag & EVE_INT_TOUCH) != 0
        && m_touchDetectedCallback)
    {
        m_touchDetectedCallback(flag);
    }

    if((flag & EVE_INT_TAG) != 0)
    {
        if(m_touchTagCallback)
        {
            m_touchTagCallback(flag);
        }
        if(m_tagNumberCallback)
        {
            m_tagNumberCallback(EVE_memRead8(REG_TOUCH_TAG));
        }
    }

    if((flag & EVE_INT_SOUND) != 0)
    {
        pc.printf("EVE_INT_SOUND: %04x \n", flag);
    }

    if((flag & EVE_INT_PLAYBACK) != 0)
    {
        pc.printf("EVE_INT_PLAYBACK: %04x \n", flag);
    }

    if((flag & EVE_INT_CMDEMPTY) != 0)
    {
        pc.printf("EVE_INT_CMDEMPTY: %04x \n", flag);
    }

    if((flag & EVE_INT_CMDFLAG) != 0)
    {
        pc.printf("EVE_INT_CMDFLAG: %04x \n", flag);
    }

    if((flag & EVE_INT_CONVCOMPLETE) != 0
        && m_touchConvCompCallback)
    {
        m_touchConvCompCallback(flag);
    }
}

void FT8xx::attach(mbed::Callback<void(uint8_t)> f, uint8_t flag)
{
    uint8_t interruptMask{0};
    //if interrupt is disabled = clear interrupt flags
    if(EVE_memRead8(REG_INT_EN) == 0x1)
    {
        interruptMask = EVE_memRead8(REG_INT_MASK);
    }
    interruptMask |= flag;
    switch (flag) {
    case EVE_INT_SWAP:
        m_pageSwapCallback = f;
        break;
    case EVE_INT_TOUCH:
        m_touchDetectedCallback = f;
        break;
    case EVE_INT_TAG:
        m_touchTagCallback = f;
        break;
    case EVE_INT_SOUND:
        pc.printf("%04x \n", flag);
        break;
    case EVE_INT_PLAYBACK:
        pc.printf("%04x \n", flag);
        break;
    case EVE_INT_CMDEMPTY:
        pc.printf("%04x \n", flag);
        break;
    case EVE_INT_CMDFLAG:
        pc.printf("%04x \n", flag);
        break;
    case EVE_INT_CONVCOMPLETE:
        m_touchConvCompCallback = f;
        break;
    default:
        return;
    }
    //set interrupts mask
    EVE_memWrite8(REG_INT_MASK, interruptMask);
    //enable interrupts
    EVE_memWrite8(REG_INT_EN, 0x1);
}

void FT8xx::attachToTag(mbed::Callback<void(uint8_t)> f)
{
    uint8_t interruptMask{0};
    if(EVE_memRead8(REG_INT_EN) == 0x1)
    {
        interruptMask = EVE_memRead8(REG_INT_MASK);
    }
    interruptMask |= EVE_INT_TAG;
    m_tagNumberCallback = f;
    //set interrupts mask
    EVE_memWrite8(REG_INT_MASK, interruptMask);
    //enable interrupts
    EVE_memWrite8(REG_INT_EN, 0x1);
}
#endif
