﻿/**
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
using namespace EVE;

#if(MBED_VERSION >= MBED_ENCODE_VERSION(5, 8, 0)) && MBED_CONF_EVENTS_PRESENT
/*!
 * \brief FT8xx::FT8xx driver for FTDI FT8xx-BT8xx
 */
FT8xx::FT8xx(
    PinName               mosi,
    PinName               miso,
    PinName               sclk,
    PinName               ssel,
    PinName               pd,
    PinName               interrupt,
    EVE_HAL::SPIFrequency spiFrequency,
    bool                  sharedEventQueue,
    uint32_t              threadStackSize,
    const char *          threadName) :
    m_interrupt(interrupt),
    m_eventThread(new Thread(osPriorityNormal, threadStackSize, nullptr, threadName))
{
    m_interrupt.mode(PullUp);
    m_hal = EVE_HAL::instance(mosi, miso, sclk, ssel, pd);
    //Initialize Screen
    //    EVE_init();
    uint8_t  chipid  = 0;
    uint16_t timeout = 0;

    m_hal->pdnSet();
    DELAY_MS(6); /* minimum time for power-down is 5ms */
    m_hal->pdnClear();
    DELAY_MS(21); /* minimum time to allow from rising PD_N to first access is 20ms */

    #if defined(EVE_HAS_CRYSTAL)
    m_hal->cmdWrite(EVE_CLKEXT, 0); /* setup EVE for external clock */
    #else
    EVE_cmdWrite(EVE_CLKINT, 0);    /* setup EVE for internal clock */
    #endif

    #if defined(BT81X_ENABLE)
    m_hal->cmdWrite(EVE_CLKSEL, 0x46); /* set clock to 72 MHz */
    #endif

    m_hal->cmdWrite(EVE_ACTIVE, 0); /* start EVE */

    /* BRT AN033 BT81X_Series_Programming_Guide V1.2 had a small change to chapter 2.4 "Initialization Sequence during Boot Up" */
    /* Send Host command ?ACTIVE? and wait for at least 300 milliseconds. */
    /* Ensure that there is no SPI access during this time. */
    /* I asked Bridgetek for clarification why this has been made stricter. */
    /* From observation with quite a few of different displays I do not agree that either the 300ms are necessary or that */
    /* *reading* the SPI while EVE inits itself is causing any issues. */
    /* But since BT815 at 72MHz need 42ms anyways before they start to answer, here is my compromise, a fixed 40ms delay */
    /* to provide at least a short moment of silence for EVE */
    DELAY_MS(40)

    while(chipid != 0x7C) /* if chipid is not 0x7c, continue to read it until it is, EVE needs a moment for it's power on self-test and configuration */
    {
        DELAY_MS(1)
        chipid = m_hal->rd8(REG_ID);
        timeout++;
        if(timeout > 400)
        {
            debug("EVE Initialization failed");
            delete this;
            return;
        }
    }
    timeout = 0;
    while(0x00 != (m_hal->rd8(REG_CPURESET) & 0x03)) /* check if EVE is in working status */
    {
        DELAY_MS(1)
        timeout++;
        if(timeout > 50) /* experimental, 10 was the lowest value to get the BT815 started with, the touch-controller was the last to get out of reset */
        {
            debug("EVE Initialization failed");
            delete this;
            return;
        }
    }

    /* we have a display with a Goodix GT911 / GT9271 touch-controller on it, so we patch our FT811 or FT813 according to AN_336 or setup a BT815 accordingly */
    #if defined(EVE_HAS_GT911)

        #if defined(BT81X_ENABLE)
    m_hal->wr32(REG_TOUCH_CONFIG, 0x000005d1); /* switch to Goodix touch controller */
        #else
    uint32_t ftAddress;

    EVE_get_cmdoffset();
    ftAddress = EVE_RAM_CMD + cmdOffset;

    EVE_cs_set();
    spi_transmit((uint8_t)(ftAddress >> 16) | MEM_WRITE); /* send Memory Write plus high address byte */
    spi_transmit((uint8_t)(ftAddress >> 8));              /* send middle address byte */
    spi_transmit((uint8_t)(ftAddress));                   /* send low address byte */
    spi_flash_write(EVE_GT911_data, EVE_GT911_len);
    EVE_cs_clear();
    EVE_cmd_execute();

    m_hal->wr8(REG_TOUCH_OVERSAMPLE, 0x0f); /* setup oversample to 0x0f as "hidden" in binary-blob for AN_336 */
    m_hal->wr16(REG_TOUCH_CONFIG, 0x05D0);  /* write magic cookie as requested by AN_336 */

    /* specific to the EVE2 modules from Matrix-Orbital we have to use GPIO3 to reset GT911 */
    m_hal->wr16(REG_GPIOX_DIR, 0x8008); /* Reset-Value is 0x8000, adding 0x08 sets GPIO3 to output, default-value for REG_GPIOX is 0x8000 -> Low output on GPIO3 */
    DELAY_MS(1);                        /* wait more than 100?s */
    m_hal->wr8(REG_CPURESET, 0x00);     /* clear all resets */
    DELAY_MS(56);                       /* wait more than 55ms */
    m_hal->wr16(REG_GPIOX_DIR, 0x8000); /* setting GPIO3 back to input */
        #endif
    #endif

    /*	m_hal->wr8(REG_PCLK, 0x00);	*/ /* set PCLK to zero - don't clock the LCD until later, line disabled because zero is reset-default and we just did a reset */

    #if defined(EVE_ADAM101)
    m_hal->wr8(REG_PWM_DUTY, 0x80); /* turn off backlight for Glyn ADAM101 module, it uses inverted values */
    #else
    m_hal->wr8(REG_PWM_DUTY, 0);    /* turn off backlight for any other module */
    #endif

    /* Initialize Display */
    m_hal->wr16(REG_HSIZE, EVE_HSIZE);     /* active display width */
    m_hal->wr16(REG_HCYCLE, EVE_HCYCLE);   /* total number of clocks per line, incl front/back porch */
    m_hal->wr16(REG_HOFFSET, EVE_HOFFSET); /* start of active line */
    m_hal->wr16(REG_HSYNC0, EVE_HSYNC0);   /* start of horizontal sync pulse */
    m_hal->wr16(REG_HSYNC1, EVE_HSYNC1);   /* end of horizontal sync pulse */
    m_hal->wr16(REG_VSIZE, EVE_VSIZE);     /* active display height */
    m_hal->wr16(REG_VCYCLE, EVE_VCYCLE);   /* total number of lines per screen, including pre/post */
    m_hal->wr16(REG_VOFFSET, EVE_VOFFSET); /* start of active screen */
    m_hal->wr16(REG_VSYNC0, EVE_VSYNC0);   /* start of vertical sync pulse */
    m_hal->wr16(REG_VSYNC1, EVE_VSYNC1);   /* end of vertical sync pulse */
    m_hal->wr8(REG_SWIZZLE, EVE_SWIZZLE);  /* FT8xx output to LCD - pin order */
    m_hal->wr8(REG_PCLK_POL, EVE_PCLKPOL); /* LCD data is clocked in on this PCLK edge */
    m_hal->wr8(REG_CSPREAD, EVE_CSPREAD);  /* helps with noise, when set to 1 fewer signals are changed simultaneously, reset-default: 1 */

    /* do not set PCLK yet - wait for just after the first display list */

    /* configure Touch */
    m_hal->wr8(REG_TOUCH_MODE, EVE_TMODE_CONTINUOUS);    /* enable touch */
    m_hal->wr16(REG_TOUCH_RZTHRESH, EVE_TOUCH_RZTHRESH); /* eliminate any false touches */

    /* disable Audio for now */
    m_hal->wr8(REG_VOL_PB, 0x00);    /* turn recorded audio volume down */
    m_hal->wr8(REG_VOL_SOUND, 0x00); /* turn synthesizer volume off */
    m_hal->wr16(REG_SOUND, 0x6000);  /* set synthesizer to mute */

    /* write a basic display-list to get things started */
    m_hal->wr32(EVE_RAM_DL, DL_CLEAR_RGB);
    m_hal->wr32(EVE_RAM_DL + 4, (DL_CLEAR | CLR_COL | CLR_STN | CLR_TAG));
    m_hal->wr32(EVE_RAM_DL + 8, DL_DISPLAY); /* end of display list */
    m_hal->wr32(REG_DLSWAP, EVE_DLSWAP_FRAME);

    /* nothing is being displayed yet... the pixel clock is still 0x00 */
    m_hal->wr8(REG_GPIO, 0x80);     /* enable the DISP signal to the LCD panel, it is set to output in REG_GPIO_DIR by default */
    m_hal->wr8(REG_PCLK, EVE_PCLK); /* now start clocking data to the LCD panel */

    #if defined(EVE_ADAM101)
    m_hal->wr8(REG_PWM_DUTY, 0x60); /* turn on backlight to 25% for Glyn ADAM101 module, it uses inverted values */
    #else
    m_hal->wr8(REG_PWM_DUTY, 0x20); /* turn on backlight to 25% for any other module */
    #endif

    //    timeout = 0;
    //    while(EVE_busy() == 1) /* just to be safe, should not even enter the loop */
    //    {
    //        DELAY_MS(1);
    //        timeout++;
    //        if(timeout > 4)
    //        {
    //            break; /* something is wrong here, but since we made it this far through the init, just leave the loop */
    //        }
    //    }

    //    #if defined(EVE_DMA)
    //    EVE_init_dma(); /* prepare DMA */
    //    #endif

    /* tell EVE that we changed the frequency from default to 72MHz for BT8xx */
    #if defined(BT81X_ENABLE)
    m_hal->wr32(REG_FREQUENCY, 72000000);
    #endif

    //Clear interrupt flags for prevent fake interrupt calling after restart
    m_hal->rd8(REG_INT_FLAGS);
    //Disable interrupts
    m_hal->wr8(REG_INT_EN, 0x0);
    //Clear interrupt mask
    m_hal->wr8(REG_INT_MASK, 0x0);

    if(sharedEventQueue == true)
    {
        m_queue = mbed_event_queue();
    }
    else
    {
        m_queue = new EventQueue(6 * EVENTS_EVENT_SIZE);
        m_eventThread->start(callback(m_queue, &EventQueue::dispatch_forever));
    }
    m_interrupt.fall(m_queue->event(callback(this, &FT8xx::interruptFound)));
    m_hal->setSPIfrequency(spiFrequency);

    m_eventFlags.set(EVEeventFlags::CoProBusy
                     | EVEeventFlags::CmdBufBusy);
}
#elif
/*!
 * \brief FT8xx::FT8xx driver for FTDI FT8xx-BT8xx
 */
