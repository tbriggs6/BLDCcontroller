/* 
 * File:   config.h
 * Author: tbriggs
 *
 * Created on January 31, 2013, 10:16 AM
 */

#ifndef CONFIG_H
#define	CONFIG_H

#ifdef	__cplusplus
extern "C" {
#endif

#define	GetSystemClock()      (80000000ul)
#define	GetPeripheralClock()  (10000000ul) // (GetSystemClock()/(1 << OSCCONbits.PBDIV))
#define	GetInstructionClock() (GetSystemClock())

#define I2C_CLOCK_FREQ (400000)



#ifdef	__cplusplus
}
#endif

#endif	/* CONFIG_H */

