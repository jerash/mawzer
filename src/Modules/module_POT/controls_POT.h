#ifndef CONTROLSPOT
#define CONTROLSPOT

///////////////////////////////////////////////////////////
// Local definitions
///////////////////////////////////////////////////////////
#define MODULE_ID			POT_MOD // see onewire.h for module id enum definition

#define ANALOG_SOURCES_NB	4U
#define CONTROL_NB			ANALOG_SOURCES_NB

enum{POT0, POT1, POT2, POT3};

#define HOOK_BIT  		0x80	// 1 = hooked / 0 = not hooked
#define POSITIVE_BIT	0x80	// 1 = physical value > cc value / 0 = physical value < cc value
#define MASK_7BITS		0x7F

// We store all controlers informations in the following structure :
typedef struct {
	uchar Physical_value;		// 7 bit physical control value + hook bit
	uchar CC_value[GROUP_NB];	// for all group, 7 bit cc control value + positive bit
} CONTROL_STRUCT;

// Hook state bytes
#define NB_HOOK_STATE_BYTES		2U		// This module must send 2 hook state bytes to MB 
#define HOOK_STATE_BYTE1		0U		// index of first hook state byte
#define HOOK_STATE_BYTE2		1U		// index of seconde hook state byte
#define low_hook_direction_bit	(1<<1)	// 0 : valeur physique < valeur du groupe
#define low_control_hooked_bit	(1<<0)	// 0 : control hooked / 1 : control not hooked
#define high_hook_direction_bit	(1<<3)	// 0 : valeur physique < valeur du groupe
#define high_control_hooked_bit	(1<<2)	// 0 : control hooked / 1 : control not hooked

///////////////////////////////////////////////////////////
// Function Prototypes
///////////////////////////////////////////////////////////
void InitAnalog(void);
void InitModule(void);
void CheckControls(void);
void ModuleChecksWrittenRegisters(uchar , uint );
uchar SetHookStateByte(uchar byte_nb);
uchar GetControlValue(uchar ctrl_nb);


#endif