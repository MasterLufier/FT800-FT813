#include <mbed.h>
#include <ft8xx.h>

Serial pc(USBTX, USBRX);

DigitalOut green(LED1);
int main()
{
    pc.baud(115200);
    FT8xx m_screen(PF_9, PF_8, PF_7, PF_12, PB_2, PD_15);
    while (1) {
        ThisThread::sleep_for(1000);
        green = !green;
    }
    return 0;
}
