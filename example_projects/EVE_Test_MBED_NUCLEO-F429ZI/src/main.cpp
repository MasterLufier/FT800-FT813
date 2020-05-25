#include "mbed_error.h"
#include "mbed_mem_trace.h"

#include <ftgui.h>
#include <mbed.h>
#include <mbed_debug.h>

int main()
{
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
    l1.setWidth(l1.parent()->width());
    l1.setFillWidth(true);

    FTGUI::Label l2("I Love Atom Audio more!", r2.contentItem());

    b1.setCallback(&layout,
                   &FTGUI::StackView::pop);

    b2.setCallback(&layout,
                   &FTGUI::StackView::push);
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
