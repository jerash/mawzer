#include <eZ8.h>
#include "main.h"
#include "commondef.h"
#include "onewire.h"

#ifdef _MODULE_POT
	#include "controls_POT.h"
#endif

#ifdef _MODULE_FAD
	#include "controls_FAD.h"
#endif

#ifdef _MODULE_BUT
	#include "controls_BUT.h"
#endif

#ifdef _MODULE_ENC
	#include "controls_ENC.h"
#endif

#include "itrs.h"
#include "hardware.h"
#include "i2c_low_level.h"

// set the option bits

///////////////////////////////////////////////////////////////////////////////////////
// GLOBAL VARS
///////////////////////////////////////////////////////////////////////////////////////

extern uchar Z8_STATUS_4;

///////////////////////////////////////////////////////////////////////////////////////////
// Main program beings here 
///////////////////////////////////////////////////////////////////////////////////////////
void main(void)
{	
	// Disable Interrupts
	DI();		
	// Initialize IOs directions, default values and alternate functions
	InitIos();
	// Initialize I2C Controller periferal
	InitI2C(); 
	// Initialize Serial periferals I2C and UARTS 0 
	InitSerial(); 
	// Initialize hardware values
	InitInitialValues();
	// Initialize module hardware and control values
	InitModule(); 
	// Initialize Interrupt controller + vectors
	InitItrs();
	// Get module sub id from dip switch
	GetModuleSubID();
	// Enable Interrupts	
	EI();	

	// Main loop start from there
	while(1) {	
				// Check for onewire  
				CheckOneWire();

				// Check controls
				CheckControls();
			
				// no more first pass flag
				Z8_STATUS_4 &= ~first_pass_z4;
	}

} // END PROGRAM



