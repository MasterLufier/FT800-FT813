#include <ft8xx.h>
#include <mbed.h>
#include <mbed_debug.h>
#include <mbed_error.h>

using namespace EVE;
class A
{
public:
    A() {}
    void cbWithTag(string name, uint8_t tag)
    {
        printf("%s touched %u \n", name.c_str(), tag);
    }
    void cbWoTag(string name)
    {
        printf("%s touched \n", name.c_str());
    }
} a;

void cbWithTag(string name, uint8_t tag)
{
    printf("%s touched %u \n", name.c_str(), tag);
}

void cbWoTag(string name)
{
    printf("%s touched \n", name.c_str());
}

DisplayList * loadStaticDL1(FT8xx * screen)
{
#if(MBED_VERSION >= MBED_ENCODE_VERSION(5, 8, 0)) && MBED_CONF_EVENTS_PRESENT
    //Auto attach lambda callback to next empty tag
    uint8_t tag = screen->setCallbackToTag(&cbWithTag, "DL1");
    screen->deattachFromTag(tag);
    tag = screen->setCallbackToTag(&cbWoTag, "DL1");
    screen->push(EVE::tag(tag));
#endif
    screen->colorRGB((rand() % 255), (rand() % 255), (rand() % 255));    // change colour

    screen->begin(Bitmaps);                            // start drawing bitmaps
    screen->push(EVE::vertex2ii(220, 50, 31, 'T'));    // ascii T in font 31
    screen->push(EVE::vertex2ii(244, 50, 31, 'E'));    // ascii E
    screen->push(EVE::vertex2ii(270, 50, 31, 'X'));    // ascii X
    screen->push(EVE::vertex2ii(299, 50, 31, 'T'));    // ascii T
    screen->push(END());
    //    screen->execute();    // Copy this display list to Flash memory for using as static DL
    return screen->ramG()->saveDisplayList("DL1");
}

DisplayList * loadStaticDL2(FT8xx * screen)
{
#if(MBED_VERSION >= MBED_ENCODE_VERSION(5, 8, 0)) && MBED_CONF_EVENTS_PRESENT
    //Auto attach callback to next empty tag
    screen->clear(0, 0, 1);
    uint8_t tag = screen->setCallbackToTag(&a, &A::cbWoTag, "DL2");
    screen->deattachFromTag(tag);
    tag = screen->setCallbackToTag(&a, &A::cbWithTag, "DL2");
    screen->push(EVE::tag(tag));
#endif
    screen->colorRGB((rand() % 255), (rand() % 255), (rand() % 255));    // change colour

    screen->push(begin(Bitmaps));                       // start drawing bitmaps
    screen->push(EVE::vertex2ii(220, 120, 31, 'T'));    // ascii T in font 31
    screen->push(EVE::vertex2ii(244, 120, 31, 'E'));    // ascii E
    screen->push(EVE::vertex2ii(270, 120, 31, 'X'));    // ascii X
    screen->push(EVE::vertex2ii(299, 120, 31, 'T'));    // ascii T
    screen->push(END());
    //    screen->execute();    // Copy this display list to Flash memory for using as static DL
    return screen->ramG()->saveDisplayList("DL2");
}

DisplayList * loadStaticDL3(FT8xx * screen)
{
#if(MBED_VERSION >= MBED_ENCODE_VERSION(5, 8, 0)) && MBED_CONF_EVENTS_PRESENT
    //Auto attach callback to next empty tag
    uint8_t tag = screen->setCallbackToTag([&](uint8_t tag) {
        printf("TAG_NUMBER: %u \n", tag);
    });
    screen->push(EVE::tag(tag));
#endif
    screen->colorRGB((rand() % 255), (rand() % 255), (rand() % 255));    // change colour

    screen->push(begin(Bitmaps));                       // start drawing bitmaps
    screen->push(EVE::vertex2ii(220, 180, 31, 'T'));    // ascii T in font 31
    screen->push(EVE::vertex2ii(244, 180, 31, 'E'));    // ascii E
    screen->push(EVE::vertex2ii(270, 180, 31, 'X'));    // ascii X
    screen->push(EVE::vertex2ii(299, 180, 31, 'T'));    // ascii T
    screen->push(END());
    //    screen->execute();    // Copy this display list to Flash memory for using as static DL
    auto dl = screen->ramG()->saveDisplayList("DL3");
    screen->deattachFromTag(tag);
#if(MBED_VERSION >= MBED_ENCODE_VERSION(5, 8, 0)) && MBED_CONF_EVENTS_PRESENT
    //Auto attach callback to next empty tag
    tag = screen->setCallbackToTag([&](uint8_t tag) {
        printf("TAG_NUMBER: %u \n", tag);
    });
    screen->push(EVE::tag(tag));
#endif
    screen->colorRGB((rand() % 255), (rand() % 255), (rand() % 255));    // change colour

    screen->push(begin(Bitmaps));                       // start drawing bitmaps
    screen->push(EVE::vertex2ii(220, 180, 31, 'R'));    // ascii T in font 31
    screen->push(EVE::vertex2ii(244, 180, 31, 'A'));    // ascii E
    screen->push(EVE::vertex2ii(270, 180, 31, 'N'));    // ascii X
    screen->push(EVE::vertex2ii(299, 180, 31, 'D'));    // ascii T
    screen->push(EVE::vertex2ii(325, 180, 31, 'R'));    // ascii T
    screen->push(END());
    //    screen->execute();    // Copy this display list to Flash memory for using as static DL
    return screen->ramG()->updateDisplayList(dl);
}

