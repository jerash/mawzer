#ifndef PINS_DEFS_ENC
#define PINS_DEFS_ENC

// PINS DEFINITIONS, PORTS MAPPING
#include "main.h"

// GENERAL PURPOSE OUTPUTS PINS
#define MUX_PORT_OUT			PCOUT	// A0,A1,A2 							GP OUT
#define A0_PIN					(1<<3U)	// A0									GP OUT
#define A1_PIN					(1<<4U)	// A1									GP OUT
#define A2_PIN					(1<<5U)	// A2									GP OUT

#define MUX_PORT_MSK			0x38 // Bits 3,4,5)

// DIGITAL INPUTS PINS
#define SUB_ID_PORT_IN			PBIN	// A0,A1,A2 							GP OUT
#define SUB_ID_PIN				(1<<4U)	// sub id switch, PB4IN					GP IN

//ENCODER PINS
#define ENC0_INPUT_A_PORT_IN		PAIN	//  PA1IN							GP IN
#define ENC0_INPUT_A_PIN			(1<<1U)	//  PA1IN							GP IN
#define ENC0_INPUT_B_PORT_IN		PAIN	//  PA0IN							GP IN
#define ENC0_INPUT_B_PIN			(1<<0U)	//  PA0IN							GP IN

#define ENC1_INPUT_A_PORT_IN		PCIN	//  PC2IN							GP IN
#define ENC1_INPUT_A_PIN			(1<<2U)	//  PC2IN							GP IN
#define ENC1_INPUT_B_PORT_IN		PCIN	//  PC0IN							GP IN
#define ENC1_INPUT_B_PIN			(1<<0U)	//  PC0IN							GP IN

#define ENC2_INPUT_A_PORT_IN		PAIN	//  PA3IN							GP IN
#define ENC2_INPUT_A_PIN			(1<<3U)	//  PA3IN							GP IN
#define ENC2_INPUT_B_PORT_IN		PCIN	//  PC1IN							GP IN
#define ENC2_INPUT_B_PIN			(1<<1U)	//  PC1IN							GP IN

#define ENC3_INPUT_A_PORT_IN		PBIN	// sw PB0IN							GP IN
#define ENC3_INPUT_A_PIN			(1<<0U)	// sw PB0IN							GP IN
#define ENC3_INPUT_B_PORT_IN		PBIN	// sw PB1IN							GP IN
#define ENC3_INPUT_B_PIN			(1<<1U)	// sw PB1IN							GP IN

#endif



