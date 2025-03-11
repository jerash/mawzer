#include <eZ8.h>
#include "pins_defs.h"
#include "commondef.h"
#include <common_z8_ez.h>
#include <ctrldef.h>
#include "onewire.h"
#include "lcd_low_level.h"
#include "lcd.h"
#include "serial.h"
#include "hardware.h"
#include "eeprom.h"
#include "i2c.h"
#include "autotest.h"
#include "assign.h"
#include "realtime.h"

///////////////////////////////////////////////////////////////////////////////////////
// GLOBAL VARS
///////////////////////////////////////////////////////////////////////////////////////

far All_Modules module[MODULES_MAX_NB];
far uchar onewire_rx_buffer[ONEWIRE_RX_BUFF_LENGHT];		// one wire serial in buffer
uchar nb_connected_modules;
// uchar timeout_answer_module_cpt;
uchar idx_id;
uchar idx_sub_id;
uchar current_module;
uchar current_onewire_bus;

far uchar total_controls_nb;

uchar onewire_rx_start_index;						// a near pointer on FAR ram
uchar onewire_rx_end_index;							// a near pointer on FAR ram
uchar onewire_rx_current_index;

//#pragma noopt

rom uchar CTRL_NB_PER_MODULE[/*MODULES_TYPE_MAX_NB*/] = {
														4U, 	// BUT module features 4 controls
														4U,  // POT module features 4 controls
														4U, 	// ENC module features 4 controls	
														3U,  // XFD module features 3 controls
														1U	// FAD module features 1 control
													};

rom uchar HOOK_INFOS_NB_PER_MODULE[/*MODULES_TYPE_MAX_NB*/] = {
														0U, 	// BUT module features 0 hook byte
														4U,  // POT module features 2 hook bytes
														0U, 	// ENC module features 0 hook byte
														1U,  // XFD module features 3 hook byte
														1U	// FAD module features 1 hook byte
													};

//#pragma optsize

extern volatile uchar Z8_STATUS_4;
extern volatile uchar Z8_STATUS_5;
extern volatile uchar Z8_STATUS_6;
extern far uchar generic_buffer[GENERIC_BUFF_SIZE];	// a generic far ram buffer of 128 bytes
extern uchar current_value;
extern far uchar lcd_text_value[MAX_LCD_TEXT_LENGHT];	// global buffer for integer to text conversion
extern rom uchar font_5x8[];	
extern rom uchar searching_txt[];
extern rom uchar updating_txt[];
extern rom uchar modules_lc_txt[];
extern rom uchar modules_txt[];
extern rom uchar found_txt[];
//extern rom uchar bmp_hook_down[];
//extern rom uchar bmp_hook_up[];
//extern rom uchar bmp_no_hook[];
extern far uchar CtrlReg[CTRL_REG_REAL_NB];			// table of control reg for eerprom saving and menus 
extern uchar control_index;
extern uchar last_control_index;
extern uint  EE_Address;							// current EEProm address
//extern uchar control_assign_index;
extern uchar assign_state;
//extern uchar control_num_in_module_assign;
extern far Assign module_assign;

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: InitModules()
// Inputs	: None
// Outputs	: None
// Process  : 
///////////////////////////////////////////////////////////////////////////////////////
void InitModules(void)
{
	uchar i, idx;

	nb_connected_modules 		= 0;
//	timeout_answer_module_cpt	= 0;
	current_module				= 0;
	current_onewire_bus			= 0;
	total_controls_nb			= 0;
	// Initialize serial In indexes
	onewire_rx_start_index 		= 0;
	onewire_rx_end_index		= 0;
	onewire_rx_current_index	= 0;

	for(i=0; i<MODULES_MAX_NB ;i++)
	{
		module[i].id 				= MODULE_UNDEF;
		module[i].sub_id 			= MODULE_UNDEF;
		module[i].bus				= MODULE_UNDEF;
		module[i].controlindexbase	= MODULE_UNDEF;
		for(idx=0; idx<MODULE_TYPE_LENGHT ; idx++)
			module[i].module_type[idx] = '-';
		for(idx=0; idx<MODULE_NAME_LENGHT ; idx++)
			module[i].module_name[idx] = '-';

	}
}

