#include "ft8xx.h"

FT8xx::FT8xx(PinName mosi, PinName miso, PinName sclk, PinName ssel, PinName pd, PinName interrupt)
{
    m_hal = EVE_HAL::instance(mosi, miso, sclk, ssel, pd, interrupt);
    EVE_init();
}

FT8xx::~FT8xx()
{
    delete m_hal;
}
