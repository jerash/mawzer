#ifndef COMMON_Z8_EZ
#define COMMON_Z8_EZ

typedef unsigned char uchar;
typedef unsigned int  uint;

#define Z8_FREQ 20000000ul   	// 20.0 MHz

#define BAUD_Z8_EZ 				31250L      		// 62.5K baud for UART0
#define BAUD_OCD_Z8_EZ 			62500L      		// 62.5K baud for Z8 On Chip debugger (Must be > 39kb at 20MHz)


// #define HW_V10	1 // uncoment only if hw v1.0 (usb monitor pin and cancel pins are inverted)

#define FW_VERSION_INDEX		0U
#define FW_SUB_VERSION_INDEX	1U
#define HW_VERSION_INDEX		2U
#define HW_SUB_VERSION_INDEX	3U
#define MANUF_YEAR_INDEX		4U
#define MANUF_MONTH_INDEX		5U
#define SERIAL_4_INDEX			6U
#define SERIAL_3_INDEX			7U
#define SERIAL_2_INDEX			8U
#define SERIAL_1_INDEX			9U
#define SERIAL_0_INDEX			10U

#define IDENTITY_LENGHT 		11

// Z8_STATUS_0_NVM bits definition (MUST BE LOADED From NVM AT STARTUP)
// bit 7 always 0
#define new_video_mode_z0nvm		(1<<6)	// 1 = inverted
#define video_mode_z0nvm			(1<<5)	// 1 = inverted
#define encoder_tempo_lock_z0nvm	(1<<4)	// 1 = lock tempo encoder
#define mtc_mode_on_off_z0nvm		(1<<3)	// 1 = realtime off, 0 = realtime on 	DO NOT CHANGE ORDER
#define mtc_mode_z0nvm				(1<<2)	// 1 = mtc / 0 = midi clock				DO NOT CHANGE ORDER
#define hook_zjump_mode_z0nvm		(1<<1)	// 1 = jumpless
#define jog_lock_z0nvm				(1<<0)	// 1 = lock big encoder

#define MTC_MODE_MASK_BIT	0x0C	// Bits 2 & 3

// Z8_STATUS_1_NVM bits definition (MUST BE LOADED From NVM AT STARTUP)
// bit 7 always 0
#define must_autoassign_z1nvm		(1<<6)	// 1 = must auto assign
#define smpte_frame_rate_1_z1nvm	(1<<2)	// 0 0 = 24 fps  	0 1 = 25 fps		DO NOT CHANGE ORDER
#define smpte_frame_rate_0_z1nvm	(1<<1)	// 1 0 = 30 fps nd	1 1 = 30 fps		DO NOT CHANGE ORDER

#define SMPTE_FRAME_RATE_MASK_BIT	0x06	// Bits 1 & 2
#define SMPTE_TYPE_BIT_POS			1U		// SMPTE type is in byte Hour Bits 1,2

// Z8_STATUS_2_NVM bits definition (MUST BE LOADED From NVM AT STARTUP)
// bit 7 always 0

////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////// REGISTERS THAT MUST BE SENT TO EZ AT STARTUP ///////////////////////////////

// CTRL_REG_0_7BITS_TO_EZ bits definition (MUST BE LOADED From NVM AT STARTUP, MUST BE SENT TO EZBOARD)
// Bit 7 must be 0 since this control reg is sent to EZBOARD
#define filter_state_z_ez0_bit_pos				6	
//#define 						 				5	
#define filter_type_z_ez0_bit_pos				4
#define midi_thru_ez0_bit_pos					3	
//#define merger_src_midi_in_state_z_ez0_bit_pos	3
//#define merger_src_usb_in_state_z_ez0_bit_pos	2
//#define merger_dst_usb_out_state_z_ez0_bit_pos	1
//#define merger_dst_out1_state_z_ez0_bit_pos		0