//#pragma noopt

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: SearchModules()
// Inputs	: None
// Outputs	: None
// Process  : 
///////////////////////////////////////////////////////////////////////////////////////
void SearchModules(void)
{
	uchar nb_module_per_bus;
	uchar step_bargraph = 0;
	uchar current_bargraph_pos = 0;
	uchar tmp;

	// indicates we are currently searching for modules
	Z8_STATUS_4 |= searching_modules_z4;

	// remove low part of the home screen and display searching infos
	FillLcdZone(LCD_ZONE_SEARCH_MOD_X, LCD_ZONE_SEARCH_MOD_Y-1, LCD_X_PIXEL_MAX, 3, FILL_BLANK);

	// display searching module text
	rom_to_lcd_text_copy(searching_txt);
	DisplayLcdText(10, 4, LCD_INVERT, font_5x8);
	rom_to_lcd_text_copy(modules_lc_txt);
	DisplayLcdText(70, 4, LCD_INVERT, font_5x8);

	control_index = 0;

	// Scan all 8 one wire buses
	for(current_onewire_bus=0; current_onewire_bus<ONEWIRE_BUS_MAX_NB ;current_onewire_bus++)
	{
		// Max nb of modules reached : stop searching
		if(nb_connected_modules == MODULES_MAX_NB) break;

		// Select the MUXES according to the current one wire bus
		SelectOneWireBusMuxes();

		// Initialize nb of module per bus
		nb_module_per_bus = 0;

		// For all possible ids
		for(idx_id=0; idx_id<MODULES_TYPE_MAX_NB ;idx_id++)
		{
			// Max nb of modules reached : stop searching
			if(nb_connected_modules == MODULES_MAX_NB) break;
			// Max nb of modules reached in the current bus : stop searching on this bus ?
			if(nb_module_per_bus == MODULES_MAX_NB_PER_BUS) break;

			// for all possible sub_ids
			for(idx_sub_id=0; idx_sub_id<MODULES_SAME_MAX_NB ;idx_sub_id++)
			{
				// reset module found flag;
				Z8_STATUS_4 &= ~module_answered_z4;

				// Wait for an answer within the 50ms
				StartTimerOneWire(MODULE_SEARCH_TIMEOUT);
	
				// Send a request for identification to the concerned module
				SendCommandToModule(ONEWIRE_CMD_REQUEST_INFO, 0, ONEWIRE_ID_SINGLE_MODULE, NULL);
	
				// Wait the answer of the connected module
				do { 
						CheckOneWireRxBuffer();
						if(Z8_STATUS_4 & module_answered_z4) 
							break;
						if(!(Z8_STATUS_5 & waiting_timeout_onewire_z5)) 
							break;
				} while(1);
			
				// update progression bar
				if(++step_bargraph == ONEWIRE_STEP_SEARCH)
				{
					step_bargraph = 0;
					current_bargraph_pos++;
					FillLcdZone(0, 5, current_bargraph_pos, 1, 0xF3);
				}

				// was the module found ? flag is updated in CheckOneWireRxBuffer()
				if(Z8_STATUS_4 & module_answered_z4)
				{
					// assign the nb of control in the module to a local register
					tmp = CTRL_NB_PER_MODULE[idx_id];
					// Populate the module structure with the info from module found
					// module[nb_connected_modules].id					= idx_id;
					// module[nb_connected_modules].sub_id				= idx_sub_id;
					// module[nb_connected_modules].bus				= current_onewire_bus;
					// module[nb_connected_modules].controlindexbase	= control_index;
					// update control index base
					control_index = (control_index + tmp) & 0x7F;
					// Increase nb of modules found
					nb_connected_modules++;
					// Increase total nb of controls found
					total_controls_nb += tmp;
					// Max nb of modules reached : stop searching
					if(nb_connected_modules == MODULES_MAX_NB) break;
					// Increase nb of modules found	on the current bus scanned
					nb_module_per_bus++;
					// Max nb of modules reached in the current bus : stop searching on this bus ?
					if(nb_module_per_bus == MODULES_MAX_NB_PER_BUS) break;
				}
			} // end all sub ids
		} // end all ids
	} // end all one wire buses
	
	// indicates we are no more searching for modules
	Z8_STATUS_4 &= ~searching_modules_z4;
	// reset module found flag;
	Z8_STATUS_4 &= ~module_answered_z4;
	// reset timeout
	Z8_STATUS_5 &= ~waiting_timeout_onewire_z5;
	// reset current module
	current_module = 0;
	// Delete the line where modules infos where displayed
	FillLcdZone(LCD_ZONE_SEARCH_MOD_X, LCD_ZONE_SEARCH_MOD_Y, LCD_X_PIXEL_MAX-LCD_ZONE_SEARCH_MOD_X-1, 1, FILL_BLANK);
	// Display nb of modules found
	DisplayLcdInt(LCD_ZONE_SEARCH_MOD_X+10, LCD_ZONE_SEARCH_MOD_Y, DIGITS_2|LCD_INVERT, font_5x8, nb_connected_modules);
	// Display modules found texts
	rom_to_lcd_text_copy(modules_txt);
	DisplayLcdText(LCD_ZONE_SEARCH_MOD_X+27, LCD_ZONE_SEARCH_MOD_Y, LCD_INVERT, font_5x8);
	rom_to_lcd_text_copy(found_txt);
	DisplayLcdText(LCD_ZONE_SEARCH_MOD_X+75, LCD_ZONE_SEARCH_MOD_Y, LCD_INVERT, font_5x8);

//	UpdateLcdZones(INIT_LCD_LIVE_SCR_BOT_FIXED);

}

