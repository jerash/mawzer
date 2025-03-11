#ifndef ONEWIRE
#define ONEWIRE

///////////////////////////////////////////////////////////
// Local Definition
///////////////////////////////////////////////////////////
#define ONEWIRE_RX_BUFF_LENGHT		128U
#define GENERIC_BUFF_SIZE	  	64U		// size of a generic buffer used for read / write multibyte

#define GROUP_NB				4U		// number of groups supported 
#define CONFIG_REG_TABLE_MAX  	64U		// size of the RAM table shared with MB
#define MODULES_SAME_MAX_NB		32U		// Up to 32 identical modules can be connected
#define MODULES_SUB_ID_NB_BITS	5U
#define BAUD1 	 				125000 // 62500      				// 62,5K baud for ONEWIRE 

//////////////////////////////////////////////////////////
////////// ONEWIRE COMMUNICATION PROTOCOL ////////////////
//////////////////////////////////////////////////////////
#define SOF_ONEWIRE						0xFFU
#define EOF_ONEWIRE						0xFEU

// MODULES IDS
enum {BUT_MOD, POT_MOD, ENC_MOD, XFD_MOD, FAD_MOD };

// PROTOCOL FRAME DESCRIPTION FROM MB TO MODULES
////////////////////////////////////////////////

// SOF_ONEWIRE, ID, SUB_ID, COMMAND, (DATA), EOF_ONEWIRE

// With ID 		: ID (0-4) of the module (can be ONEWIRE_ID_ALL_MODULES for all modules)
// 		SUB-ID	: SUB-ID (0-31) of the module (can be ONEWIRE_ID_ALL_MODULES for all modules)

////////// MOTHERBOARD TO/FROM MODULES COMMANDS ///////////////

#define ONEWIRE_CMD_REQUEST_INFO		0x00	// MB request to module if it is present
// SOF_ONEWIRE, ID, SUB_ID, COMMAND, EOF_ONEWIRE
#define ONEWIRE_CMD_GET_VALUES			0x01	// MB request to module the new values (if they changed)
// SOF_ONEWIRE, ID, SUB_ID, COMMAND, EOF_ONEWIRE
#define ONEWIRE_CMD_READ_REGISTER		0x02	// MB request to module to read registers
// SOF_ONEWIRE, ID, SUB_ID, COMMAND, MEM_TYPE, ADDRESS_HIGH, ADDRESS_LOW, LENGHT , EOF_ONEWIRE
#define ONEWIRE_CMD_WRITE_REGISTER		0x03	// MB request to module to write registers
// SOF_ONEWIRE, ID, SUB_ID, COMMAND, MEM_TYPE, ADDRESS_HIGH, ADDRESS_LOW, LENGHT , DATA, EOF_ONEWIRE
#define ONEWIRE_CMD_ERROR_RETURN    	0x04
// SOF_ONEWIRE, ID, SUB_ID, COMMAND, ERREUR_TYPE,  EOF_ONEWIRE

#define ONEWIRE_CMD_DIRECTION_BIT		(1<<7U)	//  1 : module > MB  /  0 : MB > Module

// ERROR_TYPE
enum {UNKNOWN_CMD, ONEWIRE_OVERFLOW, BAD_MEM_TYPE, UNEXPECTED_EOF, MISSING_EOF};

// MEM_TYPE selects memory type to read/write from/to the modules
enum { FLASH_MOD, RAM_MOD, EEPROM_MOD };

#define ONEWIRE_NO_NEW_DATA			0xFD
#define MODULE_TYPE_LENGHT			0x08
#define MODULE_NAME_LENGHT			0x08

#define ONEWIRE_ID_SINGLE_MODULE	0x01	// A single module ID / SUB-ID is concerned by the command
#define ONEWIRE_ID_ALL_MODULES		0xFC	// When ID and SUB-ID are 0xFC : all modules are concerned by the command

////////// ANSWERS FROM MODULES TO MOTHERBOARD /////////////////

// *** ANSWER to ONEWIRE_CMD_REQUEST_INFO ***
//
// SOF_ONEWIRE, ID, SUB_ID, COMMAND, MODULE_TYPE, MODULE_NAME, ASSIGNMENTS_GROUP0, ASSIGNMENTS_GROUP1, ... ASSIGNMENTS_GROUP3,  EOF_ONEWIRE
// With : 	MODULE_TYPE 		: FAD110, ENC410, POTh410, POTv410, BUT410, XFD010 (8 characters) (this info is stored in ROM of the module)
// 			MODULE_NAME 		: user defined string (8 characters) assigned to each connected module
//			ASSIGNMENTS_GROUPn	: all structures for all 4 groups 

// *** ANSWER to ONEWIRE_CMD_GET_VALUES ***
//
// If one value (at least) has changed in the module :
// SOF_ONEWIRE, ID, SUB_ID, COMMAND, HOOK_STATE_1, HOOK_STATE_2, VALUE_CTRL_0, VALUE_CTRL_1, VALUE_CTRL_2, VALUE_CTRL_3, EOF_ONEWIRE
// HOOK_STATE_1 defines the hook state of controls 0 and 1 on the module
// HOOK_STATE_2 defines the hook state of controls 2 and 3 on the module
// HOOK_STATE_N bits definition are :
//	bits 7-4 : 	Always 0
//	bits 3,2 : 	x,0 : le contrôle est « hooké »
//		  		0,1 : contrôle non « hooké », valeur physique < valeur du groupe
//		  		1,1 : contrôle non « hooké », valeur physique > valeur du groupe

// If no value has changed in the module :
// SOF_ONEWIRE, ID, SUB_ID, COMMAND, ONEWIRE_NO_NEW_DATA, EOF_ONEWIRE


// *** ANSWER to ONEWIRE_CMD_READ_REGISTER ***
//
// SOF_ONEWIRE, ID, SUB_ID, COMMAND, MEM_TYPE, ADDRESS, LENGHT, DATA, EOF_ONEWIRE

// *** ANSWER to ONEWIRE_CMD_WRITE_REGISTER ***
//
// No acknoledge in this firmware version


////////////////// MEMORY MAPPING OF MODULES /////////////////////

// *** RAM ***
// Address 0  : current_group
#define CURRENT_GROUP_ADR        0    // lenght = 1
 
// *** FLASH ***

// *** EEPROM ***
// See in eeprom.h

///////////////////////////////////////////////////////////
// Function Prototypes
///////////////////////////////////////////////////////////
void CheckOneWire(void);
void CheckOneWireRx(void);
void SendOneWireInfo(void);
void SendOneWireValues(void);
void SendOneWireRegisters(void);
void WriteRegisters(void);
void SendOneWireFrameHeader(void);

#endif