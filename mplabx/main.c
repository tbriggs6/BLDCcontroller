/* 
 * File:   main.main
 * Author: tbriggs
 *
 * Created on June 2, 2015, 2:47 PM
 */

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

// DEVCFG3
// USERID = No Setting

// DEVCFG2
#pragma config FPLLIDIV = DIV_2         // PLL Input Divider (2x Divider)
#pragma config FPLLMUL = MUL_20         // PLL Multiplier (20x Multiplier)
#pragma config UPLLIDIV = DIV_2         // USB PLL Input Divider (2x Divider)
#pragma config UPLLEN = ON              // USB PLL Enable (Enabled)
#pragma config FPLLODIV = DIV_1         // System PLL Output Clock Divider (PLL Divide by 2)

// DEVCFG1
#pragma config FNOSC = FRCPLL           // Oscillator Selection Bits (Fast RC Osc with PLL)
#pragma config FSOSCEN = OFF            // Secondary Oscillator Enable (Disabled)
#pragma config IESO = OFF               // Internal/External Switch Over (Disabled)
#pragma config POSCMOD = OFF            // Primary Oscillator Configuration (Primary osc disabled)
#pragma config OSCIOFNC = OFF           // CLKO Output Signal Active on the OSCO Pin (Disabled)
#pragma config FPBDIV = DIV_8           // Peripheral Clock Divisor (Pb_Clk is Sys_Clk/8)
#pragma config FCKSM = CSDCMD           // Clock Switching and Monitor Selection (Clock Switch Disable, FSCM Disabled)
#pragma config WDTPS = PS1048576        // Watchdog Timer Postscaler (1:1048576)
#pragma config FWDTEN = OFF             // Watchdog Timer Enable (WDT Disabled (SWDTEN Bit Controls))

// DEVCFG0
#pragma config DEBUG = OFF              // Background Debugger Enable (Debugger is disabled)
#pragma config ICESEL = ICS_PGx2        // ICE/ICD Comm Channel Select (ICE EMUC1/EMUD1 pins shared with PGC1/PGD1)
#pragma config PWP = OFF                // Program Flash Write Protect (Disable)
#pragma config BWP = OFF                // Boot Flash Write Protect bit (Protection Disabled)
#pragma config CP = OFF                 // Code Protect (Protection Disabled)

// setup some timer constants
#define DELAY_TICKS(x)  \
{ WriteCoreTimer(0);  while (ReadCoreTimer() < x) ; }

void init_hardware()
{
	// configure for max performance
	uint32_t pb_clk = SYSTEMConfig(GetSystemClock(), SYS_CFG_ALL);

	//    // enable UART
	uart_init();
	uart_echo(1);
	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stdin, NULL, _IONBF, 0);

	OpenTimer2(T2_ON | T2_SOURCE_INT | T2_PS_1_2, 10000);
	ConfigIntTimer2(T2_INT_ON | T2_INT_PRIOR_4);


	// digital motor drives
	LATE = 0;
	PORTSetPinsDigitalOut(IOPORT_E, BIT_0 | BIT_1 | BIT_2 | BIT_4 | BIT_5 | BIT_6 | BIT_7);
	ODCE = 0;

	// analog sense pins
	PORTSetPinsDigitalIn(IOPORT_B, BIT_0 | BIT_1 | BIT_2);

	
	// diagnostic / debug ports
	PORTClearBits(IOPORT_B, BIT_4 | BIT_5 | BIT_8 | BIT_9);
	PORTSetPinsDigitalOut(IOPORT_B, BIT_4 | BIT_5 | BIT_8 | BIT_9);

	initADC10();
	INTEnableSystemMultiVectoredInt();

}

int strtohex(const char *ptr)
{
	int sum = 0;
	while ((*ptr != '\r') && (*ptr != '\n') && (*ptr != 0)) {
		sum = sum << 4;

		if ((*ptr >= '0') && (*ptr <= '9')) sum += *ptr - '0';
		else if ((*ptr >= 'A') && (*ptr <= 'F')) sum += (*ptr - 'A' + 10);
		else if ((*ptr >= 'a') && (*ptr <= 'f')) sum += (*ptr - 'a' + 10);
		else {
			sum = sum >> 4;
			break;
		}
		ptr++;
	}
	return sum;
}


#define AHIGH (1 << 0) 
#define BHIGH (1 << 1) 
#define CHIGH (1 << 2) 
#define ALOW (1 << 4)
#define BLOW (1 << 5)
#define CLOW (1 << 6)
#define MASK (AHIGH | BHIGH | CHIGH | ALOW | BLOW | CLOW)

#define ENABLE (1 << 7)

#define ADCA 0
#define ADCB 1
#define ADCC 2

