/* 
 * File:   adc.h
 * Author: tbriggs
 *
 * Created on March 21, 2013, 2:05 PM
 */

#ifndef ADC_H
#define	ADC_H

#ifdef	__cplusplus
extern "C" {
#endif

    void initADC10( );
    double readADC(int channel);

#ifdef	__cplusplus
}
#endif

#endif	/* ADC_H */

