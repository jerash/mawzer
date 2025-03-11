#include <eZ8.h>
#include "pins_defs_ENC.h"
#include "commondef.h"
#include "onewire.h"
#include "controls_ENC.h"
#include "itrs_ENC.h"
#include "itrs.h"
#include "hardware.h"

///////////////////////////////////////////////////////////////////////////////////////
// GLOBAL VARS
///////////////////////////////////////////////////////////////////////////////////////

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
	IRQ0ENH	= 0x58;		// Enable ITRs TX0,RX0,T0,T1,ADC + select nominal priority
	IRQ0ENL	= 0x00;		// Enable ITRs TX0,RX0,T0,T1,T2,ADC + select nominal priority
	
	// PORTS A AND D IRQs 
	IRQ1ENH	= 0x00;		// Enable ITRs on port A and D + select nominal priority (pd0,1,3,4,5,6 itrs are off)
	IRQ1ENL	= 0x00;		// Enable ITRs on port A and D + select nominal priority
	IRQES 	= 0xFF;		// Define falling/rising edges itrs of both port A and D

	// Timers 3, uart1, dma, portC interrupt enables
	// Note that timer 3 and dma interrupts are not enabled here
	IRQ2ENH	= 0x00;		// Enable ITRs Port C (c0,1 itr off), TX1 + select nominal priority
	IRQ2ENL	= 0x00;		// Enable ITRs Port C, TX1 + select nominal priority
}

