#ifndef CONTROLSBUT
#define CONTROLSBUT

///////////////////////////////////////////////////////////
// Local definitions
///////////////////////////////////////////////////////////
#define MODULE_ID			BUT_MOD // see onewire.h for module id enum definition

#define CONTROL_NB			4U

enum{BUT0, BUT1, BUT2, BUT3};

#define MASK_7BITS		0x7F

#define BUTTON_ON		0x7F
#define BUTTON_OFF		0x00

// Z8_BUT_STATUS bits definition
#define but3_pressed		(1<<7)	// bit 7 : 1 = but pressed
#define but2_pressed		(1<<6)	// bit 6 : 1 = but pressed
#define but1_pressed		(1<<5)	// bit 5 : 1 = but pressed
#define but0_pressed		(1<<4)	// bit 4 : 1 = but pressed

// Ctrl[] status bit definition
//// WARNING ::: DON'T CHANGE but_state_on bit ORDER ... see in GetControlValue()
#define but3_toggle			(1<<7)	// bit 7 : 1 = toggle 0 = push
#define but2_toggle			(1<<6)	// bit 6 : 1 = toggle 0 = push
#define but1_toggle			(1<<5)	// bit 5 : 1 = toggle 0 = push
#define but0_toggle			(1<<4)	// bit 4 : 1 = toggle 0 = push
#define but3_state_on		(1<<3)	// bit 3 : 1 = but on
#define but2_state_on		(1<<2)	// bit 2 : 1 = but on
#define but1_state_on		(1<<1)	// bit 2 : 1 = but on
#define but0_state_on		(1<<0)	// bit 1 : 1 = but on

#define CTRL_STATUS_TOGGLE_IDX 0x04   // index of first toggle info in Ctrl[] data

#define ASSIGN_STRUCT_CONTROL_STATUS1_ADR	0x02   // address of control_status_1 byte in assignment struct
#define SWITCH_CONTROL_MODE_IDX				(1<<5) // index of toggle info in control_status_1 byte

// Hook state bytes
#define NB_HOOK_STATE_BYTES		0U		// This module does not send any hook state byte to MB 

///////////////////////////////////////////////////////////
// Function Prototypes
///////////////////////////////////////////////////////////
void InitModule(void);
void CheckControls(void);
void UpdateButModeStatus(void);
void UpdateLEDs(void);
void ModuleChecksWrittenRegisters(uchar , uint );
uchar SetHookStateByte(uchar byte_nb);
uchar GetControlValue(uchar ctrl_nb);


#endif