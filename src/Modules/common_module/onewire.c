#include <eZ8.h>

#ifdef _MODULE_POT
	#include "pins_defs_POT.h"
	#include "commondef.h"
	#include "onewire.h"
	#include "controls_POT.h"
#endif

#ifdef _MODULE_FAD
	#include "pins_defs_FAD.h"
	#include "commondef.h"
	#include "onewire.h"
	#include "controls_FAD.h"
#endif

#ifdef _MODULE_BUT
	#include "pins_defs_BUT.h"
	#include "commondef.h"
	#include "onewire.h"
	#include "controls_BUT.h"
#endif

#ifdef _MODULE_ENC
	#include "pins_defs_ENC.h"
	#include "commondef.h"
	#include "onewire.h"
	#include "controls_ENC.h"
#endif

#include "hardware.h"
#include "i2c.h"
#include "eeprom.h"

///////////////////////////////////////////////////////////////////////////////////////
// GLOBAL VARS
///////////////////////////////////////////////////////////////////////////////////////

far uchar onewire_rx_buffer[ONEWIRE_RX_BUFF_LENGHT];		// one wire serial in buffer
uchar onewire_rx_start_index;						// a near pointer on FAR ram
uchar onewire_rx_end_index;							// a near pointer on FAR ram
uchar onewire_rx_current_index;

extern uchar Z8_STATUS_1;
extern uchar Z8_STATUS_0;
extern uchar Z8_STATUS_8;

extern far uchar module_sub_id;
extern rom uchar module_type[MODULE_TYPE_LENGHT]; 
extern uchar module_name[MODULE_TYPE_LENGHT];
extern far uchar assignment[CONTROL_NB][GROUP_NB];

far uchar config_reg[CONFIG_REG_TABLE_MAX] ;  // this is the RAM table we use to share data with MB
far uchar generic_buffer[GENERIC_BUFF_SIZE];  // a generic far ram buffer of 64 bytes

extern uint  EE_Address;

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	CheckOneWire()
// Inputs	: 	None
// Outputs	: 	None
// Process  : 	This routine check all onewire request flags and call the relevant process.
//
//				NB: We check One Wire Rx and treat the request in different routines so
//				that we can always split this process to be able to return back to Main()
//				more often whenever useful in the future.
///////////////////////////////////////////////////////////////////////////////////////
void CheckOneWire(void)
{
	// Check for request from MB
	CheckOneWireRx();		

	// Now check if module must answer to MB
	// Check for presence anwser request
	if(Z8_STATUS_1 & onewire_presence_answer_request_z1)
		SendOneWireInfo();

	// Check for values answer request
	else if (Z8_STATUS_1 & onewire_value_answer_request_z1)
		SendOneWireValues();

	// Check for read register answer request
	else if (Z8_STATUS_1 & onewire_read_reg_answer_request_z1)
		SendOneWireRegisters();

	// Check if MB ask to write registers
	else if (Z8_STATUS_1 & onewire_write_reg_request_z1)
		WriteRegisters();

}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: 	CheckOneWireRx()
// Inputs	: 	None
// Outputs	: 	None
// Process  : 	If onwire buffer is filled out, we extract command and flag for processing
//				
///////////////////////////////////////////////////////////////////////////////////////
void CheckOneWireRx(void)
{
	// Check for MB request (Onewire buffer is filled out)
	if(Z8_STATUS_0 & onewire_mb_request_z0)
	{
		// Check all the different commands
		if(onewire_rx_buffer[onewire_rx_start_index] == ONEWIRE_CMD_REQUEST_INFO) 
		{
			Z8_STATUS_1 |= onewire_presence_answer_request_z1;
			// update start index to reset circular buffer
			onewire_rx_start_index = (onewire_rx_end_index - 1)&(ONEWIRE_RX_BUFF_LENGHT-1);
		}
		else if (onewire_rx_buffer[onewire_rx_start_index] == ONEWIRE_CMD_GET_VALUES) 
		{
			Z8_STATUS_1 |= onewire_value_answer_request_z1;
			// update start index to reset circular buffer 
			onewire_rx_start_index = (onewire_rx_end_index - 1)&(ONEWIRE_RX_BUFF_LENGHT-1);
		}
		else if (onewire_rx_buffer[onewire_rx_start_index] == ONEWIRE_CMD_READ_REGISTER) 
		{
			Z8_STATUS_1 |= onewire_read_reg_answer_request_z1;
			// update start index to next frame (MEM_TYPE)
			onewire_rx_start_index = (onewire_rx_start_index + 1)&(ONEWIRE_RX_BUFF_LENGHT-1);
		}
		else if (onewire_rx_buffer[onewire_rx_start_index] == ONEWIRE_CMD_WRITE_REGISTER) 
		{
			Z8_STATUS_1 |= onewire_write_reg_request_z1;
			// update start index to next frame (MEM_TYPE)
			onewire_rx_start_index = (onewire_rx_start_index + 1)&(ONEWIRE_RX_BUFF_LENGHT-1);
		}
		else 
		{
			// unknown command
			Z8_STATUS_8 |= onewire_mb_unknown_cmd_z8;
			// update start index anyway
			onewire_rx_start_index = (onewire_rx_end_index - 1)&(ONEWIRE_RX_BUFF_LENGHT-1);
		}

		// MB request has been treated
		Z8_STATUS_0 &= ~onewire_mb_request_z0;
	}
}


