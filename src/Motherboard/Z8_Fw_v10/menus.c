#include <eZ8.h>
#include "pins_defs.h"
#include "commondef.h"
#include <common_z8_ez.h>
#include <ctrldef.h>
#include "lcd.h"
#include "lcd_low_level.h"
#include "hardware.h"
#include "itrs.h"
#include "eeprom.h"
#include "menus.h"
#include "smpte.h"
#include "i2c.h"
#include "assign.h"
#include "autotest.h"
#include "onewire.h"
#include "menus_tables.h"

///////////////////////////////////////////////////////////////////////////////////////
// GLOBAL VARS
///////////////////////////////////////////////////////////////////////////////////////
uchar current_menu_depth;   // current menu depth
uchar current_menu_index;   	// index into the menutable of the current selected menu
uchar first_index_depth, last_index_depth;
far uchar menu_index_call_stack[MENUS_MAX_DEPTH];
uchar menu_stack_index = 0;
uchar next_value;
uchar up_down_action; 
uchar menu_state;
uchar encoder_but_lock_cpt;
uchar edit_string_idx;

extern far uchar CtrlReg[CTRL_REG_REAL_NB];		// table of control reg for eerprom saving and menus 
extern uchar Z8_STATUS_0;
extern uchar Z8_STATUS_3;
extern volatile uchar Z8_STATUS_4;
extern volatile uchar Z8_STATUS_5;
extern volatile uchar Z8_STATUS_6;
extern uchar Z8_STATUS_7;
extern volatile uchar Z8_STATUS_8;
extern uchar Z8_STATUS_9;
extern far uchar lcd_text_value[MAX_LCD_TEXT_LENGHT];	// global buffer for integer to text conversion	
// extern rom uchar menus_txt[];
extern rom uchar bmp_menu_title[];
extern rom uchar font_5x8[];
extern far All_Params	 Param[PARAM_STRUCT_IN_RAM_MAX_NB];	// V1.61 structure of parameters in EEPROM V1.61
extern uchar assign_state;
extern uchar control_index;					// current control nb
extern uchar last_control_index;			// last control nb
extern uchar current_module;

