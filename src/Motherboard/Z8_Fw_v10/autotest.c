#include <eZ8.h>
#include "pins_defs.h"
#include "commondef.h"
#include <common_z8_ez.h>
#include <ctrldef.h>
#include "serial.h"
#include "eeprom.h"
#include "lcd.h"
#include "lcd_low_level.h"
#include "hardware.h"
#include "autotest.h"
#include "realtime.h"
#include "i2c.h"
#include "onewire.h"

///////////////////////////////////////////////////////////////////////////////////////
// GLOBAL VARS
///////////////////////////////////////////////////////////////////////////////////////

extern uint  EE_Address;							// current EEProm address
extern uchar I2cCommand;							// current EEProm selection
extern far uchar generic_buffer[GENERIC_BUFF_SIZE];	// a generic far ram buffer of 128 bytes
extern far uchar lcd_text_value[MAX_LCD_TEXT_LENGHT];	// global buffer for integer to text conversion
extern rom uchar reset_txt[];
//extern rom uchar done_txt[];	
extern rom uchar font_5x8[];
extern rom uchar default_grp_name_txt[];
extern far uchar CtrlReg[CTRL_REG_REAL_NB];			// table of control reg for eerprom saving and menus 
extern far uchar CtrlReg_EE_img[CTRL_REG_REAL_NB];		// table of control reg for eerprom saving
extern volatile uchar Z8_STATUS_6;

extern far All_Modules module[MODULES_MAX_NB];
extern uchar nb_connected_modules;
extern uchar idx_id;
extern uchar idx_sub_id;
extern uchar current_module;