//#pragma optsize

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: ScanCurrentModule()
// Inputs	: None
// Outputs	: None
// Process  : 
///////////////////////////////////////////////////////////////////////////////////////
void ScanCurrentModule(void)
{
	// Exit if no module is connected
	if(!nb_connected_modules) return;

	// Treat incoming module data since the module has answered
	CheckOneWireRxBuffer();

	// here the module has answered
	if(Z8_STATUS_4 & module_answered_z4)
	{
		// pass to next module
		current_module++;
		if(current_module >= nb_connected_modules)
			current_module = 0;
	}
	else
	{
		// the module has not answered yet, are we still waiting for timeout ?
		if(Z8_STATUS_5 & waiting_timeout_onewire_z5)
			return;
		else
		{
			// Here we can display the module which was connected is now lost
			// TO COMPLETE
				
			// pass to next module if current module does not answer
			current_module++;
			if(current_module >= nb_connected_modules)
				current_module = 0;
		}
	}

	// retriewe the onewire bus where the current module is attached
	current_onewire_bus = module[current_module].bus;
	idx_id 				= module[current_module].id;
	idx_sub_id			= module[current_module].sub_id;

	// Wait for an answer within the 20ms
	// timeout_answer_module_cpt = MODULE_REQ_CHANGE_TIMEOUT;
	StartTimerOneWire(MODULE_REQ_CHANGE_TIMEOUT);
	// reset module found flag;
	Z8_STATUS_4 &= ~module_answered_z4;

	// Send a module request if change command (a new value has changed on current module ?)
	SendCommandToModule(ONEWIRE_CMD_GET_VALUES, 0, ONEWIRE_ID_SINGLE_MODULE, NULL);

}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	CheckOneWireRxBuffer()
// Inputs	: 	None
// Outputs	: 	None
// Process  : 	A module has sent its new values, get them from RX buffer and send to midi
// 				If one value (at least) has changed in the module :
// 				SOF_ONEWIRE, ID, SUB_ID, COMMAND, VALUE_CTRL_0, VALUE_CTRL_1, VALUE_CTRL_2, VALUE_CTRL_3, HOOK_STATE_1, HOOK_STATE_2, EOF_ONEWIRE
//				If no value has changed in the module :
// 				SOF_ONEWIRE, ID, SUB_ID, COMMAND, ONEWIRE_NO_NEW_DATA, EOF_ONEWIRE

///////////////////////////////////////////////////////////////////////////////////////
void CheckOneWireRxBuffer(void)
{
	uchar idx, local_command, direction/*, module_concerned*/;
	uchar local_end_index_minus_one = (onewire_rx_end_index /*- 1*/)&(ONEWIRE_RX_BUFF_LENGHT-1);
	
	// Detect if there is a valid frame into the buffer
	while(onewire_rx_start_index != local_end_index_minus_one)
	{ 
		// Check valid frame is present (if SOF and EOF are presents)
		if( (onewire_rx_buffer[onewire_rx_start_index] 	     == SOF_ONEWIRE) &&
		    (onewire_rx_buffer[(local_end_index_minus_one-1)&(ONEWIRE_RX_BUFF_LENGHT-1)] == EOF_ONEWIRE) )
		{
			// get command + direction from buffer
			local_command = onewire_rx_buffer[(onewire_rx_start_index + ONEWIRE_CMD_FRAME_POS)&(ONEWIRE_RX_BUFF_LENGHT-1)];
			// get direction
			direction = (local_command & ONEWIRE_CMD_DIRECTION_BIT);
			// get command without direction info
			local_command &= ~ONEWIRE_CMD_DIRECTION_BIT;

			////////////////////////////////////////////////////////////////////////////////////////////////////////
			/////////////////// IS IT A ONEWIRE_CMD_REQUEST_INFO ANSWER FROM MODULES /////////////////////////////////
			////////////////////////////////////////////////////////////////////////////////////////////////////////
			if(local_command == ONEWIRE_CMD_REQUEST_INFO)
			{
				// Is it coming from module
				if((direction) /*&& (module_concerned)*/)
					GetDisplayModuleInfoFromRxBuffer();
				else
					// it was a readback : ignore
					onewire_rx_start_index = (onewire_rx_start_index + ONEWIRE_HEADER_FRAME_SIZE)&(ONEWIRE_RX_BUFF_LENGHT-1);
			}
			else
			////////////////////////////////////////////////////////////////////////////////////////////////////////
			/////////////////// IS IT A ONEWIRE_CMD_GET_VALUES ANSWER FROM MODULES /////////////////////////////////
			////////////////////////////////////////////////////////////////////////////////////////////////////////
			if(local_command == ONEWIRE_CMD_GET_VALUES)
			{
				// Is it coming from module
				if((direction) /*&& (module_concerned)*/)
					GetModuleValuesFromRxBuffer();
				else
					// it was a readback : ignore
					onewire_rx_start_index = (onewire_rx_start_index + ONEWIRE_HEADER_FRAME_SIZE)&(ONEWIRE_RX_BUFF_LENGHT-1);
			}	
			else
			////////////////////////////////////////////////////////////////////////////////////////////////////////
			/////////////////// IS IT A ONEWIRE_CMD_READ_REGISTER ANSWER FROM MODULES /////////////////////////////////
			////////////////////////////////////////////////////////////////////////////////////////////////////////
			if(local_command == ONEWIRE_CMD_READ_REGISTER)
			{
				// Is it coming from module
				if((direction) /*&& (module_concerned)*/)
					GetModuleRegistersFromRxBuffer();
				else
					// it was a readback : ignore
					onewire_rx_start_index = (onewire_rx_start_index + ONEWIRE_HEADER_FRAME_SIZE + ONEWIRE_HEADER_CMD_RW_REGISTER_SIZE)&(ONEWIRE_RX_BUFF_LENGHT-1);
			}	
			else
			{
				// COMMAND NOT RECOGNIZED : reset buffer pointers
				onewire_rx_start_index = local_end_index_minus_one;
				// we are now waiting new SOF
				Z8_STATUS_5 &= ~onewire_rx_state_z5;
			}
		} // end the frame is complete
		else
		{
			// here buffer is not empty, but not a valid frame !		
			Display_Invalid_Onewire_Serial_Incrust();
			onewire_rx_start_index = local_end_index_minus_one;
			// we are now waiting new SOF
			Z8_STATUS_5 &= ~onewire_rx_state_z5;
		}
	}
	// the buffer has now been treated
	onewire_rx_start_index = local_end_index_minus_one;
}


