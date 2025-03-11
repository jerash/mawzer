#include <eZ8.h>

#ifdef _MODULE_POT
	#include "pins_defs_POT.h"
	#include "commondef.h"
	#include "onewire.h"
	#include "controls_POT.h"
	#include "hardware.h"
	#include "ios_POT.h"
#endif

#ifdef _MODULE_FAD
	#include "pins_defs_FAD.h"
	#include "commondef.h"
	#include "onewire.h"
	#include "controls_FAD.h"
	#include "hardware.h"
	#include "ios_FAD.h"
#endif

#ifdef _MODULE_BUT
	#include "pins_defs_BUT.h"
	#include "commondef.h"
	#include "onewire.h"
	#include "controls_BUT.h"
	#include "hardware.h"
	#include "ios_BUT.h"
#endif

#ifdef _MODULE_ENC
	#include "pins_defs_ENC.h"
	#include "commondef.h"
	#include "onewire.h"
	#include "controls_ENC.h"
	#include "hardware.h"
	#include "ios_ENC.h"
#endif

#include "i2c.h"
#include "eeprom.h"

///////////////////////////////////////////////////////////////////////////////////////
// GLOBAL VARS
///////////////////////////////////////////////////////////////////////////////////////
uchar Z8_STATUS_0;	
uchar Z8_STATUS_1;	
uchar Z8_STATUS_4;	
uchar Z8_STATUS_8;	

uchar TX_STATUS;
uchar tx0_start_index;
uchar tx0_end_index;
far uchar TX0_Buff[TX_BUFF_LENGTH];

far uchar module_sub_id;
far uchar assignment[CONTROL_NB][GROUP_NB];

extern uchar onewire_rx_start_index;						// a near pointer on FAR ram
extern uchar onewire_rx_end_index;							// a near pointer on FAR ram
extern uchar onewire_rx_current_index;
extern uchar module_name[MODULE_NAME_LENGHT];

