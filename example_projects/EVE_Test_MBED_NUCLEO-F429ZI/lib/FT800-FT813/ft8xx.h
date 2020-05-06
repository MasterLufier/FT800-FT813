/*!
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
#include <algorithm>
#include <functional>
#include <vector>

namespace EVE
{
struct FTDisplayList
{
    std::string m_name{};
    uint32_t    m_address{0};
    uint32_t    m_size{0};
    FTDisplayList(string name, uint32_t address, uint32_t size) :
        m_name(name), m_address(address), m_size(size)
    {}
};

class FT8xx : private NonCopyable<FT8xx>
{
public:
    enum PixelPrecision : uint8_t
    {
        Div_1,
        Div_2,
        Div_4,
        Div_8,
        Div_16
    };

    //Special container for 8-32 byte commands
    union CmdBuf_t
    {
        uint32_t word{0x00000000u};
        uint8_t  byte[4];
        uint16_t halfWord[2];
        CmdBuf_t(uint32_t data = 0x0) :
            word{data} { MBED_STATIC_ASSERT(sizeof(this) == sizeof(uint32_t), "CmdBuf_t: Padding detected"); }
        CmdBuf_t(uint16_t d1, uint16_t d2) :
            halfWord{d1, d2} {}
        CmdBuf_t(uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4) :
            byte{d1, d2, d3, d4} {}
    };

#if(MBED_VERSION >= MBED_ENCODE_VERSION(5, 8, 0)) && MBED_CONF_EVENTS_PRESENT
    struct TouchCalibrationResult
    {
        uint32_t touch_a{}, touch_b{}, touch_c{}, touch_d{}, touch_e{}, touch_f{};
    };

    enum FadeType : uint8_t{Linear, Quad, Cubic, Quart};

    FT8xx(
        PinName               mosi,
        PinName               miso,
        PinName               sclk,
        PinName               ssel,
        PinName               pd,
        PinName               interrupt,
        EVE_HAL::SPIFrequency spiFrequency     = EVE_HAL::F_20M,
        bool                  sharedEventQueue = false,
        uint32_t              threadStackSize  = (3 * 512),
        const char *          threadName       = "FT8xxThrd");
#elif
    FT8xx(PinName               mosi,
          PinName               miso,
          PinName               sclk,
          PinName               ssel,
          PinName               pd,
          EVE_HAL::SPIFrequency spiFrequency = EVE_HAL::F_20M);
#endif
    ~FT8xx();

    //*********Basic Commands
    /*!
     * \brief Push command to cmdBuffer
     * \param b - command to push
     */
    void push(const CmdBuf_t & b);

    /*!
     * \brief Load cmdBuffer to EVE cmd FIFO and start processing to copy result to Ram_DL
     *  \note now this function support only FT/BT81X
     */
    void execute();

    inline void dlStart() { push(CMD_DLSTART); }
    inline void dlEnd() { push(DL_END); }
    inline void begin(GraphicPrimitives prim) { push(EVE::begin(prim)); }
    inline void end() { push(EVE::end()); }
    void        display();
    void        swap();

    void clear(bool colorBuf   = true,
               bool stencilBuf = true,
               bool tagBuf     = true);

    void clearColorRGB(uint8_t r, uint8_t g, uint8_t b);
    void clearColorA(uint8_t a);
    void clearColorARGB(const CmdBuf_t & argb);

    void colorRGB(uint8_t r, uint8_t g, uint8_t b);
    void colorA(uint8_t a);
    void colorARGB(const CmdBuf_t & argb);

    inline void pointSize(uint16_t size) { push(EVE::pointSize(size)); }
    inline void lineWidth(uint16_t width) { push(EVE::lineWidth(width)); }

    //*************************
    //*********Drawing functions
    void vertexPointF(uint32_t x1,
                      uint32_t y1);

    void point(uint16_t x,
               uint16_t y,
               uint16_t size);

    void line(uint16_t x0,
              uint16_t y0,
              uint16_t x1,
              uint16_t y1,
              uint16_t width);

    void rectangle(uint16_t x,
                   uint16_t y,
                   uint16_t width,
                   uint16_t height,
                   uint16_t radius);
    //*************************
    //*********Drawing graphics objects
    void text(uint16_t            x,
              uint16_t            y,
              uint16_t            font,
              uint16_t            options,
              const std::string & text);
    //*************************
    //*********Special commands

#if defined(FT81X_ENABLE)
    void append(uint32_t address, uint32_t count);
    void append(const FTDisplayList * dl);
