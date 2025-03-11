#include <eZ8.h>
#include "pins_defs.h"
#include "commondef.h"
#include <common_z8_ez.h>
#include <ctrldef.h>
#include "itrs.h"
#include "serial.h"
#include "realtime.h"
#include "hardware.h"
#include "onewire.h"
#include "eeprom.h"
#include "smpte.h"

///////////////////////////////////////////////////////////////////////////////////////
// GLOBAL VARS
///////////////////////////////////////////////////////////////////////////////////////
uchar lcd_lock_incrust_cpt;
uchar midi_activity_cpt;
uint seconds_5_timer_cpt;			// used to create a 5 seconds timer from timer 0

extern uchar TX_STATUS;
extern uchar tx0_start_index;
extern uchar tx0_end_index;
extern uchar tx1_start_index;
extern uchar tx1_end_index;
extern far uchar TX0_Buff[TX_BUFF_LENGTH];
extern far uchar TX1_Buff[TX_BUFF_LENGTH];

extern far uchar realtime_generated_buffer[REALTIME_BUFFER_LENGHT];
extern uchar realtime_generated_end_index;
extern far uchar realtime_received_buffer[REALTIME_BUFFER_LENGHT];
extern uchar realtime_received_end_index;
extern far uchar CTRL_REG_7BITS_FROM_EZ[CTRL_REG_MAX_FROM_EZ_TO_Z8];	// NN=1 registers can be accessed from ez board 
extern far uchar CtrlReg[CTRL_REG_REAL_NB];			// table of control reg for eerprom saving and menus 

extern uchar serial_in_start_index;		// a near pointer on FAR ram
extern uchar serial_in_end_index;		// a near pointer on FAR ram
extern uchar serial_in_current_index;
extern far uchar serial_in_buffer[SERIAL_IN_BUFF_LENGHT];

extern volatile uchar Z8_STATUS_4;
extern volatile uchar Z8_STATUS_5;
extern volatile uchar Z8_STATUS_6;
extern volatile uchar Z8_STATUS_7;
extern volatile uchar Z8_STATUS_8;
extern uchar Z8_STATUS_9;

//extern uchar timeout_answer_module_cpt;
extern uchar midi_activity_cpt;

extern uchar onewire_rx_start_index;						// a near pointer on FAR ram
extern uchar onewire_rx_end_index;							// a near pointer on FAR ram
extern uchar onewire_rx_current_index;
extern far uchar onewire_rx_buffer[ONEWIRE_RX_BUFF_LENGHT];		// one wire serial in buffer

extern uchar menu_state;
extern uint tempo_reload_cpt;
extern uchar encoder_but_lock_cpt;
extern uchar current_smpte_reload_cpt;
extern rom uint  smpte_reload_cpt[SMPTE_FRAME_RATE_NB][3];
extern far SMPTE_STRUCT smpte_code;
extern uchar smpte_lock_cpt;

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: Encoder2Increase()
// Inputs	: None
// Outputs	: None
// Process  : 
///////////////////////////////////////////////////////////////////////////////////////
void Encoder2Increase(void)
{
	static uchar step;

	// Is the MTC mode active ?
	if(!(Z8_STATUS_0_NVM & mtc_mode_z0nvm))
	{		
		step = 1;
		if(Z8_STATUS_6 & shift_state_z6)
			step = SHIFT_HIGH_STEP;

		if(internal_tempo < MAX_TEMPO_MINUS_30)
			internal_tempo += step;

		// higher than max ?
		if(internal_tempo > MAX_TEMPO_MINUS_30)
			internal_tempo = MAX_TEMPO_MINUS_30;

		Z8_STATUS_5 |= tempo_changed_z5;
	}
	else
	// SMPTE mode
	{
		// Encoder increase current frame nb
		Z8_STATUS_7 |= must_inc_1_smpte_frame_z7;
	}

}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: Encoder2Decrease()
// Inputs	: None
// Outputs	: None
// Process  : 
///////////////////////////////////////////////////////////////////////////////////////
void Encoder2Decrease(void)
{
	static uchar step;

	// Is the MTC mode active ?
	if(!(Z8_STATUS_0_NVM & mtc_mode_z0nvm))
	{		
		step = 1;
		if(Z8_STATUS_6 & shift_state_z6)
			step = SHIFT_HIGH_STEP;

		if(internal_tempo > (step-1))
			internal_tempo -= step;
		else
			internal_tempo = 0;

		Z8_STATUS_5 |= tempo_changed_z5;
	}
	else
	// SMPTE mode
	{
		// Encoder deccrease current frame nb
		Z8_STATUS_7 |= must_dec_1_smpte_frame_z7;
	}

}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: UpdateFunctionsToIncrease()
// Inputs	: None
// Outputs	: None
// Process  : 
///////////////////////////////////////////////////////////////////////////////////////
void UpdateFunctionsToIncrease(void)
{
	// Are we in menus mode ?
	if(menu_state != NO_MENU)
	{
		Z8_STATUS_6 |= up_down_sw_changed_z6;
		Z8_STATUS_6 |= up_down_sw_direction_z6;
		return;
	}

}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: UpdateFunctionsToDecrease()
// Inputs	: None
// Outputs	: None
// Process  : 
///////////////////////////////////////////////////////////////////////////////////////
void UpdateFunctionsToDecrease(void)
{
	// Are we in menus mode ?
	if(menu_state != NO_MENU)
	{
		Z8_STATUS_6 |= up_down_sw_changed_z6;
		Z8_STATUS_6 &= (~up_down_sw_direction_z6);
		return;
	}
}


