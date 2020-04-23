#include <mbed.h>
//#include "mbed_error.h"
//#include "mbed_fault_handler.h"
//#include <ftgui.h>

Serial pc(USBTX, USBRX);

//void loadTestDL()
//{
//    EVE_start_cmd_burst();
//    EVE_cmd_dl(CMD_DLSTART);
//    EVE_cmd_dl(CLEAR(1, 1, 1));
//    EVE_cmd_dl(TAG(11));
//    EVE_cmd_dl(BEGIN(EVE_BITMAPS)); // start drawing bitmaps
//    EVE_cmd_dl(VERTEX2II(220, 110, 31, 'T')); // ascii T in font 31
//    EVE_cmd_dl(VERTEX2II(244, 110, 31, 'E')); // ascii E
//    EVE_cmd_dl(VERTEX2II(270, 110, 31, 'X')); // ascii X
//    EVE_cmd_dl(VERTEX2II(299, 110, 31, 'T')); // ascii T
//    EVE_cmd_dl(END());

//    EVE_cmd_dl(TAG(10));
//    EVE_cmd_dl(COLOR_RGB((rand() % 255), (rand() % 255), (rand() % 255))); // change colour to red
//    EVE_cmd_dl(POINT_SIZE(320)); // set point size to 20 pixels in radius
//    EVE_cmd_dl(BEGIN(EVE_POINTS)); // start drawing points
//    EVE_cmd_dl(VERTEX2II(192, 133, 0, 0)); // red point
//    EVE_cmd_dl(END());
//    EVE_cmd_dl(TAG(0));
//    EVE_cmd_dl(DL_END);

//    EVE_cmd_dl(DL_DISPLAY);
//    EVE_cmd_dl(CMD_SWAP);
//    EVE_end_cmd_burst();
//    EVE_cmd_execute();
//}

int main()
{
    pc.baud(115200);
    //**Initialize screen
//    FT8xx m_screen(PF_9, PF_8, PF_7, PF_12, PB_2, PD_15);
    //** calibrate touchscreen with predefined values
//    FT8xx::TouchCalibrationResult tcResult = m_screen.touchCalibrate(true);
    //** or uncomment next block to start calibrate touchscreen and get calibrate values
    //    FT8xx::TouchCalibrationResult tcResult = m_screen.touchCalibrate(false);
    //    pc.printf("TOUCH_TRANSFORM_A: %#08x \n", tcResult.touch_a);
    //    pc.printf("TOUCH_TRANSFORM_B: %#08x \n", tcResult.touch_b);
    //    pc.printf("TOUCH_TRANSFORM_C: %#08x \n", tcResult.touch_c);
    //    pc.printf("TOUCH_TRANSFORM_D: %#08x \n", tcResult.touch_d);
    //    pc.printf("TOUCH_TRANSFORM_E: %#08x \n", tcResult.touch_e);
    //    pc.printf("TOUCH_TRANSFORM_F: %#08x \n", tcResult.touch_f);
    //Set backlight
//    m_screen.setBacklight(100);

#if (MBED_VERSION >= MBED_ENCODE_VERSION(5,8,0)) && MBED_CONF_EVENTS_PRESENT
    //**Uncomment any required callbacks
//    m_screen.attach([&](uint8_t f){pc.printf("EVE_INT_SWAP: %04x \n", f);}, EVE_INT_SWAP);
//    m_screen.attach([&](uint8_t f){pc.printf("EVE_INT_TOUCH: %04x \n", f);}, EVE_INT_TOUCH);
//    m_screen.attach([&](uint8_t f){pc.printf("EVE_INT_TAG: %04x \n", f);}, EVE_INT_TAG);
//    m_screen.attach([&](uint8_t f){pc.printf("EVE_INT_CONVCOMPLETE: %04x \n", f);}, EVE_INT_CONVCOMPLETE);
    //Attach one callback to all Touch_tag events
//    m_screen.attachToTags([&](uint8_t f){pc.printf("TAG_NUMBER: %04x \n", f);});
    //Attach callback to specific tag
//    m_screen.attachToTag([&](uint8_t tag){pc.printf("TAG_NUMBER 10: %u \n", tag);}, 10);
//     m_screen.attachToTag([&](uint8_t tag){pc.printf("TAG_NUMBER 11: %u \n", tag);}, 11);
    //**************
//     FTGUI::ApplicationWindow a;
//     a.show();

#endif
    while (1) {
//        loadTestDL();
        ThisThread::sleep_for(1000);
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