///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: GetDisplayModuleInfoFromRxBuffer()
// Inputs	: None
// Outputs	: None
// Process  : Only called when boot search for connected modules
//			  Always reset buffer pointers - does not need circular buffer here
//			  This function is called from SearchModules(), the current module is 
//			  pointed by nb_connected_modules
///////////////////////////////////////////////////////////////////////////////////////
void GetDisplayModuleInfoFromRxBuffer(void)
{
	uchar idx, frame_lenght, module_concerned;
//	uchar local_end_index_minus_one = (onewire_rx_end_index - 1)&(ONEWIRE_RX_BUFF_LENGHT-1);
	
	// here the buffer only contains current found module information
	// SOF_ONEWIRE, ID, SUB_ID, COMMAND, MODULE_TYPE, MODULE_NAME, ASSIGNMENTS_GROUP0, ASSIGNMENTS_GROUP1, ... ASSIGNMENTS_GROUP3,  EOF_ONEWIRE
	// With : 	MODULE_TYPE 		: FAD110, ENC410, POTh410, POTv410, BUT410, XFD010 (8 characters) (this info is stored in ROM of the module)
	// 			MODULE_NAME 		: user defined string (8 characters) assigned to each connected module
	//			ASSIGNMENTS_GROUPn	: all structures for all 4 groups 


	module_concerned = FALSE;
	// Check the frame is for the concerned module
	if((onewire_rx_buffer[(onewire_rx_start_index + 1)&(ONEWIRE_RX_BUFF_LENGHT-1)] == idx_id) &&
       (onewire_rx_buffer[(onewire_rx_start_index + 2)&(ONEWIRE_RX_BUFF_LENGHT-1)] == idx_sub_id))
	   	module_concerned = TRUE;

	frame_lenght = 	4 + MODULE_TYPE_LENGHT + MODULE_NAME_LENGHT; // + (CTRL_STRUCT_TOTAL_SIZE * GROUP_NB * CTRL_NB_PER_MODULE[idx_id]) ;

	// First check that the buffer contains infos for the module we are waiting for
	if(onewire_rx_buffer[(onewire_rx_start_index + frame_lenght)&(ONEWIRE_RX_BUFF_LENGHT-1)] == EOF_ONEWIRE)
    {
		if(module_concerned)
		{
			// Stop timer 3 single shot	
			T3CTL1  = 0x38; 
			// flag the module as found
			Z8_STATUS_4 |= module_answered_z4;
			// Stop timeout
			Z8_STATUS_5 &= ~waiting_timeout_onewire_z5;

			// Display module index
//			DisplayLcdInt(LCD_ZONE_SEARCH_MOD_X, LCD_ZONE_SEARCH_MOD_Y, DIGITS_2|LCD_INVERT, font_5x8, nb_connected_modules+1);
	
			// prepare sub-id to display after module type
			IntToLcdText(idx_sub_id, DIGITS_2);
			lcd_text_value[MODULE_TYPE_LENGHT] 	= '/';
			lcd_text_value[MODULE_TYPE_LENGHT+1]= lcd_text_value[0];
			lcd_text_value[MODULE_TYPE_LENGHT+2]= lcd_text_value[1];
			lcd_text_value[MODULE_TYPE_LENGHT+3]= '\0';

			// Extract and display MODULE_TYPE
			for(idx=0; idx<MODULE_TYPE_LENGHT ; idx++)
			{
				module[nb_connected_modules].module_type[idx] = onewire_rx_buffer[(onewire_rx_start_index + 4 + idx)&(ONEWIRE_RX_BUFF_LENGHT-1)];
				lcd_text_value[idx] = onewire_rx_buffer[(onewire_rx_start_index + 4 + idx)&(ONEWIRE_RX_BUFF_LENGHT-1)];
				// if module type was < MODULE_TYPE_LENGHT : insert space
				if(lcd_text_value[idx] == '\0') lcd_text_value[idx] = ' ';
			}
			// Display MODULE TYPE + SUB-ID
			DisplayLcdText(LCD_ZONE_SEARCH_MOD_X+5, LCD_ZONE_SEARCH_MOD_Y, LCD_INVERT, font_5x8);
		
			// Extract and display MODULE_NAME
			for(idx=0; idx<MODULE_NAME_LENGHT ; idx++)
			{
				module[nb_connected_modules].module_name[idx] = onewire_rx_buffer[(onewire_rx_start_index + 4 + MODULE_TYPE_LENGHT + idx)&(ONEWIRE_RX_BUFF_LENGHT-1)];
				lcd_text_value[idx] = onewire_rx_buffer[(onewire_rx_start_index + 4 + MODULE_TYPE_LENGHT + idx)&(ONEWIRE_RX_BUFF_LENGHT-1)];
			}
			// Display MODULE NAME
			lcd_text_value[MODULE_NAME_LENGHT] = '\0';
			DisplayLcdText(LCD_ZONE_SEARCH_MOD_X+74, LCD_ZONE_SEARCH_MOD_Y, LCD_INVERT, font_5x8);
	
			// Populate the module structure with the info from module found
			module[nb_connected_modules].id					= idx_id;
			module[nb_connected_modules].sub_id				= idx_sub_id;
			module[nb_connected_modules].bus				= current_onewire_bus;
			module[nb_connected_modules].controlindexbase	= control_index;

			// Extract all assignments structures and save them in motherboard eeproms if different and if not autoinit in progress
			// GetCompareSaveStructsFromModule();

		} // end module concerned

		// Update circular buffer
		onewire_rx_start_index = (onewire_rx_start_index + frame_lenght + 1)&(ONEWIRE_RX_BUFF_LENGHT-1);
	}
	else
	{
		// Invalid frame received
		Display_Invalid_Onewire_Serial_Incrust();
		// we are now waiting new SOF
		Z8_STATUS_5 &= ~onewire_rx_state_z5;
	}

}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: SelectOneWireBusMuxes()
// Inputs	: None
// Outputs	: None
// Process  : port H bits 0-2 select current one wire bus
///////////////////////////////////////////////////////////////////////////////////////
void SelectOneWireBusMuxes(void)
{
	ONE_WIRE_MUX_PORT_OUT &= ~ONE_WIRE_MUX_PORT_OUT_MSK;
	ONE_WIRE_MUX_PORT_OUT |= (current_onewire_bus & ONE_WIRE_MUX_PORT_OUT_MSK);
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	SendCommandToModule()
// Inputs	: 	None
// Outputs	: 	None
// Process  : 	SOF_ONEWIRE, ID, SUB_ID, COMMAND, (DATA), EOF_ONEWIRE
//				if a command requires data to be sent to the module, the data bytes are 
//				stored in generic_buffer
//				destination is ONEWIRE_ID_ALL_MODULES or ONEWIRE_ID_SINGLE_MODULE
///////////////////////////////////////////////////////////////////////////////////////
void SendCommandToModule(uchar command, uchar data_lenght, uchar destination, far uchar *buffer)
{
	uchar idx;	
	uchar local_id, local_sub_id;

	if(!(Z8_STATUS_4  & searching_modules_z4))
	{
		if(destination == ONEWIRE_ID_SINGLE_MODULE)
		{
			// retriewe the onewire bus where the current module is attached
			current_onewire_bus = module[current_module].bus;
			local_id 			= module[current_module].id;
			local_sub_id		= module[current_module].sub_id;
		}
		else
		// Need to send to all modules
		{
			local_id 			= ONEWIRE_ID_ALL_MODULES;
			local_sub_id		= ONEWIRE_ID_ALL_MODULES;
		}
	
		// then select the correct onewire bus
		SelectOneWireBusMuxes();
	}
	else
	{
		local_id 		= idx_id;
		local_sub_id	= idx_sub_id;
	}

	// Disable RX onewire to avoid readback
	Z8_STATUS_6 |= disable_ow_rx_z6;

	// Send Start of frame
	SendToTx0(SOF_ONEWIRE);
	
	// Send module ID
	SendToTx0(local_id);

	// Send module SUB-ID
	SendToTx0(local_sub_id);

	// Send COMMAND
	SendToTx0(command);

	// Depending on command : complete the frame here
	for(idx=0; idx<data_lenght ;idx++)
		SendToTx0(buffer[idx]);
	
	// Send End of frame
	SendToTx0(EOF_ONEWIRE);
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: GetModuleValuesFromRxBuffer()
// Inputs	: None
// Outputs	: None
// Process  : Only called when boot search for connected modules
//			  Always reset buffer pointers - does not need circular buffer here
//			  This function is called from SearchModules(), the current module is 
//			  pointed by nb_connected_modules
///////////////////////////////////////////////////////////////////////////////////////
void GetModuleValuesFromRxBuffer(void)
{
	uchar idx, ctrl_nb_in_module, hook_info_nb_in_module, whole_hook_state, module_concerned;
	uchar next_hook_state;
	uchar hook_byte_splitted[CTRL_MAX_NB_PER_MODULE];

	module_concerned = FALSE;
	// Check the frame is for the concerned module
	if((onewire_rx_buffer[(onewire_rx_start_index + 1)&(ONEWIRE_RX_BUFF_LENGHT-1)] == idx_id) &&
       (onewire_rx_buffer[(onewire_rx_start_index + 2)&(ONEWIRE_RX_BUFF_LENGHT-1)] == idx_sub_id))
	    {
	   		module_concerned = TRUE;
			// the module has answered here
			Z8_STATUS_4 |= module_answered_z4;
			// Stop timeout
			Z8_STATUS_5 &= ~waiting_timeout_onewire_z5;
			// Stop timer 3 single shot	
			T3CTL1  = 0x38; 

			// Check if the module answered with new value avaialble or not
			if(onewire_rx_buffer[(onewire_rx_start_index + ONEWIRE_DATA_FRAME_POS)&(ONEWIRE_RX_BUFF_LENGHT-1)] == ONEWIRE_NO_NEW_DATA)
			{
				// Skip current frame and place pointer on next frame
				onewire_rx_start_index += (ONEWIRE_DATA_FRAME_POS + 2);
				onewire_rx_start_index &= (ONEWIRE_RX_BUFF_LENGHT-1);
			}
			else
			{
				// get nb of hook bytes presents in the current module
				hook_info_nb_in_module = HOOK_INFOS_NB_PER_MODULE[module[current_module].id];
				
				// get nb of controls presents in the current module
				ctrl_nb_in_module = CTRL_NB_PER_MODULE[module[current_module].id];
				
				// Init all hook status bytes to hooked
				for(idx=0; idx<ctrl_nb_in_module ; idx++)
					hook_byte_splitted[idx] = CTRL_HOOKED;
		
				// Place start pointer to data position
				onewire_rx_start_index += (ONEWIRE_DATA_FRAME_POS);
				onewire_rx_start_index &= (ONEWIRE_RX_BUFF_LENGHT-1);
		
				// get hooked states from the current module
				for(idx=0; idx<hook_info_nb_in_module ; idx=idx+2)
				{
			   		// Affect buffer to current value
					whole_hook_state = onewire_rx_buffer[onewire_rx_start_index];
					
					// Split hook bytes to one byte per control					
					hook_byte_splitted[idx]   =  whole_hook_state & 0x03;
					hook_byte_splitted[idx+1] = (whole_hook_state & 0x0C) >> 2;
		
					// increase pointer on circular buffer
					onewire_rx_start_index++;
					onewire_rx_start_index &= (ONEWIRE_RX_BUFF_LENGHT-1);
				}
		
				// Extract new values sent by the modules
				for(idx=0; idx<ctrl_nb_in_module ; idx++)
				{
			   		// Affect buffer to current value
					current_value = onewire_rx_buffer[onewire_rx_start_index];
					
					// First pass : hook control and save current value
					if(Z8_STATUS_4 & first_pass_z4)
					{
						// Get module current value
						module[current_module].values[idx] = current_value & 0x7F;
					}
					else
					{
						// is the current control value different than previous one ?
						if(module[current_module].values[idx] != current_value)
						{
							// Get control index
							control_index = module[current_module].controlindexbase + idx;								

							// are we in assign mode
							if(assign_state == ASSIGN_WAIT_TWEAK_CTRL)
							{
							  module_assign.control_assign_index 			= control_index;
							  module_assign.control_num_in_module_assign 	= idx;
							  module_assign.current_module_assign			= current_module;
							}
							else
							if(assign_state != ASSIGN_ACTIVE)
							{
								// Save new value
								module[current_module].values[idx] = current_value;
			
								// Need to fill structure to send : already filed ?
								FillParamCurrentStruct();
			
								// Is the jumpless mode enabled ?
								if(Z8_STATUS_0_NVM & hook_zjump_mode_z0nvm)
								{
									// check if hook mode already hooked or not ?
									if(!(hook_byte_splitted[idx] & control_hooked_bit))
									{
											// Send control to MIDI only if it has changed
											SendControlToMidi();
						
											// display "no hook" bmp
										 	next_hook_state = CTRL_HOOKED;
					
									} // end control was already hooked
									else
									{
										// here control is not hooked yet, display a small bitmap indicating direction
										if(hook_byte_splitted[idx] & hook_direction_bit)
										{
											// display "must decrease" bmp
											next_hook_state = CTRL_HIGHER;
										}
										else
										{
											// display "must increase" bmp	
											next_hook_state = CTRL_LOWER;
										}
									}
								} // End jumpless mode
								else
								// Jump mode : send always control to midi 
								{
									// Send control to MIDI only if it has changed
									SendControlToMidi();
				
									// display "no hook" bmp
								 	next_hook_state = CTRL_HOOKED;
								} // End jump mode
	
								// Update LCD hook bitmap if required
								UpdateLcdHookState(next_hook_state);
				
								// Display value on LCD (dont care about the hook status)
								DisplayCurrentValue();
			
								// Need to update lcd string ?
								if(last_control_index != control_index)
								{
									// Update lcd str + module name + ...
									DisplayModuleInfos();
									last_control_index = control_index;
								}
							} // end assign state is not active
			
						} // End current value for current control has changed.
					} // end first pass
					// increase pointer on circular buffer
					onewire_rx_start_index++;
					onewire_rx_start_index &= (ONEWIRE_RX_BUFF_LENGHT-1);
				}
		
				// increase pointer on circular buffer to skip EOF
				onewire_rx_start_index++;
				onewire_rx_start_index &= (ONEWIRE_RX_BUFF_LENGHT-1);
			}
	 } // end the answer was for the concerned module
} // END ONEWIRE_CMD_GET_VALUES

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: StartTimerOneWire()
// Inputs	: None
// Outputs	: None
// Process  : 
///////////////////////////////////////////////////////////////////////////////////////
void StartTimerOneWire(uchar mult1ms)
{
	uchar i;
	uint load_value = 0;

//	timeout_answer_module_cpt = mult1ms;
	Z8_STATUS_5 |= waiting_timeout_onewire_z5;

	for(i=0; i<mult1ms ; i++) 
		load_value += (uint)156L;
	
	// Init timer 3
	// Start timer 3 value is 0
	T3H   	= 0x00;	// Timer 3 start value High (156d for 1ms)
	T3L   	= 0x00;	// Timer 3 start value Low  (156d for 1ms)
	// Reload timer 3 counter
	T3RH   	= (uchar)(load_value >> 8);		// Timer 3 reload value High (156d for 1ms)
	T3RL   	= (uchar)(load_value & 0xFF);	// Timer 3 reload value Low  (156d for 1ms)
	
	// Start timer 3 single shot	
	T3CTL1  = 0xB8; 

}


///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: SendCurrentGroupToAllModules()
// Inputs	: None
// Outputs	: None
// Process  : 
///////////////////////////////////////////////////////////////////////////////////////
void SendCurrentGroupToAllModules(void)
{
	// Exit if no module is connected
	if(!nb_connected_modules) return;

	// Here group has changed : must send info to ALL connected modules
	generic_buffer[0] = RAM_MOD; 					// indicates the group register is located in RAM of the module
	generic_buffer[1] = CURRENT_GROUP_ADR >> 8; 	// the group register address in RAM
	generic_buffer[2] = CURRENT_GROUP_ADR & 0xFF; 	// the group register address in RAM
	generic_buffer[3] = 1; 							// Lenght is 1 (only group is updated)
	generic_buffer[4] = current_group; 				// data

	// Send group change info to all 8 onewire buses
	for(current_onewire_bus=0; current_onewire_bus<ONEWIRE_BUS_MAX_NB; current_onewire_bus++)
	{
		SendCommandToModule(ONEWIRE_CMD_WRITE_REGISTER, ONEWIRE_HEADER_CMD_RW_REGISTER_SIZE+1, ONEWIRE_ID_ALL_MODULES, &generic_buffer[0]);
		// we must be sure the command has been sent before switching the one wire bus mux
//		PEOUT |= (1<<3);
		while(Z8_STATUS_6 & disable_ow_rx_z6)
			CheckRealtimeMidi();
//		PEOUT &= ~(1<<3);
	}
}


///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: GetCompareSaveStructsFromModule()
// Inputs	: None
// Outputs	: None
// Process  : 
///////////////////////////////////////////////////////////////////////////////////////
void GetAllStructFromModules(void)
{
	uchar current_control_local;
	uchar ctrl_nb_in_module;
	uchar step_increase;
	uchar step_bargraph = 0;
	uchar current_bargraph_pos = 0;
	uchar step_fill = 1;
	uint tmp2_int;
	uchar current_group_saved;

	// If an autoassign will be done, then ignore this function
	if(Z8_STATUS_1_NVM & must_autoassign_z1nvm)
		return;

	// Display title before starting
	FillLcdZone(LCD_ZONE_SEARCH_MOD_X, LCD_ZONE_SEARCH_MOD_Y-2, LCD_X_PIXEL_MAX, 4, FILL_BLANK);

	// display updating module text
	rom_to_lcd_text_copy(updating_txt);
	DisplayLcdText(14, 4, LCD_INVERT, font_5x8);
	rom_to_lcd_text_copy(modules_lc_txt);
	DisplayLcdText(70, 4, LCD_INVERT, font_5x8);

	current_group_saved = current_group;

	// calculate bargraph step
	// 128 pixels max, 4 groups
	tmp2_int = (uint) total_controls_nb * (uint) GROUP_NB;
	step_increase = (uchar)(tmp2_int >> 7);
	if(step_increase == 0)
    {
		step_increase = 1;
		step_fill = LCD_X_PIXEL_MAX / (uchar)(tmp2_int);
	}

	// handle all previoulsy found modules
	for(current_module=0; current_module<nb_connected_modules ;current_module++)
	{
		// get nb of controls presents in the current module
		ctrl_nb_in_module = CTRL_NB_PER_MODULE[module[current_module].id];

		// Treat all groups
		for(current_group = 0; current_group < GROUP_NB; current_group++)
		{

			for(current_control_local=0; current_control_local<ctrl_nb_in_module ;current_control_local++)
			{
				// Calculate the current control index
				control_index = module[current_module].controlindexbase + current_control_local;

				// Prepare the struct read command
				GetModuleEEAdressFromIndexGroup(current_control_local);

				// Prepare the generic buffer to read structure from module
				generic_buffer[0] = EEPROM_MOD; 				
				generic_buffer[1] = (uchar)((EE_Address >> 7) & 0x7F); 	
				generic_buffer[2] = (uchar)( EE_Address 	  & 0x7F); 	
				generic_buffer[3] = CTRL_STRUCT_TOTAL_SIZE; 		

				// reset module found flag;
				Z8_STATUS_4 &= ~module_answered_z4;

				// Wait for an answer within the 50ms
				StartTimerOneWire(MODULE_READ_REG_TIMEOUT);

				// retriewe the onewire bus where the current module is attached
				current_onewire_bus = module[current_module].bus;
				idx_id 				= module[current_module].id;
				idx_sub_id			= module[current_module].sub_id;

				// start to Read structure from module
				SendCommandToModule(ONEWIRE_CMD_READ_REGISTER, 
									ONEWIRE_HEADER_CMD_RW_REGISTER_SIZE, 
									ONEWIRE_ID_SINGLE_MODULE, &generic_buffer[0]);
	
				// Wait the answer of the connected module
				do { 
						CheckOneWireRxBuffer();
						if(Z8_STATUS_4 & module_answered_z4) 
							break;
						if(!(Z8_STATUS_5 & waiting_timeout_onewire_z5)) 
							break;
				} while(1);

				// if the module answered
				if(Z8_STATUS_4 & module_answered_z4)
				{
					// Stop timeout
					Z8_STATUS_5 &= ~waiting_timeout_onewire_z5;
					// Stop timer 3 single shot	
					T3CTL1  = 0x38; 
					// Read structure from eeprom, compare and save if required
					GetCompareSaveStructsFromModule();
				} // end the module answered

				// update progression bar
				if(++step_bargraph == step_increase)
				{
					step_bargraph = 0;
					current_bargraph_pos += step_fill;
					FillLcdZone(0, 5, current_bargraph_pos, 1, 0xF3);
				}
							
			} // for all controls in module
	    } // end all groups
	} // end all connected modules

	// finish by reinit
	current_module = 0;
	current_group  = current_group_saved;
	control_index  = 0;

}


///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: GetCompareSaveStructsFromModule()
// Inputs	: None
// Outputs	: None
// Process  : 
///////////////////////////////////////////////////////////////////////////////////////
void GetCompareSaveStructsFromModule(void)
{
	uchar idx, ctrl_idx;

	// generic buffer 0-31 contains received struct
	// read the currently stored struct in mother board
	// get control EEprom address and I2cCommand
	GetCtrlStructEEaddFromIndexGroup();
	// Read struct full from eeprom 
	ReadMultiByteI2c((far uchar *)&generic_buffer[32], CTRL_STRUCT_TOTAL_SIZE);
	
	// compare received struct with the one stored in motherboard
	for(idx = 0; idx<CTRL_STRUCT_TOTAL_SIZE; idx++)
	{
		// is the struct identical
		if(generic_buffer[idx] != generic_buffer[idx+32])
		{
			// no the struct is different
			WriteMultiByteI2c((far uchar *)&generic_buffer[0], CTRL_STRUCT_TOTAL_SIZE);
			// finish with this struct since saved
			break;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: GetModuleRegistersFromRxBuffer()
// Inputs	: None
// Outputs	: None
// Process  : 
///////////////////////////////////////////////////////////////////////////////////////
void GetModuleRegistersFromRxBuffer(void)
{
	uchar module_concerned;
	uchar idx, reg_lenght, frame_lenght;

	module_concerned = FALSE;
	// Check the frame is for the concerned module
	if((onewire_rx_buffer[(onewire_rx_start_index + 1)&(ONEWIRE_RX_BUFF_LENGHT-1)] == idx_id) &&
       (onewire_rx_buffer[(onewire_rx_start_index + 2)&(ONEWIRE_RX_BUFF_LENGHT-1)] == idx_sub_id))
	   	module_concerned = TRUE;

	// add the lenght of data available in the frame
	reg_lenght = onewire_rx_buffer[(onewire_rx_start_index + 7)&(ONEWIRE_RX_BUFF_LENGHT-1)];
	// frame lenght contains sof,id,subid,cmd,mem type, add msb, add lsb, lenght
	frame_lenght  = 4 + ONEWIRE_HEADER_CMD_RW_REGISTER_SIZE + reg_lenght;

	// First check that the buffer contains infos for the module we are waiting for
	if(onewire_rx_buffer[(onewire_rx_start_index + frame_lenght)&(ONEWIRE_RX_BUFF_LENGHT-1)] == EOF_ONEWIRE)
    {
		if(module_concerned)
		{
			// flag the module as found
			Z8_STATUS_4 |= module_answered_z4;
			// Stop timeout
			Z8_STATUS_5 &= ~waiting_timeout_onewire_z5;
			// Stop timer 3 single shot	
			T3CTL1  = 0x38; 

			// Get all registers and save them to generic buffer
			for(idx=0; idx<reg_lenght ;idx++)
				generic_buffer[idx] = onewire_rx_buffer[(onewire_rx_start_index + 8 + idx)&(ONEWIRE_RX_BUFF_LENGHT-1)];

		} // end module concerned
	} // end eof was present
	else
	{
		// Invalid frame received
		Display_Invalid_Onewire_Serial_Incrust();
		// we are now waiting new SOF
		Z8_STATUS_5 &= ~onewire_rx_state_z5;
	}

	// Update circular buffer
	onewire_rx_start_index = (onewire_rx_start_index + frame_lenght + 1)&(ONEWIRE_RX_BUFF_LENGHT-1);
}