FT8xx(PinName               mosi,
      PinName               miso,
      PinName               sclk,
      PinName               ssel,
      PinName               pd,
      EVE_HAL::SPIFrequency spiFrequency)
{
    EVE_init();
    m_hal->setSPIfrequency(spiFrequency);
}
#endif

FT8xx::~FT8xx()
{
    m_queue->break_dispatch();
    m_eventThread->terminate();
    delete m_queue;
    delete m_eventThread;
    if(m_ramG)
        delete m_ramG;
#if defined(BT81X_ENABLE)
    if(m_flash)
        delete m_flash;
#endif
    delete m_hal;
}

const FT8xx::TouchCalibrationResult & FT8xx::touchCalibrate(bool factory)
{
    /* send pre-recorded touch calibration values, depending on the display the code is compiled for */
    if(factory == true)
    {
#if defined(EVE_CFAF240400C1_030SC)
        m_hal->wr32(REG_TOUCH_TRANSFORM_A, 0x0000ed11);
        m_hal->wr32(REG_TOUCH_TRANSFORM_B, 0x00001139);
        m_hal->wr32(REG_TOUCH_TRANSFORM_C, 0xfff76809);
        m_hal->wr32(REG_TOUCH_TRANSFORM_D, 0x00000000);
        m_hal->wr32(REG_TOUCH_TRANSFORM_E, 0x00010690);
        m_hal->wr32(REG_TOUCH_TRANSFORM_F, 0xfffadf2e);
#endif

#if defined(EVE_CFAF320240F_035T)
        m_hal->wr32(REG_TOUCH_TRANSFORM_A, 0x00005614);
        m_hal->wr32(REG_TOUCH_TRANSFORM_B, 0x0000009e);
        m_hal->wr32(REG_TOUCH_TRANSFORM_C, 0xfff43422);
        m_hal->wr32(REG_TOUCH_TRANSFORM_D, 0x0000001d);
        m_hal->wr32(REG_TOUCH_TRANSFORM_E, 0xffffbda4);
        m_hal->wr32(REG_TOUCH_TRANSFORM_F, 0x00f8f2ef);
#endif

#if defined(EVE_CFAF480128A0_039TC)
        m_hal->wr32(REG_TOUCH_TRANSFORM_A, 0x00010485);
        m_hal->wr32(REG_TOUCH_TRANSFORM_B, 0x0000017f);
        m_hal->wr32(REG_TOUCH_TRANSFORM_C, 0xfffb0bd3);
        m_hal->wr32(REG_TOUCH_TRANSFORM_D, 0x00000073);
        m_hal->wr32(REG_TOUCH_TRANSFORM_E, 0x0000e293);
        m_hal->wr32(REG_TOUCH_TRANSFORM_F, 0x00069904);
#endif

#if defined(EVE_CFAF800480E0_050SC)
        m_hal->wr32(REG_TOUCH_TRANSFORM_A, 0x000107f9);
        m_hal->wr32(REG_TOUCH_TRANSFORM_B, 0xffffff8c);
        m_hal->wr32(REG_TOUCH_TRANSFORM_C, 0xfff451ae);
        m_hal->wr32(REG_TOUCH_TRANSFORM_D, 0x000000d2);
        m_hal->wr32(REG_TOUCH_TRANSFORM_E, 0x0000feac);
        m_hal->wr32(REG_TOUCH_TRANSFORM_F, 0xfffcfaaf);
#endif

#if defined(EVE_PAF90)
        m_hal->wr32(REG_TOUCH_TRANSFORM_A, 0x00000159);
        m_hal->wr32(REG_TOUCH_TRANSFORM_B, 0x0001019c);
        m_hal->wr32(REG_TOUCH_TRANSFORM_C, 0xfff93625);
        m_hal->wr32(REG_TOUCH_TRANSFORM_D, 0x00010157);
        m_hal->wr32(REG_TOUCH_TRANSFORM_E, 0x00000000);
        m_hal->wr32(REG_TOUCH_TRANSFORM_F, 0x0000c101);
#endif

#if defined(EVE_RiTFT43)
        m_hal->wr32(REG_TOUCH_TRANSFORM_A, 0x000062cd);
        m_hal->wr32(REG_TOUCH_TRANSFORM_B, 0xfffffe45);
        m_hal->wr32(REG_TOUCH_TRANSFORM_C, 0xfff45e0a);
        m_hal->wr32(REG_TOUCH_TRANSFORM_D, 0x000001a3);
        m_hal->wr32(REG_TOUCH_TRANSFORM_E, 0x00005b33);
        m_hal->wr32(REG_TOUCH_TRANSFORM_F, 0xFFFbb870);
#endif

#if defined(EVE_EVE2_38)
        m_hal->wr32(REG_TOUCH_TRANSFORM_A, 0x00007bed);
        m_hal->wr32(REG_TOUCH_TRANSFORM_B, 0x000001b0);
        m_hal->wr32(REG_TOUCH_TRANSFORM_C, 0xfff60aa5);
        m_hal->wr32(REG_TOUCH_TRANSFORM_D, 0x00000095);
        m_hal->wr32(REG_TOUCH_TRANSFORM_E, 0xffffdcda);
        m_hal->wr32(REG_TOUCH_TRANSFORM_F, 0x00829c08);
#endif

#if defined(EVE_EVE2_35G)
        m_hal->wr32(REG_TOUCH_TRANSFORM_A, 0x000109E4);
        m_hal->wr32(REG_TOUCH_TRANSFORM_B, 0x000007A6);
        m_hal->wr32(REG_TOUCH_TRANSFORM_C, 0xFFEC1EBA);
        m_hal->wr32(REG_TOUCH_TRANSFORM_D, 0x0000072C);
        m_hal->wr32(REG_TOUCH_TRANSFORM_E, 0x0001096A);
        m_hal->wr32(REG_TOUCH_TRANSFORM_F, 0xFFF469CF);
#endif

#if defined(EVE_EVE2_43G)
        m_hal->wr32(REG_TOUCH_TRANSFORM_A, 0x0000a1ff);
        m_hal->wr32(REG_TOUCH_TRANSFORM_B, 0x00000680);
        m_hal->wr32(REG_TOUCH_TRANSFORM_C, 0xffe54cc2);
        m_hal->wr32(REG_TOUCH_TRANSFORM_D, 0xffffff53);
        m_hal->wr32(REG_TOUCH_TRANSFORM_E, 0x0000912c);
        m_hal->wr32(REG_TOUCH_TRANSFORM_F, 0xfffe628d);
#endif

#if defined(EVE_EVE2_50G)
        m_hal->wr32(REG_TOUCH_TRANSFORM_A, 0x000109E4);
        m_hal->wr32(REG_TOUCH_TRANSFORM_B, 0x000007A6);
        m_hal->wr32(REG_TOUCH_TRANSFORM_C, 0xFFEC1EBA);
        m_hal->wr32(REG_TOUCH_TRANSFORM_D, 0x0000072C);
        m_hal->wr32(REG_TOUCH_TRANSFORM_E, 0x0001096A);
        m_hal->wr32(REG_TOUCH_TRANSFORM_F, 0xFFF469CF);
#endif

#if defined(EVE_EVE2_70G)
        m_hal->wr32(REG_TOUCH_TRANSFORM_A, 0x000105BC);
        m_hal->wr32(REG_TOUCH_TRANSFORM_B, 0xFFFFFA8A);
        m_hal->wr32(REG_TOUCH_TRANSFORM_C, 0x00004670);
        m_hal->wr32(REG_TOUCH_TRANSFORM_D, 0xFFFFFF75);
        m_hal->wr32(REG_TOUCH_TRANSFORM_E, 0x00010074);
        m_hal->wr32(REG_TOUCH_TRANSFORM_F, 0xFFFF14C8);
#endif

#if defined(EVE_NHD_35)
        m_hal->wr32(REG_TOUCH_TRANSFORM_A, 0x0000f78b);
        m_hal->wr32(REG_TOUCH_TRANSFORM_B, 0x00000427);
        m_hal->wr32(REG_TOUCH_TRANSFORM_C, 0xfffcedf8);
        m_hal->wr32(REG_TOUCH_TRANSFORM_D, 0xfffffba4);
        m_hal->wr32(REG_TOUCH_TRANSFORM_E, 0x0000f756);
        m_hal->wr32(REG_TOUCH_TRANSFORM_F, 0x0009279e);
#endif

#if defined(EVE_RVT70)
        m_hal->wr32(REG_TOUCH_TRANSFORM_A, 0x000074df);
        m_hal->wr32(REG_TOUCH_TRANSFORM_B, 0x000000e6);
        m_hal->wr32(REG_TOUCH_TRANSFORM_C, 0xfffd5474);
        m_hal->wr32(REG_TOUCH_TRANSFORM_D, 0x000001af);
        m_hal->wr32(REG_TOUCH_TRANSFORM_E, 0x00007e79);
        m_hal->wr32(REG_TOUCH_TRANSFORM_F, 0xffe9a63c);
#endif

#if defined(EVE_FT811CB_HY50HD)
        m_hal->wr32(REG_TOUCH_TRANSFORM_A, 66353);
        m_hal->wr32(REG_TOUCH_TRANSFORM_B, 712);
        m_hal->wr32(REG_TOUCH_TRANSFORM_C, 4293876677);
        m_hal->wr32(REG_TOUCH_TRANSFORM_D, 4294966157);
        m_hal->wr32(REG_TOUCH_TRANSFORM_E, 67516);
        m_hal->wr32(REG_TOUCH_TRANSFORM_F, 418276);
#endif

#if defined(EVE_ADAM101)
        m_hal->wr32(REG_TOUCH_TRANSFORM_A, 0x000101E3);
        m_hal->wr32(REG_TOUCH_TRANSFORM_B, 0x00000114);
        m_hal->wr32(REG_TOUCH_TRANSFORM_C, 0xFFF5EEBA);
        m_hal->wr32(REG_TOUCH_TRANSFORM_D, 0xFFFFFF5E);
        m_hal->wr32(REG_TOUCH_TRANSFORM_E, 0x00010226);
        m_hal->wr32(REG_TOUCH_TRANSFORM_F, 0x0000C783);
#endif
        FT8xx::TouchCalibrationResult res;
        res.touch_a = m_hal->rd32(REG_TOUCH_TRANSFORM_A);
        res.touch_b = m_hal->rd32(REG_TOUCH_TRANSFORM_B);
        res.touch_c = m_hal->rd32(REG_TOUCH_TRANSFORM_C);
        res.touch_d = m_hal->rd32(REG_TOUCH_TRANSFORM_D);
        res.touch_e = m_hal->rd32(REG_TOUCH_TRANSFORM_E);
        res.touch_f = m_hal->rd32(REG_TOUCH_TRANSFORM_F);
        return std::move(res);
    }
    else
    {
        /* write down the numbers on the screen and either place them in one of the pre-defined blocks above or make a new block */
        /* calibrate touch and displays values to screen */
        push(CMD_DLSTART);
        push(DL_CLEAR_RGB | COLOR_RGB(0, 0, 0));
        push(DL_CLEAR | CLR_COL | CLR_STN | CLR_TAG);
        text((EVE_HSIZE / 2), 50, 26, EVE_OPT_CENTER, "Please tap on the dot.");
        push(CMD_CALIBRATE);
        push({0x00});
        push(DL_DISPLAY);
        push(CMD_SWAP);
        execute();

        FT8xx::TouchCalibrationResult res;
        res.touch_a = m_hal->rd32(REG_TOUCH_TRANSFORM_A);
        res.touch_b = m_hal->rd32(REG_TOUCH_TRANSFORM_B);
        res.touch_c = m_hal->rd32(REG_TOUCH_TRANSFORM_C);
        res.touch_d = m_hal->rd32(REG_TOUCH_TRANSFORM_D);
        res.touch_e = m_hal->rd32(REG_TOUCH_TRANSFORM_E);
        res.touch_f = m_hal->rd32(REG_TOUCH_TRANSFORM_F);
        return std::move(res);
    }
}

