#include "mbed_error.h"
#include "mbed_mem_trace.h"

#include <ftgui.h>
#include <mbed.h>
#include <mbed_debug.h>

int main()
{
    //Set backlight
    //    m_screen.setBacklight(100);

#if(MBED_VERSION >= MBED_ENCODE_VERSION(5, 8, 0)) && MBED_CONF_EVENTS_PRESENT
    //**Uncomment any required callbacks
    //    m_screen.attach([&](uint8_t f){printf("EVE_INT_SWAP: %04x \n", f);}, EVE_INT_SWAP);
    //    m_screen.attach([&](uint8_t f){printf("EVE_INT_TOUCH: %04x \n", f);}, EVE_INT_TOUCH);
    //    m_screen.attach([&](uint8_t f){printf("EVE_INT_TAG: %04x \n", f);}, EVE_INT_TAG);
    //    m_screen.attach([&](uint8_t f){printf("EVE_INT_CONVCOMPLETE: %04x \n", f);}, EVE_INT_CONVCOMPLETE);
    //Attach one callback to all Touch_tag events
    //    m_screen.attachToTags([&](uint8_t f){printf("TAG_NUMBER: %04x \n", f);});
    //Attach callback to specific tag
    //    m_screen.attachToTag([&](uint8_t tag){printf("TAG_NUMBER 10: %u \n", tag);}, 10);
    //     m_screen.attachToTag([&](uint8_t tag){printf("TAG_NUMBER 11: %u \n", tag);}, 11);
    //**************
    //
    //**************
    FTGUI::ApplicationWindow a(new FTGUI::Dark);

    //    FTGUI::Rectangle r(150, 150, 100, 100, &a);
    //    r.setState(FTGUI::Active);

    FTGUI::Button b1("Push Me", &a);
    ThisThread::sleep_for(1000);
    b1.setGeometry(10, 10, 80, 40);
    a.show();
    FTGUI::Label l1("I Love Atom Audio", &a);
    l1.hide();
    l1.setX(220);
    l1.setY(130);
    b1.setCallback(&l1,
                   &FTGUI::Label::toggleVisible);
#endif
    printf("%u\n", EVE_report_cmdoffset());
    while(1)
    {
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
