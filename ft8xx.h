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

#include <EVE_target.h>
#include <algorithm>
#include <ft8xxmemory.h>
#include <functional>
#include <vector>

namespace EVE
{
#if defined(EVE_CAP_TOUCH)
typedef std::function<void(uint8_t)>  tagCB;
typedef std::function<void(uint16_t)> trackCB;
#endif
class FT8xx : private NonCopyable<FT8xx>
{
    friend class RamG;
    friend class Flash;

public:
    enum EVEeventFlags
    {
        CoProBusy  = 1UL << 0,
        CmdBufBusy = 1UL << 1
    };
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
        int32_t word{0x00000000u};
        int8_t  byte[4];
        int16_t halfWord[2];
        CmdBuf_t(int32_t data = 0x0) :
            word{data} { MBED_STATIC_ASSERT(sizeof(this) == sizeof(uint32_t), "CmdBuf_t: Padding detected"); }
        CmdBuf_t(int16_t d1, int16_t d2) :
            halfWord{d1, d2} {}
        CmdBuf_t(int8_t d1, int8_t d2, int8_t d3, int8_t d4) :
            byte{d1, d2, d3, d4} {}
    };

#if defined(EVE_CAP_TOUCH)
    struct TouchCalibrationResult
    {
        uint32_t touch_a{},
            touch_b{},
            touch_c{},
            touch_d{},
            touch_e{},
            touch_f{};
    };

    enum FadeType : uint8_t{
        Linear,
        Quad,
        Cubic,
        Quart};

    FT8xx(
        PinName               mosi             = EVE_SPI_MOSI,
        PinName               miso             = EVE_SPI_MISO,
        PinName               sclk             = EVE_SPI_CLK,
        PinName               ssel             = EVE_SPI_SSEL,
        PinName               pd               = EVE_PD,
        PinName               interrupt        = EVE_INTRPT,
        EVE_HAL::SPIFrequency spiFrequency     = EVE_HAL::F_20M,
        bool                  sharedEventQueue = false,
        uint32_t              threadStackSize  = (4 * 512),
        const char *          threadName       = "FT8xxThrd");
#else
    FT8xx(PinName               mosi,
          PinName               miso,
          PinName               sclk,
          PinName               ssel,
          PinName               pd,
          EVE_HAL::SPIFrequency spiFrequency = EVE_HAL::F_20M);
#endif
    ~FT8xx();

    //*********Low level cmd
    uint16_t cmdFifoFreeSpace()
    {
        uint16_t fullness, retval, wp;

        wp = m_hal->rd16(REG_CMD_WRITE);

        fullness = (wp - m_hal->rd16(REG_CMD_READ)) & static_cast<uint16_t>(MemoryMap::RAM_CMD_Size);
        retval   = (static_cast<uint16_t>(MemoryMap::RAM_CMD_Size) - 4) - fullness;
        return (retval);
    }

    //*********Basic Commands
    /*!
     * \brief Push command to cmdBuffer
     * \param b - command to push
     */
    void push(const CmdBuf_t & b);

    /*!
     * \brief Load cmdBuffer to EVE cmd FIFO and start processing to copy result to Ram_DL
     *  \note now this function support only FT/BT81X, because use new FIFO write mechanism. For more information see BRT_AN_033 page 92.
     */
    void execute();

    /*!
     * \brief setRotate - Apply screen rotation
     * \param rotation
     */
    inline void setRotate(ScreenRotation rotation)
    {
        push(CMD_SETROTATE);
        push(rotation);
    }

    inline void dlStart() { push(CMD_DLSTART); }
    inline void begin(GraphicPrimitives prim) { push(EVE::begin(prim)); }
    inline void end() { push(EVE::end()); }
    inline void swap()
    {
        push(DL_DISPLAY);
        push(CMD_SWAP);
    }

    inline void tag(uint8_t tag) { push(EVE::tag(tag)); }
    inline void clearTag() { push(EVE::tag(0)); }

