#include "logoAtom_940x74_ARGB4.h"

#include <ft8xx.h>
#include <mbed.h>
#include <mbed_debug.h>
#include <mbed_error.h>

using namespace EVE;
class A
{
public:
    A() {}
    void cbWithTag(string name, uint16_t tag)
    {
        printf("%s touched %u \n", name.c_str(), tag);
    }
    void cbWoTag(string name)
    {
        printf("%s touched \n", name.c_str());
    }
} a;

void cbWithTag(string name, uint16_t tag)
{
    printf("%s touched %u \n", name.c_str(), tag);
}

void cbWoTag(string name)
{
    printf("%s touched \n", name.c_str());
}

DisplayList * loadStaticDL1(FT8xx * screen)
{
    //Auto attach lambda callback to next empty tag
    uint8_t tag = screen->setCallbackToTag(&cbWithTag, 0, "DL1");
    screen->deattachFromTag(tag);
    tag = screen->setCallbackToTag(&cbWoTag, 0, "DL1");
    screen->tag(tag);
    screen->colorRGB((rand() % 255), (rand() % 255), (rand() % 255));    // change colour

    screen->begin(Bitmaps);                     // start drawing bitmaps
    screen->vertexPointII(220, 50, 31, 'T');    // ascii T in font 31
    screen->vertexPointII(244, 50, 31, 'E');    // ascii E
    screen->vertexPointII(270, 50, 31, 'X');    // ascii X
    screen->vertexPointII(299, 50, 31, 'T');    // ascii T
    screen->end();
    // Copy this display list to Flash memory for using as static DL
    return screen->ramG()->saveDisplayList("DL1");
}

DisplayList * loadStaticDL2(FT8xx * screen)
{
    //Auto attach callback to next empty tag
    uint8_t tag = screen->setCallbackToTag(&a, &A::cbWoTag, 0, "DL2");
    screen->setCallbackToTag(&a, &A::cbWithTag, tag, "DL2");
    screen->tag(tag);
    screen->colorRGB((rand() % 255), (rand() % 255), (rand() % 255));    // change colour

    screen->begin(Bitmaps);                      // start drawing bitmaps
    screen->vertexPointII(220, 120, 31, 'T');    // ascii T in font 31
    screen->vertexPointII(244, 120, 31, 'E');    // ascii E
    screen->vertexPointII(270, 120, 31, 'X');    // ascii X
    screen->vertexPointII(299, 120, 31, 'T');    // ascii T
    screen->end();
    // Copy this display list to Flash memory for using as static DL
    return screen->ramG()->saveDisplayList("DL2");
}

DisplayList * loadStaticDL3(FT8xx * screen)
{
    //Auto attach callback to next empty tag
    uint8_t tag = screen->setCallbackToTag([&](uint8_t tag) {
        printf("TAG_NUMBER: %u \n", tag);
    });
    screen->tag(tag);
    screen->colorRGB((rand() % 255), (rand() % 255), (rand() % 255));    // change colour

    screen->begin(Bitmaps);                      // start drawing bitmaps
    screen->vertexPointII(220, 180, 31, 'T');    // ascii T
    screen->vertexPointII(244, 180, 31, 'E');    // ascii E
    screen->vertexPointII(270, 180, 31, 'X');    // ascii X
    screen->vertexPointII(299, 180, 31, 'T');    // ascii T
    screen->end();
    // Copy this display list to Flash memory for using as static DL
    auto dl = screen->ramG()->saveDisplayList("DL3");

    screen->deattachFromTag(tag);
    //Auto attach callback to next empty tag
    tag = screen->setCallbackToTag([&](uint8_t tag) {
        printf("TAG_NUMBER: %u \n", tag);
    });
    screen->tag(tag);
    screen->colorRGB((rand() % 255), (rand() % 255), (rand() % 255));    // change colour

    screen->push(begin(Bitmaps));                // start drawing bitmaps
    screen->vertexPointII(220, 180, 31, 'R');    // ascii T in font 31
    screen->vertexPointII(244, 180, 31, 'A');    // ascii E
    screen->vertexPointII(270, 180, 31, 'N');    // ascii X
    screen->vertexPointII(299, 180, 31, 'D');    // ascii T
    screen->vertexPointII(325, 180, 31, 'R');    // ascii T
    screen->end();
    // Copy this display list to Flash memory for using as static DL
    return screen->ramG()->updateDisplayList(dl);
}

