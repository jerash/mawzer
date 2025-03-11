#include <eZ8.h>
#include "pins_defs_ENC.h"
#include "commondef.h"
#include "onewire.h"
#include "itrs_ENC.h"
#include "controls_ENC.h"
#include "eeprom.h"
#include "i2c.h"

///////////////////////////////////////////////////////////////////////////////////////
// GLOBAL VARS
///////////////////////////////////////////////////////////////////////////////////////
rom uchar module_type[MODULE_TYPE_LENGHT] = "ENC410";
uchar module_name[MODULE_NAME_LENGHT];

extern uchar Z8_STATUS_4;
extern uchar Z8_STATUS_0;

extern far uchar config_reg[CONFIG_REG_TABLE_MAX] ;  
extern far uchar assignment[CONTROL_NB][GROUP_NB];

extern uint  EE_Address;

uchar Z8_ENC_STATUS;
uchar Ctrl[CONTROL_NB][GROUP_NB];  // Store all encoders data for all groups

//////////////////////////////////////////////////////////////////////////////////////
// Routine 	: InitModule()
// Inputs	: None
// Outputs	: None
// Process  : Initialize module hardware and control default values 
///////////////////////////////////////////////////////////////////////////////////////
void InitModule(void)
{
	uchar group, ctrl;

	// Initialize all encoders input A and input B states
	Z8_ENC_STATUS = 0x00;

	// Initialize all controls to default value
	for (group = 0; group < GROUP_NB; group++)
		for (ctrl = 0; ctrl < CONTROL_NB; ctrl++)
			Ctrl[ctrl][group] = ENC_DEFAULT_VALUE;
	
}