void loadDynamicDL(FT8xx * screen)
{
    screen->push(EVE::tag(0));
    screen->colorRGB((rand() % 255), (rand() % 255), (rand() % 255));

    screen->point((rand() % EVE_HSIZE), (rand() % EVE_VSIZE), 20);
    //    screen->point((rand() % 480), (rand() % 270), 320);
    //    screen->point((rand() % 480), (rand() % 270), 320);
    //    screen->point((rand() % 480), (rand() % 270), 320);
    //    screen->point((rand() % 480), (rand() % 270), 320);

    //    return screen->ramG()->saveDisplayList("DL4");
}

int main()
{
    //**Initialize screen
    FT8xx screen(PF_9, PF_8, PF_7, PF_12, PB_2, PD_15);
    //**Initialize DL storage in RamG
    screen.ramGInit();
    screen.flashInit(8000);
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

#if(MBED_VERSION >= MBED_ENCODE_VERSION(5, 8, 0)) && MBED_CONF_EVENTS_PRESENT
    //**Uncomment any required callbacks
    //    m_screen.attach([&](uint8_t f){printf("EVE_INT_SWAP: %04x \n", f);}, EVE_INT_SWAP);
    //    m_screen.attach([&](uint8_t f){printf("EVE_INT_TOUCH: %04x \n", f);}, EVE_INT_TOUCH);
    //    m_screen.attach([&](uint8_t f){printf("EVE_INT_TAG: %04x \n", f);}, EVE_INT_TAG);
    //    m_screen.attach([&](uint8_t f){printf("EVE_INT_CONVCOMPLETE: %04x \n", f);}, EVE_INT_CONVCOMPLETE);
    //Attach one callback to all Touch_tag events
    //    m_screen.attachToTags([&](uint8_t f){printf("TAG_NUMBER: %04x \n", f);});
    //Manual attach callback to specific tag
    //    m_screen.attachToTag([&](uint8_t tag) {
    //        printf("TAG_NUMBER 10: %u \n", tag);
    //    },
    //                         10);
    //    m_screen.attachToTag([&](uint8_t tag) {
    //        printf("TAG_NUMBER 11: %u \n", tag);
    //    },
    //                         11);
    //    m_screen.attachToTag([&](uint8_t tag) {
    //        printf("TAG_NUMBER 12: %u \n", tag);
    //    },
    //                         12);
    //**************
#endif
    //Static DL Storage

    //    loadStaticDL1(&m_screen);
    //    loadStaticDL2(&m_screen);
    //    loadStaticDL3(&m_screen);
    //    //Start DL
    //    screen.dlStart();
    //    screen.clear();
    //    screen.push(EVE::tag(0));
    //    screen.colorRGB(0xff, 0x00, 0x00);
    //    screen.point(0, 0, 50);
    //    screen.point(EVE_HSIZE, EVE_VSIZE, 50);
    //    screen.swap();
    //    screen.execute();
    //    //Take snapshot
    //    screen.ramG()->saveSnapshot("SN1");

    screen.dlStart();
    screen.clear();
    //    loadDynamicDL(&screen);
    //    screen.line(20, 20, 30, 30, 1);
    //    screen.clearColorARGB(0xFFFF0000);
    //    screen.colorRGB((rand() % 255), (rand() % 255), (rand() % 255));
    //    screen.rectangle(20, 20, 100, 100, 1);

    //    screen.text(100, 100, 27, EVE_OPT_CENTER, "TEXT");
    //    screen.button(20, 20, 200, 100, 27, ButtonOpt::Flat, "Push me!");
    screen.gradientA(0, 0, 0xccff0000, 0, 300, 0x4400ff00);
    screen.swap();
    screen.execute();

    while(1)
    {
        //        screen.dlStart();
        //        screen.clear();
        //        screen.button(20, 20, 200, 100, 27, Draw3D, "Push Me!");
        //        //        for(const auto l : m_screen.ramG()->pool())
        //        //        {
        //        //            screen.append(l);
        //        //        }
        //        //        loadDynamicDL(&m_screen);
        //        screen.swap();
        //        screen.execute();
        ThisThread::sleep_for(1000);

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
