#include "mbed_error.h"
#include "mbed_mem_trace.h"

#include <ftgui.h>
#include <mbed.h>
#include <mbed_debug.h>

int main()
{
    //**************
    auto a = new FTGUI::ApplicationWindow(new FTGUI::Dark);

    FTGUI::Button mb("=", a);
    FTGUI::List   l(a);
    FTGUI::Drawer menu(a);
    for(uint8_t i = 0; i < 6; ++i)
    {
        auto * r = new FTGUI::Rectangle(&menu);
        r->setBorderWidth(1);
        r->onReleased([&, i]() {
            l.setIndex(i);
        });
        new FTGUI::Label(to_string(i), r);
    }
    mb.onReleased(&menu, &FTGUI::Drawer::open);
    //    FTGUI::Button b1("<<", a);
    //    FTGUI::Button b2(">>", a);

    mb.setGeometry(0, 0, 80, 40);
    //    b1.setGeometry(0, 0, 80, 40);
    //    b2.setGeometry(400, 0, 80, 40);

    l.setVisibleItemCount(1);
    l.setOrientation(FTGUI::List::Vertical);
    //    l.setScrollable(false);
    l.setGeometry(0, 40, a->width(), a->height() - 40);
    //    l.setGeometry(0, 0, a->width(), a->height());
    //    b1.onReleased([&]() {
    //        l.pop();
    //    });
    //    b2.onReleased(&l, &FTGUI::List::push);

    for(uint8_t i = 0; i < 3; ++i)
    {
        auto * r = new FTGUI::Page(&l);
        new FTGUI::Label(to_string(i), r);
    }

    FTGUI::Button bb("Open", l.childAt(1));
    bb.setGeometry(50, 50, 80, 40);

    FTGUI::Dialog d(a);

    d.setWidth(300);
    d.setHeight(100);
    FTGUI::Button b("Close", &d);
    b.setGeometry(100, 20, 80, 40);

    bb.onReleased([&]() {
        d.open();
    });
    b.onReleased([&]() {
        d.close();
    });
    a->show();

    while(1)
    {
        ThisThread::sleep_for(2000);

        //        d.toggleVisible();
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
