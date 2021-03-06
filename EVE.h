/*
@file    EVE.h
@brief   Contains FT80x/FT81x/BT81x API definitions
@version 4.1
@date    2020-04-15
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

2.1
- changes to this header

2.2
- commented out "#define DISPLAY() ((0UL<<24))" as it collides with a define in the Arduino IDE - the whole section of "macros" needs a rework...

3.0
- renamed from FT800.h to FT8.h
- changed FT_ prefixes to FT8_
- switched to standard-C compliant comment-style
- changed FT81x register definitions from decimal to hex
- verified all FT81x register definitions
- moved FT81x registers marked as "reserved" to an #if 0 block

3.1
- moved several undocumented commands to an #if 0 block

3.2
- moved CMD_CRC to the block of undocumented commands as well

3.3
- changed macros BITMAP_LAYOUT_H and BITMAP_SIZE_H as submitted to github by "jventerprises"
	These macros provide the extended bits for bitmaps bigger than 511 pixels, FTDIs original implementation that I used and never touched takes the arguments
	as already processed and not the original values.
	Note the one example in "FT81x Series Programmers Guide" Version 1.1 for displaying a 800x480 sized bitmap:
	dl(BITMAP_SIZE_H(1, 0));
	dl(BITMAP_SIZE(NEAREST, BORDER, BORDER, 288, 480));
	Now you can use it like this:
	EVE_cmd_dl(BITMAP_SIZE_H(800, 480));
	EVE_cmd_dl(BITMAP_SIZE(NEAREST, BORDER, BORDER, 800, 480));

4.0
- renamed from FT8.h to EVE.h
- renamed EVE_81X_ENABLE to FT81X_ENABLE
- changed FT8_ prefixes to EVE_
- rearranged things a bit with FT80x specific includes moved to the end and a "#if defined (BT81X_ENABLE)" block on top of the chip-specific includes
- started to add specific BT81x defines
- minor maintenance
- changed OPT_FLASH to EVE_OPT_FLASH and OPT_FORMAT to EVE_OPT_FORMAT for consistency
- added EVE_OPT_FILL which has been left out of the documentation for the BT81x so far
- added a few BT81x specific macros
- added a few FT81x/BT81x specific host commands
- removed the preceding underscore from the include guard define to avoid potential undefined behavior
- removed a bunch of defines for FT80x that I never implemented for FT81x
- added an include for "EVE_target.h" in order to reduce the necessary includes in the main project file
- commented out the NOP macro as it was colliding with ESP32 includes
*/
#ifndef EVE_H_
#define EVE_H_

#include "EVE_config.h"

#include <mbed.h>

namespace EVE
{
#if defined(EVE_CAP_TOUCH)
MBED_STATIC_ASSERT((MBED_VERSION
                    >= MBED_ENCODE_VERSION(5, 8, 0)),
                   "Too old MBED Version");
MBED_STATIC_ASSERT(MBED_CONF_EVENTS_PRESENT,
                   "Mbed Events disabled. Touch events won't work");
#endif

enum class MemoryMap : uint32_t
{
    //Memory start addreses
    RAM_G          = 0x000000ul,
    ROM            = 0x200000ul,
    RAM_DL         = 0x300000ul,
    RAM_REG        = 0x302000ul,
    RAM_CMD        = 0x308000ul,
    Flash          = 0x800000ul,
    FLASH_POSTBLOB = 0x801000ul,

    //Memory sizes
    RAM_G_Size          = 1024 * 1024L,
    ROM_Size            = 1024 * 1024L,
    RAM_DL_Size         = 8 * 1024L,
    RAM_REG_Size        = 4 * 1024L,
    RAM_CMD_Size        = 4 * 1024L,
    Flash_Size          = EVE_EXT_FLASH_SIZE, /* defined in EVE_config.h */
    RAM_PNG_BUFFER_SIZE = 0x00A800UL,         /* If the loading image is in PNG format, the top 42K bytes from address 0xF5800 of RAM_G will \
        be overwritten as temporary data buffer for decoding process.  */
    RAM_G_SAFETY_SIZE   = RAM_G_Size - RAM_PNG_BUFFER_SIZE - 1,

    //Memory end
    RAM_G_end   = RAM_G + RAM_G_Size,
    ROM_end     = ROM + ROM_Size,
    RAM_DL_end  = RAM_DL + RAM_DL_Size,
    RAM_REG_end = RAM_REG + RAM_REG_Size,
    RAM_CMD_end = RAM_CMD + RAM_CMD_Size,
    Flash_end   = Flash + Flash_Size,

    //Some useful address points
    ROM_CHIPID     = 0x0C0000UL,
    ROM_FONT       = 0x1E0000UL,
    ROM_FONT_ADDR  = 0x2FFFFCUL,
    RAM_ERR_REPORT = 0x309800UL, /* max 128 bytes null terminated string */
};

#define DL_CLEAR      0x26000000UL /* requires OR'd arguments */
#define DL_CLEAR_RGB  0x02000000UL /* requires OR'd arguments */
#define DL_COLOR_RGB  0x04000000UL /* requires OR'd arguments */
#define DL_POINT_SIZE 0x0D000000UL /* requires OR'd arguments */
#define DL_END        0x21000000UL
#define DL_BEGIN      0x1F000000UL /* requires OR'd arguments */
#define DL_DISPLAY    0x00000000UL

#define CLR_COL 0x4
#define CLR_STN 0x2
#define CLR_TAG 0x1

#define EVE_ACTIVE  0x00 /* place FT8xx in active state */
#define EVE_STANDBY 0x41 /* place FT8xx in Standby (clk running) */
#define EVE_SLEEP   0x42 /* place FT8xx in Sleep (clk off) */
#define EVE_PWRDOWN 0x50 /* place FT8xx in Power Down (core off) */
#define EVE_CLKEXT  0x44 /* select external clock source */
#define EVE_CLKINT  0x48 /* select internal clock source */
#define EVE_CORERST 0x68 /* reset core - all registers default and processors reset */
#define EVE_CLK48M  0x62 /* select 48MHz PLL output */
#define EVE_CLK36M  0x61 /* select 36MHz PLL output */

/* Host commands */
enum class HostCommands : uint16_t
{
    ACTIVE  = 0x00, /* place FT8xx in active state */
    STANDBY = 0x41, /* place FT8xx in Standby (clk running) */
    SLEEP   = 0x42, /* place FT8xx in Sleep (clk off) */
    PWRDOWN = 0x50, /* place FT8xx in Power Down (core off) */
    CLKEXT  = 0x44, /* select external clock source */
    CLKINT  = 0x48, /* select internal clock source */
    CORERST = 0x68, /* reset core - all registers default and processors reset */
    CLK48M  = 0x62, /* select 48MHz PLL output */
    CLK36M  = 0x61, /* select 36MHz PLL output */

#if defined(FT81X_ENABLE)

