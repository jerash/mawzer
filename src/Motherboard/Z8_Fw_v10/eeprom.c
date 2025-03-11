#include <eZ8.h>
#include "pins_defs.h"
#include <common_z8_ez.h>
#include <ctrldef.h>
#include "commondef.h"
#include "i2c.h"
#include "eeprom.h"
#include "hardware.h"
#include "onewire.h"

///////////////////////////////////////////////////////////////////////////////////////
// Global vars
///////////////////////////////////////////////////////////////////////////////////////
extern volatile uchar Z8_STATUS_4;
extern uint EE_Address;					// current EEProm address
extern uchar I2cCommand;					// current EEProm selection
extern uchar control_index;					// current EEProm selection
extern far uchar CtrlReg[CTRL_REG_REAL_NB];			// table of control reg for eerprom saving and menus 

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


///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	GetCtrlStructEEaddFromIndexGroup()
// Inputs	: 	None
// Outputs	: 	None
// Process  : 	update EE_Address value according to group, control index, also update eeprom select
//				if current control is transport, do not consider groups
//				this function takes into account the expansions EE addresses located in EEprom 0
///////////////////////////////////////////////////////////////////////////////////////
void GetCtrlStructEEaddFromIndexGroup(void)
{
	// First control structure is located in EEPROM_ONE
	I2cCommand   = I2C_SLAVE_ADR_EE + EEPROM_ONE_I2C_ADR;
	// Calculate EEAddress where to read/write the structure
	// First offset the base address according to control number
	EE_Address  = (uint)((uint)(control_index) * (uint)(CTRL_STRUCT_TOTAL_SIZE));
	// Now need to multiply per the offset for each group
	EE_Address += (uint)((uint)current_group * (uint)MIDI_STR_EE_LENGHT_PER_GROUP);	
	// Caclulate on which EEProm we have to write (1 or 2) depending on address
	if(EE_Address >= (uint)EEPROM_ONE_FULL_SIZE)
	{
		EE_Address -= (uint)EEPROM_ONE_FULL_SIZE;
		I2cCommand   = I2C_SLAVE_ADR_EE + EEPROM_TWO_I2C_ADR;
	}
}
