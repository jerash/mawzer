#ifndef CONTROLSENC
#define CONTROLSENC

///////////////////////////////////////////////////////////
// Local definitions
///////////////////////////////////////////////////////////
#define MODULE_ID			ENC_MOD // see onewire.h for module id enum definition

#define CONTROL_NB			4U

enum{ENC0, ENC1, ENC2, ENC3};

#define MASK_7BITS		0x7F

#define ENC_DEFAULT_VALUE	0x3F  // default value for all controls


// Z8_ENC_STATUS bits definition
#define enc0_input_A		(1<<7)	// bit 7 : 
#define enc1_input_A		(1<<6)	// bit 6 : 
#define enc2_input_A		(1<<5)	// bit 5 : 
#define enc3_input_A		(1<<4)	// bit 4 : 
#define enc0_input_B		(1<<3)	// bit 3 : 
#define enc1_input_B		(1<<2)	// bit 2 : 
#define enc2_input_B		(1<<1)	// bit 2 : 
#define enc3_input_B		(1<<0)	// bit 1 : 

// Hook state bytes
#define NB_HOOK_STATE_BYTES		0U		// This module does not send any hook state byte to MB 

///////////////////////////////////////////////////////////
// Function Prototypes
///////////////////////////////////////////////////////////
void InitModule(void);
void CheckControls(void);
void ModuleChecksWrittenRegisters(uchar mem_type, uint address);
uchar SetHookStateByte(uchar byte_nb);
uchar GetControlValue(uchar ctrl_nb);


#endif