void FT8xx::setBacklight(uint8_t value)
{
#if defined(EVE_ADAM101)
    m_hal->wr8(REG_PWM_DUTY, 0x80 - value);
#else
    m_hal->wr8(REG_PWM_DUTY, value);
#endif
}

const RamG * FT8xx::ramG()
{
    return m_ramG;
}

uint8_t FT8xx::flashInit(uint32_t size)
{
    m_flash = new Flash(this, size);
    if(m_flash->flashStatus() == FlashStatus::FLASH_STATUS_DETACHED || m_flash->flashStatus() == FlashStatus::FLASH_STATUS_INIT)
    {
        delete m_flash;
        return -1;
    }
    return 0;
}

void FT8xx::rebootCoPro()
{
    /* we have a co-processor fault, make EVE play with us again */
#if defined(BT81X_ENABLE)
    uint16_t copro_patch_pointer = m_hal->rd16(REG_COPRO_PATCH_DTR);
#endif
    m_hal->wr8(REG_CPURESET, 1);   /* hold co-processor engine in the reset condition */
    m_hal->wr16(REG_CMD_READ, 0);  /* set REG_CMD_READ to 0 */
    m_hal->wr16(REG_CMD_WRITE, 0); /* set REG_CMD_WRITE to 0 */
    m_hal->wr32(REG_CMD_DL, 0);    /* reset REG_CMD_DL to 0 as required by the BT81x programming guide, should not hurt FT8xx */
    m_hal->wr8(REG_CPURESET, 0);   /* set REG_CMD_WRITE to 0 to restart the co-processor engine*/

#if defined(BT81X_ENABLE)

    m_hal->wr16(REG_COPRO_PATCH_DTR, copro_patch_pointer);

    DELAY_MS(5) /* just to be safe */

    m_hal->csSet();
    m_hal->write(static_cast<uint8_t>(EVE_RAM_CMD >> 16) | MEM_WRITE);
    m_hal->write(static_cast<uint8_t>(EVE_RAM_CMD >> 8));
    m_hal->write(static_cast<uint8_t>(EVE_RAM_CMD));

    m_hal->write(static_cast<uint8_t>(CMD_FLASHATTACH));
    m_hal->write(static_cast<uint8_t>(CMD_FLASHATTACH >> 8));
    m_hal->write(static_cast<uint8_t>(CMD_FLASHATTACH >> 16));
    m_hal->write(static_cast<uint8_t>(CMD_FLASHATTACH >> 24));

    m_hal->write(static_cast<uint8_t>(CMD_FLASHFAST));
    m_hal->write(static_cast<uint8_t>(CMD_FLASHFAST >> 8));
    m_hal->write(static_cast<uint8_t>(CMD_FLASHFAST >> 16));
    m_hal->write(static_cast<uint8_t>(CMD_FLASHFAST >> 24));
    m_hal->csClear();

    m_hal->csSet();
    m_hal->write(static_cast<uint8_t>(REG_CMD_WRITE >> 16) | MEM_WRITE);
    m_hal->write(static_cast<uint8_t>(REG_CMD_WRITE >> 8));
    m_hal->write(static_cast<uint8_t>(REG_CMD_WRITE));

    m_hal->write(static_cast<uint8_t>(8));
    m_hal->write(static_cast<uint8_t>(8 >> 8));

    m_hal->csClear();

    m_hal->wr8(REG_PCLK, EVE_PCLK); /* restore REG_PCLK in case it was set to zero by an error */

    DELAY_MS(5) /* just to be safe */
#endif
}

