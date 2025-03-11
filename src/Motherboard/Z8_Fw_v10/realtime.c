#include <eZ8.h>
#include "pins_defs.h"
#include "commondef.h"
#include <common_z8_ez.h>
#include <ctrldef.h>
#include "realtime.h"
#include "eeprom.h"
#include "serial.h"
#include "lcd.h"
#include "lcd_low_level.h"
#include "smpte.h"
#include "hardware.h"

far uchar realtime_generated_buffer[REALTIME_BUFFER_LENGHT];
far uchar realtime_received_buffer [REALTIME_BUFFER_LENGHT];
uchar realtime_generated_start_index;
uchar realtime_generated_end_index;
uchar realtime_received_start_index;
uchar realtime_received_end_index;
uchar current_realtime_state;
//uchar current_realtime_bmp_state;

extern far uchar CtrlReg[CTRL_REG_REAL_NB];	// table of control reg for eerprom saving
extern rom uchar bmp_play[];
extern rom uchar bmp_stop[];
extern rom uchar bmp_record[];
extern rom uchar bmp_pause[];
extern uchar current_smpte_index;
extern volatile uchar Z8_STATUS_6;
extern uchar quarter_frame_cpt;
extern rom uchar smpte_frame_nb[];
extern far SMPTE_STRUCT smpte_code;
extern uchar smpte_lock_cpt;

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: CheckRealtimeMidiReceived()
// Inputs	: None
// Outputs	: None
// Process  : check if a realtime midi event has been received
///////////////////////////////////////////////////////////////////////////////////////
void CheckRealtimeMidi(void)
{
	// Internal sync is off ?
	if(Z8_STATUS_0_NVM & mtc_mode_on_off_z0nvm)
		return;

	// if midi sync is configured as MIDI clock
	if(!(Z8_STATUS_0_NVM & mtc_mode_z0nvm))
	{
		//////////////////////////////////////////////////////////////////
		////////////////////////////////////////// CHECK IF INTERNAL SYNC
		//////////////////////////////////////////////////////////////////
		while(realtime_generated_start_index != realtime_generated_end_index) 
		{
			// generate internal sync
			if(realtime_generated_buffer[realtime_generated_start_index] == MIDI_CLOCK)
				SendToTx1(MIDI_CLOCK);
	
			realtime_generated_start_index = (realtime_generated_start_index + 1) & (REALTIME_BUFFER_LENGHT - 1);
		}
	
		//////////////////////////////////////////////////////////////////	
		/////////////////////////////////////////// CHECK IF EXTERNAL SYNC
		//////////////////////////////////////////////////////////////////
	
		// Treat realtime MIDI from external sync
		while(realtime_received_start_index != realtime_received_end_index) 
		{
			realtime_received_start_index = (realtime_received_start_index + 1) & (REALTIME_BUFFER_LENGHT - 1);
		}
	} // end midi clock mode
	else
	// smpte mode
	{
		// must send a new quarter frame ?
		if(Z8_STATUS_6 & must_send_smpte_quarter_z6)
		{
			// unflag
			Z8_STATUS_6 &= ~must_send_smpte_quarter_z6;
			// Update smpte time
			quarter_frame_cpt++;
			if(quarter_frame_cpt >= QUARTER_FRAME)
			{
				// Increase smpte time by +1 frame and display
				IncreaseDisplaySmpteTime(1);
				// reset counter
				quarter_frame_cpt = 0;
			}
			// Send a new smpte quarter frame
			SendSmpteToMidi(UPDATE_QUARTER);
		}
	}	
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	UpdateInternalRealtime()
// Inputs	: 	None
// Outputs	: 	None
// Process  : 	BACKWARD_TRANSPORT, FORWARD_TRANSPORT, PLAY_TRANSPORT, STOP_TRANSPORT, REC_TRANSPORT
				// Default transport mapping is aranged for cubase transport functions MMC
				// MMC is : F0 7F ID 06 NN F7 with 
				// ID = 127 for all devices
				// NN = 0x01 Stop 0x02 Play 0x03 Deferred Play 0x04 Fast Forward 0x05 Rewind 0x06 Record Strobe (Punch In) 
				// 0x07 Record Exit (Punch out) 0x09 Pause 
///////////////////////////////////////////////////////////////////////////////////////
void UpdateInternalRealtime(uchar next_state)
{
	uchar mmc_event[6] = {0xF0, 0x7F, 0x7F, 0x06, 0x00, 0xF7};
	uchar realtime_midi_data = 0;
	uchar tmp;

	// Internal sync is off ?
	if(Z8_STATUS_0_NVM & mtc_mode_on_off_z0nvm)
		return;

	switch(next_state)
	{
		case BACKWARD_TRANSPORT : 	// Assign MMC MIDI event
									mmc_event[4] = MIDI_MMC_BACKWARD;
									break;
		case FORWARD_TRANSPORT : 	// Assign MMC MIDI event
									mmc_event[4] = MIDI_MMC_FORWARD;
									break;
		case PLAY_TRANSPORT : 		// Depending on preceding state, PLAY or CONTINUE
									if(current_realtime_state == PAUSE_TRANSPORT)
										realtime_midi_data = MIDI_CONTINUE;
										//SendToTx1(MIDI_CONTINUE);
									else
									if(current_realtime_state != PLAY_TRANSPORT)
										realtime_midi_data = MIDI_START;
										//SendToTx1(MIDI_START);
									// update current state
									current_realtime_state = PLAY_TRANSPORT;
									// Assign MMC MIDI event
									mmc_event[4] = MIDI_MMC_PLAY;
									// Turn on the play led
									LED_PLAY_PORT_OUT |= LED_PLAY_PIN;
									// Display PLAY bmp
									//DisplayBmp(LCD_ZONE_TRANSPORT_X, LCD_ZONE_TRANSPORT_Y, LCD_INVERT, bmp_play);
									break;
		case STOP_TRANSPORT : 		// Depending on preceding state, STOP or PAUSE
									if(current_realtime_state == PLAY_TRANSPORT)
									{
										mmc_event[4] = MIDI_MMC_PAUSE; 
										current_realtime_state = PAUSE_TRANSPORT;
										// Display PAUSE bmp
										//DisplayBmp(LCD_ZONE_TRANSPORT_X, LCD_ZONE_TRANSPORT_Y, LCD_INVERT, bmp_pause);
									}
									else
									{
										mmc_event[4] = MIDI_MMC_STOP;
										current_realtime_state = STOP_TRANSPORT;
										// Display STOP bmp
										//DisplayBmp(LCD_ZONE_TRANSPORT_X, LCD_ZONE_TRANSPORT_Y, LCD_INVERT, bmp_stop);
									}
									realtime_midi_data = MIDI_STOP;
									// SendToTx1(MIDI_STOP);
									// Turn off the play led
									LED_PLAY_PORT_OUT &= ~LED_PLAY_PIN;
									// Turn off the rec led
									LED_RECORD_PORT_OUT &= ~LED_RECORD_PIN;
									break;
		case REC_TRANSPORT : 		// Assign MMC MIDI event
									mmc_event[4] = MIDI_MMC_REC;
									// Turn on the rec led
									LED_RECORD_PORT_OUT |= LED_RECORD_PIN;
									// update current state
									current_realtime_state = REC_TRANSPORT;
									// Display REC bmp
									// DisplayBmp(LCD_ZONE_TRANSPORT_X, LCD_ZONE_TRANSPORT_Y, LCD_INVERT, bmp_record);
									break;
	}

	// Must send MIDI realtime data ?
//	if(!(Z8_STATUS_0_NVM & mtc_mode_on_off_z0nvm))
	{
		// Midi clock mode ?
		if(!(Z8_STATUS_0_NVM & mtc_mode_z0nvm))
		{
			if(realtime_midi_data)
				SendToTx1(realtime_midi_data);

			// Now Send MMC MIDI event
			SendDispatchToMidi(mmc_event, 6, midi_out_1_en | usb_out_en);
		}
		else
		// SMPTE Mode
		{
			// Start : init smpte digit to first digit
			if( (realtime_midi_data == MIDI_START) || (realtime_midi_data == MIDI_CONTINUE))
			{
//				if(realtime_midi_data == MIDI_START)
					// Reset smpte time to zero
//					InitSmpteTime();
				// reload timer 
				ReinitTimer2();
				// Start smpte timer : prescaler is 8 in smpte mode
				// T2CTL1 = 0xB9;	
				T2CTL1 = 0x99;
				// Send a full smpte frame
				SendSmpteToMidi(FULL_SMPTE);
			}
			else
			if(mmc_event[4] == MIDI_MMC_STOP)
			{
				// Reset smpte time to zero
				InitSmpteTime();
				// Stop smpte timer
				// T2CTL1 = 0x39;	// Timer 2 continuous, not started, divide per 128
				T2CTL1 = 0x19;	// Timer 2 one shot, not started, divide per 8
				// Update display with new smpte time
				DisplaySmpteTime(SMPTE_ALL_DIGIT);
				// Send a full smpte frame
				SendSmpteToMidi(FULL_SMPTE);
			}
			else
			if(mmc_event[4] == MIDI_MMC_PAUSE)
			{
				// Stop smpte timer
				// T2CTL1 = 0x39;	// Timer 2 continuous, not started, divide per 128
				T2CTL1 = 0x19;	// Timer 2 one shot, not started, divide per 8
			}
			else
			if(mmc_event[4] == MIDI_MMC_FORWARD)
			{
				// T2CTL1 = 0x19;	// Timer 2 one shot, not started, divide per 8
				// Increase smpte time by 5 sec and display
				tmp = SMPTE_UPDT_SEC_FROM_SW;
				// if shift is active then double the time to update
				if(Z8_STATUS_6  & shift_state_z6)
					tmp = SMPTE_UPDT_SEC_FROM_SW_SHIFT;

				IncreaseDisplaySmpteTime((uchar)(smpte_frame_nb[smpte_code.type] * tmp));
				// Send a full smpte frame
				//if(!(T2CTL1 & 0x80))
				smpte_lock_cpt = 0;
				SendSmpteToMidi(FULL_SMPTE);
				// current_smpte_index = SMPTE_SECONDS_LOW_NIBBLE;
				//SendSmpteToMidi(UPDATE_QUARTER);
				// T2CTL1 = 0x99;
				smpte_lock_cpt = SMPTE_LOCK_TIME;
			}
			else
			if(mmc_event[4] == MIDI_MMC_BACKWARD)
			{
				//T2CTL1 = 0x19;	// Timer 2 one shot, not started, divide per 8
				// Decrease smpte time by 5 sec and display
				// Increase smpte time by 5 sec and display
				tmp = SMPTE_UPDT_SEC_FROM_SW;
				// if shift is active then double the time to update
				if(Z8_STATUS_6  & shift_state_z6)
					tmp = SMPTE_UPDT_SEC_FROM_SW_SHIFT;
				
				DecreaseDisplaySmpteTime((uchar)(smpte_frame_nb[smpte_code.type] * tmp));
				// Send a full smpte frame
				//if(!(T2CTL1 & 0x80))
				smpte_lock_cpt = 0;
				SendSmpteToMidi(FULL_SMPTE);
				//current_smpte_index = SMPTE_SECONDS_LOW_NIBBLE;
				//SendSmpteToMidi(UPDATE_QUARTER);
				// T2CTL1 = 0x99;
				smpte_lock_cpt = SMPTE_LOCK_TIME;
			}
		}
	}
	
	// Display the associcated bmp
	DisplayRealtimeBitmap();
	
}


