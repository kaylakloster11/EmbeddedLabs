#include "msp.h"
#include <stdint.h>
#include "lab1.h"

void main(void)
{
    unsigned int count = 0;
    unsigned int i = 0;
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;           // Stop watchdog timer

    P1out &= 0x00;      // set P1.0 output low
    P1sel1 &= 0x00;    // General I/O mode
    P1dir |= 0X01;       // P1.0 set as output direction
    P1sel0 &= 0x00; //General I/O mode

    make_square_wave();
    void SquareWave_Full();

    uint16_t var1 = 0xA274;
    var1 |= MaskSet;
    var1 &= MaskCl;
    var1 ^= MaskT;

    report_types();
    report_pointer_types();

    while (1) {                    // continuous loop
            count++;
            P1out ^= 0x01;             // Blink P1.0 LED
            for (i = 0; i < 30000; i++);    // Delay
    }

}
