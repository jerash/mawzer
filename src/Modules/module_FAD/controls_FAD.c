#include <eZ8.h>
#include "pins_defs_FAD.h"
#include "commondef.h"
#include "onewire.h"
#include "itrs_FAD.h"
#include "controls_FAD.h"
#include "eeprom.h"
#include "i2c.h"

///////////////////////////////////////////////////////////////////////////////////////
// GLOBAL VARS
///////////////////////////////////////////////////////////////////////////////////////
rom uchar module_type[MODULE_TYPE_LENGHT] = "FAD110";
uchar module_name[MODULE_NAME_LENGHT];

extern uchar Z8_STATUS_4;
extern uchar Z8_STATUS_0;
extern far uchar Analog;
extern far uchar config_reg[CONFIG_REG_TABLE_MAX] ;  
extern far uchar assignment[CONTROL_NB][GROUP_NB];

extern uint  EE_Address;

far CONTROL_STRUCT Ctrl[CONTROL_NB];

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: InitAnalog()
// Inputs	: None
// Outputs	: None
// Process  : Initilize Z8 AD converters
///////////////////////////////////////////////////////////////////////////////////////
void InitAnalog(void)
{	

	ADCCTL = 0xA0;
}

//////////////////////////////////////////////////////////////////////////////////////
// Routine 	: InitModule()
// Inputs	: None
// Outputs	: None
// Process  : Initialize module hardware and control default values 
///////////////////////////////////////////////////////////////////////////////////////
void InitModule(void)
{
	// Initialize ADC
	InitAnalog();
	
	// NOTE : default control values are set in CheckControls first pass
}


//////////////////////////////////////////////////////////////////////////////////////
// Routine 	: ModuleChecksWrittenRegisters(uchar mem_type, uint address)
// Inputs	: mem_type, address
// Outputs	: None
// Process  : The module checks what MB has written in its registers
//			  -> this module has nothing to check here
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
// Process  : Given analog values, we update Ctrl[] structure table : 
//				- physical value 
//				- cc value per group
//			  	- hook and direction bit 
///////////////////////////////////////////////////////////////////////////////////////
void CheckControls(void)
{
	uchar index, current_value;	
	uchar current_group, i;
	uchar local_cc_value, local_previous_direction, local_new_direction;
	uchar current_pot_7F;

	// Here in FAD module we only have to check one control
	index = FAD0; 

	// Get A/D converted value
	current_value = Analog;
	current_pot_7F = (Ctrl[index].Physical_value) & MASK_7BITS;
	
	// filter if not first pass nor if 127
	if((current_value != 127) && (!(Z8_STATUS_4 & first_pass_z4)))
		current_value = (current_pot_7F + current_value) >> 1;

	// Check current_value is 7 bit
	current_value &= MASK_7BITS;

	// Initialize Ctrl[] table on First Pass
	if (Z8_STATUS_4 & first_pass_z4)
	{
		// Hook and set up physical values 
		Ctrl[index].Physical_value = current_value | HOOK_BIT;
		// Set up CC values for all groups
		for(i = 0; i<GROUP_NB; i++)
			Ctrl[index].CC_value[i] = current_value;
	}

	// Check if value changed : 
	else if (current_value != current_pot_7F)
	{	
		// Flag : Physical value has changed so we will have new data to send to MB 
		Z8_STATUS_0 &= ~onewire_no_new_data_z0;

		// Set current group (local variable)
		current_group = config_reg[CURRENT_GROUP_ADR];

		// IF NOT HOOKED	
		if(!(Ctrl[index].Physical_value & HOOK_BIT)) 
		{
			local_cc_value = (Ctrl[index].CC_value[current_group] & MASK_7BITS);
			
			// Calcul previous direction sign 
			if( (Ctrl[index].Physical_value & MASK_7BITS) > local_cc_value)
				local_previous_direction = POSITIVE_BIT;
			else
				local_previous_direction = 0;

		    // Calcul the new direction sign
			if(current_value > local_cc_value)
				local_new_direction = POSITIVE_BIT;  // ie this means TRUE
			else if(current_value < local_cc_value)
				local_new_direction = 0 ;    // don't forget to reinit for futur loop pass
			else 
			{
				Ctrl[index].Physical_value |= HOOK_BIT;  // hook because cc = physical 
				local_new_direction = 0 ;
			}

			// If direction sign has changed : hook and update CC value
			if (local_new_direction != local_previous_direction)
			{
				Ctrl[index].Physical_value |= HOOK_BIT;	
				Ctrl[index].CC_value[current_group] = current_value;
			}	

			// Update direction sign to send to MB (see SendOneWireValues() in onewire.c)
			Ctrl[index].CC_value[current_group] &= MASK_7BITS;
			Ctrl[index].CC_value[current_group] |= local_new_direction;
		}

		// IF HOOKED : 
		else 
		{
			// UN-HOOK when physical value differs from CC
			if ((Ctrl[index].Physical_value & MASK_7BITS)!= (Ctrl[index].CC_value[current_group] & MASK_7BITS))
				Ctrl[index].Physical_value &= ~HOOK_BIT; 
			else
				// update CC value when still hooked
				Ctrl[index].CC_value[current_group] = current_value;
		}

		// Update (7 bits) physical value given the new analog value
		Ctrl[index].Physical_value &= ~MASK_7BITS;
		Ctrl[index].Physical_value |= current_value;

	} // check if physical value changed 	

}	

//////////////////////////////////////////////////////////////////////////////////////
// Routine 	: SetHookStateByte(uchar byte_nb)
// Inputs	: None
// Outputs	: None
// Process  : return HOOK_STATE byte given  :
//						- POT0 and POT1 hooks if argument byte_nb = 0
//						- POT2 and POT3 hooks if argument byte_nb = 1
//
// HOOK_STATE_1 defines the hook state of controls 0 and 1 on the module
// HOOK_STATE_2 defines the hook state of controls 2 and 3 on the module
// HOOK_STATE_N bits definition are :
//	bits 7-4 : 	Always 0
//	bits 3,2 : 	x,0 : le contrôle est « hooké »
//		  		0,1 : contrôle non « hooké », valeur physique < valeur du groupe
//		  		1,1 : contrôle non « hooké », valeur physique > valeur du groupe
//
// #define low_hook_direction_bit	(1<<1)	// 0 : valeur physique < valeur du groupe
// #define low_control_hooked_bit	(1<<0)	// 0 : control hooked / 1 : control not hooked
// #define high_hook_direction_bit	(1<<3)	// 0 : valeur physique < valeur du groupe
// #define high_control_hooked_bit	(1<<2)	// 0 : control hooked / 1 : control not hooked
///////////////////////////////////////////////////////////////////////////////////////
uchar SetHookStateByte(uchar byte_nb)
{

	uchar local_hook_state;
	uchar current_group;

	local_hook_state = 0;

	// Set current group (local variable)
	current_group = config_reg[CURRENT_GROUP_ADR];

	// Here in FAD module we only use HOOK_STATE_BYTE1, so we don't care about byte_nb
	if (!(Ctrl[FAD0].Physical_value & HOOK_BIT))
	{
		local_hook_state |= low_control_hooked_bit;
		if (Ctrl[FAD0].CC_value[current_group] & POSITIVE_BIT)
				local_hook_state |= low_hook_direction_bit;
	}
			
	return(local_hook_state);
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
	uchar out; //, idx;

	out = (Ctrl[ctrl_nb].Physical_value & MASK_7BITS);

	return(out);
}
