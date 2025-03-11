#ifndef COMMONDEF
#define COMMONDEF

///////////////////////////////////////////////////////////
// Common definitions
///////////////////////////////////////////////////////////
//#define Z8_COMPIL 	1

typedef unsigned char uchar;  	// 8 bits (byte)
typedef unsigned int uint;		// 16 bits
typedef unsigned long ulong;	// 32 bits
#define NULL 0

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

// Z8_STATUS_0 bits definition
#define onewire_serial_changed_z0			(1<<7)	// bit 7 : 
#define onewire_waiting_for_sof_z0			(1<<6)	// bit 6 : 
#define onewire_waiting_for_id_z0			(1<<5)	// bit 5 :
#define onewire_waiting_for_sub_id_z0		(1<<4)	// bit 4 : 
#define onewire_waiting_for_data_z0			(1<<3)	// bit 3 : 
#define onewire_mb_request_z0				(1<<2)	// bit 2 : 
#define onewire_no_new_data_z0				(1<<1)	// bit 2 : 
#define disable_ow_rx_z0					(1<<0)	// bit 1 :  1 = disable readback on onewire rx

// Z8_STATUS_1 bits definition
#define onewire_presence_answer_request_z1	(1<<7)	// bit 7 : 
#define onewire_value_answer_request_z1		(1<<6)	// bit 6 : 
#define onewire_read_reg_answer_request_z1	(1<<5)	// bit 5 :
#define onewire_write_reg_request_z1		(1<<4)	// bit 4 : 

// Z8_STATUS_4 bits definition
#define first_pass_z4						(1<<7)	// bit 7 : 
#define eeprom_locked_10ms_z4				(1<<6)	// bit 6 : 

// Z8_STATUS_8 bits definition (ERROR CODE)
#define onewire_mb_unknown_cmd_z8			(1<<7)	// bit 7 : 
#define onewire_rx_overflow_z8				(1<<6)	// bit 6 : 
#define	onewire_mb_unknown_mem_type_z8		(1<<5)	// bit 5 : 
#define	onewire_mb_unexpected_eof_z8		(1<<4)	// bit 5 : 
#define	onewire_mb_missing_eof_z8			(1<<3)	// bit 5 : 
#define onewire_mb_bad_data_lenght_type_z8  (1<<2)	// bit 5 : 
#define i2c_timout_z8						(1<<1)	// 1 if a i2c timeout has occured



////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

#define FALSE				0U
#define TRUE				1U

#endif