    void clear(bool colorBuf   = true,
               bool stencilBuf = true,
               bool tagBuf     = true);

    void clearColorRGB(uint8_t r, uint8_t g, uint8_t b);
    void clearColorRGB(uint32_t rgb);
    void clearColorA(uint8_t a);
    void clearColorARGB(const CmdBuf_t & argb);

    void colorRGB(uint8_t r, uint8_t g, uint8_t b);
    void colorRGB(uint32_t rgb);
    void colorA(uint8_t a);
    void colorARGB(const CmdBuf_t & argb);

    inline void pointSize(uint16_t size) { push(EVE::pointSize(size)); }
    inline void lineWidth(uint16_t width) { push(EVE::lineWidth(width)); }

    //*************************
    //*********Drawing functions
    void vertexPointII(uint16_t x,
                       uint16_t y,
                       uint16_t handle = 0,
                       uint16_t cell   = 0);

    void vertexPointF(int16_t x,
                      int16_t y);

    void point(int16_t  x,
               int16_t  y,
               uint16_t size);

    void line(int16_t  x0,
              int16_t  y0,
              int16_t  x1,
              int16_t  y1,
              uint16_t width);

    void rectangle(int16_t  x,
                   int16_t  y,
                   int16_t  width,
                   int16_t  height,
                   uint16_t radius = 1);

//********Bitmap commands:
#if defined(BT81X_ENABLE)
    inline void bitmapSource(MemoryMap targetMemory = MemoryMap::RAM_G,
                             int32_t   ptr          = 0)
    {
        push(EVE::bitmapSource(targetMemory, ptr));
    }
#else
    inline void bitmapSource(int32_t ptr = 0)
    {
        push(EVE::bitmapSource(ptr));
    }
#endif
#if defined(FT81X_ENABLE)
    void setBitmap(uint32_t         addr,
                   BitmapExtFormats fmt,
                   uint16_t         width,
                   uint16_t         height);
#endif

    inline void bitmapLayout(BitmapFormats format,
                             uint16_t      linestride,
                             uint16_t      height)
    {
        push(EVE::bitmapLayout(format, linestride, height));
    }

    //********Bitmap transform commands:
    inline void loadIdentity() { push(CMD_LOADIDENTITY); };
    inline void setMatrix() { push(CMD_SETMATRIX); };
    void        getMatrix(int32_t a,
                          int32_t b,
                          int32_t c,
                          int32_t d,
                          int32_t e,
                          int32_t f);

    void translate(int32_t tx, int32_t ty);
    void scale(int32_t sx, int32_t sy);
    void rotate(int32_t ang);

#if defined(BT81X_ENABLE)
    void rotateAround(int32_t x,
                      int32_t y,
                      int32_t angle,
                      int32_t scale);
#endif
    //*************************
    //*********Drawing Widgets
    //*****Colors of Widgets
    inline void fgColor(uint32_t color)
    {
        push(CMD_FGCOLOR);
        push(color);
    };

    inline void bgColor(uint32_t color)
    {
        push(CMD_BGCOLOR);
        push(color);
    }

    inline void gradColor(uint32_t color)
    {
        push(CMD_GRADCOLOR);
        push(color);
    }

    void gradient(int16_t  x0,
                  int16_t  y0,
                  uint32_t rgb0,
                  int16_t  x1,
                  int16_t  y1,
                  uint32_t rgb1);

    void gradientA(int16_t  x0,
                   int16_t  y0,
                   uint32_t argb0,
                   int16_t  x1,
                   int16_t  y1,
                   uint32_t argb1);
    //****
    void text(int16_t             x,
              int16_t             y,
              uint16_t            font,
              const std::string & text,
              TextOpt             options = TextOpt::CenterXY);

    void button(int16_t             x,
                int16_t             y,
                uint16_t            width,
                uint16_t            height,
                uint16_t            font    = 27,
                const std::string & text    = "",
                ButtonOpt           options = ButtonOpt::_3D);