#ifdef Z8_COMPIL
#define filter_state_z_ez0					(1<<filter_state_z_ez0_bit_pos)				// on/off
// #define 
#define filter_type_z_ez0					(1<<filter_type_z_ez0_bit_pos)				// 1=filter all, 0=filter realtime
#define midi_thru_ez0 				  		(1<<midi_thru_ez0_bit_pos)	// on/off
//#define merger_src_midi_in_state_z_ez0 	(1<<merger_src_midi_in_state_z_ez0_bit_pos)	// on/off
//#define merger_src_usb_in_state_z_ez0		(1<<merger_src_usb_in_state_z_ez0_bit_pos)	// on/off
//#define merger_dst_usb_out_state_z_ez0	(1<<merger_dst_usb_out_state_z_ez0_bit_pos)	// on/off
//#define merger_dst_out1_state_z_ez0		(1<<merger_dst_out1_state_z_ez0_bit_pos)	// on/off
#endif

#ifdef EZ_COMPIL
#define filter_state_z_ez0					(CTRL_REGS_7BITS_FROM_Z8[CTRL_REG_0_7BITS_TO_EZ_INDEX] ^ filter_state_z_ez0_bit_pos)
// #define 
#define filter_type_z_ez0					(CTRL_REGS_7BITS_FROM_Z8[CTRL_REG_0_7BITS_TO_EZ_INDEX] ^ filter_type_z_ez0_bit_pos)
#define midi_thru_ez0						(CTRL_REGS_7BITS_FROM_Z8[CTRL_REG_0_7BITS_TO_EZ_INDEX] ^ midi_thru_ez0_bit_pos)
//#define merger_src_midi_in_state_z_ez0 	(CTRL_REGS_7BITS_FROM_Z8[CTRL_REG_0_7BITS_TO_EZ_INDEX] ^ merger_src_midi_in_state_z_ez0_bit_pos)
//#define merger_src_usb_in_state_z_ez0		(CTRL_REGS_7BITS_FROM_Z8[CTRL_REG_0_7BITS_TO_EZ_INDEX] ^ merger_src_usb_in_state_z_ez0_bit_pos)
//#define merger_dst_usb_out_state_z_ez0	(CTRL_REGS_7BITS_FROM_Z8[CTRL_REG_0_7BITS_TO_EZ_INDEX] ^ merger_dst_usb_out_state_z_ez0_bit_pos)
//#define merger_dst_out1_state_z_ez0		(CTRL_REGS_7BITS_FROM_Z8[CTRL_REG_0_7BITS_TO_EZ_INDEX] ^ merger_dst_out1_state_z_ez0_bit_pos)
#endif


// CTRL_REG_1_7BITS_TO_EZ bits definition (MUST BE LOADED From NVM AT STARTUP, MUST BE SENT TO EZBOARD)
// Bit 7 must be 0 since this control reg is sent to EZBOARD
#define ez_fw_midi_to_z8_z_ez1_bit_pos		4	// V1.5 ; z8 indicates to EZ it must receive midi events
#define realtime_source_int_z_ez1_bit_pos	1	// DO NOT CHANGE ORDER
#define realtime_source_ext_z_ez1_bit_pos	0	// DO NOT CHANGE ORDER
#define REALTIME_SRC_MSK	0x03				// Midi in, Usb in, internal

#ifdef Z8_COMPIL
#define ez_fw_midi_to_z8_z_ez1				(1<<ez_fw_midi_to_z8_z_ez1_bit_pos)		// on/off
#define realtime_source_int_z_ez1			(1<<realtime_source_int_z_ez1_bit_pos)	// 0=internal 0ff, 1=internal on 
#define realtime_source_ext_z_ez1			(1<<realtime_source_ext_z_ez1_bit_pos)	// 0=midi in, 1=usb in (external sync input)
#endif