    /* Host commands */
    CLKSEL       = 0x61, /* configure system clock */
    RST_PULSE    = 0x68, /* reset core - all registers default and processors reset */
    PINDRIVE     = 0x70, /* setup drive strength for various pins */
    PIN_PD_STATE = 0x71  /* setup how pins behave during power down */
#endif
};

/* defines used for graphics commands */
#define EVE_NEVER    0UL
#define EVE_LESS     1UL
#define EVE_LEQUAL   2UL
#define EVE_GREATER  3UL
#define EVE_GEQUAL   4UL
#define EVE_EQUAL    5UL
#define EVE_NOTEQUAL 6UL
#define EVE_ALWAYS   7UL

enum TestFunc : uint8_t
{
    Never,
    Less,
    LEqual,
    Greater,
    GEqual,
    Equal,
    NotEqual,
    Always
};

/* Screen rotation */
enum ScreenRotation : uint8_t
{
    Landscape,
    LandscapeInverted,
    Portrait,
    PortraitInverted,
    LandscapeMirrored,
    LandscapeInvertedMirrored,
    PortraitMirrored,
    PortraitInvertedMirrored
};

/* Bitmap formats */
#define EVE_ARGB1555 0UL
#define EVE_L1       1UL
#define EVE_L4       2UL
#define EVE_L8       3UL
#define EVE_RGB332   4UL
#define EVE_ARGB2    5UL
#define EVE_ARGB4    6UL
#define EVE_RGB565   7UL
#define EVE_PALETTED 8UL
#define EVE_TEXT8X8  9UL
#define EVE_TEXTVGA  10UL
#define EVE_BARGRAPH 11UL

enum BitmapFormats : uint8_t
{
    ARGB1555 = 0UL,
    L1       = 1UL,
    L4       = 2UL,
    L8       = 3UL,
    RGB332   = 4UL,
    ARGB2    = 5UL,
    ARGB4    = 6UL,
    RGB565   = 7UL,
    //    PALETTED     = 8UL,
    TEXT8X8      = 9UL,
    TEXTVGA      = 10UL,
    BARGRAPH     = 11UL,
    PALETTED565  = 14UL,
    PALETTED4444 = 15UL,
    PALETTED8    = 16UL,
    L2           = 17UL,
    GLFORMAT     = 31UL
};

enum class BitmapExtFormats : uint32_t
{
    ARGB1555 = 0UL,
    L1       = 1UL,
    L4       = 2UL,
    L8       = 3UL,
    RGB332   = 4UL,
    ARGB2    = 5UL,
    ARGB4    = 6UL,
    RGB565   = 7UL,
    //    PALETTED                       = 8UL,
    TEXT8X8                        = 9UL,
    TEXTVGA                        = 10UL,
    BARGRAPH                       = 11UL,
    PALETTED565                    = 14UL,
    PALETTED4444                   = 15UL,
    PALETTED8                      = 16UL,
    L2                             = 17UL,
    COMPRESSED_RGBA_ASTC_4x4_KHR   = 37808,    // 8.00
    COMPRESSED_RGBA_ASTC_5x4_KHR   = 37809,    // 6.40
    COMPRESSED_RGBA_ASTC_5x5_KHR   = 37810,    // 5.12
    COMPRESSED_RGBA_ASTC_6x5_KHR   = 37811,    // 4.27
    COMPRESSED_RGBA_ASTC_6x6_KHR   = 37812,    // 3.56
    COMPRESSED_RGBA_ASTC_8x5_KHR   = 37813,    // 3.20
    COMPRESSED_RGBA_ASTC_8x6_KHR   = 37814,    // 2.67
    COMPRESSED_RGBA_ASTC_8x8_KHR   = 37815,    // 2.56
    COMPRESSED_RGBA_ASTC_10x5_KHR  = 37816,    // 2.13
    COMPRESSED_RGBA_ASTC_10x6_KHR  = 37817,    // 2.00
    COMPRESSED_RGBA_ASTC_10x8_KHR  = 37818,    // 1.60
    COMPRESSED_RGBA_ASTC_10x10_KHR = 37819,    // 1.28
    COMPRESSED_RGBA_ASTC_12x10_KHR = 37820,    // 1.07
    COMPRESSED_RGBA_ASTC_12x12_KHR = 37821,    // 0.89
};

enum class ImageJPEGFormat : uint16_t
{
    L8     = L8,
    RGB565 = RGB565,
};

enum class ImagePNGFormat : uint16_t
{
    L8           = L8,
    RGB565       = RGB565,
    PALETTED565  = PALETTED565,
    PALETTED4444 = PALETTED4444,
    ARGB4        = ARGB4
};

enum class SnapshotBitmapFormat : uint8_t
{
    RGB565 = RGB565,
    ARGB4  = ARGB4,
    ARGB8  = 0x20    // BT81x support to store snapshoot to ARGB8 format, but doesn't set this format to CMD_SETBITMAP
};

enum class SketchBitmapFormat : uint8_t
{
    L1 = L1,
    L8 = L8
};

/* Bitmap filter types */
#define EVE_NEAREST  0UL
#define EVE_BILINEAR 1UL

enum BitmapFilter : uint8_t
{
    Nearest,
    Bilinear
};

/* Bitmap wrap types */
#define EVE_BORDER 0UL
#define EVE_REPEAT 1UL

enum BitmapWrap : uint8_t
{
    Border,
    Repeat
};

enum class BitmapTransformPrecision : uint8_t
{
    p8d8  = 0,
    p1d15 = 1
};

enum class SwizzleSource : uint8_t
{
    ZERO  = 0, /* Set the source channel to zero */
    ONE   = 1, /* Set the source channel to 1 */
    RED   = 2, /* Specify RED component as source channel */
    GREEN = 3, /* Specify GREEN component as source channel */
    BLUE  = 4, /* Specify BLUE component as source  channel */
    ALPHA = 5  /* Specify ALPHA component as source channel */
};

/* Stencil defines */
#define EVE_KEEP    1UL
#define EVE_REPLACE 2UL
#define EVE_INCR    3UL
#define EVE_DECR    4UL
#define EVE_INVERT  5UL

enum class StencilActions : uint8_t
{
    Zero,
    Keep,
    Replace,
    Incr,
    Decr,
    Invert
};

/* Graphics display list swap defines */
#define EVE_DLSWAP_DONE  0UL
#define EVE_DLSWAP_LINE  1UL
#define EVE_DLSWAP_FRAME 2UL

/* Interrupt bits */
#define EVE_INT_SWAP         0x01
#define EVE_INT_TOUCH        0x02
#define EVE_INT_TAG          0x04
#define EVE_INT_SOUND        0x08
#define EVE_INT_PLAYBACK     0x10
#define EVE_INT_CMDEMPTY     0x20
#define EVE_INT_CMDFLAG      0x40
#define EVE_INT_CONVCOMPLETE 0x80

/* Touch mode */
#define EVE_TMODE_OFF        0
#define EVE_TMODE_ONESHOT    1
#define EVE_TMODE_FRAME      2
#define EVE_TMODE_CONTINUOUS 3

/* Alpha blending */
#define EVE_ZERO                0UL
#define EVE_ONE                 1UL
#define EVE_SRC_ALPHA           2UL
#define EVE_DST_ALPHA           3UL
#define EVE_ONE_MINUS_SRC_ALPHA 4UL
#define EVE_ONE_MINUS_DST_ALPHA 5UL
enum AlphaBlending
{
    Zero = 0,
    One,
    SrcAlpha,
    DstAlpha,
    OneMinusSrcAlpha,
    OneMinusDstAlpha
};
/* Graphics primitives */
#define EVE_BITMAPS      1UL
#define EVE_POINTS       2UL
#define EVE_LINES        3UL
#define EVE_LINE_STRIP   4UL
#define EVE_EDGE_STRIP_R 5UL
#define EVE_EDGE_STRIP_L 6UL
#define EVE_EDGE_STRIP_A 7UL
#define EVE_EDGE_STRIP_B 8UL
#define EVE_RECTS        9UL

enum GraphicPrimitives
{
    Bitmaps    = 1ul,
    Points     = 2ul,
    Lines      = 3ul,
    LineStrip  = 4ul,
    EdgeStripR = 5ul,
    EdgeStripL = 6ul,
    EdgeStripA = 7ul,
    EdgeStripB = 8ul,
    Rects      = 9ul
};

/* Widget command */
#define EVE_OPT_MONO      1
#define EVE_OPT_NODL      2
#define EVE_OPT_FLAT      256
#define EVE_OPT_CENTERX   512
#define EVE_OPT_CENTERY   1024
#define EVE_OPT_CENTER    (EVE_OPT_CENTERX | EVE_OPT_CENTERY)
#define EVE_OPT_NOBACK    4096
#define EVE_OPT_NOTICKS   8192
#define EVE_OPT_NOHM      16384
#define EVE_OPT_NOPOINTER 16384
#define EVE_OPT_NOSECS    32768
#define EVE_OPT_NOHANDS   49152
#define EVE_OPT_RIGHTX    2048
#define EVE_OPT_SIGNED    256

static const struct DefaultWidgetColours
{
    uint32_t bgColor   = 0x002040;
    uint32_t fgColor   = 0x003870;
    uint32_t gradColor = 0xffffff;
} defaultWidgetsState;

enum class TextOpt : uint16_t
{
    CenterX  = 512,
    CenterY  = 1024,
    CenterXY = CenterX | CenterY,
    RightX   = 2048,
    Format   = 4096,
    Fill     = 8192
};

enum class ButtonOpt : uint16_t
{
    _3D  = 0,
    Flat = 256
};

enum class ClockOpt : uint16_t
{
    _3D     = 0,
    Flat    = 256,
    NoBack  = 4096,
    NoTicks = 8192,
    NoHM    = 16384,
    NoSecks = 32768,
    NoHands = 49152,
};

enum class GaugeOpt : uint16_t
{
    _3D       = 0,
    Flat      = 256,
    NoBack    = 4096,
    NoTicks   = 8192,
    NoPointer = 16384,
};

enum class SliderOpt : uint16_t
{
    _3D  = 0,
    Flat = 256,
};

enum class ProgressOpt : uint16_t
{
    _3D  = 0,
    Flat = 256,
};

enum class ScrollBarOpt : uint16_t
{
    _3D  = 0,
    Flat = 256,
};

enum class DialOpt : uint16_t
{
    _3D  = 0,
    Flat = 256,
};

enum class ToggleOpt : uint16_t
{
    _3D  = 0,
    Flat = 256,
};

enum class KeysOpt : uint16_t
{
    _3D      = 0,
    Flat     = 256,
    CenterXY = 512 | 1024,
};

enum class SpinnerOpt : uint16_t
{
    Circle,
    Line,
    Clock,
    TwoDots
};

enum class SpinnerScale : uint16_t
{
    NoScale,
    HalfScreen,
    FullScreen
};

enum class LoadImageOpt : uint32_t
{
    RGB565     = 0,
    Mono       = 1,
    NoDL       = 2,
    Fullscreen = 8,
    MediaFIFO  = 16,
    Flash      = 64,
};

/* Defines related to inbuilt font */
#define EVE_NUMCHAR_PERFONT    (128L)      /* number of font characters per bitmap handle */
#define EVE_FONT_TABLE_SIZE    (148L)      /* size of the font table - utilized for loopup by the graphics engine */
#define EVE_FONT_TABLE_POINTER (0xFFFFCUL) /* pointer to the inbuilt font tables starting from bitmap handle 16 */

/* Audio sample type defines */
#define EVE_LINEAR_SAMPLES 0UL /* 8bit signed samples */
#define EVE_ULAW_SAMPLES   1UL /* 8bit ulaw samples */
#define EVE_ADPCM_SAMPLES  2UL /* 4bit ima adpcm samples */

/* Synthesized sound */
#define EVE_SILENCE      0x00
#define EVE_SQUAREWAVE   0x01
#define EVE_SINEWAVE     0x02
#define EVE_SAWTOOTH     0x03
#define EVE_TRIANGLE     0x04
#define EVE_BEEPING      0x05
#define EVE_ALARM        0x06
#define EVE_WARBLE       0x07
#define EVE_CAROUSEL     0x08
#define EVE_PIPS(n)      (0x0F + (n))
#define EVE_HARP         0x40
#define EVE_XYLOPHONE    0x41
#define EVE_TUBA         0x42
#define EVE_GLOCKENSPIEL 0x43
#define EVE_ORGAN        0x44
#define EVE_TRUMPET      0x45
#define EVE_PIANO        0x46
#define EVE_CHIMES       0x47
#define EVE_MUSICBOX     0x48
#define EVE_BELL         0x49
#define EVE_CLICK        0x50
#define EVE_SWITCH       0x51
#define EVE_COWBELL      0x52
#define EVE_NOTCH        0x53
#define EVE_HIHAT        0x54
#define EVE_KICKDRUM     0x55
#define EVE_POP          0x56
#define EVE_CLACK        0x57
#define EVE_CHACK        0x58
#define EVE_MUTE         0x60
#define EVE_UNMUTE       0x61

/* Synthesized sound frequencies, midi note */
#define EVE_MIDI_A0  21
#define EVE_MIDI_A_0 22
#define EVE_MIDI_B0  23
#define EVE_MIDI_C1  24
#define EVE_MIDI_C_1 25
#define EVE_MIDI_D1  26
#define EVE_MIDI_D_1 27
#define EVE_MIDI_E1  28
#define EVE_MIDI_F1  29
#define EVE_MIDI_F_1 30
#define EVE_MIDI_G1  31
#define EVE_MIDI_G_1 32
#define EVE_MIDI_A1  33
#define EVE_MIDI_A_1 34
#define EVE_MIDI_B1  35
#define EVE_MIDI_C2  36
#define EVE_MIDI_C_2 37
#define EVE_MIDI_D2  38
#define EVE_MIDI_D_2 39
#define EVE_MIDI_E2  40
#define EVE_MIDI_F2  41
#define EVE_MIDI_F_2 42
#define EVE_MIDI_G2  43
#define EVE_MIDI_G_2 44
#define EVE_MIDI_A2  45
#define EVE_MIDI_A_2 46
#define EVE_MIDI_B2  47
#define EVE_MIDI_C3  48
#define EVE_MIDI_C_3 49
#define EVE_MIDI_D3  50
#define EVE_MIDI_D_3 51
#define EVE_MIDI_E3  52
#define EVE_MIDI_F3  53
#define EVE_MIDI_F_3 54
#define EVE_MIDI_G3  55
#define EVE_MIDI_G_3 56
#define EVE_MIDI_A3  57
#define EVE_MIDI_A_3 58
#define EVE_MIDI_B3  59
#define EVE_MIDI_C4  60
#define EVE_MIDI_C_4 61
#define EVE_MIDI_D4  62
#define EVE_MIDI_D_4 63
#define EVE_MIDI_E4  64
#define EVE_MIDI_F4  65
#define EVE_MIDI_F_4 66
#define EVE_MIDI_G4  67
#define EVE_MIDI_G_4 68
#define EVE_MIDI_A4  69
#define EVE_MIDI_A_4 70
#define EVE_MIDI_B4  71
#define EVE_MIDI_C5  72
#define EVE_MIDI_C_5 73
#define EVE_MIDI_D5  74
#define EVE_MIDI_D_5 75
#define EVE_MIDI_E5  76
#define EVE_MIDI_F5  77
#define EVE_MIDI_F_5 78
#define EVE_MIDI_G5  79
#define EVE_MIDI_G_5 80
#define EVE_MIDI_A5  81
#define EVE_MIDI_A_5 82
#define EVE_MIDI_B5  83
#define EVE_MIDI_C6  84
#define EVE_MIDI_C_6 85
#define EVE_MIDI_D6  86
#define EVE_MIDI_D_6 87
#define EVE_MIDI_E6  88
#define EVE_MIDI_F6  89
#define EVE_MIDI_F_6 90
#define EVE_MIDI_G6  91
#define EVE_MIDI_G_6 92
#define EVE_MIDI_A6  93
#define EVE_MIDI_A_6 94
#define EVE_MIDI_B6  95
#define EVE_MIDI_C7  96
#define EVE_MIDI_C_7 97
#define EVE_MIDI_D7  98
#define EVE_MIDI_D_7 99
#define EVE_MIDI_E7  100
#define EVE_MIDI_F7  101
#define EVE_MIDI_F_7 102
#define EVE_MIDI_G7  103
#define EVE_MIDI_G_7 104
#define EVE_MIDI_A7  105
#define EVE_MIDI_A_7 106
#define EVE_MIDI_B7  107
#define EVE_MIDI_C8  108

/* GPIO bits */
#define EVE_GPIO0 0
#define EVE_GPIO1 1 /* default gpio pin for audio shutdown, 1 - enable, 0 - disable */
#define EVE_GPIO7 7 /* default gpio pin for display enable, 1 - enable, 0 - disable */

/* Display rotation */
#define EVE_DISPLAY_0   0 /* 0 degrees rotation */
#define EVE_DISPLAY_180 1 /* 180 degrees rotation */

/* commands common to EVE/EVE2/EVE3 */
#define CMD_APPEND       0xFFFFFF1E
#define CMD_BGCOLOR      0xFFFFFF09
#define CMD_BUTTON       0xFFFFFF0D
#define CMD_CALIBRATE    0xFFFFFF15
#define CMD_CLOCK        0xFFFFFF14
#define CMD_COLDSTART    0xFFFFFF32
#define CMD_DIAL         0xFFFFFF2D
#define CMD_DLSTART      0xFFFFFF00
#define CMD_FGCOLOR      0xFFFFFF0A
#define CMD_GAUGE        0xFFFFFF13
#define CMD_GETMATRIX    0xFFFFFF33
#define CMD_GETPROPS     0xFFFFFF25
#define CMD_GETPTR       0xFFFFFF23
#define CMD_GRADCOLOR    0xFFFFFF34
#define CMD_GRADIENT     0xFFFFFF0B
#define CMD_INFLATE      0xFFFFFF22
#define CMD_INTERRUPT    0xFFFFFF02
#define CMD_KEYS         0xFFFFFF0E
#define CMD_LOADIDENTITY 0xFFFFFF26
#define CMD_LOADIMAGE    0xFFFFFF24
#define CMD_LOGO         0xFFFFFF31
#define CMD_MEMCPY       0xFFFFFF1D
#define CMD_MEMCRC       0xFFFFFF18
#define CMD_MEMSET       0xFFFFFF1B
#define CMD_MEMWRITE     0xFFFFFF1A
#define CMD_MEMZERO      0xFFFFFF1C
#define CMD_NUMBER       0xFFFFFF2E
#define CMD_PROGRESS     0xFFFFFF0F
#define CMD_REGREAD      0xFFFFFF19
#define CMD_ROTATE       0xFFFFFF29
#define CMD_SCALE        0xFFFFFF28
#define CMD_SCREENSAVER  0xFFFFFF2F
#define CMD_SCROLLBAR    0xFFFFFF11
#define CMD_SETFONT      0xFFFFFF2B
#define CMD_SETMATRIX    0xFFFFFF2A
#define CMD_SKETCH       0xFFFFFF30
#define CMD_SLIDER       0xFFFFFF10
#define CMD_SNAPSHOT     0xFFFFFF1F
#define CMD_SPINNER      0xFFFFFF16
#define CMD_STOP         0xFFFFFF17
#define CMD_SWAP         0xFFFFFF01
#define CMD_TEXT         0xFFFFFF0C
#define CMD_TOGGLE       0xFFFFFF12
#define CMD_TRACK        0xFFFFFF2C
#define CMD_TRANSLATE    0xFFFFFF27

/* the following are undocumented commands that therefore should not be used */
#if 0
    #define CMD_CRC             0xFFFFFF03
    #define CMD_HAMMERAUX       0xFFFFFF04
    #define CMD_MARCH           0xFFFFFF05
    #define CMD_IDCT            0xFFFFFF06
    #define CMD_EXECUTE         0xFFFFFF07
    #define CMD_GETPOINT        0xFFFFFF08
    #define CMD_TOUCH_TRANSFORM 0xFFFFFF20
#endif

/* FT8xx graphics engine specific macros useful for static display list generation */
#define ALPHA_FUNC(func, ref) ((9UL << 24) | (((func)&7UL) << 8) | (((ref)&255UL) << 0))
static constexpr uint32_t alphaFunc(TestFunc func, uint16_t ref)
{
    return ((9UL << 24)
            | (((func)&7UL) << 8)
            | (((ref)&255UL) << 0));
}
#define BEGIN(prim) ((31UL << 24) | (((prim)&15UL) << 0))
static constexpr uint32_t begin(GraphicPrimitives prim)
{
    return ((31UL << 24) | (((prim)&15UL) << 0));
}
#define BITMAP_HANDLE(handle) ((5UL << 24) | (((handle)&31UL) << 0))
static constexpr uint32_t bitmapHandle(uint8_t handle)
{
    return ((5UL << 24) | (((handle)&31UL) << 0));
}
#define BITMAP_LAYOUT(format, linestride, height) ((7UL << 24) | (((format)&31UL) << 19) | (((linestride)&1023UL) << 9) | (((height)&511UL) << 0))
static constexpr uint32_t bitmapLayout(BitmapFormats format,
                                       uint16_t      linestride,
                                       uint16_t      height)
{
    return ((7UL << 24)
            | (((format)&31UL) << 19)
            | (((linestride)&1023UL) << 9)
            | (((height)&511UL) << 0));
}
#define BITMAP_SIZE(filter, wrapx, wrapy, width, height) ((8UL << 24) | (((filter)&1UL) << 20) | (((wrapx)&1UL) << 19) | (((wrapy)&1UL) << 18) | (((width)&511UL) << 9) | (((height)&511UL) << 0))
static constexpr uint32_t bitmapSize(BitmapFilter filter,
                                     BitmapWrap   wrapx,
                                     BitmapFilter wrapy,
                                     uint16_t     width,
                                     uint16_t     height)
{
    return ((8UL << 24)
            | (((filter)&1UL) << 20)
            | (((wrapx)&1UL) << 19)
            | (((wrapy)&1UL) << 18)
            | (((width)&511UL) << 9)
            | (((height)&511UL) << 0));
}
#if defined(BT81X_ENABLE)
static constexpr uint32_t bitmapTransformA(int16_t                  v,
                                           BitmapTransformPrecision p = BitmapTransformPrecision::p8d8)
{
    return ((21UL << 24)
            | ((static_cast<uint8_t>(p) & 1UL) << 17)
            | (((v)&131071UL) << 0));
}

static constexpr uint32_t bitmapTransformB(int16_t                  v,
                                           BitmapTransformPrecision p = BitmapTransformPrecision::p8d8)
{
    return ((22UL << 24)
            | ((static_cast<uint8_t>(p) & 1UL) << 17)
            | (((v)&131071UL) << 0));
}

static constexpr uint32_t bitmapTransformD(int16_t                  v,
                                           BitmapTransformPrecision p = BitmapTransformPrecision::p8d8)
{
    return ((24UL << 24)
            | ((static_cast<uint8_t>(p) & 1UL) << 17)
            | (((v)&131071UL) << 0));
}

static constexpr uint32_t bitmapTransformE(int16_t                  v,
                                           BitmapTransformPrecision p = BitmapTransformPrecision::p8d8)
{
    return ((25UL << 24)
            | ((static_cast<uint8_t>(p) & 1UL) << 17)
            | (((v)&131071UL) << 0));
}
#else
    #define BITMAP_TRANSFORM_A(a) ((21UL << 24) | (((a)&131071UL) << 0))
static constexpr uint32_t bitmapTransformA(int16_t a)
{
    return ((21UL << 24) | (((a)&131071UL) << 0));
}
    #define BITMAP_TRANSFORM_B(b) ((22UL << 24) | (((b)&131071UL) << 0))
static constexpr uint32_t bitmapTransformB(int16_t b)
{
    return ((22UL << 24) | (((b)&131071UL) << 0));
}
    #define BITMAP_TRANSFORM_C(c) ((23UL << 24) | (((c)&16777215UL) << 0))