///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: isr_timer0()
// Inputs	: None
// Outputs	: None
// Process  : Timer 0 ISR - Should be there each 10ms
///////////////////////////////////////////////////////////////////////////////////////
#pragma interrupt
void isr_timer0(void) 
{

	// UPDATE LCD INCRUST WINDOW ONLY LOCK COUNTER
	if(lcd_lock_incrust_cpt)
		if(--lcd_lock_incrust_cpt == 0)
			Z8_STATUS_5 |= must_reinit_incrust_lcd_z5;

	// Decrease timeout while waiting for encoder button debounce
	if(encoder_but_lock_cpt)
		encoder_but_lock_cpt--;

	// MANAGE MIDI activity persistency
	if(midi_activity_cpt)
	{
		midi_activity_cpt--;
		if(midi_activity_cpt == 0)
			Z8_STATUS_5 |= must_toggle_midi_activity_z5;
	}

	// UPDATE TIMER 5 SECONDS COUNTER
    if(seconds_5_timer_cpt)
		seconds_5_timer_cpt -= (uint)0x0001L;

	// MUST UNLOCK SMPTE ?
	if(smpte_lock_cpt)
		smpte_lock_cpt--;

	// Finish by acknoledging itr bits
	IRQ0  &= 0xDF;
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: isr_timer1()
// Inputs	: None
// Outputs	: None
// Process  : Timer 1 ISR
///////////////////////////////////////////////////////////////////////////////////////
#pragma interrupt
void isr_timer1(void) 
{
	// Unlock eeprom
	Z8_STATUS_4 &= ~eeprom_locked_10ms_z4;

	// Finish by acknoledging itr bits
	IRQ0  &= 0xBF;
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: isr_timer2()
// Inputs	: None
// Outputs	: None
// Process  : Timer 2 ISR
///////////////////////////////////////////////////////////////////////////////////////
#pragma interrupt
void isr_timer2(void) 
{
    // if midi sync is enabled
	if(!(Z8_STATUS_0_NVM & mtc_mode_on_off_z0nvm))
	{
		// if midi sync is configured as MIDI clock
		if(Z8_STATUS_0_NVM & mtc_mode_z0nvm)
		// SMPTE mode : update smpte time
		{
			current_smpte_reload_cpt++;
			if(current_smpte_reload_cpt == 3)
				current_smpte_reload_cpt = 0;
			tempo_reload_cpt = (uint)smpte_reload_cpt[smpte_code.type][current_smpte_reload_cpt]; 
			// Flag to send new smpte quarter frame
			Z8_STATUS_6 |= must_send_smpte_quarter_z6;
		}
		else
		{
			realtime_generated_buffer[realtime_generated_end_index] = MIDI_CLOCK;
			realtime_generated_end_index = (realtime_generated_end_index + 1) & (REALTIME_BUFFER_LENGHT - 1);
			if(Z8_STATUS_4 & must_reload_tempo_timer_z4)
			{
				Z8_STATUS_4 &= ~must_reload_tempo_timer_z4;
			}
		}

		T2RH   	= (uchar)(tempo_reload_cpt >> 8);		// Timer 2 reload value High (1562d for 10ms)
		T2RL   	= (uchar)(tempo_reload_cpt & 0xFF);		// Timer 2 reload value Low  (1562d for 10ms)

	}

	// Finish by acknoledging itr bits
	IRQ0  &= 0x7F;
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: isr_timer3()
// Inputs	: None
// Outputs	: None
// Process  : Timer 3 ISR (TIMEOUT ONEWIRE)
///////////////////////////////////////////////////////////////////////////////////////
#pragma interrupt
void isr_timer3(void) 
{
	// timeout_answer_module_cpt = 0;	
	Z8_STATUS_5 &= ~waiting_timeout_onewire_z5;

	// Finish by acknoledging itr bits
	IRQ2  &= 0x7F;
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: isr_rx0()
// Inputs	: None
// Outputs	: None
// Process  : UART 0 RX ISR (FROM ONE WIRE)
///////////////////////////////////////////////////////////////////////////////////////
#pragma interrupt
void isr_rx0(void) 
{
	uchar must_store = TRUE;
	
	// are we waiting SOF ?
	if(!(Z8_STATUS_5 & onewire_rx_state_z5))
	{
		if(U0RXD != SOF_ONEWIRE)
			must_store = FALSE;
		else
			// we are now waiting EOF
			Z8_STATUS_5 |= onewire_rx_state_z5;

	} // end waiting SOF
	// Here we are into a message received, or waiting EOF
	else
	{
		if(U0RXD == EOF_ONEWIRE)
		{
			// was the rx onewire disabled ?
			if(Z8_STATUS_6 & disable_ow_rx_z6)
			{
				// stop disabling rx buffer
				Z8_STATUS_6 &= ~disable_ow_rx_z6;
				// do not store EOF
				must_store = FALSE;
			}
			else
				// Here we received a valid EOF : update pointer end index
				onewire_rx_end_index = (onewire_rx_current_index+1)&(ONEWIRE_RX_BUFF_LENGHT-1);
			// we are now waiting new SOF
			Z8_STATUS_5 &= ~onewire_rx_state_z5;
		}
		else
		if(U0RXD == SOF_ONEWIRE)
		{
			must_store = FALSE;
			// now rewait sof
			Z8_STATUS_5 &= ~onewire_rx_state_z5;
		}
	} // end waiting EOF

	// do not store if rx is disabled
	if(Z8_STATUS_6 & disable_ow_rx_z6)
		must_store = FALSE;

	// write data to buffer if must store & if rx ow is not disabled.
	if(must_store)
	{
		onewire_rx_buffer[onewire_rx_current_index] = U0RXD;
		onewire_rx_current_index = (onewire_rx_current_index+1)&(ONEWIRE_RX_BUFF_LENGHT-1);

		// check that no overflow occured
		if(onewire_rx_current_index == onewire_rx_start_index)
			if(onewire_rx_current_index != ((onewire_rx_end_index-1)&(ONEWIRE_RX_BUFF_LENGHT-1)))
				Z8_STATUS_8 |= onewire_rx_overflow_z8;
	}

	// Finish by acknoledging itr bits
	IRQ0  &= 0xEF;
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: isr_rx1()
// Inputs	: None
// Outputs	: None
// Process  : UART 1 RX ISR (FROM EZ-USB)
///////////////////////////////////////////////////////////////////////////////////////
#pragma interrupt
void isr_rx1(void) 
{
	static uchar must_store;

	// REALTIME EVENT RECEIVED
	if((U1RXD >= MIDI_CLOCK) && (U1RXD <= MIDI_STOP))
	{
		// treat realtime incoming data only if z8 fully updated from ez
		if(CTRL_REG_7BITS_FROM_EZ[0] & z8_board_fully_updated_from_ez0)
		{
			realtime_received_buffer[realtime_received_end_index] = U1RXD;
			realtime_received_end_index = (realtime_received_end_index + 1) & (REALTIME_BUFFER_LENGHT - 1);
		} // end z8 was fully updated from ez
	} // end MIDI REALTIME event received
	else
	{
		must_store = TRUE;
	
		// are we waiting SOF ?
		if(!(Z8_STATUS_5 & serial_in_state_z5))
		{
			if(U1RXD != SOF_TO_EZ)
				must_store = FALSE;
			else
				// we are now waiting EOF
				Z8_STATUS_5 ^= serial_in_state_z5;

		} // end waiting SOF
		// Here we are into a message received, or waiting EOF
		else
		{
			if(U1RXD == EOF_TO_EZ)
			{
				// Here we received a valid EOF : update pointer end index
				serial_in_end_index = (serial_in_current_index+2)&(SERIAL_IN_BUFF_LENGHT-1);
				// we are now waiting new SOF
				Z8_STATUS_5 ^= serial_in_state_z5;
			}
			else
			if(U1RXD == SOF_TO_EZ)
			{
				must_store = FALSE;
				// now rewait sof
				Z8_STATUS_5 ^= serial_in_state_z5;
			}
		} // end waiting EOF

		if(must_store)
		{
			serial_in_buffer[serial_in_current_index] = U1RXD;
			serial_in_current_index = (serial_in_current_index+1)&(SERIAL_IN_BUFF_LENGHT-1);

			// check that no overflow occured
			if(serial_in_current_index == serial_in_start_index)
				if(serial_in_current_index != ((serial_in_end_index-1)&(SERIAL_IN_BUFF_LENGHT-1)))
					Z8_STATUS_8 |= serial_in_overflow_z8;
		}
	}

	// Finish by acknoledging itr bits
	IRQ2  &= 0xBF;
}


///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: isr_tx0()
// Inputs	: None
// Outputs	: None
// Process  : UART 0 TX ISR (TO ONEWIRE)
///////////////////////////////////////////////////////////////////////////////////////
#pragma interrupt
void isr_tx0(void) 
{
	// tx0 ready ?
	if(U0STAT0 & TDRE)
	{
		if ((tx0_start_index != tx0_end_index))// || (TX_STATUS & tx0_busy_txstat))
		{
			U0TXD = TX0_Buff[tx0_start_index++ & (TX_BUFF_LENGTH-1)];
			TX_STATUS &= ~tx0_full_txstat;
		}
		else
			TX_STATUS &= ~tx0_busy_txstat;
	}

	// Finish by acknoledging itr bits
	IRQ0  &= 0xF7;
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: isr_tx1()
// Inputs	: None
// Outputs	: None
// Process  : UART 0 TX ISR (TO EZ-USB)
///////////////////////////////////////////////////////////////////////////////////////
#pragma interrupt
void isr_tx1(void) 
{
	// tx1 ready ?
	if(U1STAT0 & TDRE)
	{
		if((tx1_start_index != tx1_end_index))// || (TX_STATUS & tx1_busy_txstat))
		{
			U1TXD = TX1_Buff[tx1_start_index++ & (TX_BUFF_LENGTH-1)];
			TX_STATUS &= ~tx1_full_txstat;
		}
		else
			TX_STATUS &= ~tx1_busy_txstat;
	}

	// Finish by acknoledging itr bits
	IRQ2  &= 0xDF;
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: isr_encoder1()
// Inputs	: None
// Outputs	: None
// Process  : ENCODER CHANGE ISR : CALLED ON FALLING EDGES of encoder A - PAD0 ISR
///////////////////////////////////////////////////////////////////////////////////////
#pragma interrupt
void isr_encoder1(void) 
{
	// falling edge
	// is B = 1 on A falling edge
	if(!(IRQES & 0x01))
	{
		if(ENCODER1_B_PORT_IN & ENCODER1_B_PIN)
		{
			UpdateFunctionsToIncrease();
		}
	    else
		{
			UpdateFunctionsToDecrease();
		}
	}
	else
	{
		if(ENCODER1_B_PORT_IN & ENCODER1_B_PIN)
		{
			UpdateFunctionsToDecrease();
		}
	    else
		{
			UpdateFunctionsToIncrease();
		}
	}

	// Change the itr edge
	IRQES ^= 0x01;

	// Finish by acknoledging itr bits
	IRQ1  &= 0xFE;
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: isr_encoder2()
// Inputs	: None
// Outputs	: None
// Process  : ENCODER CHANGE ISR : CALLED ON FALLING EDGES of encoder A - PAD2 ISR
///////////////////////////////////////////////////////////////////////////////////////
#pragma interrupt
void isr_encoder2(void) 
{
	// Change tempo only if encoder is not locked from menus
	if(!(Z8_STATUS_0_NVM & encoder_tempo_lock_z0nvm))
	// also update increase/decrease if realtime is ON
	if(!(Z8_STATUS_0_NVM & mtc_mode_on_off_z0nvm))
	{
		
		// falling edge
		// is B = 1 on A falling edge
		if(!(IRQES & 0x04))
		{
			if(ENCODER2_B_PORT_IN & ENCODER2_B_PIN)
			{
				Encoder2Increase();
			}
		    else
			{
				Encoder2Decrease();
			}
		}
		else
		{
			if(ENCODER2_B_PORT_IN & ENCODER2_B_PIN)
			{
				Encoder2Decrease();
			}
		    else
			{
				Encoder2Increase();
			}
		}
	}

	// Change the itr edge
	IRQES ^= 0x04;

	// Finish by acknoledging itr bits
	IRQ1  &= 0xFB;
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: InitItrs()
// Inputs	: None
// Outputs	: None
// Process  : Initilize Z8 ITRS
///////////////////////////////////////////////////////////////////////////////////////
void InitItrs(void)
{
	// Timers 0,1,2, uart0, adc, i2c and spi interrupt enables
	// Note that timers interrupts are not enabled here
	IRQ0ENH	= 0xF8;		// Enable ITRs TX0,RX0,T0,T1,T2 + select nominal priority
	IRQ0ENL	= 0x18;		// Enable ITRs TX0,RX0,T0,T1,T2 + select nominal priority : RX0 + TX0 are high priority
	
	// PORTS A AND D IRQs 
	// Enable ITRs on port A and D + select nominal priority (pd1,3,4,5,6 itrs are off)
	IRQ1ENH	= 0x05;		// Enable ITRs on port A and D + select nominal priority (pd0, pd2)
	IRQ1ENL	= 0x00;		// Enable ITRs on port A and D + select nominal priority
	IRQES 	= 0xFA;		// Define falling/rising edges itrs of both port A and D (falling = 0)
	IRQPS 	= 0xFF;		// Select between port A and D interrupt source (All on port D)

	// Timers 3, uart1, dma, portC interrupt enables
	// Note that dma interrupts are not enabled here
	IRQ2ENH	= 0xE0;		// Enable ITRs TX1 + RX1 + Timer 3 + select nominal priority
	IRQ2ENL	= 0x00;		// Enable ITRs TX1 + RX1 + Timer 3 + select nominal priority

	// Clear interrupts pending from boot
	IRQ0	= 0x00;
	IRQ1	= 0x00;
	IRQ2	= 0x00;

	// Create interrupts vectors
	SET_VECTOR(TIMER0, isr_timer0);
	SET_VECTOR(TIMER1, isr_timer1);
	SET_VECTOR(TIMER2, isr_timer2);
	SET_VECTOR(TIMER3, isr_timer3);
	SET_VECTOR(UART0_RX, isr_rx0);
	SET_VECTOR(UART1_RX, isr_rx1);	
	SET_VECTOR(UART0_TX, isr_tx0);
	SET_VECTOR(UART1_TX, isr_tx1);
	SET_VECTOR(P0AD, isr_encoder1);
	SET_VECTOR(P2AD, isr_encoder2);
}


