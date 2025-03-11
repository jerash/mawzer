#include <eZ8.h>
#include "pins_defs.h"
#include "commondef.h"
#include <common_z8_ez.h>
#include <ctrldef.h>
#include "hardware.h"
#include "lcd.h"
#include "lcd_low_level.h"
#include "eeprom.h"
#include "i2c.h"
#include "serial.h"
#include "realtime.h"
#include "onewire.h"
#include "autotest.h"
#include "smpte.h"
#include "assign.h"
#include "menus.h"

///////////////////////////////////////////////////////////////////////////////////////
// GLOBAL VARS
///////////////////////////////////////////////////////////////////////////////////////

uchar current_value;
uchar last_control_index;
uint tempo_reload_cpt;

uchar Z8_STATUS_0;
uchar Z8_STATUS_1;
volatile uchar Z8_STATUS_4;
volatile uchar Z8_STATUS_5;
volatile uchar Z8_STATUS_6;
volatile uchar Z8_STATUS_7;
volatile uchar Z8_STATUS_8;
uchar Z8_STATUS_9;
uchar Z8_STATUS_10;

uchar current_hook_state;
far uchar CtrlReg[CTRL_REG_REAL_NB];			// table of control reg for eerprom saving and menus 
far uchar CtrlReg_EE_img[CTRL_REG_REAL_NB];		// table of control reg for eerprom saving
far uchar generic_buffer[GENERIC_BUFF_SIZE];	// a generic far ram buffer of 64 bytes

far All_Params	 	Param[PARAM_STRUCT_IN_RAM_MAX_NB];	// structure of parameters in EEPROM V1.61
far All_Params	 	*ParamCurrent;						//current structure of parameters in EEPROM
uchar last_struct_index;								
far uchar control_index_structs_in_ram[PARAM_STRUCT_IN_RAM_MAX_NB];	

extern uchar midi_activity_cpt;
extern uchar lcd_lock_incrust_cpt;

extern far uchar lcd_text_value[MAX_LCD_TEXT_LENGHT];		// global buffer for integer to text conversion	
extern far uchar CTRL_REG_7BITS_FROM_EZ[CTRL_REG_MAX_FROM_EZ_TO_Z8];		// NN=1 registers can be accessed from ez board 
extern uint EE_Address;					// current EEProm address
extern uchar I2cCommand;					// current EEProm selection
extern uchar serial_in_start_index;		// a near pointer on FAR ram
extern uchar serial_in_end_index;		// a near pointer on FAR ram
extern uchar serial_in_current_index;