    #define BITMAP_TRANSFORM_D(d) ((24UL << 24) | (((d)&131071UL) << 0))
static constexpr uint32_t bitmapTransformD(int16_t d)
{
    return ((24UL << 24) | (((d)&131071UL) << 0));
}
    #define BITMAP_TRANSFORM_E(e) ((25UL << 24) | (((e)&131071UL) << 0))
static constexpr uint32_t bitmapTransformE(int16_t e)
{
    return ((25UL << 24) | (((e)&131071UL) << 0));
}
    #define BITMAP_TRANSFORM_F(f) ((26UL << 24) | (((f)&16777215UL) << 0))

#endif

static constexpr uint32_t bitmapTransformC(int16_t c)
{
    return ((23UL << 24) | (((c)&16777215UL) << 0));
}

static constexpr uint32_t bitmapTransformF(int16_t f)
{
    return ((26UL << 24) | (((f)&16777215UL) << 0));
}

#define BLEND_FUNC(src, dst) ((11UL << 24) | (((src)&7UL) << 3) | (((dst)&7UL) << 0))
static constexpr uint32_t blendFunc(AlphaBlending src, AlphaBlending dst)
{
    return ((11UL << 24) | (((src)&7UL) << 3) | (((dst)&7UL) << 0));
}
#define CALL(dest) ((29UL << 24) | (((dest)&65535UL) << 0))
static constexpr uint32_t call(uint32_t dest)
{
    return ((29UL << 24) | (((dest)&65535UL) << 0));
}
#define CELL(cell)     ((6UL << 24) | (((cell)&127UL) << 0))
#define CLEAR(c, s, t) ((38UL << 24) | (((c)&1UL) << 2) | (((s)&1UL) << 1) | (((t)&1UL) << 0))
static constexpr uint32_t clear(bool c, bool t, bool s)
{
    return ((38UL << 24)
            | (((c)&1UL) << 2)
            | (((s)&1UL) << 1)
            | (((t)&1UL) << 0));
}
#define CLEAR_COLOR_A(alpha) ((15UL << 24) | (((alpha)&255UL) << 0))
static constexpr uint32_t clearColorA(uint8_t a)
{
    return ((15UL << 24)
            | (((a)&255UL) << 0));
}
#define CLEAR_COLOR_RGB(red, green, blue) ((2UL << 24) | (((red)&255UL) << 16) | (((green)&255UL) << 8) | (((blue)&255UL) << 0))
static constexpr uint32_t clearColorRGB(uint8_t r,
                                        uint8_t g,
                                        uint8_t b)
{
    return ((2UL << 24)
            | (((r)&255UL) << 16)
            | (((g)&255UL) << 8)
            | (((b)&255UL) << 0));
}
#define CLEAR_STENCIL(s) ((17UL << 24) | (((s)&255UL) << 0))
static constexpr uint32_t clearStencil(bool s)
{
    return ((17UL << 24) | (((s)&255UL) << 0));
}

#define CLEAR_TAG(s) ((18UL << 24) | (((s)&255UL) << 0))
static constexpr uint32_t clearTag(bool s)
{
    return ((18UL << 24) | (((s)&255UL) << 0));
}
#define COLOR_A(alpha) ((16UL << 24) | (((alpha)&255UL) << 0))
static constexpr uint32_t colorA(uint8_t a)
{
    return ((16UL << 24) | (((a)&255UL) << 0));
}
#define COLOR_MASK(r, g, b, a) ((32UL << 24) | (((r)&1UL) << 3) | (((g)&1UL) << 2) | (((b)&1UL) << 1) | (((a)&1UL) << 0))
static constexpr uint32_t colorMask(uint8_t r,
                                    uint8_t g,
                                    uint8_t b,
                                    uint8_t a)
{
    return ((32UL << 24)
            | (((r)&1UL) << 3)
            | (((g)&1UL) << 2)
            | (((b)&1UL) << 1)
            | (((a)&1UL) << 0));
}
#define COLOR_RGB(red, green, blue) ((4UL << 24) | (((red)&255UL) << 16) | (((green)&255UL) << 8) | (((blue)&255UL) << 0))
static constexpr uint32_t colorRGB(uint8_t r,
                                   uint8_t g,
                                   uint8_t b)
{
    return ((4UL << 24) | (((r)&255UL) << 16) | (((g)&255UL) << 8) | (((b)&255UL) << 0));
}
/* #define DISPLAY() ((0UL<<24)) */
#define END() ((33UL << 24))
static constexpr uint32_t end()
{
    return ((33UL << 24));
}
#define JUMP(dest) ((30UL << 24) | (((dest)&65535UL) << 0))
static constexpr uint32_t jump(uint32_t dest)
{
    return ((30UL << 24) | (((dest)&65535UL) << 0));
}
#define LINE_WIDTH(width) ((14UL << 24) | (((width)&4095UL) << 0))
static constexpr uint32_t lineWidth(uint32_t width)
{
    return ((14UL << 24) | (((width)&4095UL) << 0));
}
#define MACRO(m) ((37UL << 24) | (((m)&1UL) << 0))
static constexpr uint32_t macro(uint8_t m)
{
    return ((37UL << 24) | (((m)&1UL) << 0));
}
#define POINT_SIZE(size) ((13UL << 24) | (((size)&8191UL) << 0))
static constexpr uint32_t pointSize(uint32_t size)
{
    return ((13UL << 24) | (((size)&8191UL) << 0));
}
#define RESTORE_CONTEXT() ((35UL << 24))
static constexpr uint32_t RestoreContext = ((35UL << 24));
#define RETURN() ((36UL << 24))
static constexpr uint32_t Return = ((36UL << 24));
#define SAVE_CONTEXT() ((34UL << 24))
static constexpr uint32_t SaveContext = ((34UL << 24));
#define STENCIL_FUNC(func, ref, mask) ((10UL << 24) | (((func)&7UL) << 16) | (((ref)&255UL) << 8) | (((mask)&255UL) << 0))
static constexpr uint32_t stencilFunc(TestFunc func, uint8_t ref, uint8_t mask)
{
    return ((10UL << 24)
            | (((func)&7UL) << 16)
            | (((ref)&255UL) << 8)
            | (((mask)&255UL) << 0));
}
#define STENCIL_MASK(mask) ((19UL << 24) | (((mask)&255UL) << 0))
static constexpr uint32_t stencilMask(uint8_t mask)
{
    return ((19UL << 24) | (((mask)&255UL) << 0));
}
#define STENCIL_OP(sfail, spass) ((12UL << 24) | (((sfail)&7UL) << 3) | (((spass)&7UL) << 0))
static constexpr uint32_t stencilOp(StencilActions sfail, StencilActions spass)
{
    return ((12UL << 24)
            | (((static_cast<uint16_t>(sfail)) & 7UL) << 3)
            | (((static_cast<uint16_t>(spass)) & 7UL) << 0));
}
#define TAG(s) ((3UL << 24) | (((s)&255UL) << 0))
static constexpr uint32_t tag(uint8_t tag)
{
    return ((3UL << 24) | (((tag)&255UL) << 0));
}
#define TAG_MASK(mask) ((20UL << 24) | (((mask)&1UL) << 0))
static constexpr uint32_t tagMask(uint8_t mask)
{
    return ((20UL << 24) | (((mask)&1UL) << 0));
}
#define VERTEX2F(x, y) ((1UL << 30) | (((x)&32767UL) << 15) | (((y)&32767UL) << 0))
static constexpr uint32_t vertex2f(int16_t x,
                                   int16_t y)
{
    return ((1UL << 30) | (((x)&32767UL) << 15) | (((y)&32767UL) << 0));
};
#define VERTEX2II(x, y, handle, cell) ((2UL << 30) | (((x)&511UL) << 21) | (((y)&511UL) << 12) | (((handle)&31UL) << 7) | (((cell)&127UL) << 0))
static constexpr uint32_t vertex2ii(uint16_t x,
                                    uint16_t y,
                                    uint16_t handle = 0,
                                    uint16_t cell   = 0)
{
    return (2UL << 30)
           | (((x)&511UL) << 21)
           | (((y)&511UL) << 12)
           | (((handle)&31UL) << 7)
           | (((cell)&127UL) << 0);
};
/* ----------------- BT81x exclusive definitions -----------------*/
#if defined(BT81X_ENABLE)

