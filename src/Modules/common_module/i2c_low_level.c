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
#include "i2c_low_level.h"
#include "i2c.h"
#include "hardware.h"
#include "onewire.h"

///////////////////////////////////////////////////////////////////////////////////////
// GLOBAL VARS
///////////////////////////////////////////////////////////////////////////////////////
extern uchar Z8_STATUS_8;

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: InitI2C()
// Inputs	: None
// Outputs	: None
// Process  : Initilize Z8 I2C controller
///////////////////////////////////////////////////////////////////////////////////////
void InitI2C(void)
{
	I2CBRH = (uchar) (BRG_I2C >> 8);		// Load the baud rate for High byte.
	I2CBRL = (uchar) (BRG_I2C & 0xFF);  	// Load the data for the LOw byte.
	I2CCTL = I2C_ENABLE + I2C_FILTER;		// En Rx/Tx; Dis Tx Int; Enable I2C filter V1.6
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: I2cStop()
// Inputs	: None
// Outputs	: None
// Process  : Initilize Z8 I2C controller
///////////////////////////////////////////////////////////////////////////////////////
void I2cStop(void)	
{		
	I2CCTL |= SEND_STOP;
#ifndef SIMUL
	while ((I2CCTL & SEND_STOP) == SEND_STOP);
#endif
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: WaitI2cTransmitEmpty()
// Inputs	: None
// Outputs	: None
// Process  : 
///////////////////////////////////////////////////////////////////////////////////////
void WaitI2cTransmitEmpty(void)
{
	uchar i = 0;
	// reset timeout flag
	Z8_STATUS_8 &= ~i2c_timout_z8;
#ifndef SIMUL
	while ((I2CSTAT & TRANSMIT_DATA_REG_EMPTY) == 0x00)
	{
		i++;
		if(i >= I2C_TIMEOUT_CPT) 
		{
			// return I2C_TIMEOUT;
			Z8_STATUS_8 |= i2c_timout_z8;
			return;
		}
	}
#endif
	// return I2C_NO_TIMEOUT;
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: WaitI2cAcknoledge()
// Inputs	: None
// Outputs	: None
// Process  : 
///////////////////////////////////////////////////////////////////////////////////////
// v1.5 void
void WaitI2cAcknoledge (void)
{
	uchar i = 0;
	// reset timeout flag
	Z8_STATUS_8 &= ~i2c_timout_z8;
#ifndef SIMUL
	while ((I2CSTAT & ACKNOWLEDGE) == 0x00)
	{
		i++;
		if(i >= I2C_TIMEOUT_CPT) 
		{
			//return I2C_TIMEOUT;
			Z8_STATUS_8 |= i2c_timout_z8;
			return;
		}
	}
#endif
	// return I2C_NO_TIMEOUT;
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: WaitI2cFullDataReceived()
// Inputs	: None
// Outputs	: None
// Process  : 
///////////////////////////////////////////////////////////////////////////////////////
void WaitI2cFullDataReceived(void)
{
	uchar i = 0;
	// reset timeout flag
	Z8_STATUS_8 &= ~i2c_timout_z8;
#ifndef SIMUL
	while ((I2CSTAT & RECEIVE_DATA_REG_FULL) == 0x00)
	{
		i++;
		if(i >= I2C_TIMEOUT_CPT) 
		{
			// return I2C_TIMEOUT;
			Z8_STATUS_8 |= i2c_timout_z8;
			return;
		}
	}
#endif
	// return I2C_NO_TIMEOUT;
}


