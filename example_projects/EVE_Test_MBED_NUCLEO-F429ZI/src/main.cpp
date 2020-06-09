#include "mbed_error.h"
#include "mbed_mem_trace.h"

#include <ftgui.h>
#include <mbed.h>
#include <mbed_debug.h>

int main()
{
    //**************
    auto a = new FTGUI::ApplicationWindow(new FTGUI::Dark);

    FTGUI::Button b1("<<", a);
    FTGUI::Button b2(">>", a);

    b1.setGeometry(0, 0, 80, 40);
    b2.setGeometry(400, 0, 80, 40);

    //    FTGUI::Page r1(&layout);
    //    FTGUI::Page r2(&layout);

    //    FTGUI::Label l1("Reaaaally, reaaaaaally, loooooooooong looooooooong teeeeeext.", r1.contentItem());
    //    l1.setWidth(l1.parent()->width());
    //    l1.setFillWidth(true);

    //    FTGUI::Label l2("I Love Atom Audio more!", r2.contentItem());

    FTGUI::List l(a);
    l.setVisibleItemCount(1);
    l.setOrientation(FTGUI::List::Vertical);
    //    l.setScrollable(false);
    l.setGeometry(0, 40, a->width(), a->height() - 40);
    //    l.setGeometry(0, 0, a->width(), a->height());
    b1.onReleased([&]() {
        l.pop();
    });
    b2.onReleased(&l, &FTGUI::List::push);

    for(uint8_t i = 0; i < 3; ++i)
    {
        auto * r = new FTGUI::Page(&l);
        new FTGUI::Label(to_string(i), r);
        //        r->setBorderWidth(1);
    }

    //    r1.setBorderWidth(1);
    //    r2.setBorderWidth(1);
    //    r3.setBorderWidth(1);
    a->show();

    while(1)
    {
        ThisThread::sleep_for(100);

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