void FT8xx::writeString(const string & text)
{
    for(auto it = text.cbegin(); it != text.cend();)
    {
        CmdBuf_t value;
        for(uint8_t i = 0; i < 4; ++i)
        {
            value.byte[i] = *it;
            ++it;
            if(it == text.cend())
                break;
        }
        push(value);
    }
    //append 4 zero byte if text aligned to 4 byte
    if(text.size() % 4 == 0)
        push(0x0);
}

void FT8xx::push(const CmdBuf_t & command)
{
    //Block any modifying cmd buffer while is not sended
    m_eventFlags.wait_all(EVEeventFlags::CmdBufBusy, osWaitForever, false);
    m_ramDLobserver += 4;
    m_cmdBuffer.push_back(command);
}

void FT8xx::execute()
{
    //if Nothing to execute
    if(m_cmdBuffer.size() == 0)
    {
        debug("if Nothing to execute\n");
        return;
    }
    //if RamDL will be overflow
    if(m_ramDLobserver > EVE_RAM_DL_SIZE)
    {
        debug("Ram DL overflow!\n");
        //Clear cmdBuffer
        m_cmdBuffer.clear();
        m_ramDLobserver = 0;
        //clear cmdBuffer lock flag
        m_eventFlags.set(CmdBufBusy);
        return;
    }
    //If cmdBuffer will be overflow
    if(m_cmdBuffer.size() > EVE_CMDFIFO_SIZE)
    {
        debug("cmdBuffer overflow\n");
        return;
    }
    //Blocking any operation with CmdBuffer while it is not sended to EVE FIFO
    m_eventFlags.clear(EVEeventFlags::CmdBufBusy);
    //If CoPro busy now - wait
    m_eventFlags.wait_any(EVEeventFlags::CoProBusy);

    m_hal->csSet();
    m_hal->write(static_cast<uint8_t>((REG_CMDB_WRITE) >> 16) | MEM_WRITE);
    m_hal->write(static_cast<uint8_t>((REG_CMDB_WRITE) >> 8));
    m_hal->write(static_cast<uint8_t>(REG_CMDB_WRITE));

    for(const auto & u : m_cmdBuffer)
    {
        m_hal->write(u.byte[0]);
        m_hal->write(u.byte[1]);
        m_hal->write(u.byte[2]);
        m_hal->write(u.byte[3]);
    }
    m_hal->csClear();

    //If CoPro commands fault reboot it
    if(m_hal->rd16(REG_CMD_READ) == 0xFFF)
    {
        debug("CoPro error. Reboot started!\n");
        rebootCoPro();
        debug("EVE Reboot compleated!\n");
        m_eventFlags.set(EVEeventFlags::CoProBusy);
        return;
    }
    //Clear cmdBuffer
    m_cmdBuffer.clear();
    m_ramDLobserver = 0;
    //clear cmdBuffer lock flag
    m_eventFlags.set(CmdBufBusy);
    //Wait while CoPro working
    for(uint32_t i = 0; i < 100; ++i)
    {
        if(m_hal->rd16(REG_CMDB_SPACE) == 4092)
            break;
        ThisThread::sleep_for(1);
        if(i == 99)
        {
            debug("CoPro error. Reboot started!\n");
            rebootCoPro();
            m_eventFlags.set(EVEeventFlags::CoProBusy);
            return;
        }
    }

    m_eventFlags.set(EVEeventFlags::CoProBusy);
}

