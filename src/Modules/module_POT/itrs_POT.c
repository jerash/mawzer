#include <eZ8.h>
#include "pins_defs_POT.h"
#include "commondef.h"
#include "onewire.h"
#include "controls_POT.h"
#include "itrs_POT.h"
#include "itrs.h"
#include "hardware.h"

///////////////////////////////////////////////////////////////////////////////////////
// GLOBAL VARS
///////////////////////////////////////////////////////////////////////////////////////
uchar current_analog_index;
far uchar Analog[ANALOG_SOURCES_NB];

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: isr_adc()
// Inputs	: None
// Outputs	: None
// Process  : UART 0 TX ISR
///////////////////////////////////////////////////////////////////////////////////////
#pragma interrupt
void isr_adc(void) 
{
	static uchar adc_tmp;

	// Disable ADC conversion
	ADCCTL &= 0x7F;								
	// A new adc result is available, store it and start next conv
	adc_tmp  = ADCD_H >> 1;
	// enlarge conversion ad to avoid DC offset
	adc_tmp += ADCD_H >> 6 ;
	// limit adc from 0 to 127
	if(adc_tmp > 127) 
		adc_tmp = 127;	

	// Store current result to current index
	Analog[current_analog_index] = adc_tmp;

	// increase current analog index
	current_analog_index++;
	if(current_analog_index >= ANALOG_SOURCES_NB)
	   current_analog_index  = 0; 	

	// IMMEDIATELY start new conversion on next channel
	ADCCTL  = (ADCCTL & 0x70) | current_analog_index ;
	ADCCTL |= 0x80;

	// Finish by acknoledging itr bits
	IRQ0  &= 0xFE;
}


///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: InitModuleItrs()
// Inputs	: None
// Outputs	: None
// Process  : Initilize Z8 module ITRS
//			  See also InitItrs()
///////////////////////////////////////////////////////////////////////////////////////
void InitModuleItrs(void)
{
	// Timers 0,1,2, uart0, adc, i2c and spi interrupt enables
	// Note that timers interrupts are not enabled here
	IRQ0ENH	= 0x59;		// Enable ITRs TX0,RX0,T0,T1,ADC + select nominal priority
	IRQ0ENL	= 0x00;		// Enable ITRs TX0,RX0,T0,T1,T2,ADC + select nominal priority
	
	// PORTS A AND D IRQs 
	IRQ1ENH	= 0x00;		// Enable ITRs on port A and D + select nominal priority (pd0,1,3,4,5,6 itrs are off)
	IRQ1ENL	= 0x00;		// Enable ITRs on port A and D + select nominal priority
	IRQES 	= 0xFF;		// Define falling/rising edges itrs of both port A and D

	// Timers 3, uart1, dma, portC interrupt enables
	// Note that timer 3 and dma interrupts are not enabled here
	IRQ2ENH	= 0x00;		// Enable ITRs Port C (c0,1 itr off), TX1 + select nominal priority
	IRQ2ENL	= 0x00;		// Enable ITRs Port C, TX1 + select nominal priority

	// Create module interrupts vectors
	SET_VECTOR(ADC, isr_adc);

}