#ifdef EZ_COMPIL
#define ez_fw_midi_to_z8_z_ez1				(CTRL_REGS_7BITS_FROM_Z8[CTRL_REG_1_7BITS_TO_EZ_INDEX] ^ ez_fw_midi_to_z8_z_ez1_bit_pos)
#define realtime_source_int_z_ez1			(CTRL_REGS_7BITS_FROM_Z8[CTRL_REG_1_7BITS_TO_EZ_INDEX] ^ realtime_source_int_z_ez1_bit_pos)
#define realtime_source_ext_z_ez1			(CTRL_REGS_7BITS_FROM_Z8[CTRL_REG_1_7BITS_TO_EZ_INDEX] ^ realtime_source_ext_z_ez1_bit_pos)
#endif

// LAST_CTRL_REG_7BITS_TO_EZ_INDEX bits definition (MUST BE LOADED From NVM AT STARTUP, MUST BE SENT TO EZBOARD)
// Bit 7 must be 0 since this control reg is sent to EZBOARD
//#define ez_board_fully_updated_ezlast		(1<<6)	// 1 when all regs dedicated to EZ have been sent
// or
#define EZ_FULLY_UPDATED	0x55U

////////////////////////////////////////////////////////////////////////////////////////////////////////
// MISC REGISTERS (PARTIALY) USED BY EZ BOARD, NOT TO SEND (COMMON DEF ONLY)
////////////////////////////////////////////////////////////////////////////////////////////////////////

// Z8_STATUS_8 bits definition
#define onewire_rx_overflow_z8			(1<<5)	// 0 =no overflow		DO NOT CHANGE ORDER
#define serial_in_overflow_z8			(1<<4)	// 0 =no overflow		DO NOT CHANGE ORDER
#define midi_in_realtime_overflow_z8	(1<<3)	// 0 =no overflow		DO NOT CHANGE ORDER
#define usb_in_realtime_overflow_z8		(1<<2)	// 0 =no overflow		DO NOT CHANGE ORDER
#define midi_in_overflow_z8				(1<<1)	// 0 =no overflow		DO NOT CHANGE ORDER
#define usb_in_overflow_z8				(1<<0)	// 0 =no overflow		DO NOT CHANGE ORDER

#define OVERFLOW_MSK					0x3FU	// mask bits 0-5
#define OVERFLOW_FROM_EZ_MSK			0x0FU	// mask bits 0-3 (only overflows from EZ board)
#define OVERFLOW_REALTIME_MSK			0x0CU	// mask bits 3-2
#define OVERFLOW_INPUT_MSK				0x03U	// mask bits 1-0
#define OVERFLOW_FROM_MIDI_MSK			0x0AU	// mask bits 1 and 3
#define OVERFLOW_FROM_USB_MSK			0x05U	// mask bits 0 and 2

//////////////////////////////////////////////////////////
// Z8 TO EZ (only) COMMUNICATION PROTOCOL ////////////////
//////////////////////////////////////////////////////////
#define SOF_TO_EZ						0xFFU
#define FRAME_FROM_Z8_TO_EZ_TYPE_MSK	0x60U 	// Bits 6-5 : data type (reg/midi)
#define FRAME_TO_EZ_IS_MIDI				0x20U
#define FRAME_TO_EZ_IS_CTRL_REG			0x00U
#define EOF_TO_EZ						0xFEU

// frame from z8 to ez status byte bits definition
// bit 7 is always 0					//  bit 7 : always 0
// bits 6:5 							// frame type 00=ctrl_reg 01=midi 
#define send_to_usb			(1 << 4)	// bit 4	// reg address or send to usb
#define send_to_out1		(1 << 3)	// bit 3 	// reg address or send to out 0
#define send_to_out2		(1 << 2)	// bit 2 	// reg address or send to out 1
// bits 1:0								// bits 1:0	// reg address or rfu

#define CTRL_REG_Z8_TO_EZ_ADDR_MSK		0x1F

//////////////////////////////////////////////////////////
// EZ TO Z8 (only) COMMUNICATION PROTOCOL ////////////////
//////////////////////////////////////////////////////////

#define CTRL_REG_MAX_FROM_EZ_TO_Z8		1U		// Nb of registers updated to the Z8 board