void FT8xx::display()
{
    if(m_cmdBuffer.back().word != DL_END)
        push(DL_END);
    push(DL_DISPLAY);
}

void FT8xx::swap()
{
    if(m_cmdBuffer.back().word != DL_DISPLAY)
        display();
    push(CMD_SWAP);
}

void FT8xx::clear(bool colorBuf, bool stencilBuf, bool tagBuf)
{
    push(CLEAR(static_cast<uint8_t>(colorBuf),
               static_cast<uint8_t>(stencilBuf),
               static_cast<uint8_t>(tagBuf)));
}

void FT8xx::clearColorRGB(uint8_t r, uint8_t g, uint8_t b)
{
    push(EVE::clearColorRGB(r, g, b));
}

void FT8xx::clearColorA(uint8_t a)
{
    push(EVE::clearColorA(a));
}

void FT8xx::clearColorARGB(const FT8xx::CmdBuf_t & argb)
{
    clearColorA(argb.byte[3]);
    clearColorRGB(argb.byte[2], argb.byte[1], argb.byte[0]);
}

void FT8xx::colorRGB(uint8_t r, uint8_t g, uint8_t b)
{
    push(EVE::colorRGB(r, g, b));
}

void FT8xx::colorA(uint8_t a)
{
    push(EVE::colorA(a));
}

