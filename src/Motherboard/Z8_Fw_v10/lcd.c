#include <eZ8.h>
#include "pins_defs.h"
#include "commondef.h"
#include <common_z8_ez.h>
#include <ctrldef.h>
#include "lcd.h"
#include "lcd_low_level.h"
#include "eeprom.h"
#include "lcd_fonts.h"
#include "lcd_texts.h"
#include "lcd_bmps.h"
#include "i2c.h"
#include "realtime.h"
#include "onewire.h"
#include "hardware.h"
#include "smpte.h"

///////////////////////////////////////////////////////////////////////////////////////////
// GLOBAL VARIABLES
///////////////////////////////////////////////////////////////////////////////////////////
far uchar lcd_text_value[MAX_LCD_TEXT_LENGHT];	// global buffer for integer to text conversion	
uchar lcd_current_incrust;

extern uchar lcd_lock_incrust_cpt;					// allow to lock LCD incrust zone
extern volatile uchar Z8_STATUS_4;
extern volatile uchar Z8_STATUS_5;
extern volatile uchar Z8_STATUS_8;
extern rom uchar identity[];
extern uint  EE_Address;
extern uchar I2cCommand;
//extern volatile far uchar CtrlReg[CTRL_REG_REAL_NB];			// table of control reg for eerprom saving and menus 
extern far uchar CTRL_REG_7BITS_FROM_EZ[CTRL_REG_MAX_FROM_EZ_TO_Z8];		// NN=1 registers can be accessed from ez board 
extern uchar current_value;
extern far uchar CtrlReg[CTRL_REG_REAL_NB];	// table of control reg for eerprom saving
extern uchar current_realtime_state;
extern far All_Modules module[MODULES_MAX_NB];
extern uchar current_module;
extern uchar current_hook_state;
extern uchar control_index;
extern far All_Params *ParamCurrent;						//current structure of parameters in EEPROM
extern rom uchar SmpteFrameRateTxtPtr[];
extern uchar nb_connected_modules;