extern uchar TX_STATUS;
extern uchar tx0_start_index;
extern uchar tx0_end_index;
extern uchar tx1_start_index;
extern uchar tx1_end_index;
extern uchar realtime_generated_start_index;
extern uchar realtime_generated_end_index;
extern uchar realtime_received_start_index;
extern uchar realtime_received_end_index;
extern far 	 uint must_reinit_lcd_from_sysex;
extern uchar lcd_current_incrust;
extern uchar current_realtime_state;
// extern uchar current_realtime_bmp_state;
extern uint  seconds_5_timer_cpt;		// used to create a 5 seconds timer from timer 0
extern uchar current_onewire_bus;
extern uchar menu_state;
extern uchar control_index;
extern uchar encoder_but_lock_cpt;
extern rom uint  smpte_reload_cpt[SMPTE_FRAME_RATE_NB][3];
extern far SMPTE_STRUCT smpte_code;
extern uchar current_smpte_reload_cpt;
extern uchar assign_state;
extern uchar smpte_lock_cpt;
//extern uchar current_smpte_index;
extern uchar up_down_action;

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: InitIos()
// Inputs	: None
// Outputs	: None
// Process  : Initilize Z8 IO ports direction and default values and alternate funtions
///////////////////////////////////////////////////////////////////////////////////////
void InitIos(void)
{
	// PORTA DIRECTION, ALTERNATE FUNCTIONS AND DEFAULT VALUES
    PAADDR 	= PORT_DIR;   		// Acces to sub-reg direction
  	PACTL 	= 0x9B;    			// Port direction 1=in 0=out
	PAADDR 	= PORT_ALT_FCT;     // Acces to sub-reg alternate_functions
  	PACTL 	= 0xF4;    			// Port alternate functions enable 1

	// PORTB DIRECTION, ALTERNATE FUNCTIONS AND DEFAULT VALUES
	// all portB is alternate function, do not init direction nor default values
    PBADDR 	= PORT_DIR;   		// Acces to sub-reg direction
  	PBCTL 	= 0x07;    			// Port direction 1=in 0=out
	PBADDR 	= PORT_ALT_FCT;     // Acces to sub-reg alternate_functions
  	PBCTL 	= 0x00;    			// Port alternate functions enable 1=in 0=out
	PBOUT   = 0xE0;

	// PORTC DIRECTION, ALTERNATE FUNCTIONS AND DEFAULT VALUES
    PCADDR 	= PORT_DIR;     	// Acces to sub-reg direction
  	PCCTL 	= 0xFF;    			// Port direction 1=in 0=out
	PCADDR 	= PORT_ALT_FCT;     // Acces to sub-reg alternate_functions
  	PCCTL 	= 0x00;    			// Port alternate functions enable 1=in 0=out

	// PORTD DIRECTION, ALTERNATE FUNCTIONS AND DEFAULT VALUES
    PDADDR 	= PORT_DIR;     	// Acces to sub-reg direction
  	PDCTL 	= 0x9F;    			// Port direction 1=in 0=out
    PDADDR 	= PORT_ALT_FCT; 	// Acces to sub-reg alternate_functions
  	PDCTL 	= 0x30;    			// Port alternate functions enable 1=in 0=out
	PDOUT   = 0x00;

	// PORTE DIRECTION, ALTERNATE FUNCTIONS AND DEFAULT VALUES
    PEADDR 	= PORT_DIR;    		// Acces to sub-reg direction
  	PECTL 	= 0x37;    			// Port direction 1=in 0=out
	// No alternate functions on this port
   	PEOUT	= 0X00;     		// Default out values on port

	// PORTF DIRECTION, ALTERNATE FUNCTIONS AND DEFAULT VALUES
    PFADDR 	= PORT_DIR;    		// Acces to sub-reg direction
  	PFCTL 	= 0xFF;    			// Port direction 1=in 0=out

	// PORTG DIRECTION, ALTERNATE FUNCTIONS AND DEFAULT VALUES
    PGADDR 	= PORT_DIR;    		// Acces to sub-reg direction
  	PGCTL 	= 0xFF;    			// Port direction 1=in 0=out
 
	// PORTH DIRECTION, ALTERNATE FUNCTIONS AND DEFAULT VALUES
	// all portH is alternate function, do not init direction nor default values
    PHADDR 	= PORT_DIR;    		// Acces to sub-reg direction
  	PHCTL 	= 0xF8;    			// Port direction 1=in 0=out
	PHADDR 	= PORT_ALT_FCT;     // Acces to sub-reg alternate_functions
  	PHCTL 	= 0x00;    			// Port alternate functions enable 1=in 0=out
   	PHOUT	= 0X00;     		// Default out values on port
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: InitTimers()
// Inputs	: None
// Outputs	: None
// Process  : Initialize all timers 
///////////////////////////////////////////////////////////////////////////////////////
void InitTimers(void)
{
	// Init Timer 0 : time base timer, continuous, overload at 10ms
	// used to debounce switches
	T0RH  = 0x06;	// Timer 0 reload value High (1562d for 10ms)
	T0RL  = 0x1A;	// Timer 0 reload value Low  (1562d for 10ms)
	T0CTL1 = 0xB9;	// Timer 0 continuous, auto reload, divide per 128

	// Init Timer 1 : time base timer, single shot, overload at 10ms (lock eeprom)
	// used to lock eeprom
	ReinitTimer1();
	T1CTL1 = 0x38;	// Timer 1 single shot, not started, divide per 128

	// Timer 2 : used to generate internal tempo
	ReinitTimer2();

	// Timer 3 for onewire timetout detect
	T3CTL1 = 0x38;	// Timer 3 single shot, not started, divide per 128
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: ReinitTimer1()
// Inputs	: None
// Outputs	: None
// Process  : Initialize timer 1 
///////////////////////////////////////////////////////////////////////////////////////
void ReinitTimer1(void)
{
	// Start timer 1 value is 0
	T1H   	= 0x01;	// Timer 1 start value High (1562d for 10ms)
	T1L   	= 0x01;	// Timer 1 start value Low  (1562d for 10ms)
	// Reload timer 1 counter
	T1RH   	= 0x06;	// Timer 1 reload value High (1562d for 10ms)
	T1RL   	= 0x1A;	// Timer 1 reload value Low  (1562d for 10ms)
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	ReinitTimer2()
// Inputs	:	None
// Outputs	: 	None
// Process  : 	Initialize timer 2
//				internal_tempo is 0-220, treat it as 30 - 250 (add 30)
//				tempo to ticks = bpm x 24 / 60 = bpm x 0.4
//				timer2 reload counter = (20000000/(bpm x 0.4))/128
//				timer2 reload counter = 156250 / (bpm x 0.4) = 390625 / bpm
// 
//				LATER SUPPORT FOR 0.1 step on tempo :
//				ex : 133.2 :	calculate tmp1 for 133 = 2937.03
//								calculate tmp2 for 134 = 2915.11
//								step = (tmp1 - tmp2) / 10 = 2.192
//								remove n times step for 0.n tempo
//								on the example above remove 4 from 2937
///////////////////////////////////////////////////////////////////////////////////////
void ReinitTimer2(void)
{
//	uint tmp;

	// depending on midi sync : enable or disable timer 2 since generating internal sync clock/smpte
	if(Z8_STATUS_0_NVM & mtc_mode_on_off_z0nvm)
		T2CTL1 = 0x39;	// Timer 2 continuous, not started, divide per 128
	else
	{
		// Start timer 2 value is 0
		T2H   	= 0x01;	// Timer 2 start value High 
		T2L   	= 0x01;	// Timer 2 start value Low  
		// Depending on sync mode; reload timer 2 with different values
		// if midi sync is configured as MIDI clock
		if(!(Z8_STATUS_0_NVM & mtc_mode_z0nvm))
		{
			// Reload timer 2 counter
			tempo_reload_cpt = (uint)(390625L / (uint)(internal_tempo + 30));
			// In Midi clock mode : start timer
			T2CTL1 = 0xB9;	// Timer 2 continuous, started, divide per 128
		}
		else
		// SMPTE Mode
		{
			// Depending on frame rate, load timer with different values
			tempo_reload_cpt = (uint)smpte_reload_cpt[smpte_code.type][current_smpte_reload_cpt];
			// Stop timer in smpte mode
			T2CTL1 = 0x39;	// Timer 2 continuous, not started, divide per 128
		}

		T2RH   	= (uchar)(tempo_reload_cpt >> 8);	// Timer 2 reload value High (1562d for 10ms)
		T2RL   	= (uchar)(tempo_reload_cpt & 0xFF);	// Timer 2 reload value Low  (1562d for 10ms)

	}
}



///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	delay_usec()
// Inputs	: 	delay_time in usec
// Outputs	: 	None
// Process  : 	generic delay function in usec
//				1 cycle is 50ns @ 20 MHz
//				1 instruction is 2 cycles = 100 ns
///////////////////////////////////////////////////////////////////////////////////////
void delay_usec(uchar delay_time)
{
#ifndef SIMUL
	uchar i;
	for(i=0;i<delay_time;i++)		// i x 1usec delay
	{
		asm("nop"); asm("nop");		// This is the 1usec delay
		asm("nop"); asm("nop");
	}
#endif
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: delay_msec()
// Inputs	: delay_time in msec
// Outputs	: None
// Process  : generic delay function in msec
///////////////////////////////////////////////////////////////////////////////////////
void delay_msec(unsigned int delay_time)
{
#ifndef SIMUL
	unsigned int i;
	uchar j;

	for(i=0;i<delay_time;i++)		// i x 1usec delay
	  for(j=0;j<4;j++)				// this is the 1 msec delay
	  	delay_usec(250);
#endif
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: CheckParamChangedFromItr()
// Inputs	: None
// Outputs	: None
// Process  : Check if some params have to be updated from ITRs
///////////////////////////////////////////////////////////////////////////////////////
void CheckParamChangedFromItr(void)
{
	// Must turn off midi activity bmp ?
	if(Z8_STATUS_5 & must_toggle_midi_activity_z5)
	{
		Z8_STATUS_5 &= ~must_toggle_midi_activity_z5;
		Z8_STATUS_5 ^=  midi_activity_z5;
		DisplayMidiActivityBmp();
	}

	// Check if internal tempo must be updated ?
	if(Z8_STATUS_5 & tempo_changed_z5)
	{
		// Reload timer with new tempo value
//		ReinitTimer2();
		tempo_reload_cpt = (uint)(390625L / (uint)(internal_tempo + 30));
		Z8_STATUS_4 |= must_reload_tempo_timer_z4;
		// Update display with new tempo
		UpdateLcdZones(INIT_LCD_TEMPO);
		Z8_STATUS_5 &= ~tempo_changed_z5;
	}

	// CHECK IF GROUP CHANGED
	if(Z8_STATUS_4 & must_change_group_z4)
	{
		Z8_STATUS_4 &= (~must_change_group_z4);
		// Update value according to its direction
		if(Z8_STATUS_4 & must_increase_group_z4)
		{
			current_group++;
			if(current_group >= GROUP_NB)
				current_group = 0;
		}
		else
		{
			if(current_group == 0)
				current_group = GROUP_NB-1;
			else
				current_group--;
		}

		// group changed, this will force to update lcd str, and reload midi structure
		last_control_index = CTRL_UNDEF;

		// Display new group on LCD
		UpdateLcdZones(INIT_LCD_GROUP);

		// Here group has changed : must send info to ALL connected modules		
		SendCurrentGroupToAllModules();

		// Will need to reload all structures from eeprom
		InitParamStructsInRam();


	} // end group changed

	//////////////////////////////////// CHECK MUST INCREASE SMPTE TIME
	///////////////////////////////////////////////////////////////////
	if(Z8_STATUS_7 & must_inc_1_smpte_frame_z7)
	{
		// T2CTL1 = 0x19;
		// Increase smpte time by 4 frames and display
		IncreaseDisplaySmpteTime(4);
		// Send a full smpte frame
		// if(!(T2CTL1 & 0x80))
		smpte_lock_cpt = 0;
		SendSmpteToMidi(FULL_SMPTE);
		//current_smpte_index = SMPTE_FRAME_LOW_NIBBLE;
		//SendSmpteToMidi(UPDATE_QUARTER);
		// T2CTL1 = 0x99;
//		smpte_lock_cpt = SMPTE_LOCK_TIME;
		// reset flag
		Z8_STATUS_7 &= ~must_inc_1_smpte_frame_z7;
	}

	//////////////////////////////////// CHECK MUST DECREASE SMPTE TIME
	///////////////////////////////////////////////////////////////////
	if(Z8_STATUS_7 & must_dec_1_smpte_frame_z7)
	{
		T2CTL1 = 0x19;
		// Increase smpte time by 1 frame and display
		DecreaseDisplaySmpteTime(4);
		// Send a full smpte frame
		smpte_lock_cpt = 0;
		SendSmpteToMidi(FULL_SMPTE);
		//current_smpte_index = SMPTE_FRAME_LOW_NIBBLE;
		// SendSmpteToMidi(UPDATE_QUARTER);
		smpte_lock_cpt = SMPTE_LOCK_TIME;
		// reset flag
		Z8_STATUS_7 &= ~must_dec_1_smpte_frame_z7;
		if(current_realtime_state == PLAY_TRANSPORT)
		 	T2CTL1 = 0x99;
	}

	// CHECK IF SERIAL IN OVERFLOW OCCURED FROM EZ OR FROM ONEWIRE
	if( (Z8_STATUS_8 & serial_in_overflow_z8) || (Z8_STATUS_8 & onewire_rx_overflow_z8))
	{
		DisplayInputOverflowIncrust();
		Z8_STATUS_8 &= (~serial_in_overflow_z8);
		Z8_STATUS_8 &= (~onewire_rx_overflow_z8);
	}

	////////////////////////////////////////////////////////////////////
	/////// CHECK IF MUST REINIT LCD ZONES FROM INCOMING SYSEX (COMMON PARAM)
	// Need to update LCD after a common param programming, not all zone reinitialized since
    // lcd zone incrust is reinitialized for common param result upload/download
	if(must_reinit_lcd_from_sysex)
	{
		UpdateLcdZones( (uint)(must_reinit_lcd_from_sysex) );
		must_reinit_lcd_from_sysex = 0;
	}

	//////////////////////////////////// CHECK IF AUTOASSIGN ALL MODULES
	////////////////////////////////////////////////////////////////////
	if(Z8_STATUS_1_NVM & must_autoassign_z1nvm)
	// Must be out of menus to start autoassign
	if(menu_state == NO_MENU)
	{
		// Launch autoassign 
		AutoAssignAllModules();
		// reinit lcd 
		Z8_STATUS_5 |= must_reinit_all_lcd_z5;
		// unflag
		Z8_STATUS_1_NVM &= ~must_autoassign_z1nvm;
	}

	//////////////////////////////////// CHECK IF LCD FULL RE-INIT
	//////////////////////////////////////////////////////////////
	if(Z8_STATUS_5 & must_reinit_all_lcd_z5)
	{
		// no reinit after this one
		Z8_STATUS_5 &= (~must_reinit_all_lcd_z5);
		// lcd no more locked by menus
		Z8_STATUS_4 &= (~lcd_locked_by_function_z4);
		// Clear display
		FillLcdZone(0, 0, LCD_X_PIXEL_MAX, LCD_Y_PAGE_MAX, FILL_BLANK);
		// Reinit LCD normal mode
		UpdateLcdZones( (uint)( INIT_LCD_ALL_ZONES ));

		// Also reinit modules infos (last displayed)
		// TO COMPLETE

		// No more incrust zone
		lcd_current_incrust = LCD_NO_INCRUST;
	}

	///////////////////////////////// CHECK IF LCD INCRUST RE-INIT
	//////////////////////////////////////////////////////////////
	if(Z8_STATUS_5 & must_reinit_incrust_lcd_z5)
	{
		// no more active inscrust zone
		lcd_current_incrust = LCD_NO_INCRUST;
		Z8_STATUS_5 &= (~must_reinit_incrust_lcd_z5);
		// reinit incrust zone only if not in menus mode
		if(!(Z8_STATUS_4 & lcd_locked_by_function_z4))
		{
			// Clear display over incrust zone only
			FillLcdZone(LCD_ZONE_INCRUST_X, LCD_ZONE_INCRUST_Y, LCD_ZONE_INCRUST_X_PIX, LCD_ZONE_INCRUST_Y_PAGE, FILL_BLANK);
			// Reinit LCD normal mode TO COMPLETE
 			UpdateLcdZones( (uint)( INIT_LCD_ALL_ZONES ));
			// No more incrust zone
			lcd_current_incrust = LCD_NO_INCRUST;
		}
	}

	//////////////////////////////////// CHECK IF VIDEO MODE CHANGED
	////////////////////////////////////////////////////////////////
	// Update new video mode only if not in menu
	if(menu_state == NO_MENU)
	{
		if(Z8_STATUS_0_NVM & video_mode_z0nvm)
		{
			if(!(Z8_STATUS_0_NVM & new_video_mode_z0nvm))
			{
				Z8_STATUS_0_NVM &= (~video_mode_z0nvm);
				// update full LCD since video mode changed
				Z8_STATUS_5 |= must_reinit_all_lcd_z5;
			}
		}
		else
		{
			if(Z8_STATUS_0_NVM & new_video_mode_z0nvm)
			{
				Z8_STATUS_0_NVM |= video_mode_z0nvm;
				// update full LCD since video mode changed
				Z8_STATUS_5 |= must_reinit_all_lcd_z5;
			}
		}
	}

}


///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: InitInitialValues()
// Inputs	: None
// Outputs	: None
// Process  : Initialize all initial values 
///////////////////////////////////////////////////////////////////////////////////////
void InitInitialValues(void)
{
	uchar index;

	// Must be initialized in the same way of EZ board 
	CTRL_REG_7BITS_FROM_EZ[0] = 0;

	current_hook_state = CTRL_HOOKED;
	control_index = 0;
	last_control_index = CTRL_UNDEF;

	encoder_but_lock_cpt = 0;

	// Init menus 
	menu_state = NO_MENU;
	assign_state = ASSIGN_OFF;
	up_down_action = CHANGE_MENU_INDEX;

	// realtime buffers indexes
	realtime_generated_start_index	= 0;
	realtime_generated_end_index	= 0;
	realtime_received_start_index	= 0;
	realtime_received_end_index 	= 0;
	current_realtime_state			= STOP_TRANSPORT;
//	current_realtime_bmp_state		= STOP_TRANSPORT;

	must_reinit_lcd_from_sysex 		= 0;
	midi_activity_cpt				= 0;
	lcd_lock_incrust_cpt			= 0;

   	// read all control registers in a single bloc read
	I2cCommand	= I2C_SLAVE_ADR_EE + EEPROM_ZERO_I2C_ADR;
	EE_Address  = (uint)CTRL_REG_START_EE_ADR;
	ReadMultiByteI2c(&CtrlReg[0], CTRL_REG_REAL_NB);

	// Check that group read from eeprom is within the valid range
	if(current_group >= GROUP_NB)
   	   current_group = 0;

	// Check that tempo read from eeprom is within the valid range
	if(internal_tempo >= MAX_TEMPO_MINUS_30)
   	   internal_tempo = 0;

	// reset autoassign flag if it was set
	Z8_STATUS_1_NVM &= ~must_autoassign_z1nvm;

	// Now all controls regs are defined and clean also assign control regs to control regs ee img
	for(index=0; index<CTRL_REG_REAL_NB; index++)
		CtrlReg_EE_img[index] = CtrlReg[index];

	// Initialize 5 seconds timer
	seconds_5_timer_cpt = (uint)CPT_5_SEC;

	Z8_STATUS_0  		= 0xFF;			// all ui buttons are already passed to zero
	Z8_STATUS_1  		= 0xFF;			// all transport buttons are already passed to zero
	Z8_STATUS_4  		= first_pass_z4;	// first pass flag is on at the begining
	Z8_STATUS_5  		= 0;
	Z8_STATUS_6			= 0;
	Z8_STATUS_7			= 0;
	Z8_STATUS_8  		= 0;
	Z8_STATUS_10 		= 0;

	TX_STATUS			= 0;
	tx0_start_index    	= 0;
	tx0_end_index   	= 0;
	tx1_start_index    	= 0;
	tx1_end_index   	= 0;

	// no incrust on LCD at beginning
	lcd_current_incrust = LCD_NO_INCRUST;

	ParamCurrent = &Param[0];
	last_struct_index = 0;	
	InitParamStructsInRam();
}


///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: CheckCtrlRegToSendToEz()
// Inputs	: None
// Outputs	: None
// Process  : check if a new EZBOARD param has to be sent to EZ board
///////////////////////////////////////////////////////////////////////////////////////
void CheckCtrlRegToSendToEz(void)
{
	uchar ctrl_reg_index;

	// V1.2 control regs are located in first eeprom
	I2cCommand  = I2C_SLAVE_ADR_EE + EEPROM_ZERO_I2C_ADR; 

	// If CTRL_REG_N_7BITS_TO_EZ has changed, need to send it to the EZ board
	for(ctrl_reg_index = CTRL_REG_TO_EZ_START_INDEX; ctrl_reg_index <= LAST_CTRL_REG_7BITS_TO_EZ_INDEX; ctrl_reg_index++)
	{
		if(CtrlReg_EE_img[ctrl_reg_index] != CtrlReg[ctrl_reg_index])
		{
			// get eeprom current control reg address			
			EE_Address = (uint)((uint)CTRL_REG_START_EE_ADR + (uint)ctrl_reg_index);
			// write new reg to eeprom
			WriteByteI2c(CtrlReg[ctrl_reg_index]);
			// update old reg with new reg value 
			CtrlReg_EE_img[ctrl_reg_index] = CtrlReg[ctrl_reg_index];

			// Also send to EZ board
			SendCtrlRegToEzBoard(ctrl_reg_index);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: CheckCtrlRegToSave5Sec()
// Inputs	: None
// Outputs	: None
// Process  : Each 5 seconds : check if controls reg have to be saved to EEPROM
///////////////////////////////////////////////////////////////////////////////////////
void CheckCtrlRegToSave5Sec(void)
{
	uchar ctrl_reg_index;

	// 5 seconds passed ?
	if(!seconds_5_timer_cpt)
	{
		// EEprom is the first one for the ctrl registers 
		I2cCommand	= I2C_SLAVE_ADR_EE + EEPROM_ZERO_I2C_ADR;

		// Now check all other control registers
		// skip both control regs to EZ board since treated separately
		for(ctrl_reg_index = Z8_STATUS_0_NVM_INDEX; ctrl_reg_index < CTRL_REG_REAL_NB; ctrl_reg_index++)
		{
			if(CtrlReg_EE_img[ctrl_reg_index] != CtrlReg[ctrl_reg_index])
			{
				// get eeprom current control reg address
				EE_Address = (uint)(CTRL_REG_START_EE_ADR + (uint)ctrl_reg_index);
				// write new reg to eeprom
				WriteByteI2c(CtrlReg[ctrl_reg_index]);
				// update old reg with new reg value 
				CtrlReg_EE_img[ctrl_reg_index] = CtrlReg[ctrl_reg_index];
			}
		}

		// reload 5 seconds counter
		seconds_5_timer_cpt = (uint)CPT_5_SEC;
	}
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: InitParamStructsInRam()
// Inputs	: None
// Outputs	: None
// Process  : 
///////////////////////////////////////////////////////////////////////////////////////
void InitParamStructsInRam(void)
{
	uchar index;

	// undefine all already loaded structs in ram (index similar but structures differents)
	for(index=0; index<PARAM_STRUCT_IN_RAM_MAX_NB; index++)
	{
		control_index_structs_in_ram[index] = CTRL_UNDEF;
	}
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: SyncToEzBoard()
// Inputs	: None
// Outputs	: None
// Process  : 
///////////////////////////////////////////////////////////////////////////////////////
void SyncToEzBoard(void)
{
	uchar index;

	// Initialize serial In indexes
	serial_in_start_index 	= 0;
	serial_in_end_index		= 1;
	serial_in_current_index	= 0;

	// turn on itrs from rx1 from ez board
	U1CTL1 = 0x00; 					// itr on rx, brg normal, 8 bits, no infrared

	// be sure LAST_CTRL_REG_7BITS_TO_EZ indicates to EZ board it has been fully initialized
	LAST_CTRL_REG_7BITS_TO_EZ =EZ_FULLY_UPDATED;
	
	for(index=CTRL_REG_TO_EZ_START_INDEX; index <= LAST_CTRL_REG_7BITS_TO_EZ_INDEX; index++)
		SendCtrlRegToEzBoard(index);

	// Wait for the registers sent by the EZ board
	do 
	{
		CheckSerialInBuffer();

		// Uncomment the following line to skip EZ-Z8 synchronization
#ifdef _DEBUG
	 	CTRL_REG_7BITS_FROM_EZ[0] |= z8_board_fully_updated_from_ez0;
#endif
	} 
	while (!(CTRL_REG_7BITS_FROM_EZ[0] & z8_board_fully_updated_from_ez0));

}


