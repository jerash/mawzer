#include <eZ8.h>
#include "pins_defs_BUT.h"
#include "commondef.h"
#include "onewire.h"
#include "itrs_BUT.h"
#include "controls_BUT.h"
#include "eeprom.h"
#include "i2c.h"

///////////////////////////////////////////////////////////////////////////////////////
// GLOBAL VARS
///////////////////////////////////////////////////////////////////////////////////////
rom uchar module_type[MODULE_TYPE_LENGHT] = "BUT410";
uchar module_name[MODULE_NAME_LENGHT];

extern uchar Z8_STATUS_4;
extern uchar Z8_STATUS_0;

extern far uchar config_reg[CONFIG_REG_TABLE_MAX] ;  
extern far uchar assignment[CONTROL_NB][GROUP_NB];

uchar Z8_BUT_STATUS;    // store wether button is pressed on or not
uchar Ctrl[GROUP_NB];   // store button states (on / off) and mode (toggle / push) 

extern uint  EE_Address;
extern far uchar generic_buffer[GENERIC_BUFF_SIZE];

//////////////////////////////////////////////////////////////////////////////////////
// Routine 	: InitModule()
// Inputs	: None
// Outputs	: None
// Process  : Initialize module hardware and control default values 
///////////////////////////////////////////////////////////////////////////////////////
void InitModule(void)
{
	uchar group;
	
	// Initialize all buttons to "not pressed"
	Z8_BUT_STATUS = 0x00;

	// Initialize all buttons to OFF for all groups
	for(group = 0; group < GROUP_NB; group++)
		Ctrl[group] = 0x00;

	// Load push / toggle mode from eeprom for all controls and all groups
	UpdateButModeStatus();

}