/////////////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	Display_Invalid_Sysex_Incrust()
// Inputs	: 	None
// Outputs	: 	None
// Process  : 	Called when an invalid or unexpected sysex has been received
//////////////////////////////////////////////////////////////////////////////////////////////
void Display_Invalid_Sysex_Incrust(void)
{
	if(Z8_STATUS_4 & lcd_locked_by_function_z4) return;

	if(lcd_current_incrust != LCD_INCRUST_UNVALID_SYSEX)
	{
		// Display incrust zone
		DisplayEmptyIncrust();
	
		rom_to_lcd_text_copy(sysex_txt);
		DisplayLcdText(LCD_ZONE_INCRUST_X+15, LCD_ZONE_INCRUST_Y+1, LCD_INVERT, font_5x8);	
	
		rom_to_lcd_text_copy(error_txt);
		DisplayLcdText(LCD_ZONE_INCRUST_X+40, LCD_ZONE_INCRUST_Y+1, LCD_INVERT, font_5x8);	
	}

	lcd_current_incrust = LCD_INCRUST_UNVALID_SYSEX;	

	// Lock LCD incrust zone for 2 seconds
	lcd_lock_incrust_cpt = CPT_2_SEC;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	Display_Invalid_Midi_Struct_Incrust()
// Inputs	: 	None
// Outputs	: 	None
// Process  : 	Called when an invalid midi structure is read from eeprom
//////////////////////////////////////////////////////////////////////////////////////////////
void Display_Invalid_Midi_Struct_Incrust(void)
{
	if(Z8_STATUS_4 & lcd_locked_by_function_z4) return;

	if(lcd_current_incrust != LCD_INCRUST_UNVALID_MIDI_STRUCT)
	{
		// Display incrust zone
		DisplayEmptyIncrust();

		// display error on received sysex
		rom_to_lcd_text_copy(corrupted_struct_txt);
		DisplayLcdText(LCD_ZONE_INCRUST_X+15, LCD_ZONE_INCRUST_Y+1, LCD_INVERT, font_5x8);
	}

	lcd_current_incrust = LCD_INCRUST_UNVALID_MIDI_STRUCT;	

	// Lock LCD incrust zone for 2 seconds
	lcd_lock_incrust_cpt = CPT_2_SEC;
}


/////////////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	Display_Invalid_Onewire_Serial_Incrust()
// Inputs	: 	None
// Outputs	: 	None
// Process  : 	Called when an invalid or unexpected one wire frame has been received
//////////////////////////////////////////////////////////////////////////////////////////////
void Display_Invalid_Onewire_Serial_Incrust(void)
{
	if(Z8_STATUS_4 & lcd_locked_by_function_z4) return;

	if(lcd_current_incrust != LCD_INCRUST_UNVALID_MIDI_STRUCT)
	{
		// Display incrust zone
		DisplayEmptyIncrust();
	
		rom_to_lcd_text_copy(modules_txt);
		DisplayLcdText(LCD_ZONE_INCRUST_X+2, LCD_ZONE_INCRUST_Y+1, LCD_INVERT, font_5x8);	
	
		rom_to_lcd_text_copy(communication_txt);
		DisplayLcdText(LCD_ZONE_INCRUST_X+48, LCD_ZONE_INCRUST_Y+1, LCD_INVERT, font_5x8);	
		
		rom_to_lcd_text_copy(error_txt);
		DisplayLcdText(LCD_ZONE_INCRUST_X+45, LCD_ZONE_INCRUST_Y+2, LCD_INVERT, font_5x8);	
	}
	
	lcd_current_incrust = LCD_INCRUST_UNVALID_ONEWIRE_FRAME;	

	// Lock LCD incrust zone for 2 seconds
	lcd_lock_incrust_cpt = CPT_2_SEC;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	DisplayInputOverflowIncrust()
// Inputs	: 	None
// Outputs	: 	None
// Process  : 	
//////////////////////////////////////////////////////////////////////////////////////////////
void DisplayInputOverflowIncrust(void)
{
	rom uchar *local_txt_ptr;
	rom uchar *local_txt_ptr2=NULL;

	if(Z8_STATUS_4 & lcd_locked_by_function_z4) return;

	// Display incrust zone
	DisplayEmptyIncrust();

	if(Z8_STATUS_8 & serial_in_overflow_z8)
	{
		lcd_current_incrust = LCD_INCRUST_SERIAL_IN_OVERFLOW;
		local_txt_ptr = serial_in_txt;
	}
	else	
	if(Z8_STATUS_8 & onewire_rx_overflow_z8)
	{
		lcd_current_incrust = LCD_INCRUST_ONEWIRE_IN_OVERFLOW;
		local_txt_ptr = modules_txt;
	}
	else
	{
		lcd_current_incrust = LCD_INCRUST_MIDI_OVERFLOW;
		if(Z8_STATUS_8 & OVERFLOW_FROM_USB_MSK)
			local_txt_ptr  = usb_in_txt;
		if(Z8_STATUS_8 & OVERFLOW_FROM_MIDI_MSK)
			local_txt_ptr  = midi_in_txt;
		if(Z8_STATUS_8 & OVERFLOW_REALTIME_MSK)
			local_txt_ptr2 = realtime_txt;
	}

	rom_to_lcd_text_copy(local_txt_ptr);
	DisplayLcdText(LCD_ZONE_INCRUST_X+11, LCD_ZONE_INCRUST_Y+1, LCD_INVERT, font_5x8);	

	if(local_txt_ptr2)
	{
		rom_to_lcd_text_copy(local_txt_ptr2);
		DisplayLcdText(LCD_ZONE_INCRUST_X+38, LCD_ZONE_INCRUST_Y+1, LCD_INVERT, font_5x8);	
	}

	rom_to_lcd_text_copy(overflow_txt);
	DisplayLcdText(LCD_ZONE_INCRUST_X+11, LCD_ZONE_INCRUST_Y+2, LCD_INVERT, font_5x8);	

	// Lock LCD incrust zone for 2 seconds
	lcd_lock_incrust_cpt = CPT_2_SEC;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	DisplayEmptyIncrust()
// Inputs	: 	None
// Outputs	: 	None
// Process  : 	
//////////////////////////////////////////////////////////////////////////////////////////////
void DisplayEmptyIncrust(void)
{

	if(Z8_STATUS_4 & lcd_locked_by_function_z4) return;

	// Display incrust zone
	// Clear display over incrust zone only
	FillLcdZone(LCD_ZONE_INCRUST_X, LCD_ZONE_INCRUST_Y, LCD_ZONE_INCRUST_X_PIX, LCD_ZONE_INCRUST_Y_PAGE, FILL_BLANK);
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	DisplayRomVersion()
// Inputs	: 	None
// Outputs	: 	None
// Process  : 	display rom version while in home screen
//////////////////////////////////////////////////////////////////////////////////////////////
void DisplayRomVersion(uchar x, uchar y)
{
	lcd_text_value[0] = identity[FW_VERSION_INDEX] + '0';
	lcd_text_value[1] = '.';
	lcd_text_value[2] = identity[FW_SUB_VERSION_INDEX] + '0';
	lcd_text_value[3] = '\0';
	DisplayLcdText(x, y, LCD_INVERT, font_5x8);	
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: rom_to_lcd_text_copy()
// Inputs	: None
// Outputs	: None
// Process  : Copy a rom table to ram location
///////////////////////////////////////////////////////////////////////////////////////
void rom_to_lcd_text_copy(rom uchar *src)
{
	uchar i=0;

	while (*src)
	   lcd_text_value[i++] = *src++;
	lcd_text_value[i] = '\0';
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: UpdateLcdZones()
// Inputs	: None
// Outputs	: None
// Process  : Initilize Graphic LCD zones
///////////////////////////////////////////////////////////////////////////////////////
void UpdateLcdZones(int update_zones)
{
	rom uchar * local_bmp_ptr;
	uchar tmp;

	// return if lcd fully locked
	if(Z8_STATUS_4 & lcd_locked_by_function_z4) return;

	// Re init live screen lines + fixed elements
	if(update_zones & INIT_LCD_LIVE_SCR_TOP_FIXED)
	{
		FillLcdZone(31, 0, 2, 3, 0x00);
		
		// Display BPM text or SMPTE Frame rate depending on MTC mode
		if( (!(Z8_STATUS_0_NVM & mtc_mode_z0nvm)) || (Z8_STATUS_0_NVM & mtc_mode_on_off_z0nvm))
		{
			// Display BPM text
			DisplayBmp(LCD_ZONE_BPM_X, LCD_ZONE_BPM_Y, LCD_INVERT, bmp_bpm_txt); 
		}
		else
		{
			// Display SMPTE type
			rom_to_lcd_text_copy(smpte_txt); 
			DisplayLcdText(LCD_ZONE_TEMPO_X, LCD_ZONE_TEMPO_Y, LCD_INVERT, font_5x8);
			local_bmp_ptr = &SmpteFrameRateTxtPtr[0];
			tmp = local_bmp_ptr[(Z8_STATUS_1_NVM & SMPTE_FRAME_RATE_MASK_BIT) >> 1];
			rom_to_lcd_text_copy(local_bmp_ptr + tmp);
			DisplayLcdText(LCD_ZONE_BPM_X-1, LCD_ZONE_BPM_Y, LCD_INVERT, font_5x8);
			// Also display __:__:__:__
			rom_to_lcd_text_copy(smpte_2points_txt);
			DisplayLcdText(LCD_ZONE_TEMPO_X, LCD_ZONE_BPM_Y+1, LCD_INVERT, font_5x8);
		}
	}

	// Re init live screen lines + fixed elements
	if(update_zones & INIT_LCD_LIVE_SCR_BOT_FIXED)
	{
		DisplayBmp(LCD_ZONE_INCRUST_X, LCD_ZONE_INCRUST_Y-1, LCD_INVERT, bmp_live_bottom_txt); 
		// Display the default current value
		DisplayCurrentValue();
		// Display all infos for current module
		DisplayModuleInfos();
		// If at least one module is present then override bmp no module
		if(nb_connected_modules)
		DisplayBmp(LCD_ZONE_OW_ACTIVITY_X, LCD_ZONE_OW_ACTIVITY_Y, LCD_INVERT, bmp_bus_on); 
	}

	// Re init LCD usb if required
	if(update_zones & INIT_LCD_MIDI_ACTIVITY)
		DisplayMidiActivityBmp();

	// Display tempo + BPM or SMPTE infos depending on mode
	if(update_zones & INIT_LCD_TEMPO)
	{
		// relatime off ?
		if(Z8_STATUS_0_NVM & mtc_mode_on_off_z0nvm)
		{
			// clear old tempo since no more active
			FillLcdZone(LCD_ZONE_TEMPO_X, LCD_ZONE_TEMPO_Y, 34, 2, FILL_BLANK);
			FillLcdZone(LCD_ZONE_TEMPO_X, LCD_ZONE_TEMPO_Y+1, 10, 1, 0xFC);
			FillLcdZone(LCD_ZONE_TEMPO_X+12, LCD_ZONE_TEMPO_Y+1, 10, 1, 0xFC);
			FillLcdZone(LCD_ZONE_TEMPO_X+24, LCD_ZONE_TEMPO_Y+1, 10, 1, 0xFC);
		}
		else
		{
			// MIDI clock mode
			if(!(Z8_STATUS_0_NVM & mtc_mode_z0nvm))
				DisplayLcdInt(LCD_ZONE_TEMPO_X, LCD_ZONE_TEMPO_Y, DIGITS_3|LCD_INVERT, font_num_8x16, internal_tempo+30 );
			else
				DisplaySmpteTime(SMPTE_ALL_DIGIT);
		}
	}

	// Re init LCD group if required
	if(update_zones & INIT_LCD_GROUP)
	{
		// Display group number
		DisplayLcdInt(LCD_ZONE_GROUP_X, LCD_ZONE_GROUP_Y, DIGITS_2|LCD_INVERT, font_5x8, current_group );

		// Now Display group name
		EE_Address  = (uint)(GROUP_NAME_BASE_EE_ADR + (uint)((uint)GROUP_NAME_EE_LENGHT * (uint)current_group));
		I2cCommand	= I2C_SLAVE_ADR_EE + EEPROM_ZERO_I2C_ADR;
		ReadMultiByteI2c((far uchar *)&lcd_text_value[0], GROUP_NAME_REAL_SIZE);
		lcd_text_value[GROUP_NAME_REAL_SIZE] = '\0';
		DisplayLcdText(LCD_ZONE_GROUP_NAME_X, LCD_ZONE_GROUP_NAME_Y, LCD_INVERT, font_5x8);		
	}
		
	// Re init LCD usb if required
	if(update_zones & INIT_LCD_USB)
	{
		if(CTRL_REG_7BITS_FROM_EZ[0] & usb_state_from_ez0)
			local_bmp_ptr = bmp_usb_on;
		else
			local_bmp_ptr = bmp_usb_off;

		DisplayBmp(LCD_ZONE_USB_X, LCD_ZONE_USB_Y, LCD_INVERT, local_bmp_ptr);
	}

	// Re init LCD usb if required
	if(update_zones & INIT_LCD_REALTIME_BMP)
	{
		DisplayRealtimeBitmap();
	}
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: DisplayMidiActivityBmp()
// Inputs	: None
// Outputs	: None
// Process  : 
///////////////////////////////////////////////////////////////////////////////////////
void DisplayMidiActivityBmp(void)
{
	rom uchar * local_bmp_ptr;

	if(Z8_STATUS_4 & lcd_locked_by_function_z4) return;

	if(Z8_STATUS_5 & midi_activity_z5)
		local_bmp_ptr = bmp_midi_on;
	else
		local_bmp_ptr = bmp_midi_off;

	DisplayBmp(LCD_ZONE_MIDI_ACTIVITY_X, LCD_ZONE_MIDI_ACTIVITY_Y, LCD_INVERT, local_bmp_ptr);
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	DisplayCurrentValue()
// Inputs	: 	None
// Outputs	: 	None
// Process  : 	Display current control value on the LCD zone value
//////////////////////////////////////////////////////////////////////////////////////////////
void DisplayCurrentValue(void)
{
	if(Z8_STATUS_4 & lcd_locked_by_function_z4) return;
	
	DisplayLcdInt(LCD_ZONE_VALUE_X, LCD_ZONE_VALUE_Y, DIGITS_3|LCD_INVERT, font_num_8x16, current_value);

}

/////////////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	DisplayRealtimeBitmap()
// Inputs	: 	None
// Outputs	: 	None
// Process  : 	Display current control value on the LCD zone value
//////////////////////////////////////////////////////////////////////////////////////////////
void DisplayRealtimeBitmap(void)
{
	if(Z8_STATUS_4 & lcd_locked_by_function_z4) return;

	switch(current_realtime_state)
	{
		case PLAY_TRANSPORT : 		// Depending on preceding state, PLAY or CONTINUE
									DisplayBmp(LCD_ZONE_TRANSPORT_X, LCD_ZONE_TRANSPORT_Y, LCD_INVERT, bmp_play);
									break;
		case STOP_TRANSPORT : 		// Display STOP bmp
									DisplayBmp(LCD_ZONE_TRANSPORT_X, LCD_ZONE_TRANSPORT_Y, LCD_INVERT, bmp_stop);
									break;
		case REC_TRANSPORT : 		// Display REC bmp
									DisplayBmp(LCD_ZONE_TRANSPORT_X, LCD_ZONE_TRANSPORT_Y, LCD_INVERT, bmp_record);
									break;
		case PAUSE_TRANSPORT : 		// Display PAUSE bmp
									DisplayBmp(LCD_ZONE_TRANSPORT_X, LCD_ZONE_TRANSPORT_Y, LCD_INVERT, bmp_pause);
									break;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	DisplayModuleName()
// Inputs	: 	None
// Outputs	: 	None
// Process  : 	
//////////////////////////////////////////////////////////////////////////////////////////////
void DisplayModuleName(uchar x, uchar y)
{
	uchar i;

//	if(Z8_STATUS_4 & lcd_locked_by_function_z4) return;

	for(i=0; i<MODULE_NAME_LENGHT; i++)
		lcd_text_value[i] = module[current_module].module_name[i];

	lcd_text_value[MODULE_NAME_LENGHT] = '\0';
	DisplayLcdText(x, y, LCD_INVERT, font_5x8);
	
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	UpdateLcdHookState()
// Inputs	: 	None
// Outputs	: 	None
// Process  : 	
//////////////////////////////////////////////////////////////////////////////////////////////
void UpdateLcdHookState(uchar next_hook_state)
{
	rom uchar *local_bmp_ptr;

	if(Z8_STATUS_4 & lcd_locked_by_function_z4) return;

	if(next_hook_state != current_hook_state)
	{
		if(next_hook_state == CTRL_HOOKED)
			local_bmp_ptr = bmp_no_hook;
		else
		if(next_hook_state == CTRL_HIGHER)
			local_bmp_ptr = bmp_hook_down;
		else
			local_bmp_ptr = bmp_hook_up;

		DisplayBmp(LCD_ZONE_HOOK_ARROW_X, LCD_ZONE_HOOK_ARROW_Y, LCD_INVERT, local_bmp_ptr); 

		current_hook_state = next_hook_state;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	CreateStdLcdStr()
// Inputs	: 	None
// Outputs	: 	None
// Process  : 	create a std lcd str from control index, result in lcd_text_value[]
//////////////////////////////////////////////////////////////////////////////////////////////
void CreateStdLcdStr(uchar cc_nb)
{
	uchar tmp0, tmp1, tmp2;

	// First transform control index into a 3 digits
	IntToLcdText(cc_nb, DIGITS_3);
	// save this 3 digit text
	tmp0 = lcd_text_value[0];
	tmp1 = lcd_text_value[1];		
	tmp2 = lcd_text_value[2];		
	// Write Program or control depending on standard control type
	lcd_text_value[0] = 'C';		// default string is CC : 
	lcd_text_value[1] = 'C';		// default string is CC : value byte is cleared
	lcd_text_value[2] = ' ';		// default string is CC : value byte is cleared
	lcd_text_value[6] = ' ';		// default string is CC : value byte is cleared
	lcd_text_value[7] = ' ';		// default string is CC : value byte is cleared
	lcd_text_value[8] = '\0';		// default string is CC : value byte is cleared
	// here current_lcd_text contains "CC "
	lcd_text_value[3] = tmp0;
	lcd_text_value[4] = tmp1;		
	lcd_text_value[5] = tmp2;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	UpdateLcdString()
// Inputs	: 	None
// Outputs	: 	None
// Process  : 	
//////////////////////////////////////////////////////////////////////////////////////////////
void UpdateLcdString(void)
{
	uchar i;

	if(Z8_STATUS_4 & lcd_locked_by_function_z4) return;

	for(i=0; i<LCD_STR_MAX_LENGHT; i++)
		lcd_text_value[i] = ParamCurrent->lcd_string[i];
	lcd_text_value[LCD_STR_MAX_LENGHT]='\0';

	DisplayLcdText(LCD_ZONE_STRING_X, LCD_ZONE_STRING_Y, LCD_INVERT, font_5x8);
}


/////////////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	DisplayModuleInfos()
// Inputs	: 	None
// Outputs	: 	None
// Process  : 	
//////////////////////////////////////////////////////////////////////////////////////////////
void DisplayModuleInfos(void)
{
	rom uchar *local_bmp_ptr;
	uchar cc_nb ;

	if(Z8_STATUS_4 & lcd_locked_by_function_z4) return;

	cc_nb = CTRL_UNDEF;

	// must display lcd string
	UpdateLcdString();

	// must display module name :
	DisplayModuleName(LCD_ZONE_MODULE_NAME_X, LCD_ZONE_MODULE_NAME_Y);

	// Display the event type
	if(ParamCurrent->message_type == CC)
	{
		local_bmp_ptr = bmp_cc_txt;
		cc_nb = ParamCurrent->midi_string[1];
	}
	else
	if(ParamCurrent->message_type == NOTE)
	{
		local_bmp_ptr = bmp_note_txt;
		cc_nb = ParamCurrent->midi_string[1];
	}
	else
	if(ParamCurrent->message_type == NRPN)
		local_bmp_ptr = bmp_nrpn_txt;
	else
	if(ParamCurrent->message_type == SYSEX)
		local_bmp_ptr = bmp_sysex_txt;

	DisplayBmp(LCD_ZONE_EVENT_TYPE_X, LCD_ZONE_EVENT_TYPE_Y, LCD_INVERT, local_bmp_ptr); 

	// Display the midi channel for module
	DisplayLcdInt(LCD_ZONE_CHANNEL_MODULE_X, LCD_ZONE_CHANNEL_MODULE_Y, DIGITS_2|LCD_INVERT, font_5x8, ParamCurrent->midi_channel + 1 );

	// Display CC nb 
	if(cc_nb == CTRL_UNDEF)
	{
		DisplayBmp(LCD_ZONE_CC_NB_X, LCD_ZONE_CC_NB_Y, LCD_INVERT, bmp_no_cc_nb); 
	}
	else
		DisplayLcdInt(LCD_ZONE_CC_NB_X, LCD_ZONE_CC_NB_Y, DIGITS_3|LCD_INVERT, font_num_8x16, cc_nb);

}