    #define EVE_GLFORMAT 31UL /* used with BITMAP_LAYOUT to indicate bitmap-format is specified by BITMAP_EXT_FORMAT */

    #define DL_BITMAP_EXT_FORMAT 0x2E000000 /* requires OR'd arguments */

    /* extended Bitmap formats */
    #define EVE_COMPRESSED_RGBA_ASTC_4x4_KHR   37808UL
    #define EVE_COMPRESSED_RGBA_ASTC_5x4_KHR   37809UL
    #define EVE_COMPRESSED_RGBA_ASTC_5x5_KHR   37810UL
    #define EVE_COMPRESSED_RGBA_ASTC_6x5_KHR   37811UL
    #define EVE_COMPRESSED_RGBA_ASTC_6x6_KHR   37812UL
    #define EVE_COMPRESSED_RGBA_ASTC_8x5_KHR   37813UL
    #define EVE_COMPRESSED_RGBA_ASTC_8x6_KHR   37814UL
    #define EVE_COMPRESSED_RGBA_ASTC_8x8_KHR   37815UL
    #define EVE_COMPRESSED_RGBA_ASTC_10x5_KHR  37816UL
    #define EVE_COMPRESSED_RGBA_ASTC_10x6_KHR  37817UL
    #define EVE_COMPRESSED_RGBA_ASTC_10x8_KHR  37818UL
    #define EVE_COMPRESSED_RGBA_ASTC_10x10_KHR 37819UL
    #define EVE_COMPRESSED_RGBA_ASTC_12x10_KHR 37820UL
    #define EVE_COMPRESSED_RGBA_ASTC_12x12_KHR 37821UL

