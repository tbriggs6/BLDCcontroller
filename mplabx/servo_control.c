#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <p32xxxx.h>
#include <math.h>

#include "config.h"
#include "adc.h"



#include <xc.h>
#include <peripheral/system.h>



#include <xc.h>
#include <peripheral/ports.h>
#include <peripheral/timer.h>
#include <peripheral/adc10.h>

#ifdef USE_SERVO_CONTROL

const float t2_tick_per_us = 80e6 / (256 * 1e6);

int t2_period(float time_us)
{
    return round2(time_us * t2_tick_per_us);
}


float on_us = 0.0;
int pwm_state = 0;

void __ISR(_TIMER_2_VECTOR, ipl4soft) Timer2Handler(void) 
{
    
    
    if (pwm_state == 0) 
    {
        PORTGbits.RG12 = 1;
        if (on_us < 2000)
            pwm_state = 1;
        else 
            pwm_state = 0;
        
        if (on_us < 1) 
            WritePeriod2(t2_period(2000));
        else if (on_us >= 2000)
            WritePeriod2(t2_period(2000));
        else 
            WritePeriod2(t2_period(on_us));
        
        WriteTimer2(0);
    }
    else 
    {
        pwm_state = 0;
        PORTGbits.RG12 = 0;
        if (on_us >= 2000.0)
            WritePeriod2(t2_period(2000.0));
        else
            WritePeriod2(t2_period(2000.0 - on_us));
        WriteTimer2(0);
    }
    
    mT2ClearIntFlag();
}


void set_ontime(float ontime)
{
    if (ontime < 0) { printf("ERR (%f)- too low \r\n", ontime); return; }
    if (ontime > 2000) { printf("ERR (%f) - too high \r\n", ontime); return; }
    on_us = ontime;
}

int servo_main() {

    PORTSetPinsDigitalOut(IOPORT_G, BIT_12);
    CloseTimer2();
     OpenTimer2(T2_ON | T2_SOURCE_INT | T2_PS_1_256, 10000);
    ConfigIntTimer2(T2_INT_ON | T2_INT_PRIOR_4);
    char ch;
    while(1) 
    {
    
        printf("************ BLDC Controller v2 ******************\r\n");
        printf("current on-time: %f (%f%%)\r\n", on_us,(on_us / 20.0));
        printf("+ - add 50 to pwm duty\r\n");
        printf("- - sub 50 from pwm duty\r\n");
        printf("0 - 9 - set PWM to 0 - 90%\r\n");
        printf("F - set PWM to full\r\n");

        ch = getchar();
        if (ch == '+') set_ontime(on_us + 100);
        else if (ch == '-') set_ontime(on_us - 100);
        else if ((ch >= '0') && (ch <= '9')) {
            int val = ch - '0';
            set_ontime(200 * val);
        }
        else if ((ch == 'F')||(ch == 'f')) {
            set_ontime(2000.0);
        }
    }
}
#endif