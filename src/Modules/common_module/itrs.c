#include <eZ8.h>

#ifdef _MODULE_POT
	#include "pins_defs_POT.h"
	#include "commondef.h"
	#include "onewire.h"
	#include "controls_POT.h"
	#include "itrs_POT.h"
#endif

#ifdef _MODULE_FAD
	#include "pins_defs_FAD.h"
	#include "commondef.h"
	#include "onewire.h"
	#include "controls_FAD.h"
	#include "itrs_FAD.h"
#endif

#ifdef _MODULE_BUT
	#include "pins_defs_BUT.h"
	#include "commondef.h"
	#include "onewire.h"
	#include "controls_BUT.h"
	#include "itrs_BUT.h"
#endif

#ifdef _MODULE_ENC
	#include "pins_defs_ENC.h"
	#include "commondef.h"
	#include "onewire.h"
	#include "controls_ENC.h"
	#include "itrs_ENC.h"
#endif

#include "itrs.h"
#include "hardware.h"

///////////////////////////////////////////////////////////////////////////////////////
// GLOBAL VARS
///////////////////////////////////////////////////////////////////////////////////////
extern uchar onewire_rx_start_index;						// a near pointer on FAR ram
extern uchar onewire_rx_end_index;							// a near pointer on FAR ram
extern uchar onewire_rx_current_index;
extern far uchar onewire_rx_buffer[ONEWIRE_RX_BUFF_LENGHT];		// one wire serial in buffer

extern uchar Z8_STATUS_0;
extern uchar Z8_STATUS_4;
extern uchar Z8_STATUS_8;
extern far uchar module_sub_id;

extern uchar TX_STATUS;
extern uchar tx0_start_index;
extern uchar tx0_end_index;
extern far uchar TX0_Buff[TX_BUFF_LENGTH];

