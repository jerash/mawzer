#ifndef EEPROM
#define EEPROM

///////////////////////////////////////////////////////////
// Local Definition
///////////////////////////////////////////////////////////
//#define EEPROM_I2C_ADR_POS				1U		// ???
#define EEPROM_I2C_ADR					(0x00U)		// EEprom I2C adress

#define EEPROM_FULL_SIZE				2048	// 2 KB
#define EEPROM_PAGE_SIZE				16U		// Page size is 16 bytes
#define I2C_SLAVE_ADR_EE				0xA0U	// I2C EEPROM control Write Address

//#define EE_READ_CTRL    				0xA1U	// I2C EEPROM control Read Address
//#define EE_WRITE_CTRL   				0xA0U	// I2C EEPROM control Write Address

///////////////////////////////////// EEPROM MAPPING //////////////////////////////////////////////////////// 
//
// We have to store following data in EEPROM : 
//
// 		- module_name[MODULE_NAME_LENGHT] ;   		(8 bytes)		Address 0			Page 0
//
// 		- POT0 GROUP0 assignment struct;  			(32 bytes)		Address 16			Page 1
//  	- POT1 GROUP0 assignment struct;  			(32 bytes)		Address 16 +1*32
//  	- POT2 GROUP0 assignment struct;  			(32 bytes)		Address 16 +2*32
//  	- POT3 GROUP0 assignment struct;  			(32 bytes)		Address 16 +3*32
//
// 		- POT0 GROUP1 assignment struct;  			(32 bytes)		Address 16+4*32		
//  	- POT1 GROUP1 assignment struct;  			(32 bytes)		Address 16+4*32	+1*32
//  	- POT2 GROUP1 assignment struct;  			(32 bytes)		Address 16+4*32	+2*32
//  	- POT3 GROUP1 assignment struct;  			(32 bytes)		Address 16+4*32	+3*32
//
// 		- POT0 GROUP2 assignment struct;  			(32 bytes)		Address 16+2*4*32		
//  	- POT1 GROUP2 assignment struct;  			(32 bytes)		Address 16+2*4*32 +1*32
//  	- POT2 GROUP2 assignment struct;  			(32 bytes)		Address 16+2*4*32 +2*32
//  	- POT3 GROUP2 assignment struct;  			(32 bytes)		Address 16+2*4*32 +3*32
//
// 		- POT0 GROUP3 assignment struct;  			(32 bytes)		Address 16+3*4*32		
//  	- POT1 GROUP3 assignment struct;  			(32 bytes)		Address 16+3*4*32 +1*32
//  	- POT2 GROUP3 assignment struct;  			(32 bytes)		Address 16+3*4*32 +2*32
//  	- POT3 GROUP3 assignment struct;  			(32 bytes)		Address 16+3*4*32 +3*32
//
// Use a linear 16b address, data must be re-aligned on EEPROM pages size (16 bytes)
//
////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// CONTROL STRUCTURE DEFINITION /////////////////////////////////
// This is the 32 bytes structure of control definition assignment mapped in EEProm
//
//			typedef struct {
//			uchar message_type;						// CC, NOTE, NRPN, RPN, PITCHBEND, SYSEX, ...
//			uchar message_nb;						// note or cc number
//			uchar ctrl_status_1;					// 1 byte, control status 
//			uchar midi_str_lenght;					// 1 byte, midi string lenght
//			uchar midi_channel_position;			// 1 byte, midi channel position
//			uchar midi_channel;						// 1 byte, the midi channel
//			uchar ctrl_value_position_0;			// 1 byte, value position 0
//			uchar ctrl_value_min;					// 1 byte, control min value
//			uchar ctrl_value_max;					// 1 byte, control max value
//			uchar midi_string[MIDI_STR_MAX_LENGHT];	// the midi string
//			uchar lcd_string[LCD_STR_MAX_LENGHT];	// the lcd string
//			} All_Params ;
//
//			enum {CC, NOTE, NRPN, RPN, PITCHBEND, SYSEX, PROG_CHANGE};
//			
//			#define CTRL_STRUCT_HEADER_SIZE 9U
//			#define CTRL_STRUCT_TOTAL_SIZE (CTRL_STRUCT_HEADER_SIZE + MIDI_STR_MAX_LENGHT + LCD_STR_MAX_LENGHT)
//			
//			enum {message_type_index, message_nb_index, ctrl_status_1_index, midi_str_lenght_index, 
//			midi_channel_position_index, midi_channel_index, ctrl_value_position_0_index, 
//			ctrl_value_min_index, ctrl_value_max_index, midi_string_index/*, lcd_string_index*/};
//			#define lcd_string_index (midi_string_index + MIDI_STR_MAX_LENGHT)
//			
//			// ctrl_status_1 bits definition
//			// Bit 7 must always be zero
//			#define switch_ctrl_mode		(1<<5)		// bit 7 = switch mode (0=push/1=toggle), not used for pots
//			#define usb_out_en				(1<<4)		// bit 4 = send over USB   		DO NOT CHANGE ORDER
//			#define midi_out_1_en			(1<<3)		// bit 3 = midi out 1 enable	DO NOT CHANGE ORDER
//
//
////////////////////////////////////////////////////////////////////////////////////////////


#define MODULE_NAME_EE_ADR                  EEPROM_I2C_ADR    // lenght = 8

// assignement struct start from 16 just to be sure we are aligned on a eeprom page
#define MODULE_ASSIGN_STRUCT_BASE_EE_ADR    16U 
#define MODULE_ASSIGN_STRUCT_SIZE			32U

// size of all control assignement struct for ONE group
#define GROUP_ASSIGN_STRUCT_SIZE			(MODULE_ASSIGN_STRUCT_SIZE*CONTROL_NB)

// total assignement struct lenght = 4 controls x 4 groups x 32 bytes
#define TOTAL_ASSIGN_STRUCT_SIZE			(GROUP_ASSIGN_STRUCT_SIZE*GROUP_NB)

// Check assignment structure is aligned on eeprom page size
#if (MODULE_ASSIGN_STRUCT_BASE_EE_ADR % EEPROM_PAGE_SIZE)
    #error MODULE_ASSIGN_STRUCT_BASE_EE_ADR is not aligned on a eeprom page size
#endif

#define TOTAL_EE_BYTES_ASSIGNED	 (MODULE_ASSIGN_STRUCT_BASE_EE_ADR + TOTAL_ASSIGN_STRUCT_SIZE)

// PERFORM FINAL CHECKS ON DEFINES
#if (TOTAL_EE_BYTES_ASSIGNED > EEPROM_FULL_SIZE)
	#error EEPROM SIZE IS TOO SHORT FOR DATA
#endif

///////////////////////////////////////////////////////////
// Function Prototypes
///////////////////////////////////////////////////////////

void  LockEEprom10Ms(void);

#endif