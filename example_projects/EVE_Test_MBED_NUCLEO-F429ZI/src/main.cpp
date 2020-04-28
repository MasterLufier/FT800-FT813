#include "mbed_error.h"
#include "mbed_mem_trace.h"
#include <mbed.h>
#include <mbed_debug.h>
#include <ftgui.h>

int main()
{
    //Set backlight
//    m_screen.setBacklight(100);

#if (MBED_VERSION >= MBED_ENCODE_VERSION(5,8,0)) && MBED_CONF_EVENTS_PRESENT
    //**Uncomment any required callbacks
    //    m_screen.attach([&](uint8_t f){printf("EVE_INT_SWAP: %04x \n", f);}, EVE_INT_SWAP);
    //    m_screen.attach([&](uint8_t f){printf("EVE_INT_TOUCH: %04x \n", f);}, EVE_INT_TOUCH);
    //    m_screen.attach([&](uint8_t f){printf("EVE_INT_TAG: %04x \n", f);}, EVE_INT_TAG);
    //    m_screen.attach([&](uint8_t f){printf("EVE_INT_CONVCOMPLETE: %04x \n", f);}, EVE_INT_CONVCOMPLETE);
    //Attach one callback to all Touch_tag events
    //    m_screen.attachToTags([&](uint8_t f){printf("TAG_NUMBER: %04x \n", f);});
    //Attach callback to specific tag
//    m_screen.attachToTag([&](uint8_t tag){pc.printf("TAG_NUMBER 10: %u \n", tag);}, 10);
//     m_screen.attachToTag([&](uint8_t tag){pc.printf("TAG_NUMBER 11: %u \n", tag);}, 11);
    //****
//    FTGUI::Color c(0xffffff);
    //**************
     FTGUI::ApplicationWindow a;
     a.setTheme(new FTGUI::Dark);
     FTGUI::Rectangle r("r", &a);
     FTGUI::Rectangle r2("r2", &a);
     FTGUI::Rectangle r3("r3", &a);

     FTGUI::Rectangle r4("r4", &a);
     FTGUI::Rectangle r5("r5", &a);
     FTGUI::Rectangle r6("r6", &a);

     r.setGeometry(0,0,100,90);
     r.setColor(0xffff0000);
     r.setBorderColor(0xffFFFFFF);

     r2.setGeometry(150,0,100,90);
     r2.setColor(0xccff0000);
     r2.setBorderColor(0xFFFFFFFF);
     r2.setBorderWidth(1);

     r3.setGeometry(300,0,100,90);
     r3.setColor(0xccff0000);
     r3.setBorderColor(0x550000FF);
     r3.setBorderWidth(13);

     r4.setGeometry(0,150,100,90);
     r4.setColor(0xccff0000);
     r4.setBorderColor(0xccFFFFFF);

     r5.setGeometry(150,150,100,90);
     r5.setColor(0xffff0000);
     r5.setBorderColor(0xccFFFFFF);
     r5.setBorderWidth(1);

     r6.setGeometry(300,150,100,90);
     r6.setColor(0xffff0000);
     r6.setBorderColor(0xffFFFFFF);
     r6.setBorderWidth(13);

     a.show();
     printf("%u\n", EVE_report_cmdoffset());
#endif
    while (1) {
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
