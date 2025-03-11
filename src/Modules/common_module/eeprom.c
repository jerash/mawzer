#include <eZ8.h>

#ifdef _MODULE_POT
	#include "pins_defs_POT.h"
#endif

#ifdef _MODULE_FAD
	#include "pins_defs_FAD.h"
#endif

#ifdef _MODULE_BUT
	#include "pins_defs_BUT.h"
#endif

#ifdef _MODULE_ENC
	#include "pins_defs_ENC.h"
#endif

#include "commondef.h"
#include "i2c.h"
#include "eeprom.h"
#include "hardware.h"


///////////////////////////////////////////////////////////////////////////////////////
// Global vars
///////////////////////////////////////////////////////////////////////////////////////
extern uchar Z8_STATUS_4;


///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	LockEEprom10Ms()
// Inputs	: 	None
// Outputs	: 	None
// Process  : 	A byte or page has been writen to EEPROM, lock eeprom during write cycle time
//				before doing another operation on this eeprom
///////////////////////////////////////////////////////////////////////////////////////
void LockEEprom10Ms(void)
{
	Z8_STATUS_4 |= eeprom_locked_10ms_z4;
	// Timer 1 (10ms) is used to lock eeprom
	ReinitTimer1();
	// Start timer 1 single shot	
	T1CTL  = 0xB8; 
}