    void clock(int16_t  x,
               int16_t  y,
               uint16_t radius,
               uint16_t h       = 10,
               uint16_t m       = 10,
               uint16_t s       = 0,
               uint16_t ms      = 0,
               ClockOpt options = ClockOpt::_3D);

    void gauge(int16_t  x,
               int16_t  y,
               uint16_t radius,
               uint16_t major,
               uint16_t minor,
               uint16_t val,
               uint16_t range,
               GaugeOpt options = GaugeOpt::_3D);

    void slider(int16_t   x,
                int16_t   y,
                uint16_t  width,
                uint16_t  height,
                uint16_t  value,
                uint16_t  range,
                SliderOpt options = SliderOpt::_3D);

    void progress(int16_t     x,
                  int16_t     y,
                  uint16_t    width,
                  uint16_t    height,
                  uint16_t    value,
                  uint16_t    size,
                  uint16_t    range,
                  ProgressOpt options = ProgressOpt::_3D);

    void scrollBar(int16_t      x,
                   int16_t      y,
                   uint16_t     width,
                   uint16_t     height,
                   uint16_t     value,
                   uint16_t     size,
                   uint16_t     range,
                   ScrollBarOpt options = ScrollBarOpt::_3D);

    void dial(int16_t  x,
              int16_t  y,
              uint16_t radius,
              uint16_t value,
              DialOpt  options = DialOpt::_3D);

    void toggle(int16_t             x,
                int16_t             y,
                uint16_t            width,
                uint16_t            font,
                uint16_t            state,
                const std::string & offText = "",
                const std::string & onText  = "",
                ToggleOpt           options = ToggleOpt::_3D);

    void keys(int16_t             x,
              int16_t             y,
              uint16_t            width,
              uint16_t            height,
              uint16_t            font,
              const std::string & text    = "",
              KeysOpt             options = KeysOpt::_3D);

    void spinner(int16_t      x     = EVE_HSIZE / 2,
                 int16_t      y     = EVE_VSIZE / 2,
                 SpinnerOpt   style = SpinnerOpt::Circle,
                 SpinnerScale scale = SpinnerScale::NoScale);

    inline void stop() { push(CMD_STOP); }
    //*************************
    //*********Special commands

    inline void interrupt(uint32_t ms)
    {
        push(CMD_INTERRUPT);
        push(ms);
    }

#if defined(FT81X_ENABLE)
    //Overload function for call with any StoredObjects
    void append(const StoredObject * o);

    void append(const DisplayList * dl);
    void append(const Snapshot * s,
                int16_t          x      = -999,
                int16_t          y      = -999,
                int16_t          width  = -1,
                int16_t          height = -1);

    void append(const Sketch * s,
                int16_t        x      = -1,
                int16_t        y      = -1,
                int16_t        width  = -1,
                int16_t        height = -1);

    void append(const ImagePNG * i,
                int16_t          x,
                int16_t          y,
                int16_t          width  = -1,
                int16_t          height = -1);
#endif

    //**************************

    //***********Ram G Commands
    void         ramGInit(uint32_t size = EVE_RAM_G_SAFETY_SIZE);
    const RamG * ramG();
    //****************

#if defined(BT81X_ENABLE)
    //***********Flash commands
    uint8_t flashInit(uint32_t size);
        //****************
#endif

    /*!
     * \brief setBacklight - set the backlight PWM duty cycle
     * \param value - value from 0(off) to 128(full)
     */
    void setBacklight(uint8_t value);
    //****************************************************************

//***Next commands works only if MBED Thread and EventQueue enabled
#if defined(EVE_CAP_TOUCH)

    void track(int16_t  x,
               int16_t  y,
               uint16_t width,
               uint16_t height,
               uint8_t  tag);

