#ifndef SERIAL
#define SERIAL

///////////////////////////////////////////////////////////
// Local defines
///////////////////////////////////////////////////////////
#define BAUD1 	 	125000 // 78125 // 62500      				// 62,5K baud for ONEWIRE

#define BRG1 	Z8_FREQ/((unsigned long)BAUD_Z8_EZ*16)
#define BRG0 	Z8_FREQ/((unsigned long)BAUD1*16)

#define TDRE	4U

#define TX_BUFF_LENGTH			128U // V1.5 128U
#define SERIAL_IN_BUFF_LENGHT	256  // V1.5 128U

#define UPLOAD_Z8_FRAME_LENGHT		0x06U 	// FF SS LL AA/NN 00/GG FE
#define LCD_MESS_Z8_FRAME_LENGHT	0x05U 	// FF SS LL AA FE
#define FRAME_HEADER_LENGHT			0x07U	// FF SS LL AA/NN 00/GG ... CS FE

#define INVALID_REQUEST				0xFFU
#define DEFAULT_INTERNAL_TEMPO		100 // 30 will be added : 130 

// TX_STATUS bits definitions
#define tx0_busy_txstat			(1<<6U)
#define tx0_full_txstat			(1<<5U)
#define tx1_busy_txstat			(1<<4U)
#define tx1_full_txstat			(1<<3U) 

// Programmation Frame mapping (valable only for FRAME_TYPE different than pure midi)
enum { FRAME_SOF_POS, FRAME_TYPE_POS, FRAME_DATA_LENGHT_POS, FRAME_ADDR_INDEX_POS, FRAME_GROUP_POS, FRAME_DATA_START };

enum {MIDI_STRUCT_VALID, MIDI_STRUCT_NOT_VALID};

//////////////////////////////////////////////////////////
// Function Prototypes
///////////////////////////////////////////////////////////
void InitSerial(void);
void SendToTx0(uchar value);
void SendToTx1(uchar value);
void CheckSerialInBuffer(void);
void SendCtrlRegToEzBoard(uchar ctrl_reg_to_ez);
void ParsePureMidiInBuffer(uchar end_index_local, uchar SYSEX_ID0_byte); // V1.5
uchar Prog_Upload_Common_Param(uchar SYSEX_ID0_byte, uchar serial_in_start_index);
uchar Check_Sysex_Validity(uchar SYSEX_ID0, uchar SYSEX_ID2, uchar SYSEX_ID3);
void SendDispatchToMidi(far uchar *buff_ptr, uchar midi_str_size, uchar destination);
uchar Serial_Nibb_To_Generic_Bytes(uchar start_index, uchar lenght);
void Byte_To_Generic_Nibbles(uchar start_index, uchar lenght, far uchar *buff_src);
void SendUploadGenericBufferToMidi(uchar SYSEX_ID0_byte, uchar SYSEX_ID1_byte, uchar SYSEX_ID2_byte, uchar SYSEX_ID3_byte);
void SendControlToMidi(void);
void FillParamCurrentStruct(void);
uchar CheckMidiStruct(void);
void Truncate_Current_Value(void);

#endif