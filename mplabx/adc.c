
#include <p32xxxx.h>
#include <plib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

#include "config.h"


void initADC10( )
{
//   CloseADC10( );
//
//   PORTSetPinsAnalogIn(IOPORT_B, BIT_0);
//   PORTSetPinsAnalogIn(IOPORT_B, BIT_1);
//   PORTSetPinsAnalogIn(IOPORT_B, BIT_2);
//
//#define CONFIG1 (ADC_MODULE_ON | ADC_FORMAT_INTG32 | ADC_IDLE_STOP | ADC_CLK_MANUAL | ADC_AUTO_SAMPLING_OFF)
//
////TODO match the samples per interrupt to match the ADC channel selection
//#define CONFIG2 (ADC_VREF_AVDD_AVSS | ADC_OFFSET_CAL_DISABLE | ADC_SCAN_OFF | ADC_SAMPLES_PER_INT_1 | ADC_BUF_16 | ADC_ALT_INPUT_OFF)
//#define CONFIG3 (ADC_CONV_CLK_INTERNAL_RC | ADC_SAMPLE_TIME_20)
//
////TODO this would need to be changed to enable certain ADC channels by configuration
//#define CONFIGPORT (ENABLE_AN0_ANA | ENABLE_AN1_ANA | ENABLE_AN2_ANA)
//#define CONFIGSCAN (0)
//
//
//OpenADC10(CONFIG1, CONFIG2, CONFIG3, CONFIGPORT, CONFIGSCAN);
//AD1PCFG = ~0b111;   
   
    PORTSetPinsAnalogIn(IOPORT_B, BIT_0 | BIT_1 | BIT_2);
    
   
}



 double readADC(int channel)
 {
//    switch(channel) {
//        case 0: SetChanADC10(ADC_CH0_POS_SAMPLEA_AN0); break;
//        case 1: SetChanADC10(ADC_CH0_POS_SAMPLEA_AN1); break;
//        case 2: SetChanADC10(ADC_CH0_POS_SAMPLEA_AN2); break;
//        default: return -1;
//    }
//     //AD1CSSL = 0xffff;
//
//    EnableADC10( );
//    AcquireADC10();
//     while(AD1CON1bits.DONE == 0) ;
//
//     uint16_t value = readADC(0);
//     
//     double v = (double) value * (3.3 / 1024.0);
//     
//     IFS1bits.AD1IF = 0;
//
//     return v;
     
    AD1PCFG = ~0b111; // AN0-2
    AD1CON1 = 0x00e0;
    AD1CHS = (channel & 0xf) << 16; // enable an0
    AD1CSSL = 0; // no scan
    
    AD1CON3bits.ADRC = 0;
    AD1CON3bits.SAMC = 16;
    AD1CON3bits.ADCS = 4;
    
//    AD1CON3 = 0x1f02; // sample time = 31 Tad
    AD1CON2 = 0;
    AD1CON1SET = 0x8000; // turn on ADC

    AD1CON1bits.SAMP = 1;
    while (AD1CON1bits.DONE == 0) ;
     
    AD1CON1bits.ADON = 0;
    
    AD1CON1 = 0;    // turn off ADC
    
     int value = ADC1BUF0; // value = step
     double v = (double) value *  (3.3/1024.0);
     return v;
 }