// extern uchar control_assign_index;
// extern uchar control_num_in_module_assign;
extern far Assign module_assign;
extern far uchar module_name_assign[MODULE_NAME_LENGHT+1];
far uchar preset_name_assign[GROUP_NAME_REAL_SIZE+1];
extern far All_Modules module[MODULES_MAX_NB];
extern far uchar generic_buffer[GENERIC_BUFF_SIZE];	// a generic far ram buffer of 128 bytes
extern uint  EE_Address;							// current EEProm address
extern uchar I2cCommand;							// current EEProm selection
extern far All_Params *ParamCurrent;						//current structure of parameters in EEPROM

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: CheckMenuSwitches()
// Inputs	: None
// Outputs	: None
// Process  : Check if a menu related sw has been pressed
///////////////////////////////////////////////////////////////////////////////////////
void CheckMenuSwitches(void)
{
	uchar encoder_button_pressed = FALSE;

	/////////////////////////////
	// CHECK FOR ENCODER 1 BUTTON
	/////////////////////////////
	if(ENCODER1_SW_PORT_IN & ENCODER1_SW_PIN)			
	{
		// does the button passed through zero and is the encoder button debounced ?
		if(Z8_STATUS_0 & enc1_button_passed_0_z0)
		if(!(encoder_but_lock_cpt))
		{
			Z8_STATUS_0 &= ~enc1_button_passed_0_z0;
			encoder_but_lock_cpt = BUTTON_DEBOUNCE_CPT;
			encoder_button_pressed = TRUE;
		}
	}
	else
		Z8_STATUS_0 |= enc1_button_passed_0_z0;

	
	// The encoder button has been pressed
	if(encoder_button_pressed)
	{
		// Need to enter menus ?
		if(menu_state == NO_MENU)
			MenuEnter();
		else
		// already in menus ?
		{
			Z8_STATUS_6 |= ok_sw_changed_z6;
		}
	}

	////////////////////////////////////////////////////////////////////////
	// here we are in menu mode : check ok/cancel/encoder switches
	////////////////////////////////////////////////////////////////////////
	
	//////////////////////////
	// CHECK FOR CANCEL BUTTON
	//////////////////////////
	if(CANCEL_PORT_IN & CANCEL_PIN)			
	{
		// does the button passed through zero ?
		if(Z8_STATUS_0 & cancel_passed_0_z0)
		{
			Z8_STATUS_0 &= ~cancel_passed_0_z0;
			ButtonCancelClick();
		}
	}
	else
		Z8_STATUS_0 |= cancel_passed_0_z0;

	// Ignore OK button if not in menus
	if(menu_state == NO_MENU) return;

	//////////////////////
	// CHECK FOR OK BUTTON
	//////////////////////
	if(OK_PORT_IN & OK_PIN)			
	{
		// does the button passed through zero ?
		if(Z8_STATUS_0 & ok_passed_0_z0)
		{
			Z8_STATUS_0 &= ~ok_passed_0_z0;
			ButtonOkClick();
		}
	}
	else
		Z8_STATUS_0 |= ok_passed_0_z0;
	

	// OK is also obtained from encoder sw when menu mode active
	if(Z8_STATUS_6 & ok_sw_changed_z6)
	{
		ButtonOkClick();
		Z8_STATUS_6 &= (~ok_sw_changed_z6);
	}

	// UP or DOWN SWITCH PRESSED ?
	if(Z8_STATUS_6 & up_down_sw_changed_z6)
	{
		Z8_STATUS_6 &= (~up_down_sw_changed_z6);

		// UP switch pressed ?
		if(Z8_STATUS_6 & up_down_sw_direction_z6)
			ChangeCurrentMenu(UP_PRESSED);
		else
			ChangeCurrentMenu(DOWN_PRESSED);				

	}
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: MenuEnter()
// Inputs	: menu_state
// Outputs	: None
// Process  : Enter into menus mode, check menus_state value
///////////////////////////////////////////////////////////////////////////////////////
void MenuEnter(void)
{	
	// cannot enter menus if assign mode is waiting for a control to assign
	if(assign_state == ASSIGN_WAIT_TWEAK_CTRL)
		return;

	// enter menus
	menu_state = MENU_PARAM;

	// Clear display
	FillLcdZone(0, 0, LCD_X_PIXEL_MAX, LCD_Y_PAGE_MAX, FILL_BLANK);

	// Display menu title
	DisplayBmp(0, 0, LCD_INVERT, bmp_menu_title);  
	// Draw bottom border for menus
	FillLcdZone(1, LCD_Y_PAGE_MAX-1, LCD_X_PIXEL_MAX-2, 1, 0xBF);
	// Draw left border for menus
	FillLcdZone(1, 1, 1, LCD_Y_PAGE_MAX-2, 0x00);
	FillLcdZone(1, LCD_Y_PAGE_MAX-1, 1, 1, 0x80);
	// Draw right border for menus
	FillLcdZone(LCD_X_PIXEL_MAX-2, 1, 1, LCD_Y_PAGE_MAX-2, 0x00);
	FillLcdZone(LCD_X_PIXEL_MAX-2, LCD_Y_PAGE_MAX-1, 1, 1, 0x80);
	FillLcdZone(LCD_X_PIXEL_MAX-2, 0, 1, 1, 0x01);
	// Draw top border for menus
	FillLcdZone(29, 0, LCD_X_PIXEL_MAX-31, 1, 0xFD);

	// menus is for assign ?
	if(assign_state == ASSIGN_ACTIVE)
	{
		current_menu_depth = MENU_DEPTH_2;
		// reconstruct a virtual caller stack
		menu_index_call_stack[0] = MODULE_CONFIG_INDEX_MENU;
		menu_stack_index = 2;

		// must assign module name or control struct ?
		if(Z8_STATUS_7 & assign_type_z7)
		{
			current_menu_index = MENUS_ASSIGN_FIRST_INDEX; 
			menu_index_call_stack[1] = MODULE_ASSIGN_INDEX_MENU;
		}
		else
		{
			current_menu_index = MODULE_NAME_DUMMY_INDEX_ASSIGN; 
			menu_index_call_stack[1] = MODULE_NAME_INDEX_MENU;
		}
	}
	else
	// menu is for standard menus
	{
		// return to top level depth
		current_menu_depth = TOP_LEVEL_MENU;
		current_menu_index = 0; 
	}

//	if(menutable[current_menu_index].display_type == DISP_EDIT_STRING)
//		up_down_action = CHANGE_MENU_VALUE;
//	else
		// up & down switches are still changing menus index
//   		up_down_action = CHANGE_MENU_INDEX;

    // Find first & last indexes for the current menu depht
	first_index_depth=current_menu_index;
    last_index_depth=current_menu_index;
   	GetFirstMenuIndex(); 
	GetLastMenuIndex(); 
 
	// lock lcd updates from controls
	Z8_STATUS_4 |= lcd_locked_by_function_z4;

	// Show menus
	DisplayMenus();
}


///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: DisplayMenus()
// Inputs	: None
// Outputs	: None
// Process  : display current menu index name according to menu depth & index
///////////////////////////////////////////////////////////////////////////////////////
void DisplayMenus(void)
{
	uchar index_local;
	uchar offset_y;
	uchar lcd_display_type;

	// Now display all available menus for the current index in the current depth
	index_local = first_index_depth;

	// Special case for manual assignments : display module name + index
	if( (current_menu_index >= MENUS_ASSIGN_FIRST_INDEX) && (current_menu_index <= LCD_STRING_DUMMY_INDEX_ASSIGN))
	{
		FillLcdZone(59, 0, 2, 1, FILL_BLANK);
		for(offset_y=0; offset_y<MODULE_NAME_LENGHT; offset_y++)
			lcd_text_value[offset_y] = module[module_assign.current_module_assign].module_name[offset_y];

		lcd_text_value[MODULE_NAME_LENGHT] = '\0';
		DisplayLcdText(60, 0, LCD_INVERT, font_5x8);
		FillLcdZone(108, 0, 2, 1, FILL_BLANK);
	}
	else
	{
		// Clear current module name assign
		FillLcdZone(59, 0, 66, 1, 0xFD);
	}

	offset_y = 0;

	// while(menutable[index_local].menu_depth == current_menu_depth)
	while(index_local <= last_index_depth)
	{
		// Clear current line before displaying new one
		FillLcdZone(LCD_ZONE_MENUS_BASE_X, LCD_ZONE_MENUS_BASE_Y+offset_y, LCD_X_PIXEL_MAX-LCD_ZONE_MENUS_BASE_X-10, 1, FILL_BLANK);
		// init to normal display	
		lcd_display_type = LCD_INVERT;
		// if current menu then highlight line
		if(index_local == current_menu_index)
		{
			lcd_display_type = LCD_NORMAL;
		}
		// Display the current menu
		rom_to_lcd_text_copy(menutable[index_local].MenuStr);
		DisplayLcdText(LCD_ZONE_MENUS_BASE_X, LCD_ZONE_MENUS_BASE_Y+offset_y, lcd_display_type, font_5x8);	
		// add a new line of menus for the same depth
		offset_y++;
		// pass to next menu
		index_local++;
	}

	// Also remove old menus lines
	index_local = LCD_Y_PAGE_MAX - (LCD_ZONE_MENUS_BASE_Y + offset_y) - 1;
	FillLcdZone(LCD_ZONE_MENUS_BASE_X, LCD_ZONE_MENUS_BASE_Y+offset_y, LCD_X_PIXEL_MAX-LCD_ZONE_MENUS_BASE_X-10, index_local, FILL_BLANK);
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: DisplayMenuValue()
// Inputs	: None
// Outputs	: None
// Process  : Display on LCD the current menu values
///////////////////////////////////////////////////////////////////////////////////////
void DisplayMenuValue(void)
{
        uchar menu_value, tmp, nb_values, tmp2, lcd_display_type;
		rom uchar *local_ptr;
		far uchar *local_far_ptr;

		// remove old menu value
		FillLcdZone(LCD_ZONE_MENUS_BASE_X, LCD_ZONE_MENUS_BASE_Y, LCD_X_PIXEL_MAX-LCD_ZONE_MENUS_BASE_X-10, MENU_MAX_VALUES, FILL_BLANK);

        // Is it a menu with affected value or submenus below this menu ?
        if(menutable[current_menu_index].SubMenuBaseIndex == UNDEFINED_VALUE)
        {
			// Get number of menu values 
			nb_values = menutable[current_menu_index].value_max;
			
			// List all possible values for the current menu
       		if(menutable[current_menu_index].display_type == DISP_STRING)
            {
					// List all possible values for the current menu
		  			for(tmp2 = 0; tmp2 < nb_values; tmp2++)
					{
		   				// Retriewe the string (read fat)
	   					local_ptr = menutable[current_menu_index].MenuValueStr;
						tmp = (uchar) (local_ptr[tmp2]);
						rom_to_lcd_text_copy(local_ptr + tmp);
						lcd_display_type = LCD_INVERT;
						// if current menu then highlight line
						if(tmp2 == next_value)
							lcd_display_type = LCD_NORMAL;
					
						DisplayLcdText(LCD_ZONE_MENUS_BASE_X, LCD_ZONE_MENUS_BASE_Y+tmp2, lcd_display_type, font_5x8);
					}
			}
			else
      		if(menutable[current_menu_index].display_type == DISP_EDIT_STRING)
            {
				if(menutable[current_menu_index].table_id == TABLE_MODULE_NAME)
					local_far_ptr = &module_name_assign[0];
				else
				if(menutable[current_menu_index].table_id == TABLE_PRESET_NAME)
					local_far_ptr = &preset_name_assign[0];
				else
				if(menutable[current_menu_index].table_id == TABLE_CTRL_ASSIGN)
					local_far_ptr = &Param[0].lcd_string[0];

				tmp = 0;		
				for(tmp2 = 0; tmp2 < menutable[current_menu_index].value_max; tmp2++)
				{	
					lcd_text_value[0] = local_far_ptr[tmp2];
					lcd_text_value[1] = '\0';
					lcd_display_type = LCD_INVERT;
					if(tmp2 == edit_string_idx)
						lcd_display_type = LCD_NORMAL;
					DisplayLcdText(LCD_ZONE_MENUS_BASE_X+tmp, LCD_ZONE_MENUS_BASE_Y, lcd_display_type, font_5x8);
					tmp += 6;
				}
			}
            else
			{	// DISP_NUMERIC or DISP_NUMERIC_PLUS_1 or DISP_NUMERIC_PLUS_30
				menu_value = next_value;
            	if(menutable[current_menu_index].display_type == DISP_NUMERIC_PLUS_1)
					menu_value++;
				else
            	if(menutable[current_menu_index].display_type == DISP_NUMERIC_PLUS_30)
					menu_value += 30;
     			
				DisplayLcdInt(LCD_ZONE_MENUS_BASE_X, LCD_ZONE_MENUS_BASE_Y, DIGITS_3|LCD_INVERT, font_5x8, menu_value);
			}
		}
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: GetBitPositionFromMask()
// Inputs	: READ or WRITE reg, plus the bits mask
// Outputs	: None
// Process  : Enter into menus mode
///////////////////////////////////////////////////////////////////////////////////////
uchar GetBitPositionFromMask(uchar bits_mask, uchar action)
{
        uchar pos = 0;
        uchar msk = 1;
        while(!(bits_mask & msk))
        {
                msk = msk<<1;
                pos++;
        }
        // if action is to read the reg value, then return position of the bit
        if(action == READ_REG_VALUE)
           return pos;

        // if action is to write the reg value, then return single bit toogle info
        // or the bit position if multi bit mask to update
        // Check if it was a single bit mask
        msk = msk << 1;
        // if next msk is 0, then single bit to toggle
        if(bits_mask & msk)
           return pos;
        else
           return MUST_TOGGLE_SINGLE_BIT;
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: MenuGetValue()
// Inputs	: None
// Outputs	: None
// Process  : Return the current menu associated value
///////////////////////////////////////////////////////////////////////////////////////
uchar MenuGetValue(void)
{
        uchar value;
        uchar bit_pos;

        if(menutable[current_menu_index].table_id == TABLE_GLOBAL_PARAMS)
        {
            // A full reg must be read
            if(menutable[current_menu_index].bit_pos_msk == NO_BIT)
                    value = CtrlReg[menutable[current_menu_index].value_index];
            else
            {
                    // A Bit Masked reg must be read
                    bit_pos = GetBitPositionFromMask(menutable[current_menu_index].bit_pos_msk, READ_REG_VALUE);
                    value   = ((CtrlReg[menutable[current_menu_index].value_index]) & (menutable[current_menu_index].bit_pos_msk)) >> bit_pos;
            }
        }
		else
        if(menutable[current_menu_index].table_id == TABLE_CTRL_ASSIGN)
        {
            if(menutable[current_menu_index].display_type ==  DISP_EDIT_STRING)
			{
				value = *((far uchar *)(&Param[0].message_type + (uint)(menutable[current_menu_index].value_index + edit_string_idx)));	
			}
			else
			// A full reg must be read
            if(menutable[current_menu_index].bit_pos_msk == NO_BIT)
                    value = *((far uchar *)(&Param[0].message_type + (uint)(menutable[current_menu_index].value_index)));
            else
            {
                    // A Bit Masked reg must be read
                    bit_pos = GetBitPositionFromMask(menutable[current_menu_index].bit_pos_msk, READ_REG_VALUE);
                    value   = (*(far uchar *)(&Param[0].message_type + (uint)(menutable[current_menu_index].value_index)) & (menutable[current_menu_index].bit_pos_msk)) >> bit_pos;
            }
		}	
		else
        if(menutable[current_menu_index].table_id == TABLE_MODULE_NAME)
        {
            // value = menutable[current_menu_index].value_index;
			value = module_name_assign[edit_string_idx];
		}	
		else
        if(menutable[current_menu_index].table_id == TABLE_PRESET_NAME)
        {
            // value = menutable[current_menu_index].value_index;
			value = preset_name_assign[edit_string_idx];
		}	

		return value;
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: MenuSetValue()
// Inputs	: None
// Outputs	: None
// Process  : Set the current selected value for the current menu
///////////////////////////////////////////////////////////////////////////////////////
void MenuSetValue(uchar next_value)
{
        uchar value;
        uchar bit_pos;

		//////////////////////////////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////// MENUS VARIAVBLES ////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////////////////////////////
        if(menutable[current_menu_index].table_id == TABLE_GLOBAL_PARAMS)
        {
                // A full reg must be written
                if(menutable[current_menu_index].bit_pos_msk == NO_BIT)
                        CtrlReg[menutable[current_menu_index].value_index] = next_value;
                else
                {
                        // A Bit Masked reg must be written
                        bit_pos = GetBitPositionFromMask(menutable[current_menu_index].bit_pos_msk, WRITE_REG_VALUE);
                        // Must toggle single bit ?
                        if(bit_pos == MUST_TOGGLE_SINGLE_BIT)
						{
                          // CtrlReg[menutable[current_menu_index].value_index] ^= menutable[current_menu_index].bit_pos_msk;
						  if(next_value)
							  CtrlReg[menutable[current_menu_index].value_index] |=  menutable[current_menu_index].bit_pos_msk;
						  else
							  CtrlReg[menutable[current_menu_index].value_index] &=  ~menutable[current_menu_index].bit_pos_msk;	
                        }
						else
                        {
                                // Must update a Multi bit masked value
                                CtrlReg[menutable[current_menu_index].value_index] &= (~menutable[current_menu_index].bit_pos_msk);
                                CtrlReg[menutable[current_menu_index].value_index] |= (next_value << bit_pos);
                        }
                }
				//////////////////////////////////////////////////////////////
				// specific actions to perform when a menu variable is changed
				//////////////////////////////////////////////////////////////
				if((current_menu_index == MTC_MODE_INDEX_MENU) || (current_menu_index == SMPTE_FRAME_RATE_INDEX_MENU))
					// reload timer 2 for tempo/smpte update
					ReinitTimer2();
				else
				if(current_menu_index == MODULE_AUTOASSIGN_INDEX_MENU)
				{
					// if autoassign confirmed : exit menus
					if(Z8_STATUS_1_NVM & must_autoassign_z1nvm)
					{
						ExitMenusMode();
						return;
					}
				}
       }
		else
        if(menutable[current_menu_index].table_id == TABLE_MODULE_NAME)
        {
			module_name_assign[edit_string_idx] = next_value;
		}	
		else
        if(menutable[current_menu_index].table_id == TABLE_PRESET_NAME)
        {
			preset_name_assign[edit_string_idx] = next_value;
		}	
        // Insert here other tables to modify
		else
		//////////////////////////////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////// CONTROL ASSIGN //////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////////////////////////////
        if(menutable[current_menu_index].table_id == TABLE_CTRL_ASSIGN)
        {
           if(menutable[current_menu_index].display_type ==  DISP_EDIT_STRING)
			{
				*((far uchar *)(&Param[0].message_type + (uint)(menutable[current_menu_index].value_index + edit_string_idx))) = next_value; 	
			}
			else            
			// A full reg must be written
            if(menutable[current_menu_index].bit_pos_msk == NO_BIT)
                    *((far uchar *)(&Param[0].message_type + (uint)(menutable[current_menu_index].value_index))) = next_value;
            else
            {
                    // A Bit Masked reg must be written
                    bit_pos = GetBitPositionFromMask(menutable[current_menu_index].bit_pos_msk, WRITE_REG_VALUE);
                    // Must toggle single bit ?
                    if(bit_pos == MUST_TOGGLE_SINGLE_BIT)
                      *((far uchar *)(&Param[0].message_type + (uint)(menutable[current_menu_index].value_index))) ^= menutable[current_menu_index].bit_pos_msk;
                    else
                    {
                            // Must update a Multi bit masked value
                            *((far uchar *)(&Param[0].message_type + (uint)(menutable[current_menu_index].value_index))) &= (~menutable[current_menu_index].bit_pos_msk);
                            *((far uchar *)(&Param[0].message_type + (uint)(menutable[current_menu_index].value_index))) |= (next_value << bit_pos);
                    }
            }

			//////////////////////////////////////////////////////////////
			// specific actions to perform when a menu variable is changed
			//////////////////////////////////////////////////////////////
			if( (current_menu_index == EVENT_TYPE_INDEX_ASSIGN) ||
				(current_menu_index == EVENT_NB_INDEX_ASSIGN)	||
				(current_menu_index == MIDI_CHANNEL_INDEX_ASSIGN))
			{
				// The MIDI event type or nb or channel has been updated : also update midi structure
				if(Param[0].message_type == CC)
				{
					Param[0].midi_str_lenght		= 0x03;
					Param[0].midi_channel_position	= 0x01;
					Param[0].ctrl_value_position_0	= 0x03;
					Param[0].midi_string[0] 		= MIDI_CONTROL_CHANGE;
					Param[0].midi_string[1] 		= Param[0].message_nb;
				}
				else
				if(Param[0].message_type == NOTE)
				{
					Param[0].midi_str_lenght		= 0x03;
					Param[0].midi_channel_position	= 0x01;
					Param[0].ctrl_value_position_0	= 0x03;
					Param[0].midi_string[0] 		= MIDI_NOTE_ON;
					Param[0].midi_string[1] 		= Param[0].message_nb;
				}
			}
        }
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: ButtonCancelClick()
// Inputs	: None
// Outputs	: None
// Process  : Cancel key has been pressed
///////////////////////////////////////////////////////////////////////////////////////
void ButtonCancelClick(void)
{
	// if waiting for assign : cancel
	if(assign_state == ASSIGN_WAIT_TWEAK_CTRL)
	{
		// no more in assign mode
		assign_state	= ASSIGN_OFF;
		ExitMenusMode();
		return;
	}

	// exit menus if current depth is 0
	if(current_menu_depth==TOP_LEVEL_MENU)
	{
		ExitMenusMode();
		return;
	}

	// Here we are in a menu mode
	if( (up_down_action == CHANGE_MENU_VALUE) && (menutable[current_menu_index].display_type != DISP_EDIT_STRING))
	{
		// here we were changing menu value, need to cancel only
		up_down_action = CHANGE_MENU_INDEX;

	}
	else
	// here we were navigating into menus, need to decrease menu depth
	if(current_menu_depth)
	{
		current_menu_depth--;
		current_menu_index = menu_index_call_stack[--menu_stack_index];
	}

	// retrieve first and last indexes in this menu depth
	GetFirstMenuIndex(); 
	GetLastMenuIndex(); 
	DisplayMenus();
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: ButtonOkClick()
// Inputs	: None
// Outputs	: None
// Process  : OK key has been pressed
///////////////////////////////////////////////////////////////////////////////////////
void ButtonOkClick(void)
{

	if(menutable[current_menu_index].display_type == DISP_EDIT_STRING)
	{
		if(up_down_action == CHANGE_MENU_VALUE)
			up_down_action = CHANGE_MENU_INDEX;
		else
		{
			up_down_action = CHANGE_MENU_VALUE;
			next_value = MenuGetValue();
			// if we are assigning : the struct or module name has been modified
			if(assign_state == ASSIGN_ACTIVE)
				Z8_STATUS_7 |= must_save_assign_struct_z7;
		}

		DisplayMenuValue();
		return;
	}

    // ok pressed, save new value if we were updating values
    if(up_down_action == CHANGE_MENU_VALUE)
    {
       	MenuSetValue(next_value);
		// reload update value since may be different
	    next_value = MenuGetValue();
        // automatically return to last menu depth
        up_down_action = CHANGE_MENU_INDEX;
		// Now display all available menus
		DisplayMenus();
		// if we are assigning : the struct or module name has been modified
		if(assign_state == ASSIGN_ACTIVE)
			Z8_STATUS_7 |= must_save_assign_struct_z7;
       	return;
    }

	// Special case for preset name assign
	if(current_menu_index == PRESET_NAME_INDEX_MENU)
	{
		// calculate group address in eeprom to load preset name into ram
		I2cCommand	= I2C_SLAVE_ADR_EE + EEPROM_ZERO_I2C_ADR;
		EE_Address = (uint)(GROUP_NAME_BASE_EE_ADR + (uint)((uint)(current_group) * GROUP_NAME_EE_LENGHT));
		// load current preset name
		ReadMultiByteI2c((far uchar *)&preset_name_assign[0], GROUP_NAME_REAL_SIZE);
		// be sure to start string edition with index on string
		up_down_action = CHANGE_MENU_VALUE;
		// be sure to save updated preset name after edition
		Z8_STATUS_7 |= must_save_preset_name_z7;

	}

	// Special case for manual assignments
	if(current_menu_index == MODULE_NAME_INDEX_MENU)
	{
		// exit from menu mode
		ExitMenusMode();
		// be sure to NOT reinit the lcd (flag set in above fucntion)
		Z8_STATUS_5 &= ~must_reinit_all_lcd_z5;
		// flag to assign module structure
		Z8_STATUS_7 &= ~assign_type_z7;
		// Start assign module structure
		AssignEnter();
		// be sure to start string edition with index on string
		up_down_action = CHANGE_MENU_VALUE;
		// exit from menus
		return;
	}
	
	// Special case for manual assignments
	if(current_menu_index == MODULE_ASSIGN_INDEX_MENU)
	{
		// exit from menu mode
		ExitMenusMode();
		// be sure to NOT reinit the lcd (flag set in above fucntion)
		Z8_STATUS_5 &= ~must_reinit_all_lcd_z5;
		// flag to assign module structure
		Z8_STATUS_7 |= assign_type_z7;
		// Start assign module structure
		AssignEnter();
		// exit from menus
		return;
	}

    // is there an associated sub menu : go into next submenu ?
    if(menutable[current_menu_index].SubMenuBaseIndex != UNDEFINED_VALUE)
    {    
		// go to next menu depth level
        current_menu_depth++;
        // push caller current_index to stack
        menu_index_call_stack[menu_stack_index++] = current_menu_index;
        // get sub menu base index from struct
        current_menu_index = menutable[current_menu_index].SubMenuBaseIndex;
		// retrieve first and last indexes in this menu depth
    	GetFirstMenuIndex(); 
  		GetLastMenuIndex(); 
        // up & down switches are still changing menus index
        up_down_action = CHANGE_MENU_INDEX;
		// Now display all available menus
        DisplayMenus();

		if(menutable[current_menu_index].display_type == DISP_EDIT_STRING)
		{
			// be sure next edit string will start with index change instead of value change
			up_down_action  = CHANGE_MENU_VALUE;
			// reset index of string char to 0
			edit_string_idx = 0;
		}
    }
    // else go into value change field
    else
    {
            // now up & down switches are changing menus value
            up_down_action = CHANGE_MENU_VALUE;
            next_value = MenuGetValue();
			DisplayMenuValue();
    }

}


///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: ChangeCurrentMenu()
// Inputs	: None
// Outputs	: None
// Process  : up or down switch is pressed : change menu value or menu index
///////////////////////////////////////////////////////////////////////////////////////
void ChangeCurrentMenu(uchar direction)
{
      // need to change current menu or current menu value ?
      if(up_down_action == CHANGE_MENU_INDEX)
      {
			if(menutable[current_menu_index].display_type == DISP_EDIT_STRING)
			{
				if(direction==DOWN_PRESSED)
				{
					if(edit_string_idx)
						edit_string_idx--;
				}
				else
				{
					if(edit_string_idx < menutable[current_menu_index].value_max - 1)
						edit_string_idx++;
				}

				DisplayMenuValue();
			}
			else
			{
	        	GetFirstMenuIndex(); 
		  		GetLastMenuIndex(); 
	
		        if(direction==DOWN_PRESSED)
		        {
		                 // get index UP on same menu depht
		                 if(current_menu_index > first_index_depth)
		                        current_menu_index--;
		        }
		        else
		        {
		               // get index DOWN on same menu depht
		              if(current_menu_index < last_index_depth)
		                 current_menu_index++;
		        }
		        // Now display menu value
		        DisplayMenus();
	   		}
     }
	 else
	// need to change current menu or current menu value ?
	if(up_down_action == CHANGE_MENU_VALUE)
	{
		if(direction==UP_PRESSED)
		{
				if((next_value < (menutable[current_menu_index].value_max)-1) || (menutable[current_menu_index].display_type == DISP_EDIT_STRING))
					next_value++;
		}
		else
		{
		        if(next_value)
		             next_value--;
		}

		if(menutable[current_menu_index].display_type == DISP_EDIT_STRING)
		{
			if(next_value < ' ') next_value = ' ';
			else
			if(next_value > '~') next_value = '~';

			MenuSetValue(next_value);
		}
		
		DisplayMenuValue();
	}
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: ExitMenusMode()
// Inputs	: None
// Outputs	: None
// Process  : 
///////////////////////////////////////////////////////////////////////////////////////
void ExitMenusMode(void)
{
	uchar tmp0;
	uchar current_module_saved;

	// need to save preset name before exiting ?
	if(Z8_STATUS_7 & must_save_preset_name_z7)
	{
		preset_name_assign[GROUP_NAME_REAL_SIZE] = '\0';
		// preset names are stored in eeprom 0
		I2cCommand	= I2C_SLAVE_ADR_EE + EEPROM_ZERO_I2C_ADR;
		// calculate group name address		
		EE_Address  = (uint)(GROUP_NAME_BASE_EE_ADR + (uint)((uint)GROUP_NAME_EE_LENGHT * (uint)current_group));
		// write group name to eeprom
		WriteMultiByteI2c((far uchar *)&preset_name_assign[0], GROUP_NAME_REAL_SIZE);
		// unflag 
		Z8_STATUS_7 &= ~must_save_preset_name_z7;
	}

	// need to save struct before exiting ?
	if(assign_state == ASSIGN_ACTIVE)
	{
		if(Z8_STATUS_7 & must_save_assign_struct_z7)
		{
			// restore current control index
			control_index = module_assign.control_assign_index;
			// be sure to reload structures
			last_control_index = CTRL_UNDEF;
			ParamCurrent = &Param[0];
			// be sure to program the correct module
			current_module_saved = current_module;
			current_module = module_assign.current_module_assign;
			// must assign module name or control structure ?
			if(Z8_STATUS_7 & assign_type_z7)
			{
				// save full structure
				GetCtrlStructEEaddFromIndexGroup();				
				WriteMultiByteI2c((far uchar *)&Param[0].message_type, CTRL_STRUCT_TOTAL_SIZE);
				// Write also this new structure into the module
				SendFullStructureToModule(module_assign.control_num_in_module_assign);
			}
			else
			{
				module_name_assign[MODULE_NAME_LENGHT] = '\0';
				// Prepare generic buffer to send to the current module
				generic_buffer[0] = EEPROM_MOD; 				
				generic_buffer[1] = (MODULE_NAME_EE_ADR >> 7) & 0x7F ; 	
				generic_buffer[2] =  MODULE_NAME_EE_ADR 	  & 0x7F; 	
				generic_buffer[3] = MODULE_NAME_LENGHT; 		

				for(tmp0=0; tmp0<MODULE_NAME_LENGHT ;tmp0++)
				{
					module[module_assign.current_module_assign].module_name[tmp0] = module_name_assign[tmp0];
					generic_buffer[4+tmp0] = module_name_assign[tmp0];
				}

				// Send this module name to the current module
				SendCommandToModule(ONEWIRE_CMD_WRITE_REGISTER, 
									ONEWIRE_HEADER_CMD_RW_REGISTER_SIZE + MODULE_NAME_LENGHT, 
									ONEWIRE_ID_SINGLE_MODULE, &generic_buffer[0]);

			}
			// restore current module
			current_module = current_module_saved;
			// reinit all param and lcd structs already preloaded in RAM
			InitParamStructsInRam();
		}

		// no more in assign mode
		assign_state	= ASSIGN_OFF;
	} // end assign active

	// no more in menu mode
	menu_state 		= NO_MENU;
	// will reinit full lcd
	Z8_STATUS_5 |= must_reinit_all_lcd_z5;
	// return to top level depth
	current_menu_depth = TOP_LEVEL_MENU;
	// retrun to first index
	current_menu_index = 0; 
    // up & down switches are still changing menus index
    up_down_action = CHANGE_MENU_INDEX;
	// empty the stack
	menu_stack_index = 0;
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: GetFirstMenuIndex()
// Inputs	: None
// Outputs	: None
// Process  : 
///////////////////////////////////////////////////////////////////////////////////////
void GetFirstMenuIndex(void)
{
	uchar current_menu_index_saved = current_menu_index;
	first_index_depth = current_menu_index;

	// retriewe first index on the current menu depth
    while(current_menu_index)
    {
       if(menutable[--current_menu_index].NextMenuExist)
               first_index_depth = current_menu_index;
       else break;
    }

    // restore current menu index
    current_menu_index = current_menu_index_saved;
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: GetLastMenuIndex()
// Inputs	: None
// Outputs	: None
// Process  : 
///////////////////////////////////////////////////////////////////////////////////////
void GetLastMenuIndex(void)
{
	 uchar current_menu_index_saved = current_menu_index;
	 last_index_depth = current_menu_index;

     // retriewe last index on the current menu depth
     while(menutable[current_menu_index].NextMenuExist)
     {
            last_index_depth++;
            current_menu_index++;
     }

     // restore current menu index
     current_menu_index = current_menu_index_saved;
}