    /*!
     * \brief touchXY read touch screen XY register
     * \return touch coordinate or 0x80008000 if no touch present
     */
    int32_t touchXY();

    /*!
     * \brief touchCalibrate - function for calibrate touchscreen
     * \param factory - if true - load factory calibration, else - start new calibration
     */
    const FT8xx::TouchCalibrationResult & touchCalibrate(bool factory = true);

    /*!
     * \brief animate - change value with specific range, speed, and easing.
     * \note duration % delay must be equal 0
     * \param value - value to animate
     * \param from - start of animation
     * \param to - end of animation
     * \param duration - time to animation in ms
     * \param fadeType - easing
     * \param delay - delay between every steps of animation in ms. Decrease this value for smooth or increace for performance
     */
    void animate(int32_t * value,
                 int32_t   from,
                 int32_t   to,
                 uint32_t  duration = 1000,
                 FadeType  fadeType = Linear,
                 uint8_t   delay    = 10);

    /*!
     * \brief backlightFade - change screen backlight PWM duty cycle with specific time and easing
     * \note duration % delay must be equal 0
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
     * \param flag - one of the possible interrupt mask bit
     * for more information see EVE.h Interrupt bits and BT81X (815/6) datasheet pg 19
     */
    void attach(mbed::Callback<void(uint8_t)> f, uint8_t flag);

    inline void attachPageSwapCallback(mbed::Callback<void(uint8_t)> f)
    {
        attach(f, EVE_INT_SWAP);
    }
    inline void attachTouchDetectedCallback(mbed::Callback<void(uint8_t)> f)
    {
        attach(f, EVE_INT_TOUCH);
    }

    inline void attachTouchConversionsCallback(mbed::Callback<void(uint8_t)> f)
    {
        attach(f, EVE_INT_CONVCOMPLETE);
    }

    /*!
     * \brief deattachFromTag. Remove callback from tag. This function remove all callbacks and trackers from tag, if many callbacks attached to one tag.
     * \param tag - tag number.
     */
    void deattachFromTag(uint8_t tag);

    /*!
     * \brief setCallbackToTag. Attach callback to tag
     * \param f - callback function will be attached to tag. Last argument in function must be uint8_t for passing tag number
     * \param tag - tag number for adding. if it is 0 - automatic add callback to first empty tag
     * \param args - arguments of callback, if provided
     * \return Tag number
     */
    template<typename F, typename... Args>
    uint8_t setCallbackToTag(F && f, uint8_t tag = 0, Args... args)
    {
        auto * fp = new tagCB(
            [f, args...](uint8_t tag) -> void {
                (f)(args..., tag);
            });
        return setCallback(fp, tag);
    }

