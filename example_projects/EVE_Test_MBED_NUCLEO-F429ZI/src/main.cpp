#include <ft8xx.h>
#include <mbed.h>
#include <mbed_debug.h>
#include <mbed_error.h>

FTDisplayList * loadStaticDL1(FT8xx * screen)
{
    //Auto attach callback to next empty tag
    screen->setCallbackToTag([&](uint8_t tag) {
        printf("TAG_NUMBER: %u \n", tag);
    });
    EVE_cmd_dl(COLOR_RGB((rand() % 255), (rand() % 255), (rand() % 255)));    // change colour
    EVE_cmd_dl(BEGIN(EVE_BITMAPS));                                           // start drawing bitmaps
    EVE_cmd_dl(VERTEX2II(220, 170, 31, 'T'));                                 // ascii T in font 31
    EVE_cmd_dl(VERTEX2II(244, 170, 31, 'E'));                                 // ascii E
    EVE_cmd_dl(VERTEX2II(270, 170, 31, 'X'));                                 // ascii X
    EVE_cmd_dl(VERTEX2II(299, 170, 31, 'T'));                                 // ascii T
    EVE_cmd_dl(END());
    EVE_cmd_execute();
    // Copy this display list to Flash memory for using as static DL
    return screen->ramG()->saveDisplayList("DL1");
}

FTDisplayList * loadStaticDL2(FT8xx * screen)
{
    //Auto attach callback to next empty tag
    screen->setCallbackToTag([&](uint8_t tag) {
        printf("TAG_NUMBER: %u \n", tag);
    });
    EVE_cmd_dl(COLOR_RGB((rand() % 255), (rand() % 255), (rand() % 255)));    // change colour
    EVE_cmd_dl(BEGIN(EVE_BITMAPS));                                           // start drawing bitmaps
    EVE_cmd_dl(VERTEX2II(220, 50, 31, 'T'));                                  // ascii T in font 31
    EVE_cmd_dl(VERTEX2II(244, 50, 31, 'E'));                                  // ascii E
    EVE_cmd_dl(VERTEX2II(270, 50, 31, 'X'));                                  // ascii X
    EVE_cmd_dl(VERTEX2II(299, 50, 31, 'T'));                                  // ascii T
    EVE_cmd_dl(END());
    EVE_cmd_execute();
    // Copy this display list to Flash memory for using as static DL
    return screen->ramG()->saveDisplayList("DL2");
}

void loadDynamicDL()
{
    EVE_cmd_dl(COLOR_RGB((rand() % 255), (rand() % 255), (rand() % 255)));    // change colour
    EVE_cmd_dl(POINT_SIZE(320));                                              // set point size to 20 pixels in radius
    EVE_cmd_dl(BEGIN(EVE_POINTS));                                            // start drawing points
    EVE_cmd_dl(VERTEX2II(192, 133, 0, 0));                                    // red point
    EVE_cmd_dl(END());
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

    while(1)
    {
        EVE_cmd_dl(CMD_DLSTART);
        EVE_cmd_dl(CLEAR(1, 1, 1));
        for(const auto & l : dlVect)
            EVE_cmd_append(l->m_address, l->m_size);
        loadDynamicDL();
        EVE_cmd_dl(DL_END);
        EVE_cmd_dl(DL_DISPLAY);
        EVE_cmd_dl(CMD_SWAP);
        EVE_cmd_execute();
        EVE_memWrite16(REG_CMD_DL, 0);
        ThisThread::sleep_for(100);

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