extern rom uchar autoassign_txt[];
extern rom uchar modules_txt[];
extern uchar control_index;
extern far All_Params	 	Param[PARAM_STRUCT_IN_RAM_MAX_NB];	// structure of parameters in EEPROM V1.61
extern far All_Params *ParamCurrent;						//current structure of parameters in EEPROM
extern rom uchar CTRL_NB_PER_MODULE[MODULES_TYPE_MAX_NB];
extern far uchar total_controls_nb;
extern uchar last_struct_index;								
extern uchar last_control_index;

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	CheckBootStatus()
// Inputs	: 	None
// Outputs	: 	None
// Process  : 	start a autotest or factory default
//				Keys functions at boot :
//					PLAY + REC + STOP			:	FACTORY DEFAULT
///////////////////////////////////////////////////////////////////////////////////////
void CheckBootStatus(void)
{
	uchar index; // V1.5
	
	// V1.5 if the eeproms are not intitialized then perform a factory default at startup
	// read the control registers from eeprom  : if all bytes are FF then perform a factory default
	I2cCommand	= I2C_SLAVE_ADR_EE + EEPROM_ZERO_I2C_ADR;
	EE_Address = (uint)CTRL_REG_START_EE_ADR;
	ReadMultiByteI2c((far uchar *)&generic_buffer[0], CTRL_REG_REAL_NB);	// place the first 64 bytes of eeprom 0 in generic buffer 0-63
	// Check that all control registers are not equal to FF
	for(index=0 ; index < CTRL_REG_REAL_NB ; index++)
	{
		if(generic_buffer[index] != 0xFF) break;
	}

	// V1.5 All registers were equal to FF : must perform a factory default
	if(index == CTRL_REG_REAL_NB)
		FactoryDefault();

	if(PLAY_PORT_IN & PLAY_PIN)
	if(REC_PORT_IN & REC_PIN)
	{
		if(STOP_PORT_IN & STOP_PIN)
		{
			FactoryDefault();
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: FactoryDefault()
// Inputs	: None
// Outputs	: None
// Process  : Reset All parameters to factory default
///////////////////////////////////////////////////////////////////////////////////////
void FactoryDefault(void)
{
	uchar index;
	uchar tmp, tmp2;

	// First clear display
	FillLcdZone(0, 0, LCD_X_PIXEL_MAX, LCD_Y_PAGE_MAX, FILL_BLANK);	

	// display Initializing text
	rom_to_lcd_text_copy(reset_txt);
	DisplayLcdText(15, 3, LCD_INVERT, font_5x8);

	// current EEProm selection
	I2cCommand 	= I2C_SLAVE_ADR_EE + EEPROM_ZERO_I2C_ADR;

	//////////////////////////////////////////////////////////////////////////////////////////////////////////
 	// Reset all control registers to their default values
	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	CTRL_REG_0_7BITS_TO_EZ 		= midi_thru_ez0/*merger_src_midi_in_state_z_ez0 | merger_src_usb_in_state_z_ez0 | merger_dst_usb_out_state_z_ez0 | merger_dst_out1_state_z_ez0*/;
	CTRL_REG_1_7BITS_TO_EZ 		= realtime_source_int_z_ez1; 
	//LAST_CTRL_REG_7BITS_TO_EZ = ez_board_fully_updated_ezlast;
	Z8_STATUS_0_NVM				= hook_zjump_mode_z0nvm;
	Z8_STATUS_1_NVM				= 0;
	current_channel				= 0;
	current_group 				= 0;
   	internal_tempo				= DEFAULT_INTERNAL_TEMPO;

	// Now all controls regs are defined and clean also assign control regs to control regs ee img
	for(tmp=0; tmp<CTRL_REG_REAL_NB; tmp++)
		CtrlReg_EE_img[tmp] = CtrlReg[tmp];

	// Save all control registers in a multi byte eeprom write
	EE_Address = (uint)CTRL_REG_START_EE_ADR; 
	WriteMultiByteI2c(&CtrlReg[0], CTRL_REG_REAL_NB);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////
 	// Write default group names : group 1, group 2 ..
	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	rom_to_lcd_text_copy(default_grp_name_txt);
	// Program all groups names
	for(current_group=0;current_group<GROUP_NB;current_group++)
	{
		// extarct the 2 digits of current group
		tmp  = current_group;
		tmp2 = '0'; 
   		while(tmp > 9)
   		{
      		tmp -= 10;
      		tmp2++;
   		}
		lcd_text_value[7] = tmp2;
		lcd_text_value[8] = tmp + '0';
		lcd_text_value[9] = ' ';
		lcd_text_value[10] = '\0';
		// calculate group name address
		EE_Address = (uint)(GROUP_NAME_BASE_EE_ADR + (uint)((uint)(current_group) * GROUP_NAME_EE_LENGHT));
		// write group name to eeprom
		WriteMultiByteI2c((far uchar *)&lcd_text_value[0], GROUP_NAME_REAL_SIZE);
	}

	// V1.6 : reinit initial values after factory default !
	InitInitialValues();

	// display done text
//	rom_to_lcd_text_copy(done_txt);
//	DisplayLcdText(15, 4, LCD_INVERT, font_5x8);

	// wait 1.5sec on home screen
	delay_msec(1500);

	// indicates that modules must be autoassigned as soon as the list is known
	Z8_STATUS_1_NVM |= must_autoassign_z1nvm;

}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: AutoAssignAllModules()
// Inputs	: None
// Outputs	: None
// Process  : Reset All connected modules to factory default
///////////////////////////////////////////////////////////////////////////////////////
void AutoAssignAllModules(void)
{
	uchar current_control_local;
	uchar tmp;
	uchar ctrl_nb_in_module;
	uchar step_increase;
	uchar step_bargraph = 0;
	uchar current_bargraph_pos = 0;
	uchar step_fill = 1;
	uint tmp2_int;
	uchar current_group_saved;
	uchar cc_nb;

	// remove low part of the home screen and display autoassign infos
	FillLcdZone(0, 0, LCD_X_PIXEL_MAX, LCD_Y_PAGE_MAX, FILL_BLANK);

	// display searching module text
	rom_to_lcd_text_copy(autoassign_txt);
	DisplayLcdText(5, 4, LCD_INVERT, font_5x8);
	rom_to_lcd_text_copy(modules_txt);
	DisplayLcdText(75, 4, LCD_INVERT, font_5x8);

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
	
	ParamCurrent = &Param[0];

 	//////////////////////////////////////////////////////////////////////////////////////////////////////////
 	// Write default MIDI struct and strings and LCD strings
	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Fill parts of control struct that will not change
	ParamCurrent->message_type				= CC;
	ParamCurrent->ctrl_status_1				= midi_out_1_en | usb_out_en;
	ParamCurrent->midi_str_lenght 			= 0x03;		// default are 3 bytes for CCs				
	ParamCurrent->midi_channel_position		= 0x01;		// midi channel position on first byte 	
	ParamCurrent->midi_channel				= current_channel;	// midi channel
	ParamCurrent->ctrl_value_position_0		= 0x03;		// default control value on byte 3			
	ParamCurrent->ctrl_value_min			= 0x00;		// Min default is 0					
	ParamCurrent->ctrl_value_max			= 0x7F;		// Max default is 127					
	ParamCurrent->midi_string[0] 			= 0xB0;		// default string is CC : 
	ParamCurrent->midi_string[2] 			= 0x00;		// default string is CC : value byte is cleared

	cc_nb = 0;

	// Program all groups
	for(current_group=0;current_group < GROUP_NB; current_group++)
	{
		control_index = 0;

		// handle all previoulsy found modules
		for(current_module=0; current_module<nb_connected_modules ;current_module++)
		{
			// get nb of controls presents in the current module
			ctrl_nb_in_module = CTRL_NB_PER_MODULE[module[current_module].id];

			// Send module name only one time : in group 0
			if(current_group == 0)
			{
				// Auto assign module name + prepare generic buffer to send to module
				CreateStdModuleNameFromCurrentModule();
	
				// Send this module name to the current module
				SendCommandToModule(ONEWIRE_CMD_WRITE_REGISTER, 
									ONEWIRE_HEADER_CMD_RW_REGISTER_SIZE + MODULE_NAME_LENGHT, 
									ONEWIRE_ID_SINGLE_MODULE, &generic_buffer[0]);

				// wait for the whole command sent before going to next
				while(Z8_STATUS_6 & disable_ow_rx_z6);

				// Give the time to the module to parse & write the structure
				delay_msec(20);
			}

			for(current_control_local=0; current_control_local<ctrl_nb_in_module ;current_control_local++)
			{
				// assign current cc number
				ParamCurrent->midi_string[1] = cc_nb;
				ParamCurrent->message_nb	 = cc_nb;
				// Define default LCD string
				CreateStdLcdStr(cc_nb);
				tmp = 0;
				while (lcd_text_value[tmp] != '\0')
				   ParamCurrent->lcd_string[tmp] = lcd_text_value[tmp++];

				// Write control struct and midi str to ext EEPROM
				GetCtrlStructEEaddFromIndexGroup();
				WriteMultiByteI2c((far uchar *)&ParamCurrent->message_type, CTRL_STRUCT_TOTAL_SIZE);

				// Send the fullfilled structure to the concerned module
				SendFullStructureToModule(current_control_local);

				// wait for the whole command sent before going to next
				while(Z8_STATUS_6 & disable_ow_rx_z6);

				// Give the time to the module to parse & write the structure
				delay_msec(35);

				// update progression bar
				if(++step_bargraph == step_increase)
				{
					step_bargraph = 0;
					current_bargraph_pos += step_fill;
					FillLcdZone(0, 5, current_bargraph_pos, 1, 0xF3);
				}

				// increase cc nb
				control_index = (control_index + 1) & 0x7F;
				// increase assigned cc nb
				cc_nb = (cc_nb + 1) & 0x7F;

			} // end all controls			
		} // end all modules
	} // end all groups	

	// finish by reinit
	current_module	 	= 0;
	current_group  		= current_group_saved;
	control_index  		= 0;
	last_control_index 	= CTRL_UNDEF;
	// be sure all structs will be reloaded from eeprom
	ParamCurrent = &Param[0];
	last_struct_index = 0;	
	InitParamStructsInRam();

	// wait 1sec on home screen
	delay_msec(1000);

}

/////////////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	CreateStdModuleNameFromCurrentModule()
// Inputs	: 	None
// Outputs	: 	None
// Process  : 	
//////////////////////////////////////////////////////////////////////////////////////////////
void CreateStdModuleNameFromCurrentModule(void)
{
	uchar tmp0, tmp1;

	// First transform current_module into a 2 digits
	IntToLcdText(current_module, DIGITS_2);
	// save this 2 digit text
	tmp0 = lcd_text_value[0];
	tmp1 = lcd_text_value[1];		

	// Update module names of all connected modules
	module[current_module].module_name[0] = 'M';
 	module[current_module].module_name[1] = 'o';
	module[current_module].module_name[2] = 'd';
 	module[current_module].module_name[3] = 'u';
 	module[current_module].module_name[4] = 'l';
	module[current_module].module_name[5] = ' ';
	module[current_module].module_name[6] = tmp0;
	module[current_module].module_name[7] = tmp1;

	// Prepare generic buffer to send to the current module
	generic_buffer[0] = EEPROM_MOD; 				
	generic_buffer[1] = (MODULE_NAME_EE_ADR >> 7) & 0x7F ; 	
	generic_buffer[2] =  MODULE_NAME_EE_ADR 	  & 0x7F; 	
	generic_buffer[3] = MODULE_NAME_LENGHT; 		

	for(tmp0=0; tmp0<MODULE_NAME_LENGHT ;tmp0++)
		generic_buffer[4+tmp0] = module[current_module].module_name[tmp0];

}

/////////////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	SendFullStructureToModule()
// Inputs	: 	None
// Outputs	: 	None
// Process  : 	
//////////////////////////////////////////////////////////////////////////////////////////////
void SendFullStructureToModule(uchar current_control_in_module)
{
	uchar idx;
	far uchar *myptr;

	// Check the structure is coherent
	if(CheckMidiStruct() == MIDI_STRUCT_NOT_VALID)
    {
		// Here must display struct error on LCD incrust;
		Display_Invalid_Midi_Struct_Incrust();
		return;
	}

	// calculate the eeprom address of module where to write current structure
	GetModuleEEAdressFromIndexGroup(current_control_in_module);
	
	// Prepare the generic buffer to send structure to module
	generic_buffer[0] = EEPROM_MOD; 				
	generic_buffer[1] = (uchar)((EE_Address >> 7) & 0x7F) ; 	
	generic_buffer[2] = (uchar)(EE_Address 	  & 0x7F); 	
	generic_buffer[3] = CTRL_STRUCT_TOTAL_SIZE; 		

	myptr = &ParamCurrent->message_type;

	for(idx=0; idx<CTRL_STRUCT_TOTAL_SIZE ;idx++)
		generic_buffer[4+idx] = *(myptr + idx);

	// Send the fullfilled structure to the concerned module
	SendCommandToModule(ONEWIRE_CMD_WRITE_REGISTER, 
						ONEWIRE_HEADER_CMD_RW_REGISTER_SIZE + CTRL_STRUCT_TOTAL_SIZE, 
						ONEWIRE_ID_SINGLE_MODULE, &generic_buffer[0]);
}


/////////////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	GetModuleEEAdressFromIndexGroup()
// Inputs	: 	None
// Outputs	: 	None
// Process  : 	
//////////////////////////////////////////////////////////////////////////////////////////////
void GetModuleEEAdressFromIndexGroup(uchar current_control_in_module)
{
	// calculate the eeprom address of module where to write current structure
	EE_Address  = (uint)((uint)(current_control_in_module) * (uint)(CTRL_STRUCT_TOTAL_SIZE)); // from 0 to 96 for group 0
	// Now need to multiply per the offset for each group
	EE_Address += (uint)((uint)current_group * (uint)STRUCT_EE_LGT_PER_GRP_IN_MODUL);	
	// Now need to offset the address to the base address
	EE_Address += (uint)MODULE_ASSIGN_STRUCT_BASE_EE_ADR;

}