    #define EVE_RAM_ERR_REPORT      0x309800UL /* max 128 bytes null terminated string */
    #define EVE_RAM_FLASH           0x800000UL
    #define EVE_RAM_FLASH_POSTBLOB  0x801000UL
    #define EVE_RAM_PNG_BUFFER_SIZE 0x00A800UL /* If the loading image is in PNG format, the top 42K bytes from address 0xF5800 of RAM_G will \
    be overwritten as temporary data buffer for decoding process.  */
    #define EVE_RAM_G_SAFETY_SIZE (EVE_RAM_G_SIZE) - EVE_RAM_PNG_BUFFER_SIZE - 1

    #define EVE_OPT_FLASH  64UL
    #define EVE_OPT_FORMAT 4096UL
    #define EVE_OPT_FILL   8192UL
enum FlashStatus : uint8_t
{
    FLASH_STATUS_INIT     = 0b00u,
    FLASH_STATUS_DETACHED = 0b01u,
    FLASH_STATUS_BASIC    = 0b10u,
    FLASH_STATUS_FULL     = 0b11u,
};

enum FlashInitResult : uint32_t
{
    FlashSucsess      = 0,         // Ok
    FlashNotSupported = 0xE001,    // flash is not supported
    NoHeaderDetected  = 0xE002,    // no header detected in sector 0 – is flash blank?
    Sector0DataFailed = 0xE003,    // sector 0 data failed integrity check
    DeviceMismatch    = 0xE004,    // device/blob mismatch – was correct blob loaded?
    FailedFulSpeedTest
    = 0xE005    // failed full-speed test – check board wiring
};

