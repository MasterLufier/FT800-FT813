EVEGUI

This project originally based on the https://github.com/RudolphRiedel/FT800-FT813  library for EVE/EVE2/EVE3 graphics controller ICs from FTDI/Bridgetek, but fully rewritten for MBED.

The library contains two parts: 

1) Low level API FT8xx:

    #include <ft8xx.h>

        //**Initialize screen
        FT8xx screen;
        //**Initialize storage in RamG if needed
        screen.ramGInit();
        //** calibrate touchscreen with predefined values if needed
        screen.touchCalibrate(true);