void FT8xx::colorARGB(const FT8xx::CmdBuf_t & argb)
{
    colorA((argb.byte[3]));
    colorRGB(argb.byte[2], argb.byte[1], argb.byte[0]);
}

void FT8xx::vertexPointII(uint16_t x, uint16_t y, uint16_t handle, uint16_t cell)
{
    debug_if(x > 511 || y > 511, "x and  y must be <511\n");
    push(vertex2ii(x,
                   y,
                   handle,
                   cell));
}

//*************************
//*********Drawing functions
void FT8xx::vertexPointF(int16_t x, int16_t y)
{
    switch(m_pixelPrecision)
    {
    case FT8xx::Div_1:
        debug_if(x > 16383
                     || y > 16383
                     || x < -16384
                     || y < -16384,
                 "x and  y must be between -16384 to 16383\n");
        push(vertex2f(x,
                      y));
        break;
    case FT8xx::Div_2:
        debug_if(x > 8191
                     || y > 8191
                     || x < -8192
                     || y < -8192,
                 "x and  y must be between -8192 to 8191\n");
        push(vertex2f(x * 2,
                      y * 2));
        break;
    case FT8xx::Div_4:
        debug_if(x > 4095
                     || y > 4095
                     || x < -4096
                     || y < -4096,
                 "x and  y must be between -4096 to 4095\n");
        push(vertex2f(x * 4,
                      y * 4));
        break;
    case FT8xx::Div_8:
        debug_if(x > 2047
                     || y > 2047
                     || x < -2048
                     || y < -2048,
                 "x and  y must be between -2048 to 2047\n");
        push(vertex2f(x * 8,
                      y * 8));
        break;
    case FT8xx::Div_16:
        debug_if(x > 1023
                     || y > 1023
                     || x < -1024
                     || y < -1024,
                 "x and  y must be between -1024 to 1023\n");
        push(vertex2f(x * 16,
                      y * 16));
        break;
    }
}

void FT8xx::point(int16_t x, int16_t y, uint16_t size)
{
    pointSize(size * 16);
    begin(Points);
    vertexPointF(x, y);
    end();
}