    /* additional commands for BT81x */
    #define CMD_BITMAP_TRANSFORM 0xFFFFFF21
    #define CMD_SYNC             0xFFFFFF42 /* does not need a dedicated function, just use EVE_cmd_dl(CMD_SYNC) */
    #define CMD_FLASHERASE       0xFFFFFF44 /* does not need a dedicated function, just use EVE_cmd_dl(CMD_FLASHERASE) */
    #define CMD_FLASHWRITE       0xFFFFFF45
    #define CMD_FLASHREAD        0xFFFFFF46
    #define CMD_FLASHUPDATE      0xFFFFFF47
    #define CMD_FLASHDETACH      0xFFFFFF48 /* does not need a dedicated function, just use EVE_cmd_dl(CMD_FLASHDETACH) */
    #define CMD_FLASHATTACH      0xFFFFFF49 /* does not need a dedicated function, just use EVE_cmd_dl(CMD_FLASHATTACH) */
    #define CMD_FLASHFAST        0xFFFFFF4A
    #define CMD_FLASHSPIDESEL    0xFFFFFF4B /* does not need a dedicated function, just use EVE_cmd_dl(CMD_FLASHSPIDESEL) */
    #define CMD_FLASHSPITX       0xFFFFFF4C
    #define CMD_FLASHSPIRX       0xFFFFFF4D
    #define CMD_FLASHSOURCE      0xFFFFFF4E
    #define CMD_CLEARCACHE       0xFFFFFF4F /* does not need a dedicated function, just use EVE_cmd_dl(CMD_CLEARCACHE) */
    #define CMD_INFLATE2         0xFFFFFF50
    #define CMD_ROTATEAROUND     0xFFFFFF51
    #define CMD_RESETFONTS       0xFFFFFF52 /* does not need a dedicated function, just use EVE_cmd_dl(CMD_RESETFONTS) */
    #define CMD_ANIMSTART        0xFFFFFF53
    #define CMD_ANIMSTOP         0xFFFFFF54
    #define CMD_ANIMXY           0xFFFFFF55
    #define CMD_ANIMDRAW         0xFFFFFF56
    #define CMD_GRADIENTA        0xFFFFFF57
    #define CMD_FILLWIDTH        0xFFFFFF58
    #define CMD_APPENDF          0xFFFFFF59
    #define CMD_ANIMFRAME        0xFFFFFF5A
    #define CMD_VIDEOSTARTF      0xFFFFFF5F /* does not need a dedicated function, just use EVE_cmd_dl(CMD_VIDEOSTARTF) */

    #if 0
/* some undocumented commands for BT81x */
        #define CMD_NOP   0xFFFFFF5B
        #define CMD_SHA1  0xFFFFFF5C
        #define CMD_HMAC  0xFFFFFF5D
        #define CMD_LAST_ 0xFFFFFF5E

    #endif

    /* additional registers for BT81x */
    #define REG_ADAPTIVE_FRAMERATE 0x30257cUL
    #define REG_PLAYBACK_PAUSE     0x3025ecUL
    #define REG_FLASH_STATUS       0x3025f0UL
    #define REG_FLASH_SIZE         0x309024UL
    #define REG_PLAY_CONTROL       0x30914eUL
    #define REG_COPRO_PATCH_DTR    0x309162UL

    /* BT81x graphics engine specific macros */
    #define BITMAP_EXT_FORMAT(format) ((46UL << 24) | (((format)&65535UL) << 0))
static constexpr uint32_t bitmapExtFormat(BitmapExtFormats format)
{
    return ((46UL << 24)
            | (((static_cast<uint32_t>(format)) & 65535UL) << 0));
}
    #define BITMAP_SWIZZLE(r, g, b, a) ((47UL << 24) | (((r)&7UL) << 9) | (((g)&7UL) << 6) | (((b)&7UL) << 3) | (((a)&7UL) << 0))
static constexpr uint32_t bitmapSwizzle(SwizzleSource r,
                                        SwizzleSource g,
                                        SwizzleSource b,
                                        SwizzleSource a)
{
    return ((47UL << 24)
            | ((static_cast<uint8_t>(r) & 7UL) << 9)
            | ((static_cast<uint8_t>(g) & 7UL) << 6)
            | ((static_cast<uint8_t>(b) & 7UL) << 3)
            | ((static_cast<uint8_t>(a) & 7UL) << 0));
}
    #if defined(BT81X_ENABLE)
//        #define BITMAP_SOURCE2(flash_or_ram, addr) ((1UL << 24) | ((flash_or_ram) << 23) | (((addr)&8388607UL) << 0))
static constexpr uint32_t bitmapSource(MemoryMap targetMemory,
                                       uint32_t  addr)
{
    return ((1UL << 24)
            | (static_cast<uint8_t>(targetMemory) << 23)
            | (((addr)&8388607UL) << 0));
}
    #else
//        #define BITMAP_SOURCE(addr) ((1UL << 24) | (((addr)&4194303UL) << 0))
static constexpr uint32_t bitmapSource(uint32_t addr)
{
    return ((1UL << 24) | (((addr)&4194303UL) << 0));
}
    #endif

    #define INT_FRR() ((48UL << 24))
#endif

/* ----------------- FT81x / BT81x exclusive definitions -----------------*/
#if defined(FT81X_ENABLE)

    /* Host commands */
    #define EVE_CLKSEL       0x61 /* configure system clock */
    #define EVE_RST_PULSE    0x68 /* reset core - all registers default and processors reset */
    #define EVE_PINDRIVE     0x70 /* setup drive strength for various pins */
    #define EVE_PIN_PD_STATE 0x71 /* setup how pins behave during power down */

    /* Memory definitions */
    #define EVE_RAM_G         0x000000UL
    #define EVE_ROM_CHIPID    0x0C0000UL
    #define EVE_ROM_FONT      0x1E0000UL
    #define EVE_ROM_FONT_ADDR 0x2FFFFCUL
    #define EVE_RAM_DL        0x300000UL
    #define EVE_RAM_REG       0x302000UL
    #define EVE_RAM_CMD       0x308000UL

    /* Memory buffer sizes */
    #define EVE_RAM_G_SIZE   1024 * 1024L
    #define EVE_CMDFIFO_SIZE 4 * 1024L
    #define EVE_RAM_DL_SIZE  8 * 1024L

    /* various additional defines for FT81x */
    #define EVE_ADC_DIFFERENTIAL 1UL
    #define EVE_ADC_SINGLE_ENDED 0UL

    #define EVE_INT_G8  18UL
    #define EVE_INT_L8C 12UL
    #define EVE_INT_VGA 13UL

    #define EVE_OPT_MEDIAFIFO  16UL
    #define EVE_OPT_FULLSCREEN 8UL
    #define EVE_OPT_NOTEAR     4UL
    #define EVE_OPT_SOUND      32UL

