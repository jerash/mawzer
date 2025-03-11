#ifndef CONTROLDEF
#define CONTROLDEF

///////////////////////////////////////////////////////////
// Control structure definition
///////////////////////////////////////////////////////////

#define GROUP_NB					4U
#define CHANNEL_NB					16U
//////////////////////////////////////////////////////////////////////////////
#define MIDI_STR_MAX_LENGHT			15U
#define LCD_STR_MAX_LENGHT			8U

////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// CONTROL STRUCTURE DEFINITION /////////////////////////////////
// This is the structure of control definition mapped in EEProm as well
typedef struct {
uchar message_type;						// CC, NOTE, NRPN, RPN, PITCHBEND, SYSEX, ...
uchar message_nb;						// note or cc number
uchar ctrl_status_1;					// 1 byte, control status 
uchar midi_str_lenght;					// 1 byte, midi string lenght
uchar midi_channel_position;			// 1 byte, midi channel position
uchar midi_channel;						// 1 byte, the midi channel
uchar ctrl_value_position_0;			// 1 byte, value position 0
uchar ctrl_value_min;					// 1 byte, control min value
uchar ctrl_value_max;					// 1 byte, control max value
uchar midi_string[MIDI_STR_MAX_LENGHT];	// the midi string
uchar lcd_string[LCD_STR_MAX_LENGHT];	// the lcd string
} All_Params ;

enum {CC, NOTE, NRPN, RPN, PITCHBEND, SYSEX, PROG_CHANGE};

#define CTRL_STRUCT_HEADER_SIZE 9U
#define CTRL_STRUCT_TOTAL_SIZE (CTRL_STRUCT_HEADER_SIZE + MIDI_STR_MAX_LENGHT + LCD_STR_MAX_LENGHT)

enum {message_type_index, message_nb_index, ctrl_status_1_index, midi_str_lenght_index, 
midi_channel_position_index, midi_channel_index, ctrl_value_position_0_index, 
ctrl_value_min_index, ctrl_value_max_index, midi_string_index/*, lcd_string_index*/};
#define lcd_string_index (midi_string_index + MIDI_STR_MAX_LENGHT)

// ctrl_status_1 bits definition
// Bit 7 must always be zero
#define switch_ctrl_mode		(1<<5)		// bit 7 = switch mode (0=push/1=toggle), not used for pots
#define usb_out_en				(1<<4)		// bit 4 = send over USB   		DO NOT CHANGE ORDER
#define midi_out_1_en			(1<<3)		// bit 3 = midi out 1 enable	DO NOT CHANGE ORDER

#define MIDI_OUT_SEL_MSK		 0x18U		// bits 3, 4

// ctrl_status_2 bits definition
// reserved for future use

enum {	TRANSPORT_REC, TRANSPORT_PLAY, TRANSPORT_BW, TRANSPORT_STOP, TRANSPORT_FW, TRANSPORT_PAUSE };

#endif
