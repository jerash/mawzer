#ifndef PINS_DEFS_BUT
#define PINS_DEFS_BUT

// PINS DEFINITIONS, PORTS MAPPING
#include "main.h"

// #define SIMUL						1

// GENERAL PURPOSE OUTPUTS PINS
#define MUX_PORT_OUT			PCOUT	// A0,A1,A2 							GP OUT
#define A0_PIN					(1<<3U)	// A0									GP OUT
#define A1_PIN					(1<<4U)	// A1									GP OUT
#define A2_PIN					(1<<5U)	// A2									GP OUT

#define MUX_PORT_MSK			0x38 // Bits 3,4,5)

// DIGITAL INPUTS PINS
#define SUB_ID_PORT_IN			PBIN	// A0,A1,A2 							GP OUT
#define SUB_ID_PIN				(1<<4U)	// sub id switch, PB4IN					GP IN

//BUTTON PINS
#define BUT0_SW_PORT_IN			PAIN	// sw PA1IN								GP IN
#define BUT0_SW_PIN				(1<<1U)	// sw PA1IN								GP IN
#define BUT1_SW_PORT_IN			PCIN	// sw PC2IN								GP IN
#define BUT1_SW_PIN				(1<<2U)	// sw PC2IN								GP IN
#define BUT2_SW_PORT_IN			PCIN	// sw PC1IN								GP IN
#define BUT2_SW_PIN				(1<<1U)	// sw PC1IN								GP IN
#define BUT3_SW_PORT_IN			PBIN	// sw PB1IN								GP IN
#define BUT3_SW_PIN				(1<<1U)	// sw PB1IN								GP IN

// LEDS OUTPUTS PINS
#define LED_BUT0_PORT_OUT		PAOUT	// led sw 0 pin, PAOUT0					GP OUT
#define LED_BUT0_PIN			(1<<0U)	// led sw 0 pin, PAOUT0					GP OUT
#define LED_BUT1_PORT_OUT		PCOUT	// led sw 1 pin, PCOUT0					GP OUT
#define LED_BUT1_PIN			(1<<0U)	// led sw 1 pin, PCOUT0					GP OUT
#define LED_BUT2_PORT_OUT		PAOUT	// led sw 2 pin, PAOUT3					GP OUT
#define LED_BUT2_PIN			(1<<3U)	// led sw 2 pin, PAOUT3					GP OUT
#define LED_BUT3_PORT_OUT		PBOUT	// led sw 3 pin, PBOUT0					GP OUT
#define LED_BUT3_PIN			(1<<0U)	// led sw 3 pin, PBOUT0					GP OUT

#endif