    #define EVE_PALETTED565  14UL
    #define EVE_PALETTED4444 15UL
    #define EVE_PALETTED8    16UL
    #define EVE_L2           17UL

    /* additional commands for FT81x */
    #define CMD_MEDIAFIFO  0xFFFFFF39
    #define CMD_PLAYVIDEO  0xFFFFFF3A
    #define CMD_ROMFONT    0xFFFFFF3F
    #define CMD_SETBASE    0xFFFFFF38
    #define CMD_SETBITMAP  0xFFFFFF43
    #define CMD_SETFONT2   0xFFFFFF3B
    #define CMD_SETROTATE  0xFFFFFF36
    #define CMD_SETSCRATCH 0xFFFFFF3C
    #define CMD_SNAPSHOT2  0xFFFFFF37
    #define CMD_VIDEOFRAME 0xFFFFFF41
    #define CMD_VIDEOSTART 0xFFFFFF40

    /* the following are undocumented commands that therefore should not be used */
    #if 0
        #define CMD_CSKETCH         0xFFFFFF35
        #define CMD_INT_RAMSHARED   0xFFFFFF3D
        #define CMD_INT_SWLOADIMAGE 0xFFFFFF3E
    #endif

    /* Register definitions */
    #define REG_ANA_COMP          0x302184UL /* only listed in datasheet */
    #define REG_BIST_EN           0x302174UL /* only listed in datasheet */
    #define REG_CLOCK             0x302008UL
    #define REG_CMDB_SPACE        0x302574UL
    #define REG_CMDB_WRITE        0x302578UL
    #define REG_CMD_DL            0x302100UL
    #define REG_CMD_READ          0x3020f8UL
    #define REG_CMD_WRITE         0x3020fcUL
    #define REG_CPURESET          0x302020UL
    #define REG_CSPREAD           0x302068UL
    #define REG_CTOUCH_MODE       0x302104UL
    #define REG_CTOUCH_EXTENDED   0x302108UL
    #define REG_CTOUCH_TOUCH0_XY  0x302124UL /* only listed in datasheet */
    #define REG_CTOUCH_TOUCH4_X   0x30216cUL
    #define REG_CTOUCH_TOUCH4_Y   0x302120UL
    #define REG_CTOUCH_TOUCH1_XY  0x30211cUL
    #define REG_CTOUCH_TOUCH2_XY  0x30218cUL
    #define REG_CTOUCH_TOUCH3_XY  0x302190UL
    #define REG_TOUCH_CONFIG      0x302168UL
    #define REG_DATESTAMP         0x302564UL /* only listed in datasheet */
    #define REG_DITHER            0x302060UL
    #define REG_DLSWAP            0x302054UL
    #define REG_FRAMES            0x302004UL
    #define REG_FREQUENCY         0x30200cUL
    #define REG_GPIO              0x302094UL
    #define REG_GPIOX             0x30209cUL
    #define REG_GPIOX_DIR         0x302098UL
    #define REG_GPIO_DIR          0x302090UL
    #define REG_HCYCLE            0x30202cUL
    #define REG_HOFFSET           0x302030UL
    #define REG_HSIZE             0x302034UL
    #define REG_HSYNC0            0x302038UL
    #define REG_HSYNC1            0x30203cUL
    #define REG_ID                0x302000UL
    #define REG_INT_EN            0x3020acUL
    #define REG_INT_FLAGS         0x3020a8UL
    #define REG_INT_MASK          0x3020b0UL
    #define REG_MACRO_0           0x3020d8UL
    #define REG_MACRO_1           0x3020dcUL
    #define REG_MEDIAFIFO_READ    0x309014UL /* only listed in programmers guide */
    #define REG_MEDIAFIFO_WRITE   0x309018UL /* only listed in programmers guide */
    #define REG_OUTBITS           0x30205cUL
    #define REG_PCLK              0x302070UL
    #define REG_PCLK_POL          0x30206cUL
    #define REG_PLAY              0x30208cUL
    #define REG_PLAYBACK_FORMAT   0x3020c4UL
    #define REG_PLAYBACK_FREQ     0x3020c0UL
    #define REG_PLAYBACK_LENGTH   0x3020b8UL
    #define REG_PLAYBACK_LOOP     0x3020c8UL
    #define REG_PLAYBACK_PLAY     0x3020ccUL
    #define REG_PLAYBACK_READPTR  0x3020bcUL
    #define REG_PLAYBACK_START    0x3020b4UL
    #define REG_PWM_DUTY          0x3020d4UL
    #define REG_PWM_HZ            0x3020d0UL
    #define REG_RENDERMODE        0x302010UL /* only listed in datasheet */
    #define REG_ROTATE            0x302058UL
    #define REG_SNAPFORMAT        0x30201cUL /* only listed in datasheet */
    #define REG_SNAPSHOT          0x302018UL /* only listed in datasheet */
    #define REG_SNAPY             0x302014UL /* only listed in datasheet */
    #define REG_SOUND             0x302088UL
    #define REG_SPI_WIDTH         0x302188UL /* listed with false offset in programmers guide V1.1 */
    #define REG_SWIZZLE           0x302064UL
    #define REG_TAG               0x30207cUL
    #define REG_TAG_X             0x302074UL
    #define REG_TAG_Y             0x302078UL
    #define REG_TAP_CRC           0x302024UL /* only listed in datasheet */
    #define REG_TAP_MASK          0x302028UL /* only listed in datasheet */
    #define REG_TOUCH_ADC_MODE    0x302108UL
    #define REG_TOUCH_CHARGE      0x30210cUL
    #define REG_TOUCH_DIRECT_XY   0x30218cUL
    #define REG_TOUCH_DIRECT_Z1Z2 0x302190UL
    #define REG_TOUCH_MODE        0x302104UL
    #define REG_TOUCH_OVERSAMPLE  0x302114UL
    #define REG_TOUCH_RAW_XY      0x30211cUL
    #define REG_TOUCH_RZ          0x302120UL
    #define REG_TOUCH_RZTHRESH    0x302118UL
    #define REG_TOUCH_SCREEN_XY   0x302124UL
    #define REG_TOUCH_SETTLE      0x302110UL
    #define REG_TOUCH_TAG         0x30212cUL
    #define REG_TOUCH_TAG1        0x302134UL /* only listed in datasheet */
    #define REG_TOUCH_TAG1_XY     0x302130UL /* only listed in datasheet */
    #define REG_TOUCH_TAG2        0x30213cUL /* only listed in datasheet */
    #define REG_TOUCH_TAG2_XY     0x302138UL /* only listed in datasheet */
    #define REG_TOUCH_TAG3        0x302144UL /* only listed in datasheet */
    #define REG_TOUCH_TAG3_XY     0x302140UL /* only listed in datasheet */
    #define REG_TOUCH_TAG4        0x30214cUL /* only listed in datasheet */
    #define REG_TOUCH_TAG4_XY     0x302148UL /* only listed in datasheet */
    #define REG_TOUCH_TAG_XY      0x302128UL
    #define REG_TOUCH_TRANSFORM_A 0x302150UL
    #define REG_TOUCH_TRANSFORM_B 0x302154UL
    #define REG_TOUCH_TRANSFORM_C 0x302158UL
    #define REG_TOUCH_TRANSFORM_D 0x30215cUL
    #define REG_TOUCH_TRANSFORM_E 0x302160UL
    #define REG_TOUCH_TRANSFORM_F 0x302164UL
    #define REG_TRACKER           0x309000UL /* only listed in programmers guide */
    #define REG_TRACKER_1         0x309004UL /* only listed in programmers guide */
    #define REG_TRACKER_2         0x309008UL /* only listed in programmers guide */
    #define REG_TRACKER_3         0x30900cUL /* only listed in programmers guide */
    #define REG_TRACKER_4         0x309010UL /* only listed in programmers guide */
    #define REG_TRIM              0x302180UL
    #define REG_VCYCLE            0x302040UL
    #define REG_VOFFSET           0x302044UL
    #define REG_VOL_PB            0x302080UL
    #define REG_VOL_SOUND         0x302084UL
    #define REG_VSIZE             0x302048UL
    #define REG_VSYNC0            0x30204cUL
    #define REG_VSYNC1            0x302050UL

