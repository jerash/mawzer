#include <eZ8.h>
#include "pins_defs.h"
#include "commondef.h"
#include <common_z8_ez.h>
#include <ctrldef.h>
#include "serial.h"
#include "eeprom.h"
#include "lcd.h"
#include "hardware.h"
#include "i2c.h"
#include "onewire.h"

///////////////////////////////////////////////////////////////////////////////////////
// GLOBAL VARS
///////////////////////////////////////////////////////////////////////////////////////
far   uchar serial_in_buffer[SERIAL_IN_BUFF_LENGHT];	// serial in buffer

far uchar CTRL_REG_7BITS_FROM_EZ[CTRL_REG_MAX_FROM_EZ_TO_Z8];	// NN=1 registers can be accessed from ez board
uchar serial_in_start_index;						// a near pointer on FAR ram
uchar serial_in_end_index;							// a near pointer on FAR ram
uchar serial_in_current_index;
uchar current_to_midi;
uchar control_index;

uchar TX_STATUS;
uchar tx0_start_index;
uchar tx0_end_index;
uchar tx1_start_index;
uchar tx1_end_index;
far uchar TX0_Buff[TX_BUFF_LENGTH];
far uchar TX1_Buff[TX_BUFF_LENGTH];
far	uint must_reinit_lcd_from_sysex;

