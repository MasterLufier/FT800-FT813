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
    auto a = new FTGUI::ApplicationWindow(new FTGUI::Dark);

    //    FTGUI::Rectangle r(150, 150, 100, 100, &a);
    //    r.setState(FTGUI::Active);

    FTGUI::Button b1("<<", a);
    FTGUI::Button b2(">>", a);

    b1.setGeometry(0, 0, 80, 40);
    b2.setGeometry(400, 0, 80, 40);

    FTGUI::StackView layout(a);
    layout.setGeometry(0, 40, a->width(), a->height() - 40);

    FTGUI::Page r1(&layout);
    FTGUI::Page r2(&layout);

    FTGUI::Label l1("Reaaaally, reaaaaaally, loooooooooong looooooooong teeeeeext.", r1.contentItem());

    FTGUI::Label l2("I Love Atom Audio more!", r2.contentItem());

    b1.setCallback(&layout,
                   &FTGUI::StackView::pop);

    b2.setCallback(&layout,
                   &FTGUI::StackView::push);
    a->show();
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
