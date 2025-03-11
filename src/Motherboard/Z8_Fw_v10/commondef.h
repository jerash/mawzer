#ifndef COMMONDEF
#define COMMONDEF

///////////////////////////////////////////////////////////
// Common definitions
///////////////////////////////////////////////////////////
//#define Z8_COMPIL 	1

typedef unsigned char uchar;
typedef unsigned int uint;
#define NULL 0

//////////////////////////////////////////////////////////////////////////////
// REGISTERS THAT MUST BE SAVED INTO NVM
//////////////////////////////////////////////////////////////////////////////

// Control register definitions
#define CTRL_REG_0_7BITS_TO_EZ_EE_IMG		(CtrlReg_EE_img[CTRL_REG_0_7BITS_TO_EZ_INDEX])
#define CTRL_REG_1_7BITS_TO_EZ_EE_IMG		(CtrlReg_EE_img[CTRL_REG_1_7BITS_TO_EZ_INDEX])
#define LAST_CTRL_REG_7BITS_TO_EZ_EE_IMG	(CtrlReg_EE_img[LAST_CTRL_REG_7BITS_TO_EZ_INDEX])
#define Z8_STATUS_0_NVM_EE_IMG				(CtrlReg_EE_img[Z8_STATUS_0_NVM_INDEX])
#define Z8_STATUS_1_NVM_EE_IMG				(CtrlReg_EE_img[Z8_STATUS_1_NVM_INDEX])
#define current_channel_EE_IMG				(CtrlReg_EE_img[CHANNEL_INDEX])
#define current_group_EE_IMG				(CtrlReg_EE_img[GROUP_INDEX])
#define internal_tempo_EE_IMG				(CtrlReg_EE_img[INT_TEMPO_INDEX])

#define CTRL_REG_0_7BITS_TO_EZ				(CtrlReg[CTRL_REG_0_7BITS_TO_EZ_INDEX])
#define CTRL_REG_1_7BITS_TO_EZ				(CtrlReg[CTRL_REG_1_7BITS_TO_EZ_INDEX])
#define LAST_CTRL_REG_7BITS_TO_EZ			(CtrlReg[LAST_CTRL_REG_7BITS_TO_EZ_INDEX])
#define Z8_STATUS_0_NVM						(CtrlReg[Z8_STATUS_0_NVM_INDEX])
#define Z8_STATUS_1_NVM						(CtrlReg[Z8_STATUS_1_NVM_INDEX])
#define current_channel						(CtrlReg[CHANNEL_INDEX])
#define current_group						(CtrlReg[GROUP_INDEX])
#define internal_tempo						(CtrlReg[INT_TEMPO_INDEX])

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

// Z8_STATUS_0 bits definition (only use this register for buttons since init to 0xFF)
#define enc1_button_passed_0_z0			(1<<5)	// 1 if button passed zero
#define cancel_passed_0_z0				(1<<4)	// 1 if button passed zero
#define ok_passed_0_z0					(1<<3)	// 1 if button passed zero
#define down_passed_0_z0				(1<<2)	// 1 if button passed zero
#define up_passed_0_z0					(1<<1)	// 1 if button passed zero
#define shift_passed_0_z0				(1<<0)	// 1 if button passed zero

// Z8_STATUS_1 bits definition (only use this register for buttons since init to 0xFF)
#define stop_passed_0_z1				(1<<4)	// 1 if button passed zero
#define rec_passed_0_z1					(1<<3)	// 1 if button passed zero
#define play_passed_0_z1				(1<<2)	// 1 if button passed zero
#define fw_passed_0_z1					(1<<1)	// 1 if button passed zero
#define bw_passed_0_z1					(1<<0)	// 1 if button passed zero


// Z8_STATUS_4 bits definition
#define must_reload_tempo_timer_z4		(1<<7)	// 1 if tempo changed
#define must_change_group_z4			(1<<6)	// 1 if group changed
#define eeprom_locked_10ms_z4			(1<<5)	// allow to lock eeprom durring 10ms
#define first_pass_z4					(1<<4)	// allow to detect first pass
#define must_increase_group_z4			(1<<3)	// 1 if group must be increased
#define searching_modules_z4			(1<<2)	// 1 if currently searching modules
#define module_answered_z4				(1<<1)	// 1= module is found
#define lcd_locked_by_function_z4		(1<<0)	// 1= lcd is locked if menus/assign/motion active

// Z8_STATUS_5 bits definition
#define waiting_timeout_onewire_z5		(1<<7)	// 1 midi activity bmp
#define midi_activity_z5				(1<<6)	// 1 midi activity bmp
#define must_toggle_midi_activity_z5	(1<<5)	// 1 when midi activity bmp must be toggled
#define serial_in_state_z5				(1<<4)	// 0=waiting SOF / 1=waiting EOF
#define onewire_rx_state_z5				(1<<3)	// 0=waiting SOF / 1=waiting EOF
#define tempo_changed_z5				(1<<2)	// 1 if the tempo must be updated
#define must_reinit_incrust_lcd_z5		(1<<1)	// 1 if the incrust zone must be initialized
#define must_reinit_all_lcd_z5			(1<<0)	// must reinit the whole lcd

// Z8_STATUS_6 bits definition
#define encoder_rising_state_z6			(1<<7)	// the state of encoder B pin on A pin rising edge itr
#define must_send_smpte_quarter_z6		(1<<6)	// 1: must send new smpte quarter frame
#define ok_sw_changed_z6				(1<<5)	// 1: ok sw pressed
#define disable_ow_rx_z6				(1<<4)	// 1: disable readback on onewire rx
#define up_down_sw_changed_z6			(1<<3)	// 1: up OR dwn sw has been pressed
#define up_down_sw_direction_z6			(1<<2)	// 1: up pressed else down pressed
#define shift_state_z6					(1<<1)	// 1 = shift active 
#define i2c_timout_z6					(1<<0)	// 1 if a i2c timeout has occured

// Z8_STATUS_7 bits definition
#define must_inc_1_smpte_frame_z7		(1<<7)	// must increase smpte time by 1 frame
#define must_dec_1_smpte_frame_z7		(1<<6)	// must decrease smpte time by 1 frame
#define must_save_assign_struct_z7		(1<<5)	// must save assigned struct if 1
#define assign_type_z7					(1<<4)	// 1: assign module name / 0 : assign control
#define must_save_preset_name_z7		(1<<3)	// 1: must save preset name from menu edition


#define CPT_5_SEC			500L
#define CPT_2_5_SEC			255U	
#define CPT_2_SEC			200U
#define CPT_1_SEC			100U

#define FALSE				0U
#define TRUE				1U
#define SHIFT_HIGH_STEP		5U

enum {	LCD_NO_INCRUST, LCD_INCRUST_MIDI_OVERFLOW, LCD_INCRUST_SERIAL_IN_OVERFLOW, LCD_INCRUST_ONEWIRE_IN_OVERFLOW,
	 	LCD_INCRUST_UNVALID_SYSEX, LCD_INCRUST_UNVALID_ONEWIRE_FRAME, LCD_INCRUST_COMMON_PARAM, LCD_INCRUST_UNVALID_MIDI_STRUCT }

enum {NO_MENU, MENU_PARAM, MENU_STATE_NB};

#define PARAM_STRUCT_IN_RAM_MAX_NB	8U	// max nb of param struct in ram

#define MAX_TEMPO_MINUS_30	220U

#endif