void FT8xx::line(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t width)
{
    begin(Lines);
    lineWidth(width * 16);
    vertexPointF(x0, y0);
    vertexPointF(x1, y1);
    end();
}

void FT8xx::rectangle(int16_t x, int16_t y, int16_t width, int16_t height, uint16_t radius)
{
    debug_if(radius == 0, "Radius must be > 0\n");
    begin(Rects);
    lineWidth(radius * 16);
    vertexPointF(x, y);
    vertexPointF(x + width - 1, y + height - 1);
    end();
}

void FT8xx::gradient(uint16_t x0,
                     uint16_t y0,
                     uint32_t rgb0,
                     uint16_t x1,
                     uint16_t y1,
                     uint32_t rgb1)
{
    push(CMD_GRADIENT);
    push({x0, y0});
    push(rgb0);
    push({x1, y1});
    push(rgb1);
}

void FT8xx::gradientA(uint16_t x0,
                      uint16_t y0,
                      uint32_t argb0,
                      uint16_t x1,
                      uint16_t y1,
                      uint32_t argb1)
{
    push(CMD_GRADIENTA);
    push({x0, y0});
    push(argb0);
    push({x1, y1});
    push(argb1);
}

void FT8xx::text(uint16_t       x,
                 uint16_t       y,
                 uint16_t       font,
                 uint16_t       options,
                 const string & text)
{
    if(text.size() == 0)
        return;
    push(CMD_TEXT);
    push({x, y});
    push({font, options});

    writeString(text);
}

void FT8xx::button(uint16_t       x,
                   uint16_t       y,
                   uint16_t       width,
                   uint16_t       height,
                   uint16_t       font,
                   const string & text,
                   ButtonOpt      options)
{
    push(CMD_BUTTON);
    push({x, y});
    push({width, height});
    push({font, static_cast<uint16_t>(options)});

    writeString(text);
}

void FT8xx::clock(uint16_t x,
                  uint16_t y,
                  uint16_t radius,
                  uint16_t h,
                  uint16_t m,
                  uint16_t s,
                  uint16_t ms,
                  ClockOpt options)
{
    push(CMD_CLOCK);
    push({x, y});
    push({radius, static_cast<uint16_t>(options)});
    push({h, m});
    push({s, ms});
}

void FT8xx::gauge(uint16_t x,
                  uint16_t y,
                  uint16_t radius,
                  uint16_t major,
                  uint16_t minor,
                  uint16_t val,
                  uint16_t range,
                  GaugeOpt options)
{
    push(CMD_GAUGE);
    push({x, y});
    push({radius, static_cast<uint16_t>(options)});
    push({major, minor});
    push({val, range});
}

void FT8xx::append(uint32_t address, uint32_t count)
{
    push(CMD_APPEND);
    push({address});
    push({count});
    m_ramDLobserver += count - 12;
}

void FT8xx::append(StoredObject * o)
{
    switch(o->type())
    {
    case EVE::UnknowType:
        debug("Unknow Type\n");
        break;
    case EVE::DisplayListType:
        append(reinterpret_cast<DisplayList *>(o));
        break;
    case EVE::SnapshotType:
        append(reinterpret_cast<Snapshot *>(o));
        break;
    case EVE::ImageType:
        debug("ImageType not supported yet\n");
        break;
    }
}

void FT8xx::append(const DisplayList * dl)
{
    append(dl->address(), dl->size());
}

void FT8xx::append(const Snapshot * s,
                   int16_t          x,
                   int16_t          y,
                   int16_t          width,
                   int16_t          height)
{
    if(s->format() == SnapshotBitmapFormat::ARGB8_s)
    {
        debug("You don't load snapshot stored in ARGB8_s format \n");
        return;
    }
    push(CMD_SETBITMAP);
    push(s->address());
    push({static_cast<uint16_t>(s->format()),
          width < 0 ? s->width() : static_cast<uint16_t>(width)});
    push({height < 0 ? s->height() : static_cast<uint16_t>(height),
          0});

    begin(Bitmaps);
    vertexPointII(x < 0 ? s->x() : x,
                  y < 0 ? s->y() : y);
    this->end();
}

void FT8xx::ramGInit(uint32_t size)
{
    m_ramG = new RamG(this, size);
}
//*********************************************************************************
#if(MBED_VERSION >= MBED_ENCODE_VERSION(5, 8, 0)) && MBED_CONF_EVENTS_PRESENT

void FT8xx::backlightFade(uint8_t from, uint8_t to, uint32_t duration, FadeType fadeType, uint8_t delay)
{
    //Check if fade is not started now
    if(m_fadeBlock == true)
        return;
    //block calling fade when already started
    m_fadeBlock = true;

    BacklightFade bf{
        0,
        static_cast<int32_t>(duration),
        static_cast<int16_t>(to - from),
        from,
        from,
        delay,
        fadeType,
    };
    m_queue->call(this, &FT8xx::p_backlightFade, bf);
}