//////////////////////////////////////////////////////////////////////////////////////
// Routine 	: ModuleChecksWrittenRegisters(uchar mem_type, uint address)
// Inputs	: mem_type, address
// Outputs	: None
// Process  : The module checks what MB has written in its registers
//
///////////////////////////////////////////////////////////////////////////////////////
void ModuleChecksWrittenRegisters(uchar mem_type, uint address)
{
	// Check if MB wrote module_name in eeprom
	if ((mem_type == EEPROM_MOD) && (address == MODULE_NAME_EE_ADR))
	{
		EE_Address = MODULE_NAME_EE_ADR;   
		ReadMultiByteI2c_24C16((far uchar *)(&module_name[0]), MODULE_NAME_LENGHT);
	}

}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: CheckControls()
// Inputs	: None
// Outputs	: None
// Process  : Given encoder A and B inputs state, we update Z8_ENC_CONTROL_STATUS 
///////////////////////////////////////////////////////////////////////////////////////
void CheckControls(void)
{
	uchar must_check_b = FALSE;

	uchar current_group, local_ctrl;

	// Set current group local variable
	current_group = config_reg[CURRENT_GROUP_ADR];

	////////////////////////////
	// CHECK FOR ENC 0
	////////////////////////////
	local_ctrl = Ctrl[ENC0][current_group];

	if (ENC0_INPUT_A_PORT_IN & ENC0_INPUT_A_PIN)		
	{
		// Check if status changed
		if( !(Z8_ENC_STATUS & enc0_input_A) )
		{
			Z8_ENC_STATUS |= enc0_input_A;					  	// input A  0 -> 1
			// Update control value regarding input B
			must_check_b = TRUE;
		}
	}
	else if(Z8_ENC_STATUS & enc0_input_A)
	{
			Z8_ENC_STATUS &= ~enc0_input_A;					// input A 1 -> 0
			// Update control value regarding input B
			must_check_b = TRUE;
	}

	if(must_check_b)
	{
		// A is high
		if(Z8_ENC_STATUS & enc0_input_A)
		{
			if(ENC0_INPUT_B_PORT_IN & ENC0_INPUT_B_PIN)
				local_ctrl++;
			else
				if(local_ctrl) local_ctrl--;
		}
		else
		{
			if (ENC0_INPUT_B_PORT_IN & ENC0_INPUT_B_PIN)
				if(local_ctrl) local_ctrl--;
			else
				local_ctrl++;
		}
		// Flag : value has changed so we will have new data to send to MB 
		Z8_STATUS_0 &= ~onewire_no_new_data_z0;
	}

	// Update Ctrl[] to local variable
	if(local_ctrl > 127) local_ctrl = 127;
	Ctrl[ENC0][current_group] = local_ctrl;


	must_check_b = FALSE;

	////////////////////////////
	// CHECK FOR ENC 1
	////////////////////////////
	local_ctrl = Ctrl[ENC1][current_group];

	if (ENC1_INPUT_A_PORT_IN & ENC1_INPUT_A_PIN)		
	{
		// Check if status changed
		if( !(Z8_ENC_STATUS & enc1_input_A) )
		{
			Z8_ENC_STATUS |= enc1_input_A;					  	// input A  0 -> 1
			// Update control value regarding input B
			must_check_b = TRUE;
		}
	}
	else if(Z8_ENC_STATUS & enc1_input_A)
	{
			Z8_ENC_STATUS &= ~enc1_input_A;					// input A 1 -> 0
			// Update control value regarding input B
			must_check_b = TRUE;
	}

	if(must_check_b)
	{
		// A is high
		if(Z8_ENC_STATUS & enc1_input_A)
		{
			if(ENC1_INPUT_B_PORT_IN & ENC1_INPUT_B_PIN)
				local_ctrl++;
			else
				if(local_ctrl) local_ctrl--;
		}
		else
		{
			if (ENC1_INPUT_B_PORT_IN & ENC1_INPUT_B_PIN)
				if(local_ctrl) local_ctrl--;
			else
				local_ctrl++;
		}
		// Flag : value has changed so we will have new data to send to MB 
		Z8_STATUS_0 &= ~onewire_no_new_data_z0;
	}

	// Update Ctrl[] to local variable
	if(local_ctrl > 127) local_ctrl = 127;
	Ctrl[ENC1][current_group] = local_ctrl;
	
	must_check_b = FALSE;

	////////////////////////////
	// CHECK FOR ENC 2
	////////////////////////////
	local_ctrl = Ctrl[ENC2][current_group];

	if (ENC2_INPUT_A_PORT_IN & ENC2_INPUT_A_PIN)		
	{
		// Check if status changed
		if( !(Z8_ENC_STATUS & enc2_input_A) )
		{
			Z8_ENC_STATUS |= enc2_input_A;					  	// input A  0 -> 1
			// Update control value regarding input B
			must_check_b = TRUE;
		}
	}
	else if(Z8_ENC_STATUS & enc2_input_A)
	{
			Z8_ENC_STATUS &= ~enc2_input_A;					// input A 1 -> 0
			// Update control value regarding input B
			must_check_b = TRUE;
	}

	if(must_check_b)
	{
		// A is high
		if(Z8_ENC_STATUS & enc2_input_A)
		{
			if(ENC2_INPUT_B_PORT_IN & ENC2_INPUT_B_PIN)
				local_ctrl++;
			else
				if(local_ctrl) local_ctrl--;
		}
		else
		{
			if (ENC2_INPUT_B_PORT_IN & ENC2_INPUT_B_PIN)
				if(local_ctrl) local_ctrl--;
			else
				local_ctrl++;
		}
		// Flag : value has changed so we will have new data to send to MB 
		Z8_STATUS_0 &= ~onewire_no_new_data_z0;
	}	

	// Update Ctrl[] to local variable
	if(local_ctrl > 127) local_ctrl = 127;
	Ctrl[ENC2][current_group] = local_ctrl;

	must_check_b = FALSE;

	////////////////////////////
	// CHECK FOR ENC 3
	////////////////////////////
	local_ctrl = Ctrl[ENC3][current_group];

	if (ENC3_INPUT_A_PORT_IN & ENC3_INPUT_A_PIN)		
	{
		// Check if status changed
		if( !(Z8_ENC_STATUS & enc3_input_A) )
		{
			Z8_ENC_STATUS |= enc3_input_A;					  	// input A  0 -> 1
			// Update control value regarding input B
			must_check_b = TRUE;
		}
	}
	else if(Z8_ENC_STATUS & enc3_input_A)
	{
			Z8_ENC_STATUS &= ~enc3_input_A;					// input A 1 -> 0
			// Update control value regarding input B
			must_check_b = TRUE;
	}

	if(must_check_b)
	{
		// A is high
		if(Z8_ENC_STATUS & enc3_input_A)
		{
			if(ENC3_INPUT_B_PORT_IN & ENC3_INPUT_B_PIN)
				local_ctrl++;
			else
				if(local_ctrl) local_ctrl--;
		}
		else
		{
			if (ENC3_INPUT_B_PORT_IN & ENC3_INPUT_B_PIN)
				if(local_ctrl) local_ctrl--;
			else
				local_ctrl++;
		}
		// Flag : value has changed so we will have new data to send to MB 
		Z8_STATUS_0 &= ~onewire_no_new_data_z0;
	}	

	// Update Ctrl[] to local variable
	if(local_ctrl > 127) local_ctrl = 127;
	Ctrl[ENC3][current_group] = local_ctrl;

}	

//////////////////////////////////////////////////////////////////////////////////////
// Routine 	: SetHookStateByte(uchar byte_nb)
// Inputs	: None
// Outputs	: None
// Process  : This module does not return any HOOK_STATE byte 
///////////////////////////////////////////////////////////////////////////////////////
uchar SetHookStateByte(uchar byte_nb)
{		
	return(0);
}
 
		
///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: GetControlValue(uchar ctrl_nb)
// Inputs	: None
// Outputs	: None
// Process  : Return value of a given control
//			  This routine is used by SendOneWireValues()
///////////////////////////////////////////////////////////////////////////////////////
uchar GetControlValue(uchar ctrl_nb)
{
	uchar out;
	
	out = Ctrl[ctrl_nb][config_reg[CURRENT_GROUP_ADR]];

	return(out);
}