void loadDynamicDL(FT8xx * screen)
{
    screen->tag(0);
    screen->colorRGB((rand() % 255), (rand() % 255), (rand() % 255));

    screen->point((rand() % EVE_HSIZE), (rand() % EVE_VSIZE), 20);
}

int main()
{
    //**Initialize screen
    FT8xx screen;    //(PF_9, PF_8, PF_7, PF_12, PB_2, PD_15);
    //**Initialize DL storage in RamG
    screen.ramGInit();
    //    screen.flashInit(8000);
    //** calibrate touchscreen with predefined values
    screen.touchCalibrate(true);

    //** or uncomment next block to start calibrate touchscreen and get calibrate values
    //    FT8xx::TouchCalibrationResult tcResult = m_screen.touchCalibrate(false);
    //    printf("TOUCH_TRANSFORM_A: %#08x \n", tcResult.touch_a);
    //    printf("TOUCH_TRANSFORM_B: %#08x \n", tcResult.touch_b);
    //    printf("TOUCH_TRANSFORM_C: %#08x \n", tcResult.touch_c);
    //    printf("TOUCH_TRANSFORM_D: %#08x \n", tcResult.touch_d);
    //    printf("TOUCH_TRANSFORM_E: %#08x \n", tcResult.touch_e);
    //    printf("TOUCH_TRANSFORM_F: %#08x \n", tcResult.touch_f);
    //Set backlight
    screen.setBacklight(100);

    //**Uncomment any required callbacks
    //    m_screen.attach([&](uint8_t f){printf("EVE_INT_SWAP: %04x \n", f);}, EVE_INT_SWAP);
    //        m_screen.attach([&](uint8_t f){printf("EVE_INT_TOUCH: %04x \n", f);}, EVE_INT_TOUCH);
    //    screen.attach([&](uint8_t f) {
    //        printf("EVE_INT_CONVCOMPLETE: %04x \n", f);
    //    },
    //                  EVE_INT_CONVCOMPLETE);

    //**************
    //Static DL Storage
    //    loadStaticDL1(&screen);
    //    loadStaticDL2(&screen);
    //    loadStaticDL3(&screen);

    //    int32_t val = 0;

    //    uint8_t tag = screen.setCallbackToTag([&](uint8_t) -> void {
    //        //        val < 65535
    //        //            ? screen.animate(&val, 0, 65535, 400, FT8xx::Cubic)
    //        //            : screen.animate(&val, 65535, 0, 400, FT8xx::Quart);
    //    });

    screen.attachTouchDetectedCallback([&](uint8_t c) {
        int32_t xy = screen.touchXY();
        debug("TD: %i:%i\n", xy & 0xffff, xy >> 16);
    });
    screen.attachTouchConversionsCallback([&](uint8_t c) {
        int32_t xy = screen.touchXY();
        debug("TC: %i:%i\n", xy & 0xffff, xy >> 16);
    });
    auto png = screen.ramG()->loadPNG("logo",
                                      logoAtom,
                                      ImagePNGFormat::ARGB4,
                                      940,
                                      74);

    screen.dlStart();
    screen.clearColorARGB(0xFF000000);
    screen.clear();
    screen.append(png, -220, 80);
    screen.swap();
    screen.execute();
    while(1)
    {
        ThisThread::sleep_for(10);

        //**Example Backlight fade conrol
        //        //Linear
        //        screen.backlightFade(0, 128, 500);
        //        ThisThread::sleep_for(1000);
        //        screen.backlightFade(128, 0, 500);
        //        ThisThread::sleep_for(1000);
        //        loadTestDL();
        //        //Quad
        //        screen.backlightFade(0, 128, 500, FT8xx::Quad);
        //        ThisThread::sleep_for(1000);
        //        screen.backlightFade(128, 0, 500, FT8xx::Quad);
        //        ThisThread::sleep_for(1000);
        //        loadTestDL();
        //        //Cubic
        //        screen.backlightFade(0, 128, 500, FT8xx::Cubic);
        //        ThisThread::sleep_for(1000);
        //        screen.backlightFade(128, 0, 500, FT8xx::Cubic);
        //        ThisThread::sleep_for(1000);
        //        loadTestDL();
        //        //Quart
        //        screen.backlightFade(0, 128, 500, FT8xx::Quart);
        //        ThisThread::sleep_for(1000);
        //        screen.backlightFade(128, 0, 500, FT8xx::Quart);
        //        ThisThread::sleep_for(1000);
        //*********
    }
    return 0;
}
