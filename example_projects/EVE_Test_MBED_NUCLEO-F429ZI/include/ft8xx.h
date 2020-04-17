#ifndef FT8XX_H
#define FT8XX_H

#include <EVE_commands.h>

class FT8xx
{
public:
    FT8xx(PinName mosi, PinName miso, PinName sclk, PinName ssel, PinName pd, PinName interrupt) :
          m_spi(mosi, miso, sclk),
          m_ssel(ssel),
          m_pd(pd),
          m_interrupt(interrupt)
    {
        EVE_cmdWrite(1,0);
    }
    void _cs_set()
    {
        m_ssel = 1;
    }
private:
    SPI m_spi;
    DigitalOut m_ssel;
    DigitalOut m_pd;
    DigitalIn m_interrupt;
};

#endif // FT8XX_H
