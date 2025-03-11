#include <eZ8.h>
#include "pins_defs.h"
#include "commondef.h"
#include <common_z8_ez.h>
#include <ctrldef.h>
#include "lcd.h"
#include "lcd_low_level.h"
#include "assign.h"
#include "hardware.h"
#include "eeprom.h"
#include "menus.h"
#include "i2c.h"
#include "serial.h"
#include "onewire.h"

far Assign module_assign;
far uchar module_name_assign[MODULE_NAME_LENGHT+1];
uchar assign_state;
extern uchar control_index;					// current control nb
extern uchar last_control_index;			// last control nb

extern uchar Z8_STATUS_7;
extern far All_Params	 Param[PARAM_STRUCT_IN_RAM_MAX_NB];	// V1.61 structure of parameters in EEPROM V1.61
extern rom uchar control_txt[];
extern rom uchar module_txt[];
extern rom uchar to_assign_txt[];
extern rom uchar font_5x8[];
extern far All_Modules module[MODULES_MAX_NB];

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: AssignEnter()
// Inputs	: menu_state
// Outputs	: None
// Process  : Enter into assign menus mode
///////////////////////////////////////////////////////////////////////////////////////
void AssignEnter(void)
{	
	// Clear display
	FillLcdZone(0, 0, LCD_X_PIXEL_MAX, LCD_Y_PAGE_MAX, FILL_BLANK);
	// must assign module name or control structure ?
	if(Z8_STATUS_7 & assign_type_z7)
	{
		// Display text waiting control
		rom_to_lcd_text_copy(control_txt);
		// reset flag to save structure assigned
		Z8_STATUS_7 &= ~must_save_assign_struct_z7;
	}
	else
	{
		// Display text waiting module
		rom_to_lcd_text_copy(module_txt);
		// set flag to save module name assigned
		Z8_STATUS_7 |= must_save_assign_struct_z7;
	}

	DisplayLcdText(7, 3, LCD_INVERT, font_5x8);	
	rom_to_lcd_text_copy(to_assign_txt);
	DisplayLcdText(53, 3, LCD_INVERT, font_5x8);	
	// wait for the user to confirm control to assign
	assign_state = ASSIGN_WAIT_TWEAK_CTRL;
	// be sure to wait for the control to assign
	module_assign.control_assign_index  = CTRL_UNDEF;
	module_assign.current_module_assign = CTRL_UNDEF;
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: AssignEnter()
// Inputs	: menu_state
// Outputs	: None
// Process  : Enter into assign menus mode
///////////////////////////////////////////////////////////////////////////////////////
void AssignWaitControl(void)
{
	uchar idx;

	// continue only if waiting control to assign 
	if(assign_state != ASSIGN_WAIT_TWEAK_CTRL)
		return;

	// the control has been tweaked
	if(module_assign.control_assign_index == CTRL_UNDEF)
		return;

	// place control index for assign to control index
	control_index = module_assign.control_assign_index;
	// be sure struct will be reloaded after assign
	last_control_index = CTRL_UNDEF;
	// set the new assign mode
	assign_state = ASSIGN_ACTIVE;

	// depending on assign type, load structure or module name
	if(Z8_STATUS_7 & assign_type_z7)
	{
		// Load full structure
		GetCtrlStructEEaddFromIndexGroup();
		// Read struct full or status only from eeprom 
		ReadMultiByteI2c((far uchar *)&Param[0].message_type, CTRL_STRUCT_TOTAL_SIZE);
	}
	else
	{
		// here we need to assign a module name : load it to special ram
		for(idx=0; idx<MODULE_NAME_LENGHT ; idx++)
			module_name_assign[idx] = module[module_assign.current_module_assign].module_name[idx];
	}

	// Show assign menus
	MenuEnter();
}