///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: SendOneWireInfo()
// Inputs	: None
// Outputs	: None
// Process  : 
// *** ANSWER to ONEWIRE_CMD_REQUEST_INFO ***
//
// SOF_ONEWIRE, ID, SUB_ID, COMMAND, MODULE_TYPE, MODULE_NAME, ASSIGNMENTS_GROUP0, ASSIGNMENTS_GROUP1, ... ASSIGNMENTS_GROUP3,  EOF_ONEWIRE
// With : 	MODULE_TYPE 		: FAD110, ENC410, POTh410, POTv410, BUT410, XFD010 (8 characters) (this info is stored in ROM of the module)
// 			MODULE_NAME 		: user defined string (8 characters) assigned to each connected module
//			ASSIGNMENTS_GROUPn	: all structures for all 4 groups 
///////////////////////////////////////////////////////////////////////////////////////
void SendOneWireInfo(void)
{
	uchar idx;
	uchar group_idx, ctrl_idx;

	// Send SOF, ID, SUBID
	SendOneWireFrameHeader();

	// Send COMMAND
	SendToTx0(ONEWIRE_CMD_REQUEST_INFO | ONEWIRE_CMD_DIRECTION_BIT);

	// Send module TYPE               
	for(idx=0; idx<MODULE_TYPE_LENGHT ;idx++)
		SendToTx0(module_type[idx]);

	// Send module NAME			    	
	for(idx=0; idx<MODULE_NAME_LENGHT ;idx++)
		SendToTx0(module_name[idx] & 0x7F);
	
	// Send End of frame
	SendToTx0(EOF_ONEWIRE);
	
	// Reset request flag
	Z8_STATUS_1 &= ~onewire_presence_answer_request_z1;
}


