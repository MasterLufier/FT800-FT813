#ifndef FT8XX_H
#define FT8XX_H

#include <EVE_commands.h>

extern Serial pc;

class FT8xx
{
public:
    FT8xx(PinName mosi, PinName miso, PinName sclk, PinName ssel, PinName pd, PinName interrupt);
    ~FT8xx();
private:
    EVE_HAL * m_hal;
    uint8_t tft_active = 0;
};

#endif // FT8XX_H