/*
///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: isr_timer0()
// Inputs	: None
// Outputs	: None
// Process  : Timer 0 ISR - Should be there each 10ms
///////////////////////////////////////////////////////////////////////////////////////
#pragma interrupt
void isr_timer0(void) 
{

	// Finish by acknoledging itr bits
	IRQ0  &= 0xDF;
}
*/

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: isr_timer1()
// Inputs	: None
// Outputs	: None
// Process  : Timer 1 ISR
///////////////////////////////////////////////////////////////////////////////////////
#pragma interrupt
void isr_timer1(void) 
{
	// Unlock eeprom
	Z8_STATUS_4 &= ~eeprom_locked_10ms_z4;

	// Finish by acknoledging itr bits
	IRQ0  &= 0xBF;
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: isr_rx0()
// Inputs	: None
// Outputs	: None
// Process  : UART 0 RX ISR (FROM ONE WIRE)
//			  Use a flag based "state machine" to decode one wire protocol.
//			  If module matches ID and SUB ID, we store the rest of the 
//			  command into onewire_rx_buffer[]
///////////////////////////////////////////////////////////////////////////////////////
#pragma interrupt
void isr_rx0(void) 
{

	// Check if readback on onewire RX is enabled 
	if( !(Z8_STATUS_0 & disable_ow_rx_z0) ) 
	{
		// Are we waiting for SOF ?
		if (Z8_STATUS_0 & onewire_waiting_for_sof_z0)
		{
			// check if we get a SOF
			if(U0RXD == SOF_ONEWIRE)
			{
				// no more waiting for SOF
				Z8_STATUS_0 &= ~onewire_waiting_for_sof_z0;
				// we are waiting for id
				Z8_STATUS_0 |= onewire_waiting_for_id_z0;
			}
		}
		// Are we waiting for ID ?
		else if (Z8_STATUS_0 & onewire_waiting_for_id_z0)
		{
			// check if we get module ID
			if((U0RXD == MODULE_ID) || (U0RXD == ONEWIRE_ID_ALL_MODULES))
			{
				// no more waiting for ID
				Z8_STATUS_0 &= ~onewire_waiting_for_id_z0;
				// we are waiting for SUB ID
				Z8_STATUS_0 |= onewire_waiting_for_sub_id_z0;
			}
			else 
				// we are not concerned by this ID : rewait for SOF
				Z8_STATUS_0 |= onewire_waiting_for_sof_z0;
		}
		// Are we waiting for SUB ID ?
		else if (Z8_STATUS_0 & onewire_waiting_for_sub_id_z0)
		{
			// check if we get module SUB ID
			if((U0RXD == module_sub_id) || (U0RXD == ONEWIRE_ID_ALL_MODULES))
			{
				// no more waiting for SUB ID
				Z8_STATUS_0 &= ~onewire_waiting_for_sub_id_z0;
				// we are waiting for data to store 
				Z8_STATUS_0 |= onewire_waiting_for_data_z0;
			}
			else 
				// we are not concerned by this SUB ID : rewait for SOF
				Z8_STATUS_0 |= onewire_waiting_for_sof_z0;
		}
		// Are we waiting for DATA to store ?
		else if (Z8_STATUS_0 & onewire_waiting_for_data_z0)
		{
		
			// Store data into buffer
			onewire_rx_buffer[onewire_rx_current_index] = U0RXD;
			onewire_rx_current_index = (onewire_rx_current_index+1)&(ONEWIRE_RX_BUFF_LENGHT-1);
				
			// check that no overflow occured
			if(onewire_rx_current_index == onewire_rx_start_index)
				if(onewire_rx_current_index != ((onewire_rx_end_index-1)&(ONEWIRE_RX_BUFF_LENGHT-1)))
					Z8_STATUS_8 |= onewire_rx_overflow_z8;
					
			if(U0RXD == EOF_ONEWIRE)
			// note : EOF has been stored into buffer 
			{
				// Update pointer end index
				onewire_rx_end_index = (onewire_rx_current_index+1)&(ONEWIRE_RX_BUFF_LENGHT-1);
				// Got EOF, so we are no more waiting for DATA
				Z8_STATUS_0 &= ~onewire_waiting_for_data_z0;
				// buffer is filled out, flag we got a request from MB
				Z8_STATUS_0 |= onewire_mb_request_z0;
				// Wait for SOF
				Z8_STATUS_0 |= onewire_waiting_for_sof_z0;
			}
		}		
	} // endif readback enabled
	else
    // This means we got something on RX but readback was disabled
	{  
		// Re-enable RX readback when TX has send all its data (ending with EOF)
		if(U0RXD == EOF_ONEWIRE)
			Z8_STATUS_0 &= ~disable_ow_rx_z0;
	}


	// Finish by acknoledging itr bits
	IRQ0  &= 0xEF;
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: isr_tx0()
// Inputs	: None
// Outputs	: None
// Process  : UART 0 TX ISR
///////////////////////////////////////////////////////////////////////////////////////
#pragma interrupt
void isr_tx0(void) 
{
	// tx0 ready ?
	if(U0STAT0 & TDRE)
	{
		if ((tx0_start_index != tx0_end_index))// || (TX_STATUS & tx0_busy_txstat))
		{
			U0TXD = TX0_Buff[tx0_start_index++ & (TX_BUFF_LENGTH-1)];
			TX_STATUS &= ~tx0_full_txstat;
		}
		else
			TX_STATUS &= ~tx0_busy_txstat;
	}

	// Finish by acknoledging itr bits
	IRQ0  &= 0xF7;
}


///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: InitItrs()
// Inputs	: None
// Outputs	: None
// Process  : Initilize Z8 ITRS
//			  See also InitModuleItrs()
///////////////////////////////////////////////////////////////////////////////////////
void InitItrs(void)
{
	// Initialize module ITRS
	InitModuleItrs();

	// Clear interrupts pending from boot
	IRQ0	= 0x00;
	IRQ1	= 0x00;
	IRQ2	= 0x00;

	// Create interrupts vectors
//	SET_VECTOR(TIMER0, isr_timer0);
	SET_VECTOR(TIMER1, isr_timer1);
	SET_VECTOR(UART0_RX, isr_rx0);
	SET_VECTOR(UART0_TX, isr_tx0);
}