///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: SendOneWireValues()
// Inputs	: None
// Outputs	: None
// Process  : 
/// *** ANSWER to ONEWIRE_CMD_GET_VALUES ***
// 
//   If one value (at least) has changed in the module :
// SOF_ONEWIRE, ID, SUB_ID, COMMAND, HOOK_STATE_1, HOOK_STATE_2, VALUE_CTRL_0, VALUE_CTRL_1, VALUE_CTRL_2, VALUE_CTRL_3, EOF_ONEWIRE
//
// If no value has changed in the module :
// SOF_ONEWIRE, ID, SUB_ID, COMMAND, ONEWIRE_NO_NEW_DATA, EOF_ONEWIRE
//
///////////////////////////////////////////////////////////////////////////////////////
void SendOneWireValues(void)
{
	uchar index;
	uchar local_hook_state;
	uchar current_group;

	// Send SOF, ID, SUBID
	SendOneWireFrameHeader();

	// Send COMMAND
	SendToTx0(ONEWIRE_CMD_GET_VALUES | ONEWIRE_CMD_DIRECTION_BIT);

	// Check if any value has changed in Ctrl[] table
	if (Z8_STATUS_0 & onewire_no_new_data_z0)
		SendToTx0(ONEWIRE_NO_NEW_DATA);
	else 	
	{ 
		// Send Hook state bytes
		for (index = 0; index < NB_HOOK_STATE_BYTES; index++)
		{
			local_hook_state = SetHookStateByte(index);
			SendToTx0(local_hook_state);
		}

		// Send CC values
	 	for (index = 0; index < CONTROL_NB; index++)
			SendToTx0(GetControlValue(index));
	}
	
	// Send End of frame
	SendToTx0(EOF_ONEWIRE);

	//Now we don't have any new data to send to MB until next change in Ctrl[] table (see in CheckControls())
	Z8_STATUS_0 |= onewire_no_new_data_z0;

	// Reset request flag
	Z8_STATUS_1 &= ~onewire_value_answer_request_z1;

}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: SendOneWireRegisters()
// Inputs	: None
// Outputs	: None
// Process  : 
/// *** ANSWER to ONEWIRE_CMD_READ_REGISTER ***
//
// SOF_ONEWIRE, ID, SUB_ID, COMMAND, MEM_TYPE, ADDRESS, LENGHT, DATA, EOF_ONEWIRE
//
///////////////////////////////////////////////////////////////////////////////////////
void SendOneWireRegisters(void)
{
	uchar local_mem_type, local_lenght, local_lenght_saved, local_offset;
	uchar idx, current_value;
	uint local_address;
	
	// First we get the rest of the request
	local_mem_type = onewire_rx_buffer[onewire_rx_start_index]; 
	onewire_rx_start_index = (onewire_rx_start_index + 1)&(ONEWIRE_RX_BUFF_LENGHT-1);

	local_address = (uint)((uint)(onewire_rx_buffer[onewire_rx_start_index]) << 7) & (uint)(0x7F80);   	// ADDRESS HIGH        
	onewire_rx_start_index = (onewire_rx_start_index + 1)&(ONEWIRE_RX_BUFF_LENGHT-1);	
	
	local_address += (uint)(onewire_rx_buffer[onewire_rx_start_index] & 0x007F); 	// ADDRESS LOW
	onewire_rx_start_index = (onewire_rx_start_index + 1)&(ONEWIRE_RX_BUFF_LENGHT-1);

	local_lenght = onewire_rx_buffer[onewire_rx_start_index]; 
	onewire_rx_start_index = (onewire_rx_start_index + 1)&(ONEWIRE_RX_BUFF_LENGHT-1);	
	
	local_lenght_saved = local_lenght;

	// Check if MB request respects the protocol
	if(onewire_rx_buffer[onewire_rx_start_index] != EOF_ONEWIRE)
		Z8_STATUS_8 |= onewire_mb_bad_data_lenght_type_z8;
	
	// Request is totally stored, now update start index 
	onewire_rx_start_index = (onewire_rx_end_index - 1)&(ONEWIRE_RX_BUFF_LENGHT-1);

	// Send SOF, ID, SUBID
	SendOneWireFrameHeader();

	// Send COMMAND
	SendToTx0(ONEWIRE_CMD_READ_REGISTER | ONEWIRE_CMD_DIRECTION_BIT);

	// Send MEM_TYPE
	SendToTx0(local_mem_type);

    // Send ADDRESS HIGH
	SendToTx0((uchar)(local_address >> 7) & 0x7F);

    // Send ADDRESS LOW
	SendToTx0((uchar)(local_address & 0x007F));

	 // Send LENGHT
	SendToTx0(local_lenght);

	// Reach data on relevent MEM_TYPE and send it to MB 	
 	if(local_mem_type == RAM_MOD)
		for(idx = 0; idx < local_lenght; idx++)
		{
			// Send value to MB
			SendToTx0(config_reg[((uchar) local_address)+idx]);   // should I cast (uchar) local_adress+idx ?
		}
	else if(local_mem_type == EEPROM_MOD)
	{
		// Set read address 
		EE_Address = local_address;   
		
		local_offset = 0;
		// Check if data exceedes write page size 
		while (local_lenght > EEPROM_PAGE_SIZE)
		{
			// read EEPROM data into a generic buffer
			ReadMultiByteI2c_24C16((far uchar *)(&generic_buffer[local_offset]), EEPROM_PAGE_SIZE);
			// Update write address
			EE_Address += EEPROM_PAGE_SIZE;
			// Update local_length
			local_lenght -= EEPROM_PAGE_SIZE;
			// Update offset
			local_offset += EEPROM_PAGE_SIZE;
		}
		// read last paquet if still data to read
		if(local_lenght)
			ReadMultiByteI2c_24C16((far uchar *)(&generic_buffer[local_offset]), local_lenght);

		local_lenght = local_lenght_saved;

		// Now send data to MB
		for(idx = 0; idx < local_lenght; idx++)
			SendToTx0(generic_buffer[idx]);
	}
	/* 
	else if(local_mem_type == FLASH_MOD)
	........ 
	*/
	else 
		// unknown MEM_TYPE
		Z8_STATUS_8 |= onewire_mb_unknown_mem_type_z8;

	// Send End of frame
	SendToTx0(EOF_ONEWIRE);

	// Reset request flag
	Z8_STATUS_1 &= ~onewire_read_reg_answer_request_z1;
}


///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: WriteRegisters()
// Inputs	: None
// Outputs	: None
// Process  : 
// *** ANSWER to ONEWIRE_CMD_WRITE_REGISTER ***
//
// No acknoledge in this firmware version