//////////////////////////////////////////////////////////////////////////////////////
// Routine 	: LoadModeInfo()
// Inputs	: None
// Outputs	: None
// Process  : Load push / toggle mode from eeprom for all controls and 
//				all groups in Ctrl[] table
///////////////////////////////////////////////////////////////////////////////////////
void UpdateButModeStatus(void)
{
	uchar group, ctrl, tmp; 
	
	// Set EEPROM read address
	EE_Address = MODULE_ASSIGN_STRUCT_BASE_EE_ADR + ASSIGN_STRUCT_CONTROL_STATUS1_ADR;

	// Loops on all group, all control
	for(group = 0; group < GROUP_NB; group++)
	{
		// remove old toggle/push modes
		Ctrl[group] &= 0x0F;

		for(ctrl = 0; ctrl < CONTROL_NB; ctrl++)
		{
			// Read control_status_1 byte in assignment structure from EEPROM
			ReadMultiByteI2c_24C16((far uchar *)(&tmp), 1);
			
			// Check push / toggle information 
			if (tmp & SWITCH_CONTROL_MODE_IDX)
				Ctrl[group] |= (uchar)(0x01 << (ctrl + CTRL_STATUS_TOGGLE_IDX));

			// Update EEPROM read address to next control
			EE_Address += MODULE_ASSIGN_STRUCT_SIZE;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////
// Routine 	: ModuleChecksWrittenRegisters(uchar mem_type, uint address)
// Inputs	: mem_type, address
// Outputs	: None
// Process  : The module checks what MB has written in its registers
//			  -> if assignement struct is written we update toggle / push information 
//			  -> if group changes we update LEDs
///////////////////////////////////////////////////////////////////////////////////////
void ModuleChecksWrittenRegisters(uchar mem_type, uint address)
{
	// Check if MB wrote module_name in eeprom
	if ((mem_type == EEPROM_MOD) && (address == MODULE_NAME_EE_ADR))
	{
		EE_Address = MODULE_NAME_EE_ADR;   
		ReadMultiByteI2c_24C16((far uchar *)(&module_name[0]), MODULE_NAME_LENGHT);
	}

	// Check if MB wrote assignement struct in eeprom
	if ((mem_type == EEPROM_MOD) && (address >= MODULE_ASSIGN_STRUCT_BASE_EE_ADR) && (address < TOTAL_EE_BYTES_ASSIGNED))
		UpdateButModeStatus();

	// Check if MB changed current group
	if ((mem_type == RAM_MOD) && (((uchar) address) == CURRENT_GROUP_ADR))
		UpdateLEDs();

}


///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: UpdateLEDs()
// Inputs	: None
// Outputs	: None
// Process  : Update LEDS according to the new group control status
///////////////////////////////////////////////////////////////////////////////////////
void UpdateLEDs(void)
{
	uchar local_ctrl;

	// Set control with current group (local variable)
	local_ctrl = Ctrl[config_reg[CURRENT_GROUP_ADR]];

	// Check button 0 state
	if(local_ctrl & but0_state_on)
		LED_BUT0_PORT_OUT |= LED_BUT0_PIN;
	else	
		LED_BUT0_PORT_OUT &= ~LED_BUT0_PIN;

	// Check button 1 state	
	if(local_ctrl & but1_state_on)
		LED_BUT1_PORT_OUT |= LED_BUT1_PIN;
	else	
		LED_BUT1_PORT_OUT &= ~LED_BUT1_PIN;

	// Check button 2 state
	if(local_ctrl & but2_state_on)
		LED_BUT2_PORT_OUT |= LED_BUT2_PIN;
	else	
		LED_BUT2_PORT_OUT &= ~LED_BUT2_PIN;
	
	// Check button 3 state
	if(local_ctrl & but3_state_on)
		LED_BUT3_PORT_OUT |= LED_BUT3_PIN;
	else	
		LED_BUT3_PORT_OUT &= ~LED_BUT3_PIN;

}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: CheckControls()
// Inputs	: None
// Outputs	: None
// Process  : Given pin states, we update Z8_BUT_STATUS controls and leds  
///////////////////////////////////////////////////////////////////////////////////////
void CheckControls(void)
{

	uchar local_ctrl;

	// Set control with current group (local variable)
	local_ctrl = Ctrl[config_reg[CURRENT_GROUP_ADR]];

	////////////////////////////
	// CHECK FOR BUT 0
	////////////////////////////
	// Check if button is pressed on	
	if (BUT0_SW_PORT_IN & BUT0_SW_PIN)		
	{
		// Put the button on if not already on
		if( !(Z8_BUT_STATUS & but0_pressed) )
		{
			Z8_BUT_STATUS |= but0_pressed;					  // pin 0 -> 1

			// Check for toggle mode
			if ((local_ctrl & but0_toggle) && (local_ctrl & but0_state_on))
			{
				// Put BUTTON OFF
				local_ctrl &= ~but0_state_on; 
				LED_BUT0_PORT_OUT &= ~LED_BUT0_PIN;
			}
			else
			{
				// Put BUTTON ON
				local_ctrl |= but0_state_on; 
				LED_BUT0_PORT_OUT |= LED_BUT0_PIN;
			}
			// Flag : value has changed so we will have new data to send to MB 
			Z8_STATUS_0 &= ~onewire_no_new_data_z0;
		}
	}
	else
	// Button is not pressed
	{
		// Put the button off if not already off
		if(Z8_BUT_STATUS & but0_pressed)
		{
			Z8_BUT_STATUS &= ~but0_pressed;					// pin 1 -> 0
			
			// Check for push mode
			if (!(local_ctrl & but0_toggle))
			{
				// Put BUTTON OFF
				local_ctrl &= ~but0_state_on; 
				LED_BUT0_PORT_OUT &= ~LED_BUT0_PIN;	
				// Flag : value has changed so we will have new data to send to MB 
				Z8_STATUS_0 &= ~onewire_no_new_data_z0;
			}
		}
	 }
	
	////////////////////////////
	// CHECK FOR BUT 1
	////////////////////////////
	// Check if button is pressed on	
	if (BUT1_SW_PORT_IN & BUT1_SW_PIN)		
	{
		// Put the button on if not already on
		if( !(Z8_BUT_STATUS & but1_pressed) )
		{
			Z8_BUT_STATUS |= but1_pressed;					  // pin 0 -> 1

			// Check for toggle mode
			if ((local_ctrl & but1_toggle) && (local_ctrl & but1_state_on))
			{
				// Put BUTTON OFF
				local_ctrl &= ~but1_state_on; 
				LED_BUT1_PORT_OUT &= ~LED_BUT1_PIN;
			}
			else
			{
				// Put BUTTON ON
				local_ctrl |= but1_state_on; 
				LED_BUT1_PORT_OUT |= LED_BUT1_PIN;
			}
			// Flag : value has changed so we will have new data to send to MB 
			Z8_STATUS_0 &= ~onewire_no_new_data_z0;
		}
	}
	else
	// Button is not pressed
	{
		// Put the button off if not already off
		if(Z8_BUT_STATUS & but1_pressed)
		{
			Z8_BUT_STATUS &= ~but1_pressed;					// pin 1 -> 0
			
			// Check for push mode
			if (!(local_ctrl & but1_toggle))
			{
				// Put BUTTON OFF
				local_ctrl &= ~but1_state_on; 
				LED_BUT1_PORT_OUT &= ~LED_BUT1_PIN;	
				// Flag : value has changed so we will have new data to send to MB 
				Z8_STATUS_0 &= ~onewire_no_new_data_z0;
			}
		}
	 }
	
	////////////////////////////
	// CHECK FOR BUT 2
	////////////////////////////
	// Check if button is pressed on	
	if (BUT2_SW_PORT_IN & BUT2_SW_PIN)		
	{
		// Put the button on if not already on
		if( !(Z8_BUT_STATUS & but2_pressed) )
		{
			Z8_BUT_STATUS |= but2_pressed;					  // pin 0 -> 1

			// Check for toggle mode
			if ((local_ctrl & but2_toggle) && (local_ctrl & but2_state_on))
			{
				// Put BUTTON OFF
				local_ctrl &= ~but2_state_on; 
				LED_BUT2_PORT_OUT &= ~LED_BUT2_PIN;
			}
			else
			{
				// Put BUTTON ON
				local_ctrl |= but2_state_on; 
				LED_BUT2_PORT_OUT |= LED_BUT2_PIN;
			}
			// Flag : value has changed so we will have new data to send to MB 
			Z8_STATUS_0 &= ~onewire_no_new_data_z0;
		}
	}
	else
	// Button is not pressed
	{
		// Put the button off if not already off
		if(Z8_BUT_STATUS & but2_pressed)
		{
			Z8_BUT_STATUS &= ~but2_pressed;					// pin 1 -> 0
			
			// Check for push mode
			if (!(local_ctrl & but2_toggle))
			{
				// Put BUTTON OFF
				local_ctrl  &= ~but2_state_on; 
				LED_BUT2_PORT_OUT &= ~LED_BUT2_PIN;	
				// Flag : value has changed so we will have new data to send to MB 
				Z8_STATUS_0 &= ~onewire_no_new_data_z0;
			}
		}
	 }

	////////////////////////////
	// CHECK FOR BUT 3
	////////////////////////////
	// Check if button is pressed on	
	if (BUT3_SW_PORT_IN & BUT3_SW_PIN)		
	{
		// Put the button on if not already on
		if( !(Z8_BUT_STATUS & but3_pressed) )
		{
			Z8_BUT_STATUS |= but3_pressed;					  // pin 0 -> 1

			// Check for toggle mode
			if ((local_ctrl & but3_toggle) && (local_ctrl & but3_state_on))
			{
				// Put BUTTON OFF
				local_ctrl &= ~but3_state_on; 
				LED_BUT3_PORT_OUT &= ~LED_BUT3_PIN;
			}
			else
			{
				// Put BUTTON ON
				local_ctrl |= but3_state_on; 
				LED_BUT3_PORT_OUT |= LED_BUT3_PIN;
			}
			// Flag : value has changed so we will have new data to send to MB 
			Z8_STATUS_0 &= ~onewire_no_new_data_z0;
		}
	}
	else
	// Button is not pressed
	{
		// Put the button off if not already off
		if(Z8_BUT_STATUS & but3_pressed)
		{
			Z8_BUT_STATUS &= ~but3_pressed;					// pin 1 -> 0
			
			// Check for push mode
			if (!(local_ctrl & but3_toggle))
			{
				// Put BUTTON OFF
				local_ctrl &= ~but3_state_on; 
				LED_BUT3_PORT_OUT &= ~LED_BUT3_PIN;	
				// Flag : value has changed so we will have new data to send to MB 
				Z8_STATUS_0 &= ~onewire_no_new_data_z0;
			}
		}
	 }

	 // Update Ctrol[] table with local variable
	Ctrl[config_reg[CURRENT_GROUP_ADR]] = local_ctrl;

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
	uchar out, idx;
	
	// Initialization
	out = BUTTON_OFF;
	idx = 0x01 << ctrl_nb;

	// Check button state
	if ((Ctrl[config_reg[CURRENT_GROUP_ADR]]) & idx)
		out = BUTTON_ON;

	return(out);
}