#endif
    //**************************

    //***********Ram G Commands
    void ramGInit(uint32_t size = EVE_RAM_G_SAFETY_SIZE) { m_ramG = new RamG(size); }

    /*!
     * \brief Saved curent Ram_DL data to Ram_G for next using with append(...) function for reduce SPI overhead
     * \param name Display list name
     * \return pointer to display list memory object
     */
    FTDisplayList * saveDisplayList(string name);
    //****************

    /*!
     * \brief touchCalibrate - function for calibrate touchscreen
     * \param factory - if true - load factory calibration, else - start new calibration
     */
    const FT8xx::TouchCalibrationResult & touchCalibrate(bool factory = true);

    /*!
     * \brief setBacklight - set the backlight PWM duty cycle
     * \param value - value from 0(off) to 128(full)
     */
    void setBacklight(uint8_t value);
//****************************************************************

//***Next commands works only if MBED Thread enabled
#if(MBED_VERSION >= MBED_ENCODE_VERSION(5, 8, 0)) && MBED_CONF_EVENTS_PRESENT

    /*!
     * \brief backlightFade - change screen backlight PWM duty cycle with specific time and easing
     * \param from - start value. Must be <= 128
     * \param to - end value. Must be <= 128
     * \param duration - total time in ms
     * \param fadeType - type of easing
     * \param delay - delay between every steps in ms. Decrease this value for smooth or increace for performance
     */
    void backlightFade(
        uint8_t  from,
        uint8_t  to,
        uint32_t duration = 1000,
        FadeType fadeType = Linear,
        uint8_t  delay    = 10);

    /*!
     * \brief attach one callback to any number of interrupt flags
     * \param f - callback function void(uint8_t flag) f;
     * \param flag - one of the possible interrupt mas bit
     * for more information see EVE.h Interrupt bits and BT81X (815/6) datasheet pg 19
     */
    void attach(mbed::Callback<void(uint8_t)> f, uint8_t flag);

    inline void attachPageSwapCallback(mbed::Callback<void(uint8_t)> f) { attach(f, EVE_INT_SWAP); }
    inline void attachTouchDetectedCallback(mbed::Callback<void(uint8_t)> f) { attach(f, EVE_INT_TOUCH); }
    inline void attachTouchTagCallback(mbed::Callback<void(uint8_t)> f) { attach(f, EVE_INT_TAG); }
    inline void attachTouchConversionsCallback(mbed::Callback<void(uint8_t)> f)
    {
        attach(f, EVE_INT_CONVCOMPLETE);
    }

    /*!
     * \brief attachToTag. attach callback to all tags. Passing tag number as parameter to callback function.
     * \param f - callback function attached to all tags (1-254)
     */
    void attachToTags(mbed::Callback<void(uint8_t)> f);

    /*!
     * \brief attachToTag. Attach calback to specific tag number.
     * Do not use this method both with setCallbackToTag(mbed::Callback<void(uint8_t)> f)
     * \param f - callback function will be attached to tag
     * \param tag - tag number (1-254)
     */
    void attachToTag(mbed::Callback<void(uint8_t)> f, uint8_t tag);

    /*!
     * \brief deattachFromTag. Remove callback from tag. This function remove all callbacks from tag, if many callbacks attached to one tag.
     * \param tag - tag number.
     */
    void deattachFromTag(uint8_t tag);

    /*!
     * \brief setCallbackToTag. Automatic attach callback to next empty tag
     * \param f - callback function will be attached to tag
     * \return Tag number
     */
    uint8_t setCallbackToTag(mbed::Callback<void(uint8_t)> f);

    /*!
     * \brief setCallbackToTag. Automatic attach callback to next empty tag. Last argument in function must be uint8_t for pushing tag number
     * \param obj - pointer to object
     * \param method - pointer to  member callback function will be attached to tag
     * \return Tag number
     */
    template<typename R,
             typename T,
             typename U,
             typename... Types,
             typename... Args>
    typename std::enable_if<!(sizeof...(Types)
                              == sizeof...(Args)),
                            uint8_t>::type
    setCallbackToTag(U * obj,
                     R (T::*method)(Types...),
                     Args... args)
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

        //Wrap callback with different args type
        auto * fp = new std::function<void(uint8_t)>([=](uint8_t tag) -> void {
            (obj->*method)(args..., tag);
        });

        mbed::Callback<void(uint8_t)> cb([fp](uint8_t tag) {
            fp->operator()(tag);
        });

        TagCallback cbs{
            findFirstEmptyTag(),
            cb,
            fp};

        m_tagCallbacksPool.push_back(cbs);
        return cbs.tagNumber;
    }

    template<typename R,
             typename T,
             typename U,
             typename... Types,
             typename... Args>
    typename std::enable_if<(sizeof...(Types)
                             == sizeof...(Args)),
                            uint8_t>::type
    setCallbackToTag(U * obj,
                     R (T::*method)(Types...),
                     Args... args)
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

        //Wrap callback with different args type
        auto * fp = new std::function<void(uint8_t)>([=](uint8_t tag) -> void {
            (obj->*method)(args...);
        });

        mbed::Callback<void(uint8_t)> cb([fp](uint8_t tag) {
            fp->operator()(tag);
        });

        TagCallback cbs{
            findFirstEmptyTag(),
            cb,
            fp};

        m_tagCallbacksPool.push_back(cbs);
        return cbs.tagNumber;
    }

    /*!
     * \brief setCallbackToTag. Automatic attach callback to next empty tag.
     * \param f - pointer to callback function will be attached to tag
     * \return Tag number
     */
    template<typename F,
             typename... Types,
             typename... Args>
    typename std::enable_if<(sizeof...(Types) == sizeof...(Args)), uint8_t>::type
    setCallbackToTag(F (*f)(Types...),
                     Args... args)
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
        //Wrap callback with different args type
        auto * fp = new std::function<void(uint8_t)>([f, args...](uint8_t tag) -> void {
            (*f)(args...);
        });

        mbed::Callback<void(uint8_t)> cb([fp](uint8_t tag) {
            fp->operator()(tag);
        });

        TagCallback cbs{
            findFirstEmptyTag(),
            cb,
            fp};

        m_tagCallbacksPool.push_back(cbs);
        return cbs.tagNumber;
    }

    /*!
     * \brief setCallbackToTag. Automatic attach callback to next empty tag.
     * \param f - pointer to callback function will be attached to tag
     * \return Tag number
     */
    template<typename F,
             typename... Types,
             typename... Args>
    typename std::enable_if<!(sizeof...(Types) == sizeof...(Args)), uint8_t>::type
    setCallbackToTag(F (*f)(Types...),
                     Args... args)
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
        //Wrap callback with different args type
        auto * fp = new std::function<void(uint8_t)>([f, args...](uint8_t tag) -> void {
            (*f)(args..., tag);
        });

        mbed::Callback<void(uint8_t)> cb([fp](uint8_t tag) {
            fp->operator()(tag);
        });

        TagCallback cbs{
            findFirstEmptyTag(),
            cb,
            fp};

        m_tagCallbacksPool.push_back(cbs);
        return cbs.tagNumber;
    }
