#include <mbed.h>
#include <ft8xx.h>

Serial pc(USBTX, USBRX);

extern void loadTestDL()
{
    EVE_cmd_dl(CMD_DLSTART);
    EVE_cmd_dl(CLEAR(1, 1, 1));
    EVE_cmd_dl(TAG(0));
    EVE_cmd_dl(BEGIN(EVE_BITMAPS)); // start drawing bitmaps
    EVE_cmd_dl(VERTEX2II(220, 110, 31, 'T')); // ascii T in font 31
    EVE_cmd_dl(VERTEX2II(244, 110, 31, 'E')); // ascii E
    EVE_cmd_dl(VERTEX2II(270, 110, 31, 'X')); // ascii X
    EVE_cmd_dl(VERTEX2II(299, 110, 31, 'T')); // ascii T
    EVE_cmd_dl(END());

    EVE_cmd_dl(TAG(10));
    EVE_cmd_dl(COLOR_RGB(160, 22, 22)); // change colour to red
    EVE_cmd_dl(POINT_SIZE(320)); // set point size to 20 pixels in radius
    EVE_cmd_dl(BEGIN(EVE_POINTS)); // start drawing points
    EVE_cmd_dl(VERTEX2II(192, 133, 0, 0)); // red point
    EVE_cmd_dl(END());
    EVE_cmd_dl(TAG(0));
    EVE_cmd_dl(DL_END);

    EVE_cmd_dl(DL_DISPLAY);
    EVE_cmd_dl(CMD_SWAP);
    EVE_cmd_execute();
}
DigitalOut green(LED1);

int main()
{
    pc.baud(115200);
    //**Initialize screen
    FT8xx m_screen(PF_9, PF_8, PF_7, PF_12, PB_2, PD_15);
    //** calibrate touchscreen with predefined values
    FT8xx::TouchCalibrationResult tcResult = m_screen.touchCalibrate(true);
    //** or uncomment next block to start calibrate touchscreen and get calibrate values
    //    FT8xx::TouchCalibrationResult tcResult = m_screen.touchCalibrate(false);
    pc.printf("TOUCH_TRANSFORM_A: %#010x \n", tcResult.touch_a);
    pc.printf("TOUCH_TRANSFORM_B: %#010x \n", tcResult.touch_b);
    pc.printf("TOUCH_TRANSFORM_C: %#010x \n", tcResult.touch_c);
    pc.printf("TOUCH_TRANSFORM_D: %#010x \n", tcResult.touch_d);
    pc.printf("TOUCH_TRANSFORM_E: %#010x \n", tcResult.touch_e);
    pc.printf("TOUCH_TRANSFORM_F: %#010x \n", tcResult.touch_f);
    //** load test display list
    loadTestDL();

#if (MBED_VERSION >= MBED_ENCODE_VERSION(5,8,0)) && MBED_CONF_EVENTS_PRESENT
    //**Uncomment any required callbacks
    m_screen.attach([&](uint8_t f){pc.printf("EVE_INT_SWAP: %04x \n", f);}, EVE_INT_SWAP);
    m_screen.attach([&](uint8_t f){pc.printf("EVE_INT_TOUCH: %04x \n", f);}, EVE_INT_TOUCH);
    m_screen.attach([&](uint8_t f){pc.printf("EVE_INT_TAG: %04x \n", f);}, EVE_INT_TAG);
    m_screen.attach([&](uint8_t f){pc.printf("EVE_INT_CONVCOMPLETE: %04x \n", f);}, EVE_INT_CONVCOMPLETE);
    m_screen.attachToTag([&](uint8_t f){pc.printf("TAG_NUMBER: %04x \n", f);});

#endif
    while (1) {
        ThisThread::sleep_for(1000);
        green = !green;
    }
    return 0;
}
