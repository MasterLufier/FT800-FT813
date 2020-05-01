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

struct FTDisplayList
{
    std::string m_name{};
    uint32_t    m_address{0};
    uint32_t    m_size{0};
    FTDisplayList(string name, uint32_t address, uint32_t size) :
        m_name(name), m_address(address), m_size(size)
    {}
};

class FTRamG
{
public:
    FTRamG(uint32_t size = EVE_RAM_G_SAFETY_SIZE);
    FTDisplayList * saveDisplayList(std::string name = "Display List");

private:
    uint32_t m_start{0x0}, m_size{0x0}, m_currentPosition{0x0};
};

class FT8xx : private NonCopyable<FT8xx>
{
public:
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

    void ramGInit(uint32_t size = EVE_RAM_G_SAFETY_SIZE) { m_ramG = new FTRamG(size); }

    FTRamG * ramG() const { return m_ramG; }

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
//*************************************************************************************
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
    //*************************************************************

private:
    EVE_HAL * m_hal{nullptr};
    FTRamG *  m_ramG{nullptr};

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
};

#endif    // FT8XX_H