#endif
    //**************************************************************

private:
    struct RamG
    {
        uint32_t m_start{0x0},
            m_size{0x0},
            m_currentPosition{0x0};
        RamG(uint32_t size = EVE_RAM_G_SAFETY_SIZE)
        {
            if(size > EVE_RAM_G_SIZE)
                error("Allocated size must be less than EVE_RAM_G_SIZE");
            debug_if(
                size > EVE_RAM_G_SAFETY_SIZE,
                "Note: If the loading image is in PNG format, the top 42K bytes from address "
                "0xF5800 of RAM_G will be overwritten as temporary data buffer for decoding "
                "process. \n\n");
            m_start = EVE_RAM_G;
            m_size  = m_start + size;
        }
    };
    EVE_HAL *             m_hal{nullptr};
    RamG *                m_ramG{nullptr};
    PixelPrecision        m_pixelPrecision{Div_16};
    std::vector<CmdBuf_t> m_cmdBuffer;

    void rebootCoPro()
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

        DELAY_MS(5); /* just to be safe */

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

        DELAY_MS(5); /* just to be safe */
#endif
    }

    //    bool busy();
    void
    writeString(string text);
    /* Raw memory commands. Users actually does'n use in directly.
     * To opperate with Ram_G call RamGInit() and work with public
     * memory commands block */
    void memCopy(uint32_t dest, uint32_t src, uint32_t num);
    //**********************************

#if(MBED_VERSION >= MBED_ENCODE_VERSION(5, 8, 0)) && MBED_CONF_EVENTS_PRESENT
    void    enableTagInterrupt();
    void    interruptFound();
    uint8_t findFirstEmptyTag();

    struct BacklightFade
    {
        float    cycCount;
        int32_t  duration;
        int16_t  range;
        uint8_t  start;
        uint8_t  value;
        uint8_t  freq;
        FadeType fadeType;
    };

    struct TagCallback
    {
        uint8_t                        tagNumber;
        mbed::Callback<void(uint8_t)>  callback{nullptr};
        std::function<void(uint8_t)> * cbPonter{nullptr};

        bool operator<(const TagCallback & c) const
        {
            return (tagNumber < c.tagNumber);
        }
    };

    void p_backlightFade(BacklightFade bf);

    bool         m_fadeBlock{false};
    InterruptIn  m_interrupt;
    Thread *     m_eventThread{nullptr};
    EventQueue * m_queue{nullptr};
    //Calbacks for interrupt events
    mbed::Callback<void(uint8_t)> m_pageSwapCallback{nullptr};
    mbed::Callback<void(uint8_t)> m_touchDetectedCallback{nullptr};
    mbed::Callback<void(uint8_t)> m_touchTagCallback{nullptr};
    mbed::Callback<void(uint8_t)> m_touchConvCompCallback{nullptr};

    mbed::Callback<void(uint8_t)> m_tagNumberCallback{nullptr};
    std::vector<TagCallback>      m_tagCallbacksPool;
#endif
};    // namespace EVE
}    // namespace EVE

#endif    // FT8XX_H