void FT8xx::interruptFound()
{
    uint8_t flag = m_hal->rd8(REG_INT_FLAGS);
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
            m_tagNumberCallback(m_hal->rd8(REG_TOUCH_TAG));
        }
        if(m_tagCallbacksPool.size() != 0)
        {
            uint8_t tag = m_hal->rd8(REG_TOUCH_TAG);
            for(const auto & f : m_tagCallbacksPool)
            {
                if(f.tagNumber == tag)
                    f.callback(tag);
            }
        }
    }

    if((flag & EVE_INT_SOUND) != 0)
    {
        //        printf("EVE_INT_SOUND: %04x \n", flag);
    }

    if((flag & EVE_INT_PLAYBACK) != 0)
    {
        //        printf("EVE_INT_PLAYBACK: %04x \n", flag);
    }

    if((flag & EVE_INT_CMDEMPTY) != 0)
    {
        //        printf("EVE_INT_CMDEMPTY: %04x \n", flag);
    }

    if((flag & EVE_INT_CMDFLAG) != 0)
    {
        //        printf("EVE_INT_CMDFLAG: %04x \n", flag);
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
    if(m_hal->rd8(REG_INT_EN) == 0x1)
    {
        interruptMask = m_hal->rd8(REG_INT_MASK);
    }
    interruptMask |= flag;
    switch(flag)
    {
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
        //        printf("%04x \n", flag);
        break;
    case EVE_INT_PLAYBACK:
        //        printf("%04x \n", flag);
        break;
    case EVE_INT_CMDEMPTY:
        //        printf("%04x \n", flag);
        break;
    case EVE_INT_CMDFLAG:
        //        printf("%04x \n", flag);
        break;
    case EVE_INT_CONVCOMPLETE:
        m_touchConvCompCallback = f;
        break;
    default:
        return;
    }
    //set interrupts mask
    m_hal->wr8(REG_INT_MASK, interruptMask);
    //enable interrupts
    m_hal->wr8(REG_INT_EN, 0x1);
}

void FT8xx::attachToTags(mbed::Callback<void(uint8_t)> f)
{
    uint8_t interruptMask{0};
    if(m_hal->rd8(REG_INT_EN) == 0x1)
    {
        interruptMask = m_hal->rd8(REG_INT_MASK);
    }
    interruptMask |= EVE_INT_TAG;
    m_tagNumberCallback = f;
    //set interrupts mask
    m_hal->wr8(REG_INT_MASK, interruptMask);
    //enable interrupts
    m_hal->wr8(REG_INT_EN, 0x1);
}

void FT8xx::attachToTag(mbed::Callback<void(uint8_t)> f, uint8_t tag)
{
    //First calling needed to enable TAG interrupt
    if(m_hal->rd8(REG_INT_EN) == 0x0)
    {
        uint8_t interruptMask = m_hal->rd8(REG_INT_MASK);
        interruptMask |= EVE_INT_TAG;
        //set interrupts mask
        m_hal->wr8(REG_INT_MASK, interruptMask);
        //enable interrupts
        m_hal->wr8(REG_INT_EN, 0x1);
    }
    for(auto & c : m_tagCallbacksPool)
    {
        if(c.tagNumber == tag)
        {
            c.callback = f;
            return;
        }
    }
    m_tagCallbacksPool.push_back(TagCallback{tag, f});
}

void FT8xx::deattachFromTag(uint8_t tag)
{
    m_tagCallbacksPool.erase(
        std::remove_if(
            m_tagCallbacksPool.begin(),
            m_tagCallbacksPool.end(),
            [&](const TagCallback & c) {
                if(c.tagNumber == tag)
                {
                    delete c.cbPonter;
                    return true;
                }
                return false;
            }),
        m_tagCallbacksPool.end());
}

uint8_t FT8xx::setCallbackToTag(mbed::Callback<void(uint8_t)> f)
{
    //Switch on tag interrupt if this a first call
    if(m_tagCallbacksPool.size() == 0)
        enableTagInterrupt();

    //Check tag pool size
    if(m_tagCallbacksPool.size() > 254)
    {
        debug("TagPool is full");
        return 0;
    }

    TagCallback cb{findFirstEmptyTag(),
                   f};
    m_tagCallbacksPool.push_back(cb);
    return cb.tagNumber;
}

uint8_t FT8xx::findFirstEmptyTag()
{
    if(m_tagCallbacksPool.empty())
        return 1;

    std::sort(m_tagCallbacksPool.begin(),
              m_tagCallbacksPool.end());

    uint8_t next = 1;
    for(const auto & t : m_tagCallbacksPool)
    {
        if(t.tagNumber != next)
            return next;
        ++next;
    }
    return next;
}

void FT8xx::enableTagInterrupt()
{
    if(m_hal->rd8(REG_INT_EN) == 0x0)
    {
        uint8_t interruptMask = m_hal->rd8(REG_INT_MASK);
        interruptMask |= EVE_INT_TAG;
        //set interrupts mask
        m_hal->wr8(REG_INT_MASK, interruptMask);
        //enable interrupts
        m_hal->wr8(REG_INT_EN, 0x1);
    }
}

void FT8xx::p_backlightFade(BacklightFade bf)
{
    if(bf.cycCount <= bf.duration)
    {
        switch(bf.fadeType)
        {
        case Linear:
            bf.value = static_cast<uint8_t>((bf.range * bf.cycCount) / bf.duration + bf.start);
            break;
        case Quad:
        {
            float t  = bf.cycCount / bf.duration;
            bf.value = static_cast<uint8_t>(bf.range * t * t + bf.start);
            break;
        }
        case Cubic:
        {
            float t  = bf.cycCount / bf.duration;
            bf.value = static_cast<uint8_t>(bf.range * t * t * t + bf.start);
            break;
        }
        case Quart:
        {
            float t  = bf.cycCount / bf.duration;
            bf.value = static_cast<uint8_t>(bf.range * t * t * t * t + bf.start);
            break;
        }
        }
        setBacklight(bf.value);
        bf.cycCount += bf.freq;
        m_queue->call_in(bf.freq, this, &FT8xx::p_backlightFade, bf);
        //        printf("%u \n", bf.value);
    }
    else
    {
        m_fadeBlock = false;
    }
}
#endif
//***********************************************************************
