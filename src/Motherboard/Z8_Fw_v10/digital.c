#include <eZ8.h>
#include "pins_defs.h"
#include "commondef.h"
#include <common_z8_ez.h>
#include <ctrldef.h>
#include "digital.h"
#include "realtime.h"
#include "eeprom.h"
#include "onewire.h" // DBG
 
 
///////////////////////////////////////////////////////////////////////////////////////
// GLOBAL VARS
///////////////////////////////////////////////////////////////////////////////////////

extern uchar Z8_STATUS_0;
extern uchar Z8_STATUS_1;
extern volatile uchar Z8_STATUS_4;
extern volatile uchar Z8_STATUS_6;

extern far uchar CtrlReg[CTRL_REG_REAL_NB];			// table of control reg for eerprom saving and menus 

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: CheckTransportButtons()
// Inputs	: None
// Outputs	: None
// Process  : Check BW, FW, START, STOP, REC buttons
///////////////////////////////////////////////////////////////////////////////////////
void CheckTransportButtons(void)
{
	////////////////////////////
	// CHECK FOR BACKWARD BUTTON
	////////////////////////////
	if(BW_PORT_IN & BW_PIN)			
	{
		// does the button passed through zero ?
		if(Z8_STATUS_1 & bw_passed_0_z1)
		{
			Z8_STATUS_1 &= ~bw_passed_0_z1;
			UpdateInternalRealtime(BACKWARD_TRANSPORT);
		}
	}
	else
		Z8_STATUS_1 |= bw_passed_0_z1;

	////////////////////////////
	// CHECK FOR FORWARD BUTTON
	////////////////////////////
	if(FW_PORT_IN & FW_PIN)			
	{
		// does the button passed through zero ?
		if(Z8_STATUS_1 & fw_passed_0_z1)
		{
			Z8_STATUS_1 &= ~fw_passed_0_z1;
			UpdateInternalRealtime(FORWARD_TRANSPORT);
		}
	}
	else
		Z8_STATUS_1 |= fw_passed_0_z1;

	////////////////////////////
	// CHECK FOR PLAY BUTTON
	////////////////////////////
	if(PLAY_PORT_IN & PLAY_PIN)			
	{
		// does the button passed through zero ?
		if(Z8_STATUS_1 & play_passed_0_z1)
		{
			Z8_STATUS_1 &= ~play_passed_0_z1;
			UpdateInternalRealtime(PLAY_TRANSPORT);
		}
	}
	else
		Z8_STATUS_1 |= play_passed_0_z1;

	////////////////////////////
	// CHECK FOR REC BUTTON
	////////////////////////////
	if(REC_PORT_IN & REC_PIN)			
	{
		// does the button passed through zero ?
		if(Z8_STATUS_1 & rec_passed_0_z1)
		{
			Z8_STATUS_1 &= ~rec_passed_0_z1;
			UpdateInternalRealtime(REC_TRANSPORT);
		}
	}
	else
		Z8_STATUS_1 |= rec_passed_0_z1;

	////////////////////////////
	// CHECK FOR STOP BUTTON
	////////////////////////////
	if(STOP_PORT_IN & STOP_PIN)			
	{
		// does the button passed through zero ?
		if(Z8_STATUS_1 & stop_passed_0_z1)
		{
			Z8_STATUS_1 &= ~stop_passed_0_z1;
			UpdateInternalRealtime(STOP_TRANSPORT);
		}
	}
	else
		Z8_STATUS_1 |= stop_passed_0_z1;

}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: CheckButtons()
// Inputs	: None
// Outputs	: None
// Process  : check OK, CANCEL, SHIFT, ENCODER1 BUTTON, UP, DOWN
///////////////////////////////////////////////////////////////////////////////////////
void CheckButtons(void)
{
	/////////////////////////
	// CHECK FOR SHIFT BUTTON
	/////////////////////////
	if(SHIFT_PORT_IN & SHIFT_PIN)			
	{
		// does the button passed through zero ?
		if(Z8_STATUS_0 & shift_passed_0_z0)
		{
			Z8_STATUS_0 &= ~shift_passed_0_z0;
			Z8_STATUS_6 |= shift_state_z6;
			// DBG
			//SearchModules();
			// asm("JP START");
			//goto main;
		}
	}
	else
	{
		Z8_STATUS_0  |= shift_passed_0_z0;
		Z8_STATUS_6  &= ~shift_state_z6;
	}

	//////////////////////
	// CHECK FOR UP BUTTON
	//////////////////////
	if(UP_SW_PORT_IN & UP_SW_PIN)			
	{
		// does the button passed through zero ?
		if(Z8_STATUS_0 & up_passed_0_z0)
		{
			Z8_STATUS_0 &= ~up_passed_0_z0;
			// By default this button is used to change group
			Z8_STATUS_4  |= must_change_group_z4;
			Z8_STATUS_4  |= must_increase_group_z4;
		}
	}
	else
		Z8_STATUS_0 |= up_passed_0_z0;

	////////////////////////
	// CHECK FOR DOWN BUTTON
	////////////////////////
	if(DOWN_SW_PORT_IN & DOWN_SW_PIN)			
	{
		// does the button passed through zero ?
		if(Z8_STATUS_0 & down_passed_0_z0)
		{
			Z8_STATUS_0 &= ~down_passed_0_z0;
			// By default this button is used to change group
			Z8_STATUS_4  |= must_change_group_z4;
			Z8_STATUS_4  &= ~must_increase_group_z4;

		}
	}
	else
		Z8_STATUS_0 |= down_passed_0_z0;

}