///////////////////////////////////////////////////////////////////////////////////////
void WriteRegisters(void)
{
	uchar local_mem_type, local_lenght, local_offset;
	uchar idx, current_value;
	uint local_address;
	
	// First we get the rest of the request
	local_mem_type = onewire_rx_buffer[onewire_rx_start_index]; 
	onewire_rx_start_index = (onewire_rx_start_index + 1)&(ONEWIRE_RX_BUFF_LENGHT-1);

	local_address = (uint)((uint)(onewire_rx_buffer[onewire_rx_start_index]) << 7) & (uint)(0x7F80);   	// ADDRESS HIGH        
	onewire_rx_start_index = (onewire_rx_start_index + 1)&(ONEWIRE_RX_BUFF_LENGHT-1);	
	
	local_address += (uint)(onewire_rx_buffer[onewire_rx_start_index] & 0x007F); 	// ADDRESS LOW
	onewire_rx_start_index = (onewire_rx_start_index + 1)&(ONEWIRE_RX_BUFF_LENGHT-1);

	local_lenght = onewire_rx_buffer[onewire_rx_start_index]; 
	onewire_rx_start_index = (onewire_rx_start_index + 1)&(ONEWIRE_RX_BUFF_LENGHT-1);	

	// Now write MB data on relevent MEM_TYPE 
	if(local_mem_type == RAM_MOD)
		for(idx = 0; idx < local_lenght; idx++)
		// note : here we use a local idx loop instead of circular buffer pointer 
		// to avoid endless loop in case of request protocol problem 
		{
			// Set value and update start index
			config_reg[((uchar) local_address)+idx] = onewire_rx_buffer[onewire_rx_start_index]; // sould I cast (uchar) local_adress+idx ?
			onewire_rx_start_index = (onewire_rx_start_index + 1)&(ONEWIRE_RX_BUFF_LENGHT-1);
		}
	else if(local_mem_type == EEPROM_MOD)
	{
		// Set write address
		EE_Address = (uint)(local_address);

		local_offset = 0;
		// JDS : Copy all data to write to ee to generic buffer
		for(idx = 0; idx < local_lenght; idx++)
			generic_buffer[idx] = onewire_rx_buffer[(onewire_rx_start_index + idx)&(ONEWIRE_RX_BUFF_LENGHT-1)];

		// Check if data exceedes write page size 
		while (local_lenght > EEPROM_PAGE_SIZE)
		{
			// Write one page in EEprom	
			// WriteMultiByteI2c((far uchar *)(&onewire_rx_buffer[onewire_rx_start_index]),EEPROM_PAGE_SIZE);
			WriteMultiByteI2c_24C16((far uchar *)(&generic_buffer[local_offset]),EEPROM_PAGE_SIZE); 
			// Update write address
			EE_Address += EEPROM_PAGE_SIZE;
			//FF this may not be coherent with the circular buffer...
			// Update onewire buffer index (see protocol respect check later)	
			onewire_rx_start_index = (onewire_rx_start_index + EEPROM_PAGE_SIZE)&(ONEWIRE_RX_BUFF_LENGHT-1);
			// Update local_length
			local_lenght -= EEPROM_PAGE_SIZE;
			// Update offset
			local_offset += EEPROM_PAGE_SIZE;
	   	}		
		// Write last data paquet in EEprom	if still data
		if(local_lenght)
			WriteMultiByteI2c_24C16((far uchar *)(&generic_buffer[local_offset]),local_lenght);
		//FF this may not be coherent with the circular buffer...
		// Update onewire buffer index (see protocol respect check later)	
		onewire_rx_start_index = (onewire_rx_start_index + local_lenght)&(ONEWIRE_RX_BUFF_LENGHT-1);	
	}
	/*
	else if(local_mem_type == FLASH_MOD)
	........
	*/
	else 
		// unknown MEM_TYPE
		Z8_STATUS_8 |= onewire_mb_unknown_mem_type_z8;

	// Check if MB request respect the protocol
	if(onewire_rx_buffer[onewire_rx_start_index] != EOF_ONEWIRE)
		Z8_STATUS_8 |= onewire_mb_bad_data_lenght_type_z8;

	// Request is treated, now update start index 
	onewire_rx_start_index = (onewire_rx_end_index - 1)&(ONEWIRE_RX_BUFF_LENGHT-1);

	// Reset request flag
	Z8_STATUS_1 &= ~onewire_write_reg_request_z1;

	// Let the module checks what MB wrote in its registers
	ModuleChecksWrittenRegisters(local_mem_type, local_address);
}

///////////////////////////////////////////////////////////////////////////////////////
// Routine 	: SendOneWireFrameHeader()
// Inputs	: None
// Outputs	: None
// Process  : 
///////////////////////////////////////////////////////////////////////////////////////
void SendOneWireFrameHeader(void)
{

	// Disable readback on one wire RX
	Z8_STATUS_0 |= disable_ow_rx_z0;
	
	// Send Start of frame
	SendToTx0(SOF_ONEWIRE);
	
	// Send module ID
	SendToTx0(MODULE_ID);

	// Send module SUB-ID
	SendToTx0(module_sub_id);
}