    #if 0
        #define REG_BUSYBITS     0x3020e8UL /* only listed as "reserved" in datasheet */
        #define REG_CRC          0x302178UL /* only listed as "reserved" in datasheet */
        #define REG_SPI_EARLY_TX 0x30217cUL /* only listed as "reserved" in datasheet */
        #define REG_ROMSUB_SEL   0x3020f0UL /* only listed as "reserved" in datasheet */
        #define REG_TOUCH_FAULT  0x302170UL /* only listed as "reserved" in datasheet */
    #endif

    /* FT81x graphics engine specific macros useful for static display list generation */

    /* beware, these are different to FTDIs implementation as these take the original values as parameters and not only the upper bits */
    #define BITMAP_LAYOUT_H(linestride, height) ((40UL << 24) | ((((linestride & 0xC00) >> 10) & 3UL) << 2) | ((((height & 0x600) >> 9) & 3UL) << 0))
static constexpr uint32_t bitmapLayoutH(int16_t linestride,
                                        int16_t height)
{
    return ((40UL << 24)
            | ((((linestride & 0xC00) >> 10) & 3UL) << 2)
            | ((((height & 0x600) >> 9) & 3UL) << 0));
}
    #define BITMAP_SIZE_H(width, height) ((41UL << 24) | ((((width & 0x600) >> 9) & 3UL) << 2) | ((((height & 0x600) >> 9) & 3UL) << 0))
static constexpr uint32_t bitmapSizeH(uint16_t width, uint16_t height)
{
    return ((41UL << 24)
            | ((((width & 0x600) >> 9) & 3UL) << 2)
            | ((((height & 0x600) >> 9) & 3UL) << 0));
}

    //#define NOP() ((45UL<<24))
    #define PALETTE_SOURCE(addr)        ((42UL << 24) | (((addr)&4194303UL) << 0))
    #define SCISSOR_SIZE(width, height) ((28UL << 24) | (((width)&4095UL) << 12) | (((height)&4095UL) << 0))
    #define SCISSOR_XY(x, y)            ((27UL << 24) | (((x)&2047UL) << 11) | (((y)&2047UL) << 0))
    #define VERTEX_FORMAT(frac)         ((39UL << 24) | (((frac)&7UL) << 0))
    #define VERTEX_TRANSLATE_X(x)       ((43UL << 24) | (((x)&131071UL) << 0))
    #define VERTEX_TRANSLATE_Y(y)       ((44UL << 24) | (((y)&131071UL) << 0))

/* ----------------- FT80x exclusive definitions -----------------*/
#else

    /* Memory definitions */
    #define EVE_RAM_G                   0x000000UL
    #define EVE_ROM_CHIPID              0x0C0000UL
    #define EVE_ROM_FONT                0x0BB23CUL
    #define EVE_ROM_FONT_ADDR           0x0FFFFCUL
    #define EVE_RAM_DL                  0x100000UL
    #define EVE_RAM_PAL                 0x102000UL
    #define EVE_RAM_CMD                 0x108000UL
    #define EVE_RAM_SCREENSHOT          0x1C2000UL

    /* Memory buffer sizes */
    #define EVE_RAM_G_SIZE              256 * 1024L
    #define EVE_CMDFIFO_SIZE            4 * 1024L
    #define EVE_RAM_DL_SIZE             8 * 1024L
    #define EVE_RAM_PAL_SIZE            1 * 1024L

    /* Register definitions */
    #define REG_ID                      0x102400UL
    #define REG_FRAMES                  0x102404UL
    #define REG_CLOCK                   0x102408UL
    #define REG_FREQUENCY               0x10240CUL
    #define REG_SCREENSHOT_EN           0x102410UL
    #define REG_SCREENSHOT_Y            0x102414UL
    #define REG_SCREENSHOT_START        0x102418UL
    #define REG_CPURESET                0x10241CUL
    #define REG_TAP_CRC                 0x102420UL
    #define REG_TAP_MASK                0x102424UL
    #define REG_HCYCLE                  0x102428UL
    #define REG_HOFFSET                 0x10242CUL
    #define REG_HSIZE                   0x102430UL
    #define REG_HSYNC0                  0x102434UL
    #define REG_HSYNC1                  0x102438UL
    #define REG_VCYCLE                  0x10243CUL
    #define REG_VOFFSET                 0x102440UL
    #define REG_VSIZE                   0x102444UL
    #define REG_VSYNC0                  0x102448UL
    #define REG_VSYNC1                  0x10244CUL
    #define REG_DLSWAP                  0x102450UL
    #define REG_ROTATE                  0x102454UL
    #define REG_OUTBITS                 0x102458UL
    #define REG_DITHER                  0x10245CUL
    #define REG_SWIZZLE                 0x102460UL
    #define REG_CSPREAD                 0x102464UL
    #define REG_PCLK_POL                0x102468UL
    #define REG_PCLK                    0x10246CUL
    #define REG_TAG_X                   0x102470UL
    #define REG_TAG_Y                   0x102474UL
    #define REG_TAG                     0x102478UL
    #define REG_VOL_PB                  0x10247CUL
    #define REG_VOL_SOUND               0x102480UL
    #define REG_SOUND                   0x102484UL
    #define REG_PLAY                    0x102488UL
    #define REG_GPIO_DIR                0x10248CUL
    #define REG_GPIO                    0x102490UL
    #define REG_INT_FLAGS               0x102498UL
    #define REG_INT_EN                  0x10249CUL
    #define REG_INT_MASK                0x1024A0UL
    #define REG_PLAYBACK_START          0x1024A4UL
    #define REG_PLAYBACK_LENGTH         0x1024A8UL
    #define REG_PLAYBACK_READPTR        0x1024ACUL
    #define REG_PLAYBACK_FREQ           0x1024B0UL
    #define REG_PLAYBACK_FORMAT         0x1024B4UL
    #define REG_PLAYBACK_LOOP           0x1024B8UL
    #define REG_PLAYBACK_PLAY           0x1024BCUL
    #define REG_PWM_HZ                  0x1024C0UL
    #define REG_PWM_DUTY                0x1024C4UL
    #define REG_MACRO_0                 0x1024C8UL
    #define REG_MACRO_1                 0x1024CCUL
    #define REG_SCREENSHOT_BUSY         0x1024D8UL
    #define REG_CMD_READ                0x1024E4UL
    #define REG_CMD_WRITE               0x1024E8UL
    #define REG_CMD_DL                  0x1024ECUL
    #define REG_TOUCH_MODE              0x1024F0UL
    #define REG_TOUCH_ADC_MODE          0x1024F4UL
    #define REG_TOUCH_CHARGE            0x1024F8UL
    #define REG_TOUCH_SETTLE            0x1024FCUL
    #define REG_TOUCH_OVERSAMPLE        0x102500UL
    #define REG_TOUCH_RZTHRESH          0x102504UL
    #define REG_TOUCH_RAW_XY            0x102508UL
    #define REG_TOUCH_RZ                0x10250CUL
    #define REG_TOUCH_SCREEN_XY         0x102510UL
    #define REG_TOUCH_TAG_XY            0x102514UL
    #define REG_TOUCH_TAG               0x102518UL
    #define REG_TOUCH_TRANSFORM_A       0x10251CUL
    #define REG_TOUCH_TRANSFORM_B       0x102520UL
    #define REG_TOUCH_TRANSFORM_C       0x102524UL
    #define REG_TOUCH_TRANSFORM_D       0x102528UL
    #define REG_TOUCH_TRANSFORM_E       0x10252CUL
    #define REG_TOUCH_TRANSFORM_F       0x102530UL
    #define REG_SCREENSHOT_READ         0x102554UL
    #define REG_TRIM                    0x10256CUL
    #define REG_TOUCH_DIRECT_XY         0x102574UL
    #define REG_TOUCH_DIRECT_Z1Z2       0x102578UL
    #define REG_TRACKER                 0x109000UL

    /* FT80x graphics engine specific macros useful for static display list generation */
    #define BITMAP_SOURCE(addr)         ((1UL << 24) | (((addr)&1048575UL) << 0))
    #define SCISSOR_SIZE(width, height) ((28UL << 24) | (((width)&1023UL) << 10) | (((height)&1023UL) << 0))
    #define SCISSOR_XY(x, y)            ((27UL << 24) | (((x)&511UL) << 9) | (((y)&511UL) << 0))

#endif
}    // namespace EVE
#endif /* EVE_H_ */
