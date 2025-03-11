#ifndef ONEWIRE
#define ONEWIRE

///////////////////////////////////////////////////////////
// Local Definition
///////////////////////////////////////////////////////////

#define MODULES_MAX_NB				32U		// Up to 32 modules can be connected
#define MODULES_SAME_MAX_NB			32U		// Up to 32 identical modules can be connected
#define MODULES_TYPE_MAX_NB			5U		// FAD, XFAD, ENC, POT, BUT
#define ONEWIRE_BUS_MAX_NB			8U		// There are 8 one wire buses
#define ONE_WIRE_MUX_PORT_OUT_MSK	0x07	// one wire buses muxes are on bits 0-2 of port H
#define MODULES_MAX_NB_PER_BUS		4U		// there are 4 modules max per one wire bus
#define CTRL_MAX_NB_PER_MODULE		4U		// there are 4 controls max per module
#define MAX_HOOK_BYTES_PER_MODULE	2U		// there are 2 hook bytes max per module (since 4 controls max)

#define MODULE_UNDEF				0xFF
#define MODULE_SEARCH_TIMEOUT		5U		// in multiple of 1ms (timer 3)
#define MODULE_REQ_CHANGE_TIMEOUT	5U		// in multiple of 1ms (timer 3)
#define MODULE_READ_REG_TIMEOUT		30U		// in multiple of 1ms (timer 3)

#define ONEWIRE_RX_BUFF_LENGHT		128U

#define ONEWIRE_MAX_SEARCH			(MODULES_MAX_NB * MODULES_TYPE_MAX_NB * ONEWIRE_BUS_MAX_NB)
#define ONEWIRE_STEP_SEARCH			(ONEWIRE_MAX_SEARCH / 128)
// #define ONEWIRE_STEP_AUTOASSIGN		((MODULES_MAX_NB * CTRL_MAX_NB_PER_MODULE * GROUP_NB) / 128)

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

#define ONEWIRE_HEADER_CMD_RW_REGISTER_SIZE		0x04	// MEM_TYPE, ADDRESS_HIGH, ADDRESS_LOW, LENGHT

#define ONEWIRE_CMD_DIRECTION_BIT		(1<<7U)	//  1 : module > MB  /  0 : MB > Module

// MEM_TYPE selects memory type to read/write from/to the modules
enum { FLASH_MOD, RAM_MOD, EEPROM_MOD };

#define ONEWIRE_NO_NEW_DATA			0xFD
#define MODULE_TYPE_LENGHT			0x08
#define MODULE_NAME_LENGHT			0x08

#define ONEWIRE_ID_SINGLE_MODULE	0x01	// A single module ID / SUB-ID is concerned by the command
#define ONEWIRE_ID_ALL_MODULES		0xFC	// When ID and SUB-ID are 0xFC : all modules are concerned by the command

enum {SOF_ONEWIRE_POS, ID_ONEWIRE_POS, SUB_ID_ONEWIRE_POS, ONEWIRE_CMD_FRAME_POS, ONEWIRE_DATA_FRAME_POS};
#define ONEWIRE_HEADER_FRAME_SIZE 	0x05	// SOF, ID, SUB_ID, CMD, EOF

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

#define hook_direction_bit	(1<<1)	// 0 : valeur physique < valeur du groupe
#define control_hooked_bit	(1<<0)	// 0 : control hooked / 1 : control not hooked

enum {CTRL_HOOKED, CTRL_LOWER, CTRL_HIGHER};

// If no value has changed in the module :
// SOF_ONEWIRE, ID, SUB_ID, COMMAND, ONEWIRE_NO_NEW_DATA, EOF_ONEWIRE


// *** ANSWER to ONEWIRE_CMD_READ_REGISTER ***
//
// SOF_ONEWIRE, ID, SUB_ID, COMMAND, MEM_TYPE, ADDRESS, LENGHT, DATA, EOF_ONEWIRE

// *** ANSWER to ONEWIRE_CMD_WRITE_REGISTER ***
//
// No acknoledge in this firmware version

////////////////// MEMORY MAPPING OF MODULES /////////////////////

#define MODULE_EEPROM_PAGE_SIZE		16

// *** RAM ***
// Address 0  : current_group
#define CURRENT_GROUP_ADR		0	// lenght = 1
 
// *** FLASH ***

// *** EEPROM ***
#define MODULE_NAME_EE_ADR					0	// lenght = 8
#define MODULE_ASSIGN_STRUCT_BASE_EE_ADR	16	// lenght = 4 controls max x 4 groups x 32 bytes 
												// start from 16 just to be sure we are aligned on a eeprom page

#if (MODULE_ASSIGN_STRUCT_BASE_EE_ADR % MODULE_EEPROM_PAGE_SIZE)
	#error MODULE_ASSIGN_STRUCT_BASE_EE_ADR is not aligned on a eeprom page size
#endif


//////////////////////////////////////////////////////////////////////////////
// This is the structure of all connected modules
//////////////////////////////////////////////////////////////////////////////
typedef struct {
uchar id;								// the id of the connected module (0-4)(BUT_MOD, POT_MOD, ENC_MOD, XFD_MOD, FAD_MOD)
uchar sub_id;							// the sub-id of the connected module (0-31)
uchar bus;								// the current bus (0-7) where the module is attached
uchar controlindexbase;					// control index base for this module
uchar module_type[MODULE_TYPE_LENGHT];	// the type of the module (FAD110, ENC410, POTh410, POTv410, BUT410, XFD010)
uchar module_name[MODULE_NAME_LENGHT];	// the name of the module (8 chars max - user defined)
uchar values[CTRL_MAX_NB_PER_MODULE];	// there are 4 controls max per module		
} All_Modules;

///////////////////////////////////////////////////////////
// Function Prototypes
///////////////////////////////////////////////////////////
void InitModules(void);
void SearchModules(void);
void ScanCurrentModule(void);
void CheckOneWireRxBuffer(void);
void GetDisplayModuleInfoFromRxBuffer(void);
void GetModuleValuesFromRxBuffer(void);
void SelectOneWireBusMuxes(void);
void SendCommandToModule(uchar command, uchar data_lenght, uchar destination, far uchar *buffer);
void StartTimerOneWire(uchar mult1ms);
void SendCurrentGroupToAllModules(void);
void GetAllStructFromModules(void);
void GetCompareSaveStructsFromModule(void);
void GetModuleRegistersFromRxBuffer(void);
#endif