    /*!
     * \brief setCallbackToTag. Attach member function for callback to tag. Last argument in function must be uint8_t for passing tag number.
     * \param obj - pointer to object
     * \param method - pointer to  member callback function will be attached to tag
     * \param tag - tag number for adding. if it is 0 - automatic add callback to first empty tag
     * \param args - arguments of callback, if provided
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
                     uint8_t tag = 0,
                     Args... args)
    {
        //Wrap callback with different args type
        auto * fp = new tagCB(
            [obj, method, args...](uint8_t tag) -> void {
                (obj->*method)(args..., tag);
            });
        return setCallback(fp, tag);
    }
    /*!
     * \brief setCallbackToTag. Attach member function for callback to tag.
     * \param obj - pointer to object
     * \param method - pointer to  member callback function will be attached to tag
     * \param tag - tag number for adding. if it is 0 - automatic add callback to first empty tag
     * \param args - arguments of callback, if provided
     * \return Tag number
     */
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
                     uint8_t tag = 0,
                     Args... args)
    {
        //Wrap callback with different args type
        auto * fp = new tagCB(
            [obj, method, args...](uint8_t) -> void {
                (obj->*method)(args...);
            });
        return setCallback(fp, tag);
    }

    /*!
     * \brief setCallbackToTag. Attach function for callback to tag.
     * \param tag - tag number for adding. if it is 0 - automatic add callback to first empty tag
     * \param args - arguments of callback, if provided
     * \return Tag number
     */
    template<typename F,
             typename... Types,
             typename... Args>
    typename std::enable_if<(sizeof...(Types)
                             == sizeof...(Args)),
                            uint8_t>::type
    setCallbackToTag(F (*f)(Types...),
                     uint8_t tag = 0,
                     Args... args)
    {
        //Wrap callback with different args type
        auto * fp = new tagCB(
            [f, args...](uint8_t) -> void {
                (*f)(args...);
            });
        return setCallback(fp, tag);
    }

    //*******************
    /*!
     * \brief setTrackingToTag. Attach callback to tag.
     * \param f - callback function will be attached to tag. Last argument in function must be uint16_t for passing tracking value
     * \param tag - tag number for adding. if it is 0 - automatic add callback to first empty tag
     * \param args - arguments of callback, if provided
     * \return tag number
     */
    template<typename F, typename... Args>
    uint8_t setTrackingToTag(F &&    f,
                             uint8_t tag = 0,
                             Args... args)
    {
        auto * fp = new trackCB(
            [f, args...](uint16_t value) -> void {
                (f)(args..., value);
            });
        return setTracking(fp, tag);
    }

    template<typename R,
             typename T,
             typename U,
             typename... Types,
             typename... Args>
    uint8_t setTrackingToTag(U * obj,
                             R (T::*method)(Types...),
                             uint8_t tag = 0,
                             Args... args)
    {
        //Wrap callback with different args type
        auto * fp = new trackCB(
            [obj, method, args...](uint16_t value) -> void {
                (obj->*method)(args..., value);
            });
        return setTracking(fp, tag);
    }

#endif
    //**************************************************************

    EVE_HAL * hal() const;

private:
    EVE_HAL * m_hal{nullptr};
    RamG *    m_ramG{nullptr};
#if defined(BT81X_ENABLE)
    Flash * m_flash{nullptr};
#endif
    PixelPrecision        m_pixelPrecision{Div_16};
    std::vector<CmdBuf_t> m_cmdBuffer;
    uint16_t              m_ramDLobserver{0};
    EventFlags            m_eventFlags;

    void rebootCoPro();
    void writeString(const string & text);
#if defined(FT81X_ENABLE)
    void append(uint32_t address, uint32_t count);
#endif
    //**********************************

#if defined(EVE_CAP_TOUCH)
    void    interruptFound();
    uint8_t findFirstEmptyTag();

    uint8_t setCallback(tagCB * f, uint8_t tag);
    uint8_t setTracking(trackCB * f, uint8_t tag);

    struct Fade
    {
        float    cycCount;
        int32_t  duration;
        int32_t  range;
        int32_t  start;
        uint8_t  freq;
        FadeType fadeType;
    };

    struct TagCB
    {
        uint8_t                tagNumber;
        std::vector<tagCB *>   tagCBs;
        std::vector<trackCB *> trackCBs;

        bool operator<(const TagCB & c) const
        {
            return (tagNumber < c.tagNumber);
        }
    };

    void p_backlightFade(uint8_t * value, Fade * fade);
    void p_animate(int32_t * value, Fade * fade);

    bool         m_fadeBlock{false};
    InterruptIn  m_interrupt;
    Thread *     m_eventThread{nullptr};
    EventQueue * m_queue{nullptr};

    //Callbacks for interrupt events
    mbed::Callback<void(uint8_t)> m_pageSwapCallback{nullptr};
    mbed::Callback<void(uint8_t)> m_touchDetectedCallback{nullptr};
    mbed::Callback<void(uint8_t)> m_touchConvCompCallback{nullptr};

    std::vector<TagCB> m_tagCBPool;
#endif
};
}    // namespace EVE

#endif    // FT8XX_H
