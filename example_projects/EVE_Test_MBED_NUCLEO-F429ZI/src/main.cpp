#include "mbed_error.h"
#include "mbed_mem_trace.h"

#include <ftgui.h>
#include <mbed.h>
#include <mbed_debug.h>

class A
{
public:
    A() {}
    void cbWXY(uint16_t x, uint16_t y)
    {
        debug("Mb cbWxy: %i:%i\n", x, y);
    }

    void cbWoXY()
    {
        debug("Mb cbWoXY\n");
    }

    void cbWarg(string arg)
    {
        debug("Mb cbWoXY: %s\n", arg.c_str());
    }
} aa;

void cbWXY(uint16_t x, uint16_t y)
{
    debug("Fn cbWxy: %i:%i\n", x, y);
}

void cbWoXY()
{
    debug("FN cbWoXY\n");
}

void cbWarg(string arg)
{
    debug("FN cbWoXY: %s\n", arg.c_str());
}

int main()
{
    //**************
    auto a = new FTGUI::ApplicationWindow(new FTGUI::Dark);

    //    FTGUI::Button b1("<<", a);
    //    FTGUI::Button b2(">>", a);

    //    b1.setGeometry(0, 0, 80, 40);
    //    b2.setGeometry(400, 0, 80, 40);

    //    FTGUI::StackView layout(a);
    //    layout.setGeometry(0, 40, a->width(), a->height() - 40);

    //    FTGUI::Page r1(&layout);
    //    FTGUI::Page r2(&layout);

    //    FTGUI::Label l1("Reaaaally, reaaaaaally, loooooooooong looooooooong teeeeeext.", r1.contentItem());
    //    l1.setWidth(l1.parent()->width());
    //    l1.setFillWidth(true);

    //    FTGUI::Label l2("I Love Atom Audio more!", r2.contentItem());

    //    b1.setCallback(&layout,
    //                   &FTGUI::StackView::pop);

    //    b2.setCallback(&layout,
    //                   &FTGUI::StackView::push);
    FTGUI::List l(a);
    l.setVisibleItemCount(5);
    l.setGeometry(0, 0, a->width(), a->height());
    FTGUI::Rectangle r1(&l), r2(&l), r3(&l);
    for(uint8_t i = 0; i < 100; ++i)
    {
        FTGUI::Rectangle * r = new FTGUI::Rectangle(&l);
        r->setBorderWidth(1);
    }
    //    r1.onPressed(&aa, &A::cbWXY);
    //    r2.onPressed(&aa, &A::cbWoXY);
    //    r3.onPressed(&aa, &A::cbWarg, "Str");

    //    r1.onPressed(&cbWXY);
    //    r2.onPressed(&cbWoXY);
    //    r3.onPressed(&cbWarg, "Str");

    //    r1.onPressed([](uint16_t x, uint16_t y) {
    //        debug("R1, %i^%i\n", x, y);
    //    });
    //    r2.onPressed([](uint16_t z) {
    //        debug("R2, %i\n", z);
    //    },
    //                 10);
    //    r3.onPressed([]() {
    //        debug("R3\n");
    //    });

    r1.setBorderWidth(1);
    r2.setBorderWidth(1);
    r3.setBorderWidth(1);
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