extern uint  EE_Address;
extern far uchar generic_buffer[GENERIC_BUFF_SIZE];  // a generic far ram buffer of 64 bytes

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: GetModuleSubID()
// Inputs	: None
// Outputs	: None
// Process  : store module sub id from dip switch selector to module_sub_id
///////////////////////////////////////////////////////////////////////////////////////
void GetModuleSubID(void)
{
	uchar j;
	signed char i;
	uchar msk = 1;
	module_sub_id = 0;

	// scan all bits
	// for(i=MODULES_SUB_ID_NB_BITS-1; i>=0; i--)
	for(j=0; j<MODULES_SUB_ID_NB_BITS; j++)	
	{
		i = MODULES_SUB_ID_NB_BITS - j ;

		// Change external mux value
		MUX_PORT_OUT &= ~MUX_PORT_MSK;
		MUX_PORT_OUT |= (i << 3) ;

		// Wait for the mux setting time
		delay_msec(2);

		// bit is 1 ?
		if(!(SUB_ID_PORT_IN & SUB_ID_PIN))
			module_sub_id |= msk;

		msk = msk << 1;
	}

	// limit value from 0 to MODULE_SUB_ID_MAX
	if(module_sub_id >= MODULES_SAME_MAX_NB) 
		module_sub_id = MODULES_SAME_MAX_NB-1;	

}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: InitIos()
// Inputs	: None
// Outputs	: None
// Process  : Initilize Z8 IO ports direction and default values and alternate funtions
///////////////////////////////////////////////////////////////////////////////////////
void InitIos(void)
{
	// PORTA DIRECTION, ALTERNATE FUNCTIONS AND DEFAULT VALUES
    PAADDR 	= PORT_DIR;   			// Acces to sub-reg direction
  	PACTL 	= MODULE_PORT_A_DIR;    // Port direction 1=in 0=out
	PAADDR 	= PORT_ALT_FCT;     	// Acces to sub-reg alternate_functions
  	PACTL 	= MODULE_PORT_A_ALT;   	// Port alternate functions enable (i2c, serial 0)

	// PORTB DIRECTION, ALTERNATE FUNCTIONS AND DEFAULT VALUES
	// all port B is alternate function, do not init direction nor default values
    PBADDR 	= PORT_DIR;   			// Acces to sub-reg direction
  	PBCTL 	= MODULE_PORT_B_DIR;    // Port direction 1=in 0=out
	PBADDR 	= PORT_ALT_FCT;     	// Acces to sub-reg alternate_functions
  	PBCTL 	= MODULE_PORT_B_ALT;   	// Port alternate functions enable 1=in 0=out

	// PORTC DIRECTION, ALTERNATE FUNCTIONS AND DEFAULT VALUES
    PCADDR 	= PORT_DIR;     		// Acces to sub-reg direction
  	PCCTL 	= MODULE_PORT_C_DIR;    // Port direction 1=in 0=out
	// Port alternate functions not used on this port
   	PCOUT	= MODULE_PORT_C_DEFAULT; // Default out values on port, cs2 is high
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
//	T0RH  = 0x06;	// Timer 0 reload value High (1562d for 10ms)
//	T0RL  = 0x1A;	// Timer 0 reload value Low  (1562d for 10ms)
//	T0CTL = 0xB9;	// Timer 0 continuous, auto reload, divide per 128

	// Init Timer 1 : time base timer, single shot, overload at 10ms (lock eeprom)
	// used to lock eeprom
	ReinitTimer1();
	T1CTL = 0x38;	// Timer 1 single shot, not started, divide per 128
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
	T1H   	= 0x00;	// Timer 1 start value High (1562d for 10ms)
	T1L   	= 0x00;	// Timer 1 start value Low  (1562d for 10ms)
	// Reload timer 1 counter
	T1RH   	= 0x06;	// Timer 1 reload value High (1562d for 10ms)
	T1RL   	= 0x1A;	// Timer 1 reload value Low  (1562d for 10ms)
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
// Routine 	: InitSerial()
// Inputs	: None
// Outputs	: None
// Process  : Initilize Z8 Serials periferals I2C and UARTS
///////////////////////////////////////////////////////////////////////////////////////
void InitSerial(void)
{	
	// Init UART 0
	U0CTL1 = 0x08; 					// itr on rx, brg normal, 8 bits, depol inverted, no infrared
	U0CTL0 = 0xC0; 					// rx & tx on, no parity, no loop, no break, no cts
  	U0BRH = (uchar)(BRG0 >> 8);		// set uart baud rate generator freq
  	U0BRL = (uchar)(BRG0 & 0xFF);	// set uart baud rate generator freq
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: SendToTx0()
// Inputs	: data to send
// Outputs	: None
// Process  : Send a byte to uart 0
///////////////////////////////////////////////////////////////////////////////////////
void SendToTx0(uchar value)
{		
	while( TX_STATUS & tx0_full_txstat );

	DI();	// Disable Interrupts

	if(!(TX_STATUS & tx0_busy_txstat))
	{
		U0TXD = value;
		TX_STATUS |= tx0_busy_txstat;
    }
    else 
	{
		TX0_Buff[tx0_end_index++ & (TX_BUFF_LENGTH-1)] = value;
		if( ((tx0_end_index ^ tx0_start_index) & (TX_BUFF_LENGTH-1)) == 0 )
			TX_STATUS |= tx0_full_txstat;
    }	

	EI();	// Re-Enable interrupts

}


///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: InitInitialValues()
// Inputs	: None
// Outputs	: None
// Process  : Initialize all initial values 
///////////////////////////////////////////////////////////////////////////////////////
void InitInitialValues(void)
{
	uchar group_idx, ctrl_idx;
	uchar local_default;
	uchar idx;

	// onewire buffer indexes
	onewire_rx_start_index = 0;					
	onewire_rx_end_index = 0;							
	onewire_rx_current_index = 0;
	
	// On startup we are waiting for SOF
	Z8_STATUS_0 = onewire_waiting_for_sof_z0;

	// first pass flag is on at the begining
	Z8_STATUS_4 = first_pass_z4;

	Z8_STATUS_1 = 0;
	Z8_STATUS_8 = 0;

	// default assignments structure for all groups
	local_default = module_sub_id;
	for(group_idx = 0; group_idx<GROUP_NB; group_idx++)
		for(ctrl_idx = 0; ctrl_idx<CONTROL_NB; ctrl_idx++)
		{
			assignment[ctrl_idx][group_idx] = (local_default & MASK_7BITS);
			local_default++;
		}

	// TX status and buffer indexes
	TX_STATUS			= 0;
	tx0_start_index    	= 0;
	tx0_end_index   	= 0;

	// Read module name stored in EEPROM
	EE_Address = MODULE_NAME_EE_ADR;   
	ReadMultiByteI2c_24C16((far uchar *)(&module_name[0]), MODULE_NAME_LENGHT);

}




