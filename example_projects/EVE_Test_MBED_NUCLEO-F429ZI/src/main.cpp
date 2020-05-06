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

FTDisplayList * loadStaticDL1(FT8xx * screen)
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
    screen->execute();    // Copy this display list to Flash memory for using as static DL
    return screen->saveDisplayList("DL1");
}

FTDisplayList * loadStaticDL2(FT8xx * screen)
{
#if(MBED_VERSION >= MBED_ENCODE_VERSION(5, 8, 0)) && MBED_CONF_EVENTS_PRESENT
    //Auto attach callback to next empty tag
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
    screen->execute();    // Copy this display list to Flash memory for using as static DL
    return screen->saveDisplayList("DL2");
}

FTDisplayList * loadStaticDL3(FT8xx * screen)
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
    screen->execute();    // Copy this display list to Flash memory for using as static DL
    return screen->saveDisplayList("DL3");
}

void loadDynamicDL(FT8xx * screen)
{
    screen->push(EVE::tag(0));
    screen->colorRGB((rand() % 255), (rand() % 255), (rand() % 255));    // change colour

    screen->point(192, 133, 320);
    //    screen->pushText(1, 200, 31, 0, "TEXT TEXT TEXT TEXT0");
    //    screen->pushText(1, 150, 31, 0, "TEXT0");
    //    screen->pushText(1, 100, 31, 0, "TEX0");
    //    screen->pushText(1, 50, 31, 0, "TE0");
    //    screen->pushText(1, 1, 31, 0, "");
}

int main()
{
    //**Initialize screen
    FT8xx m_screen(PF_9, PF_8, PF_7, PF_12, PB_2, PD_15);
    //**Initialize DL storage in RamG
    m_screen.ramGInit();
    //** calibrate touchscreen with predefined values
    FT8xx::TouchCalibrationResult tcResult = m_screen.touchCalibrate(true);
    //** or uncomment next block to start calibrate touchscreen and get calibrate values
    //    FT8xx::TouchCalibrationResult tcResult = m_screen.touchCalibrate(false);
    //    printf("TOUCH_TRANSFORM_A: %#08x \n", tcResult.touch_a);
    //    printf("TOUCH_TRANSFORM_B: %#08x \n", tcResult.touch_b);
    //    printf("TOUCH_TRANSFORM_C: %#08x \n", tcResult.touch_c);
    //    printf("TOUCH_TRANSFORM_D: %#08x \n", tcResult.touch_d);
    //    printf("TOUCH_TRANSFORM_E: %#08x \n", tcResult.touch_e);
    //    printf("TOUCH_TRANSFORM_F: %#08x \n", tcResult.touch_f);
    //Set backlight
    m_screen.setBacklight(100);

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
    std::vector<FTDisplayList *> dlVect;
    dlVect.push_back(loadStaticDL1(&m_screen));
    dlVect.push_back(loadStaticDL2(&m_screen));
    dlVect.push_back(loadStaticDL3(&m_screen));

    int i = 0;
    while(1)
    {
        m_screen.dlStart();
        m_screen.clear();
        for(const auto l : dlVect)
            m_screen.append(l);
        loadDynamicDL(&m_screen);
        if(i == 0)
            m_screen.push(0xffffffff);
        //        m_screen.push(DL_END);
        //        m_screen.push(DL_DISPLAY);
        //        m_screen.push(CMD_SWAP);
        m_screen.swap();
        m_screen.execute();
        ThisThread::sleep_for(100);
        ++i;
        //**Example Backlight fade conrol
        //        //Linear
        //        m_screen.backlightFade(0, 128, 500);
        //        ThisThread::sleep_for(1000);
        //        m_screen.backlightFade(128, 0, 500);
        //        ThisThread::sleep_for(1000);
        //        loadTestDL();
        //        //Quad
        //        m_screen.backlightFade(0, 128, 500, FT8xx::Quad);
        //        ThisThread::sleep_for(1000);
        //        m_screen.backlightFade(128, 0, 500, FT8xx::Quad);
        //        ThisThread::sleep_for(1000);
        //        loadTestDL();
        //        //Cubic
        //        m_screen.backlightFade(0, 128, 500, FT8xx::Cubic);
        //        ThisThread::sleep_for(1000);
        //        m_screen.backlightFade(128, 0, 500, FT8xx::Cubic);
        //        ThisThread::sleep_for(1000);
        //        loadTestDL();
        //        //Quart
        //        m_screen.backlightFade(0, 128, 500, FT8xx::Quart);
        //        ThisThread::sleep_for(1000);
        //        m_screen.backlightFade(128, 0, 500, FT8xx::Quart);
        //        ThisThread::sleep_for(1000);
        //*********
    }
    return 0;
}