extern far uchar CtrlReg[CTRL_REG_REAL_NB];	// table of control reg for eerprom saving
extern volatile uchar Z8_STATUS_5;
extern volatile uchar Z8_STATUS_8;
extern far uchar generic_buffer[GENERIC_BUFF_SIZE];	// a generic far ram buffer of 64 bytes
extern rom char identity[];
extern uchar I2cCommand;
extern uchar lcd_lock_incrust_cpt;
extern uchar midi_activity_cpt;
extern uchar lcd_current_incrust;
extern uchar current_value;
extern uchar current_module;
extern far uchar control_index_structs_in_ram[PARAM_STRUCT_IN_RAM_MAX_NB];	 
extern far All_Params	 	Param[PARAM_STRUCT_IN_RAM_MAX_NB];	// structure of parameters in EEPROM V1.61
extern far All_Params	 	*ParamCurrent;						//current structure of parameters in EEPROM
extern uchar last_struct_index;								
extern uchar last_control_index;

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: InitSerial()
// Inputs	: None
// Outputs	: None
// Process  : Initilize Z8 Serials periferals I2C and UARTS
///////////////////////////////////////////////////////////////////////////////////////
void InitSerial(void)
{	
	// Init UART 0 (COMM WITH ONEWIRE)
	U0CTL1 = 0x08; 					// itr on rx, depol is on, brg normal, 8 bits, no infrared
	U0CTL0 = 0xC0; 					// rx & tx on, no parity, no loop, no break, no cts
  	U0BRH = (uchar)(BRG0 >> 8);		// set uart baud rate generator freq
  	U0BRL = (uchar)(BRG0 & 0xFF);	// set uart baud rate generator freq

	// Init UART 1 (COMM WITH EZ)
	U1CTL1 = 0x02; 					// NO itr on rx FOR NOW, brg normal, 8 bits, no infrared	
	U1CTL0 = 0xC0; 					// rx & tx on, no parity, no loop, no break, no cts
  	U1BRH = (uchar)(BRG1 >> 8);		// set uart baud rate generator freq
  	U1BRL = (uchar)(BRG1 & 0xFF);	// set uart baud rate generator freq
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: SendToTx0()
// Inputs	: data to send
// Outputs	: None
// Process  : Send a byte to uart 0 (One wire buses)
///////////////////////////////////////////////////////////////////////////////////////
void SendToTx0(uchar value)
{		
	while( TX_STATUS & tx0_full_txstat );

	DI();	// Disable Interrupts

	if(!(TX_STATUS & tx0_busy_txstat))
	{
		TX_STATUS |= tx0_busy_txstat;
		U0TXD = value;
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
// Routine 	: SendToTx1()
// Inputs	: data to send
// Outputs	: None
// Process  : Send a byte to uart 1 (ez board)
///////////////////////////////////////////////////////////////////////////////////////
void SendToTx1(uchar value)
{		
	while( TX_STATUS & tx1_full_txstat );

	DI();	// Disable Interrupts

	if(!(TX_STATUS & tx1_busy_txstat))
	{
		TX_STATUS |= tx1_busy_txstat;
		U1TXD = value;
    }
    else 
	{
		TX1_Buff[tx1_end_index++ & (TX_BUFF_LENGTH-1)] = value;
		if( ((tx1_end_index ^ tx1_start_index) & (TX_BUFF_LENGTH-1)) == 0 )
			TX_STATUS |= tx1_full_txstat;
    }	

	EI();	// Re-Enable interrupts
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	SendCtrlRegToEzBoard()
// Inputs	: 	ctrl_reg_index_to_ez (address of register to send)
// Outputs	: 	None
// Process  : 	Send a single control register to EZ Board
///////////////////////////////////////////////////////////////////////////////////////
void SendCtrlRegToEzBoard(uchar ctrl_reg_index_to_ez)
{
	// Send Start of frame
	SendToTx1(SOF_TO_EZ);
	// Send also frame type indicator + register select value 
	SendToTx1(FRAME_TO_EZ_IS_CTRL_REG | ctrl_reg_index_to_ez);
	// Send the register value
	SendToTx1(CtrlReg[ctrl_reg_index_to_ez] & 0x7F);
	// Send End of frame
	SendToTx1(EOF_TO_EZ);
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: CheckSerialInBuffer()
// Inputs	: None
// Outputs	: None
// Process  : Check for serial Input buffer, program rom if required
///////////////////////////////////////////////////////////////////////////////////////
void CheckSerialInBuffer(void)
{
	uchar local_end_index_minus_one = (serial_in_end_index - 1)&(SERIAL_IN_BUFF_LENGHT-1);
	uchar SYSEX_ID0_byte, start_index_offset;//, SYSEX_ID2_byte;
	
	// Detect if there is a valid frame into the buffer
	while(serial_in_start_index != local_end_index_minus_one)
	{ 
		// Check valid frame is present (if SOF and EOF are presents)
		if( (serial_in_buffer[serial_in_start_index] 	     == SOF_TO_EZ) &&
		    (serial_in_buffer[(local_end_index_minus_one-1)&(SERIAL_IN_BUFF_LENGHT-1)] == EOF_TO_EZ) )
		{
			// Detect here which frame type has been received
			SYSEX_ID0_byte  = serial_in_buffer[(serial_in_start_index + FRAME_TYPE_POS)&(SERIAL_IN_BUFF_LENGHT-1)];

			if( (SYSEX_ID0_byte & FRAME_TO_Z8_TYPE_MSK) == /*v1.1 FRAME_IS_PURE_MIDI*/ FRAME_IS_FIRMWARE_OR_PURE_MIDI)
			{
				////////////////////////////// PURE MIDI RECEIVED //////////////////////////////////
				// Here we received a pure midi data buffer 
				// It may be a Note ON/OFF event 
				// It may be a Sysex dedicated to update module firmware
				// Bit 5 of SYSEX_ID0 contains the MIDI In source (usb or midi in connector)
				////////////////////////////////////////////////////////////////////////////////////
				ParsePureMidiInBuffer(local_end_index_minus_one, SYSEX_ID0_byte);
				serial_in_start_index = local_end_index_minus_one;
			}	
			else // frame is not pure midi
			{
				// Detect here which frame type has been received
				if( (SYSEX_ID0_byte & FRAME_TO_Z8_TYPE_MSK) == FRAME_IS_COMMON_PARAM)
				{
					//////////////////////////////////////////////////////////////////////////////////////
					////////////////////// COMMON PARAM REG RECEIVED / REQUESTED /////////////////////////				
					//////////////////////////////////////////////////////////////////////////////////////
					start_index_offset = Prog_Upload_Common_Param(SYSEX_ID0_byte, serial_in_start_index);
					if(start_index_offset == INVALID_REQUEST)
					{
						Display_Invalid_Sysex_Incrust();
						serial_in_start_index = local_end_index_minus_one;
					}
					else
						serial_in_start_index = (serial_in_start_index + start_index_offset)&(SERIAL_IN_BUFF_LENGHT-1);
				}
				else
				if( ( (SYSEX_ID0_byte & FRAME_TO_Z8_TYPE_MSK) == FRAME_IS_LCD_CTRL_ASSIGN) ||
					( (SYSEX_ID0_byte & FRAME_TO_Z8_TYPE_MSK) == FRAME_IS_MIDI_CTRL_ASSIGN) )
				{
					//////////////////////////////////////////////////////////////////////////////////////
					////////////////////// CONTROL ASSIGN RECEIVED / REQUESTED ///////////////////////////
					//////////////////////////////////////////////////////////////////////////////////////
					// TO COMPLETE
				}
			} // end frame was not pure midi
		} // end valid frame 
		else
		{
			// here buffer is not empty, but not a valid frame !		
			Display_Invalid_Sysex_Incrust();
			serial_in_start_index = local_end_index_minus_one;
		}
	} // end while buffer not fully parsed

	// the buffer has now been treated
	serial_in_start_index = local_end_index_minus_one;
}


///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	ParsePureMidiInBuffer()
// Inputs	: 	
// Outputs	: 	
// Process  : 	Pure MIDI is present in the RX buffer : check arpeggio and mackie mode
///////////////////////////////////////////////////////////////////////////////////////
void ParsePureMidiInBuffer(uchar end_index_local, uchar SYSEX_ID0_byte)
{
	return;
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	Prog_Upload_Common_Param()
// Inputs	: 	Frame status, frame start index
// Outputs	: 	return frame lenght treated
// Process  : 	program or upload a common parameter
///////////////////////////////////////////////////////////////////////////////////////
uchar Prog_Upload_Common_Param(uchar SYSEX_ID0_byte, uchar serial_in_start_index)
{
	uchar frame_index		   	= 0;
	uchar common_param_lenght  	= serial_in_buffer[(serial_in_start_index + FRAME_DATA_LENGHT_POS)&(SERIAL_IN_BUFF_LENGHT-1)];
	uchar common_param_address 	= serial_in_buffer[(serial_in_start_index + FRAME_ADDR_INDEX_POS )&(SERIAL_IN_BUFF_LENGHT-1)];
	uchar SYSEX_ID3_byte		= serial_in_buffer[(serial_in_start_index + FRAME_GROUP_POS 	 )&(SERIAL_IN_BUFF_LENGHT-1)];
	uchar tmp					= 0;
	far uchar *local_ptr;
	uchar must_program_eeprom  	= FALSE;
	uchar must_read_eeprom  	= FALSE;
	uchar must_nibble_to_byte  	= FALSE;
	uchar must_display_lcd		= FALSE;
	uchar tmp2;

	// Check the received sysex validity
	if(Check_Sysex_Validity(SYSEX_ID0_byte, common_param_address, SYSEX_ID3_byte) == INVALID_REQUEST)
		return(INVALID_REQUEST);

	// nothing to re-init on LCD for now
	must_reinit_lcd_from_sysex = 0;

	///// COMMON PARAMETERS ALSO CONTAINS LCD MESSAGES TO DISPLAY FROM EZ TO Z8 BOARD /////
	if(common_param_address == LCD_MESSAGE_TO_Z8_INDEX)
	{
				////////////////////////////// LCD MESSAGE REQUESTED /////////////////////////////////
				// Decode message to display on lcd z8 in SYSEX_ID3_byte
				// copy overflow flags from EZ to Z8_STATUS_8 buffer
				Z8_STATUS_8 &= ~OVERFLOW_FROM_EZ_MSK;
 				// Z8_STATUS_8 |= SYSEX_ID3_byte;
				Z8_STATUS_8 |= serial_in_buffer[(serial_in_start_index + FRAME_DATA_START)&(SERIAL_IN_BUFF_LENGHT-1)];
				// Display overflow errors
				DisplayInputOverflowIncrust();
				return (FRAME_DATA_START + common_param_lenght + 1);
	}

	///// COMMON PARAMETERS ALSO CONTAINS THE 3X IDENTITY REQUEST (UPLOAD ONLY) /////
	if(common_param_address == IDENTITY_REQUEST_INDEX)
	{
				// indicates we are programming/uploading : no scenes allowed
				// Z8_STATUS_9 |= programming_z9;

				////////////////////////////// MAWZER IDENTITY REQUESTED /////////////////////////////////
				generic_buffer[0] = MIDI_SOX;
				generic_buffer[1] = JLABS_SYX_ID0;
				generic_buffer[2] = JLABS_SYX_ID1;
				generic_buffer[3] = JLABS_SYX_ID2;
				generic_buffer[4] = JLABS_SYX_ID3;
				generic_buffer[5] = JLABS_SYX_ID4;
				// frame type
				generic_buffer[6] = SYSEX_ID0_byte | sysex_direction_sysxid0; //| prog_upload_sysxid0;
				// frame lenght
				generic_buffer[7] = common_param_lenght; 
				// frame address or index
				generic_buffer[8] = common_param_address;
				// frame group or dont care
				generic_buffer[9] = SYSEX_ID3_byte;
				// Now copy all identity bytes to generic buffer
				for(tmp=0; tmp<common_param_lenght; tmp++)
					generic_buffer[10+tmp] = identity[tmp] & 0x7F;
				// Send EOX
				generic_buffer[10 + common_param_lenght] = MIDI_EOX;
				// force identity to be sent on all midi outputs
				//ParamCurrent->ctrl_status_1 |= (midi_out_1_en | midi_out_2_en | usb_out_en);
				// total lenght is upload header + all datas uploaded + F7
				SendDispatchToMidi(&generic_buffer[0], (11 + common_param_lenght), (midi_out_1_en | usb_out_en));
				// indicates we are have finished programming/uploading
				// Z8_STATUS_9 &= ~programming_z9;

				return (UPLOAD_Z8_FRAME_LENGHT);
	}

	// V1.2 : from there, there are read/write to ext eeprom 0 since common parameters 
	I2cCommand = I2C_SLAVE_ADR_EE + EEPROM_ZERO_I2C_ADR;	

	//////////////////////////////// NEW COMMON PARAM REG RECEIVED /////////////////////////
	///// 0xFF 0b00pxxx00 0b0lllllll 0b0000aaaa 00 vv vv vv 0xFE 
	///// (p=prog/upload) (l=control reg lenght) (a=reg address 0-31) (vv=reg value 0-127)
	////////////////////////////////////////////////////////////////////////////////////////
	if(common_param_address <= INT_TEMPO_INDEX)
	{
			/////////////////// PROGRAM/UPLOAD SINGLE CONTROL REGISTER /////////////////////
			////////////////////////////////////////////////////////////////////////////////
			local_ptr  = &CtrlReg[common_param_address];
			must_display_lcd 		= TRUE;
			// the following zones may have been updated, then reinit LCD

			// tempo is sent as 2 nibbles			
			if(common_param_address == INT_TEMPO_INDEX)
				must_nibble_to_byte = TRUE;
	}
	else
	if(common_param_address == CTRL_REG_0_7B_FROM_EZ_INDEX)
	{
			/////////////////// PROGRAM/UPLOAD SINGLE COMMON PARAMETER /////////////////////
			////////////////////////////////////////////////////////////////////////////////
			local_ptr  = &CTRL_REG_7BITS_FROM_EZ[0];
			// only the USB bmp may have been affected by this common parameter
			must_reinit_lcd_from_sysex   = (uint) INIT_LCD_USB;
	}

	////////////////////////////////////////////////////////////////////////////////
	/////////////////// NEED TO PROGRAM OR UPLOAD COMMON PARAMETERS ? //////////////
	////////////////////////////////////////////////////////////////////////////////
	if(!(SYSEX_ID0_byte & prog_upload_sysxid0))
	{
		////////////////////////////////////////////////////////////////////////////////
		//////////////////////// PROGRAM a new common parameters ///////////////////////
		////////////////////////////////////////////////////////////////////////////////
		// V1.3 NEW BLOCK
		// Copy all bytes to program to generic buffer, nibbles to byte if required
		if(must_nibble_to_byte)
		{
			Serial_Nibb_To_Generic_Bytes((uchar)(serial_in_start_index + FRAME_DATA_START), common_param_lenght);
			// tmp2 will be used as lenght for programming eeprom : double since un-nibbeled data writen to eeprom
			tmp2 = common_param_lenght >> 1;
		}
		else
			// tmp2 will be used as lenght for programming eeprom
			tmp2 = common_param_lenght;

		//Now copy generic buffer to real registers
		for(tmp=0; tmp < tmp2; tmp++)
		{
			// here the received data were NOT nibbleled
			if(!must_nibble_to_byte) 
				// standard byte to byte copy of serial in buffer to generic buffer
				generic_buffer[tmp]  = serial_in_buffer[(serial_in_start_index + FRAME_DATA_START + tmp)&(SERIAL_IN_BUFF_LENGHT-1)];
			
			// now recopy genereic buffer to real register
			local_ptr[tmp] = generic_buffer[tmp];

		}
		
		// V1.3 special action required here
		if(common_param_address == INT_TEMPO_INDEX)
			ReinitTimer2();

		// Write new common parameter to eeprom if required
		if(must_program_eeprom)
			// Write all new common parameters bytes to external eeprom
			WriteMultiByteI2c(&generic_buffer[0], tmp2);

		// size of the treated buffer from start index (+1 is for eof)
		tmp2 = FRAME_DATA_START + common_param_lenght + 1;

	} // end program new common parameter
	else
	{
		////////////////////////////////////////////////////////////////////////////////
		//////////////////////// UPLOAD a common parameters ///////////////////////////
		////////////////////////////////////////////////////////////////////////////////
		// depending on nibbles or not, calculate eeprom buffer lenght to read
		if(must_nibble_to_byte)
			tmp2 = common_param_lenght >> 1;
		else
			tmp2 = common_param_lenght;

		// common param to upload is located in eeprom ?
		if(must_read_eeprom)	
			ReadMultiByteI2c(&local_ptr[0], tmp2);
		
		// byte to byte or byte to nibbles ?
		if(must_nibble_to_byte)
			// Copy all nibbles to upload to generic buffer, send buffer to midi outs
			Byte_To_Generic_Nibbles(JLABS_SYX_UPLOAD_HEADER_SIZE, common_param_lenght, &local_ptr[0]);
		else
		{
			// Copy all bytes to upload to generic buffer, send buffer to midi outs
			for(tmp=0; tmp < common_param_lenght; tmp++)
				generic_buffer[tmp+JLABS_SYX_UPLOAD_HEADER_SIZE] = local_ptr[tmp] & 0x7F;
		}

		// Send to MIDI the uploaded common paramerer
		SendUploadGenericBufferToMidi(SYSEX_ID0_byte, common_param_lenght, common_param_address, SYSEX_ID3_byte);

		// size of the treated buffer from start index
		tmp2 = UPLOAD_Z8_FRAME_LENGHT;

	} // end upload
	
	// A common parameter has been programmed/uploaded : must update LCD ?
	if(must_display_lcd)
	{
		// TO COMPLETE		

		// Lock LCD incrust zone for 1 second
		lcd_lock_incrust_cpt = CPT_1_SEC;

		lcd_current_incrust = LCD_INCRUST_COMMON_PARAM;			
	}

	// Update serial in index for treated frame
	return (tmp2);
}


///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	Check_Sysex_Validity()
// Inputs	: 	
// Outputs	: 	None
// Process  : 	check the received sysex is valid
///////////////////////////////////////////////////////////////////////////////////////
uchar Check_Sysex_Validity(uchar SYSEX_ID0, uchar SYSEX_ID2, uchar SYSEX_ID3)
{
    uchar result = 0;

    ////////////////////////////////////////////////////////////////
    // Invalid request list for COMMON PARAMETERS UPL/DOWN
    ////////////////////////////////////////////////////////////////
    if( (SYSEX_ID0 & FRAME_TO_Z8_TYPE_MSK) == FRAME_IS_COMMON_PARAM)
    {
        // 1) Invalid request if program request on the following common params
        if( (!(SYSEX_ID0 & prog_upload_sysxid0))&&(SYSEX_ID2 == IDENTITY_REQUEST_INDEX))
             result = INVALID_REQUEST;

        // 2) Invalid request if common params address is out of range
        if( SYSEX_ID2 > LAST_COMMON_PARAM_INDEX)
			// only identity address 0x7F can be out of range
	  		if(SYSEX_ID2 != IDENTITY_REQUEST_INDEX)
	            result = INVALID_REQUEST;

        // 3) Invalid request if group number is out of range
        if( SYSEX_ID3 >= GROUP_NB)
             result = INVALID_REQUEST;

    }

    return(result);
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	SendDispatchToMidi()
// Inputs	: 	size of midi str to send, far ram pointer on buffer to send, destination outs 
// Outputs	: 	None
// Process  : 	send  ParamCurrent->midi_str global buffer to different midi outputs
//				Also update MIDI led outputs
///////////////////////////////////////////////////////////////////////////////////////
void SendDispatchToMidi(far uchar *buff_ptr, uchar midi_str_size, uchar destination)
{
	uchar index;

	// Turn on MIDI activity bmp if not already active
	if(!(Z8_STATUS_5 & midi_activity_z5))
	{
		// flag for midi activity was off: it becomes on
		Z8_STATUS_5 |= midi_activity_z5;
		// Update bitmap for midi activity
		DisplayMidiActivityBmp();
	}
	
	// Load counter for MIDI persistency
	midi_activity_cpt = MIDI_ACTIVITY_PERSISTENCY;

	// Here midi data must also be sent on TX0 or USB : send begining of frame to TX0
	SendToTx1(SOF_TO_EZ);
	// Send also frame type indicator + outputs to select 
	SendToTx1(FRAME_TO_EZ_IS_MIDI | (destination & MIDI_OUT_SEL_MSK));
	// send also midi data lenght
	SendToTx1(midi_str_size);

	// Now send all MIDI useful bytes
	for(index = 0; index < midi_str_size; index++)
	{
		//current_to_midi = ParamCurrent->midi_string[index];
		current_to_midi = buff_ptr[index];
		SendToTx1(current_to_midi);		
			
	} // End of Midi str bytes to send

	// Here midi data must was sent on TX0 or USB : send end of frame to TX0
	SendToTx1(EOF_TO_EZ);
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	Serial_Nibb_To_Generic_Bytes()
// Inputs	: 	None
// Outputs	: 	checksum on all nibbles
// Process  : 	convert nibbles to bytes, place in geneic buffer
///////////////////////////////////////////////////////////////////////////////////////
uchar Serial_Nibb_To_Generic_Bytes(uchar start_index, uchar lenght)
{
	uchar tmp2, tmp4;
	uchar tmp, tmp5;

	tmp  = 0; // used as index
	tmp5 = 0; // used as checksum

	for(tmp4=0; tmp4 < lenght; tmp4++)
	{
		// Unsplit nibble to bytes, treat only even bytes, update checksum
		if(!(tmp4 & 1))
		{
			// get MSB nibble
			tmp2 = serial_in_buffer[(start_index + tmp4) & (SERIAL_IN_BUFF_LENGHT-1)];
			generic_buffer[tmp]  = tmp2 << 4;
			tmp5 = (tmp5 + tmp2) & 127;
			// get LSB nibble
			generic_buffer[tmp] |=  serial_in_buffer[(start_index + tmp4 + 1) & (SERIAL_IN_BUFF_LENGHT-1)];
			tmp5 = (tmp5 + (generic_buffer[tmp] & 0x0F) ) & 127;
			tmp++;
		}
	}

	return(tmp5);
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	Byte_To_Generic_Nibbles()
// Inputs	: 	
// Outputs	: 	None
// Process  : 	convert nibbles to bytes, place in geneic buffer
///////////////////////////////////////////////////////////////////////////////////////
void Byte_To_Generic_Nibbles(uchar start_index, uchar lenght, far uchar *buff_src)
{
	uchar tmp2, tmp4;

	tmp2=0;
	for(tmp4=0; tmp4 < lenght; tmp4=tmp4+2)
	{
		// create MSB nibble, offset index from header size (f0 20 4f... ) (int cast is a workaround for zdsII v4.8.0)
		generic_buffer[start_index+tmp4]    = buff_src[tmp2] >> (int)4;
		//generic_buffer[start_index+tmp4]    = buff_src[tmp2] >> 4;
		// create LSB nibble
		generic_buffer[start_index+tmp4+1]  = buff_src[tmp2] & 0x0F;
		tmp2++;
	}
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	SendUploadCtrlToMidi()
// Inputs	: 	buffer_size : all nibbles size to upload + headers
// Outputs	: 	None
// Process  : 	generic_buffer contains all nibbles to send from index WID_SYX_UPLOAD_HEADER_SIZE (9)
///////////////////////////////////////////////////////////////////////////////////////
void SendUploadGenericBufferToMidi(uchar SYSEX_ID0_byte, uchar SYSEX_ID1_byte, uchar SYSEX_ID2_byte, uchar SYSEX_ID3_byte)
{
	uchar tmp, checksum_send;
//	uchar tmp2 = 0;

	generic_buffer[0] = MIDI_SOX;
	generic_buffer[1] = JLABS_SYX_ID0;
	generic_buffer[2] = JLABS_SYX_ID1;
	generic_buffer[3] = JLABS_SYX_ID2;
	generic_buffer[4] = JLABS_SYX_ID3;
	generic_buffer[5] = JLABS_SYX_ID4;
	// frame type
	generic_buffer[6] = SYSEX_ID0_byte | sysex_direction_sysxid0; //| prog_upload_sysxid0;
	// frame lenght
	generic_buffer[7] = SYSEX_ID1_byte; 
	// frame address or index
	generic_buffer[8] = SYSEX_ID2_byte;
	// frame group or dont care
	generic_buffer[9] = SYSEX_ID3_byte;

	// A chekcsum result on all datas has to be sent ?
//	if(SYSEX_ID0_byte & checksum_return_sysxid0)
//	{
		// all other generic_buffer bytes contain nibbles to send to midi
		checksum_send = 0;
		// calculate checksum to send on all nibbles into the buffer
		for(tmp=0; tmp < SYSEX_ID1_byte; tmp++)
			checksum_send = (checksum_send + generic_buffer[10 + tmp]) & 127;

//		tmp2 = 1;
		generic_buffer[10 + SYSEX_ID1_byte] = checksum_send;
//	}

//	generic_buffer[10 + SYSEX_ID1_byte + tmp2] = MIDI_EOX;
	generic_buffer[10 + SYSEX_ID1_byte + 1] = MIDI_EOX;

	// force acknoledge to be sent on all midi outputs
	//ParamCurrent->ctrl_status_1 |= (midi_out_1_en | midi_out_2_en | usb_out_en);

	// total lenght is upload header + all datas uploaded + checksum + F7
	// SendDispatchToMidi(&generic_buffer[0], (11 + SYSEX_ID1_byte + tmp2), (midi_out_1_en | midi_out_2_en | usb_out_en));
	SendDispatchToMidi(&generic_buffer[0], (11 + SYSEX_ID1_byte + 1), (midi_out_1_en | usb_out_en));
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: SendControlToMidi()
// Inputs	: None
// Outputs	: None
// Process  : Send to midi current_value, control_index
///////////////////////////////////////////////////////////////////////////////////////
void SendControlToMidi(void)
{

	// V1.2 Check MIDI structure is coherent with no corrupted bytes
	if(last_control_index != control_index)
	if(CheckMidiStruct() == MIDI_STRUCT_NOT_VALID)
    {
		// Here must display struct error on LCD incrust;
		Display_Invalid_Midi_Struct_Incrust();
		return;
	}

	// Need to insert midi channel ?
	if(ParamCurrent->midi_channel_position)
	{
		ParamCurrent->midi_string[ParamCurrent->midi_channel_position-1] &= 0xF0;
		ParamCurrent->midi_string[ParamCurrent->midi_channel_position-1] |= ( ParamCurrent->midi_channel & 0x0F); 
	}

	// Check if Min & Max values are different from 0-127
	Truncate_Current_Value();

	// insert control value 0 if required
	if(ParamCurrent->ctrl_value_position_0)
		ParamCurrent->midi_string[ParamCurrent->ctrl_value_position_0-1]  = current_value & 0x7F;

	// Now send the full midi string in user defined mode
	SendDispatchToMidi(&ParamCurrent->midi_string[0], ParamCurrent->midi_str_lenght, ParamCurrent->ctrl_status_1);
}


// ***************************************************************************
// Routine	: FillParamCurrentStruct()
// Input	: None
// Output	: 
// Process	: Check if current param struct to send is already present in the 
// param struct located in RAM or if we need to load it from external eeprom
// ***************************************************************************
void FillParamCurrentStruct(void)
{
	uchar i;
	uchar tmp;

	// check if current control index param struct is already in ram ?
	if(control_index != CTRL_UNDEF)
	{
		for(i=0; i<PARAM_STRUCT_IN_RAM_MAX_NB; i++)
		{
			if(control_index  == control_index_structs_in_ram[i])
			{
				// set current struct pointer to current control index
				ParamCurrent = &Param[i];
				// return since control index found in table
				return;
			}
		}
	}

	// HERE NO CONTROL INDEX FOUND IN RAM (otherwise the above retrun exits from this function

	// increase index on control indexes located in table	
	last_struct_index++;
	if(last_struct_index >= PARAM_STRUCT_IN_RAM_MAX_NB) 
		last_struct_index = 0;

	// add current struct for current control index into the table
	control_index_structs_in_ram[last_struct_index] = control_index;

	// get control EEprom address and I2cCommand
	GetCtrlStructEEaddFromIndexGroup();
	// Read struct full from eeprom 
	ReadMultiByteI2c((far uchar *)&Param[last_struct_index].message_type, CTRL_STRUCT_TOTAL_SIZE);
	// set current struct pointer to current control index
	ParamCurrent = &Param[last_struct_index];
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	CheckMidiStruct()
// Inputs	: 	
// Outputs	: 	MIDI_STRUCT_VALID - MIDI_STRUCT_NOT_VALID
// Process  : 	check and correct the midi struct before sending to MIDI or to modules
///////////////////////////////////////////////////////////////////////////////////////
uchar CheckMidiStruct(void)
{
	uchar i, tmp;
	far uchar *myptr;
	uchar local_lenght = ParamCurrent->midi_str_lenght;
 
	if(local_lenght <= MIDI_STR_MAX_LENGHT)
  	 if(ParamCurrent->midi_channel_position <= local_lenght)
	  if(ParamCurrent->midi_channel < CHANNEL_NB)
	   if(ParamCurrent->ctrl_value_position_0 <= local_lenght)
	    if(ParamCurrent->ctrl_value_min < ParamCurrent->ctrl_value_max)
		{
			myptr = &ParamCurrent->message_type;

			// check the midi and lcd structs does not contain 0xFF or 0xFE that are used to control frame
			for(i=0; i<CTRL_STRUCT_TOTAL_SIZE; i++)
			{	
				tmp = *(myptr + i);
				if( (tmp == SOF_ONEWIRE) || (tmp == EOF_ONEWIRE))
					return(MIDI_STRUCT_NOT_VALID);
			}
			return(MIDI_STRUCT_VALID);
		}

	return(MIDI_STRUCT_NOT_VALID);
}


///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	Truncate_Current_Value()
// Inputs	: 	None
// Outputs	: 	None
// Process  : 	Truncate current_value according to min and max
///////////////////////////////////////////////////////////////////////////////////////
void Truncate_Current_Value(void)
{
	uchar  range;

	range = ParamCurrent->ctrl_value_max - ParamCurrent->ctrl_value_min + 1;

	if(range == 128) return; 

	if(current_value == 0)   current_value = ParamCurrent->ctrl_value_min;
   else
 	if(current_value == 127) current_value = ParamCurrent->ctrl_value_max;
   else
   {
      current_value = (uchar)((((uint)current_value * (uint)(range)) >> (int)7) & 0x7F);
      current_value += ParamCurrent->ctrl_value_min;
   }
}