// CTRL_REG_0_7BITS_FROM_EZ bits definition : TOGGLE REGISTER
// the following bits def MUST be duplicated in main.c from EZ project file
//  bit 7 is always 0							//  bit 7 : always 0
#define usb_state_from_ez0_bit_pos					6
#define z8_board_fully_updated_from_ez0_bit_pos		5
#define must_start_bootloader_from_ez0_bit_pos		4
#define usb_enumrated_from_ez0_bit_pos				3

#ifdef Z8_COMPIL
#define usb_state_from_ez0					(1<<usb_state_from_ez0_bit_pos)	// 0=midi in, 1=usb in
#define z8_board_fully_updated_from_ez0		(1<<z8_board_fully_updated_from_ez0_bit_pos)	// on/off
#define must_start_bootloader_from_ez0		(1<<must_start_bootloader_from_ez0_bit_pos)	// on/off
#define usb_enumrated_from_ez0				(1<<usb_enumrated_from_ez0_bit_pos)	// on/off
#endif

#ifdef EZ_COMPIL
#define usb_state_from_ez0					(CTRL_REGS_7BITS_TO_Z8[0] ^ usb_state_from_ez0_bit_pos)
#define z8_board_fully_updated_from_ez0  	(CTRL_REGS_7BITS_TO_Z8[0] ^ z8_board_fully_updated_from_ez0_bit_pos)
#define must_start_bootloader_from_ez0		(CTRL_REGS_7BITS_TO_Z8[0] ^ must_start_bootloader_from_ez0_bit_pos)
#define usb_enumrated_from_ez0				(CTRL_REGS_7BITS_TO_Z8[0] ^ usb_enumrated_from_ez0_bit_pos)
#endif

// SYSEX ID0 (BYTE 6)	
//  bit 7 is always 0							//  bit 7 : always 0
#define sysex_direction_sysxid0			(1<<6U)	//  bit 6 : 							0=received sysex, 1=sent sysex
#define prog_upload_sysxid0				(1<<5U)	//	bit 5 : program or upload request	0=program request/answer 1=upload request/answer
#define prog_ez_z8_sysxid0				(1<<5U)	//	bit 5 : 0= program ez firmware 1= program z8 firmware 
#define pure_midi_from_usb_sysxid0		(1<<5U)	//	bit 5 : 0= pure midi from MIDI in conn 1= pure Midi from USB in (V1.5)
#define acknoledge_sysxid0				(1<<4U)	//	bit 4 : ack request/answer			0=no ack required/prog error  1=ack required/prog succeeded
#define checksum_check_sysxid0			(1<<3U)	//	bit 3 : checksum check request		0=no checksum to check  1=must check embedeed checksum (calculated from SYSEX ID2 to checksum)
//#define checksum_return_sysxid0		(1<<2U)	//	bit 2 : checksum return request		0=checksum return is not requested 1:checksum return is requested 
#define fw_znext_finish_sysxid0			(1<<2U)	//	bit 2 : 0= next firmware data available / 1=finish programming firmware
// bits 1:0 : frame type content				//  00=common parameter 01=lcd control assign	// DO NOT CHANGE ORDER
												//  10=firmware or midi	11=midi control assign  // DO NOT CHANGE ORDER
#define FRAME_TO_Z8_TYPE_MSK		 	0x03U	//	sysex content 
						
//v1.1 enum { FRAME_IS_COMMON_PARAM, FRAME_IS_LCD_CTRL_ASSIGN, FRAME_IS_FIRMWARE, FRAME_IS_MIDI_CTRL_ASSIGN, FRAME_IS_PURE_MIDI};
enum { FRAME_IS_COMMON_PARAM, FRAME_IS_LCD_CTRL_ASSIGN, FRAME_IS_FIRMWARE_OR_PURE_MIDI, FRAME_IS_MIDI_CTRL_ASSIGN};