void set_phase(int phase)
{
	switch(phase) {
	case 0: LATE = (LATE & ~MASK) | BHIGH | ALOW; break;
	case 1: LATE = (LATE & ~MASK) | CHIGH | ALOW; break;
	case 2: LATE = (LATE & ~MASK) | CHIGH | BLOW; break;
	case 3: LATE = (LATE & ~MASK) | AHIGH | BLOW; break;
	case 4: LATE = (LATE & ~MASK) | AHIGH | CLOW; break;
	case 5: LATE = (LATE & ~MASK) | BHIGH | CLOW; break;
	
	case 20: LATE = (LATE & ~MASK) | AHIGH; break;
	case 21: LATE = (LATE & ~MASK) | AHIGH | BLOW; break;
	case 22: LATE = (LATE & ~MASK) | AHIGH | CLOW; break;
	
	case 'R': LATE = LATE & ~MASK; break;
	default: LATE = LATE & ~MASK; break;
	}
	
	if (phase >= 0) LATE = LATE | ENABLE;
	else LATE = LATE & ~ENABLE;
	
//	switch (phase) {
//	case 0: PORTE = BHIGH | ALOW;
//		break;
//	case 1: PORTE = CHIGH | ALOW;
//		break;
//	case 2: PORTE = CHIGH | BLOW;
//		break;
//	case 3: PORTE = AHIGH | BLOW;
//		break;
//	case 4: PORTE = AHIGH | CLOW;
//		break;
//	case 5: PORTE = BHIGH | CLOW;
//		break;
//
//	case 10:
//	case 11: PORTE = ALOW;
//		break;
//
//	case 12:
//	case 13: PORTE = BLOW;
//		break;
//
//	case 14:
//	case 15: PORTE = CLOW;
//		break;
//
//	case 20:
//		PORTE = AHIGH ;
//		break;
//		
//	case 21:
//		PORTE = AHIGH | BLOW;
//		break;
//		
//	case 22:
//		PORTE = AHIGH | CLOW;
//		break;
//		
//
//
//	case -1: PORTE = 0;
//		break;
//	case -2: PORTE = 0;
//		break;
//
//	default:
//		PORTE = 0;
//		return;
//	}
}

int round2(float v)
{
	return(int) (v + 0.5);
}

void state_machine();

const float t2_tick_per_us = 80e6 / (2 * 1e6);
int t2_period(int time_us)
{
	return round2(time_us * t2_tick_per_us);
}

void t2_set_timeout(int time_us)
{
	WritePeriod2(t2_period(time_us));
	WriteTimer2(0);
}

void __ISR(_TIMER_2_VECTOR, ipl4soft) Timer2Handler(void)
{
	state_machine();
	mT2ClearIntFlag();
}

/**********************************************************/
/**************** STATE MACHINE DEFINITIONS ***************/

/**********************************************************/
typedef enum {
	IDLE,
	PWM_ON, PWM_OFF,
	DONE
} bldc_state_t;

bldc_state_t state = IDLE, next_state = IDLE;


const int startup_init_wait = 8000;		// wait 8ms between periods
const int startup_min_wait = 4000;		// minimum wait between periods
const float startup_threshold = 0.95;	// BEMF threshold 
const int startup_max_tries = 33;		// max pwm cycles
const int startup_max_cycles = 32;		// max startup cycles

volatile int run = 0;
volatile int count = 0;

void state_machine()
{
	if (run == 0) state = IDLE;
	
	switch(state) {
	case IDLE:
		if (run == 0) {
			t2_set_timeout(1000);
			break;
		}
		
	case PWM_ON:
		state = PWM_OFF;
		set_phase(20);
		t2_set_timeout(120);
		break;
		
	case PWM_OFF:
		count = (count + 1) % 64;
		if (count <32 ) 
			set_phase(21);
		else
			set_phase(22);
		t2_set_timeout(120);
		state = PWM_ON;
		break;
		
	default:
		run = 0;
		state = IDLE;
		t2_set_timeout(1000);
		break;
	}
}

int main(int argc, char** argv)
{
	int ch, err;
	
	init_hardware();

	printf("************ BLDC Controller v3 ******************\r\n");
	printf("R = START, T = STOP BLDC Drive\r\n");
	printf("0-5 = Set BLDC Phase, - = Off\r\n");
	fflush(stdout);
	
//	set_phase(-2);
//	run = 1;

	
	while (1) {
		ch = getchar();
		if (((ch >= '0') && (ch <= '5')) || (ch == '-')){
			// wait for timer to settle
			run = 0;
			while (state != IDLE) ;
			
			printf("Setting phase %c\r\n", ch);

			switch (ch) {
			case '0': set_phase(0); break;
			case '1': set_phase(1); break;
			case '2': set_phase(2); break;
			case '3': set_phase(3); break;
			case '4': set_phase(4); break;
			case '5': set_phase(5); break;
			case '-': set_phase('R'); break;
			default: run = 0;
				set_phase(-1);
				break;
			}
			
			printf("LAT: %x ", LATE & MASK);
			if (LATE & AHIGH) printf("AH ");
			if (LATE & BHIGH) printf("BH ");
			if (LATE & CHIGH) printf("CH ");
			
			if (LATE & ALOW) printf("AL ");
			if (LATE & BLOW) printf("BL ");
			if (LATE & CLOW) printf("CL ");
			printf("\r\n");
		}
		else if (ch == 'R') { 
			printf("Running\r\n");
			run = 1;
		}
		else if (ch == 'T') {
			printf("Stopping\r\n");
			run = 0;
		}
	}


	CloseTimer2();

	printf("ABORTED \r\n");
	return(EXIT_SUCCESS);
}


