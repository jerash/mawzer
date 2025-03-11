#ifndef PINS_DEFS_POT
#define PINS_DEFS_POT

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


#endif