// Addresses of common parameters uploaded/programmed from EZ board or external sysex
enum { 	// then with NVM regs dedicated to EZ board
		CTRL_REG_0_7BITS_TO_EZ_INDEX, CTRL_REG_1_7BITS_TO_EZ_INDEX, 
		LAST_CTRL_REG_7BITS_TO_EZ_INDEX,
		// continue with NVM regs dedicated to Z8 only
		Z8_STATUS_0_NVM_INDEX, Z8_STATUS_1_NVM_INDEX, CHANNEL_INDEX, GROUP_INDEX, INT_TEMPO_INDEX, 
        // the following regs cannot be accessed (from EZ board only)
        CTRL_REG_0_7B_FROM_EZ_INDEX, LCD_MESSAGE_TO_Z8_INDEX };

#define IDENTITY_REQUEST_INDEX 0x7F

// first index of NVM registers dedicated to EZ
#define CTRL_REG_TO_EZ_START_INDEX	 CTRL_REG_0_7BITS_TO_EZ_INDEX	
// last index of NVM register that can be programmed by external sysex

// last index of NVM register that can be programmed by external sysex or EZ board
#define LAST_COMMON_PARAM_INDEX      LCD_MESSAGE_TO_Z8_INDEX
// Nb of registers updated from the Z8 board
#define CTRL_REG_NB_FROM_Z8_TO_EZ	 (LAST_CTRL_REG_7BITS_TO_EZ_INDEX-CTRL_REG_0_7BITS_TO_EZ_INDEX+1)		

//SYSEX ID3 (BYTE 8)	
//  bit 7 		: always 0
//	bits 6:0 	: group or address	: control assign group GG
 
// JLABS SYSEX header from MMA
#define JLABS_SYX_ID0				0x00U
#define JLABS_SYX_ID1				0x20U
#define JLABS_SYX_ID2				0x4FU
#define JLABS_SYX_ID3				0x00U
#define JLABS_SYX_ID4				0x01U

#define MIDI_PROGRAM_CHANGE			0xC0U
#define MIDI_CONTROL_CHANGE			0xB0U
#define MIDI_PITCH_WHEEL			0xE0U
#define MIDI_CHANN_PRESSURE			0xD0U
#define MIDI_NOTE_ON				0x90U
#define MIDI_NOTE_OFF				0x80U
#define MIDI_MMC_PAUSE				0x09U
#define MIDI_MMC_BACKWARD			0x05U
#define MIDI_MMC_FORWARD			0x04U
#define MIDI_MMC_PLAY				0x02U
#define MIDI_MMC_STOP				0x01U
#define MIDI_MMC_REC				0x06U
#define MIDI_CLOCK					0xF8U
#define MIDI_TICK					0xF9U
#define MIDI_START					0xFAU
#define MIDI_CONTINUE				0xFBU
#define MIDI_STOP					0xFCU
#define MIDI_SOX					0xF0U
#define MIDI_EOX					0xF7U
#define MIDI_ACTIVE_SENSE			0xFEU	// never received, filtered by EZ board
#define MIDI_TIME_CODE				0xF1U
#define MIDI_SONG_POSITION_POINTER	0xF2U
#define MIDI_SONG_SELECTED			0xF3U
#define MIDI_STATUS_MASK			0x80U

#define JLABS_SYX_HEADER_SIZE			6U								// f0 00 20 4f 00 01 
#define JLABS_SYX_UPLOAD_HEADER_SIZE	(JLABS_SYX_HEADER_SIZE + 4U) 	// f0 00 20 4f 00 01 UL LL GG NN 

#define DATA_LENGHT_START_BOOTLOADER	0x7F
#define Z8_ERASING_TIME					4000
#define EE_24C64_PAGE_LENGHT			32
#define EE24C64_SIZE					16384

#define MAX_LCD_TEXT_LENGHT			(16U + 1U)		// 16 chars + eoc


#define DEFAULT_INTERNAL_TEMPO		100 // 30 will be added : 130
#define MAX_REAL_TEMPO				255
#define TEMPO_ADD_OFFSET			30

#endif
