#ifndef PINS_DEFS
#define PINS_DEFS

// PINS DEFINITIONS, PORTS MAPPING
#include "main.h"

// #define PD6_CONTROL_BL_EN			1
#define PD6_CONTROL_LCD_RESET 			1
// #define SIMUL						1
#define MICROTIPS_MTGS12864KFGHSGW01 	1
//#define SUNLIKE_SG12864HBWBGBK		1


// LCD RELATED PINS
#define LCD_DATA_PORT_OUT		PCOUT	// 8 bits data port io to/from LCD		GP OUT
#define LCD_DATA_PORT_IN		PCIN	// 8 bits data port io to/from LCD		GP IN
#define LCD_E_PORT_OUT			PBOUT	// lcd_e pin, PBOUT3					GP OUT
#define LCD_E_PIN				(1<<3U)	// lcd_e pin, PBOUT3					GP OUT
#define LCD_RS_PORT_OUT			PBOUT	// lcd_rs pin, PBOUT4					GP OUT
#define LCD_RS_PIN				(1<<4U)	// lcd_rs pin, PBOUT4					GP OUT
#define LCD_RW_PORT_OUT			PBOUT	// lcd_rw pin, PBOUT5					GP OUT
#define LCD_RW_PIN				(1<<5U)	// lcd_rw pin, PBOUT5					GP OUT
#ifdef SUNLIKE_SG12864HBWBGBK
#define LCD_CS1_PORT_OUT		PBOUT	// lcd_cs1 pin, PBOUT6					GP OUT
#define LCD_CS1_PIN				(1<<6U)	// lcd_cs1 pin, PBOUT6					GP OUT
#define LCD_CS2_PORT_OUT		PBOUT	// lcd_cs2 pin, PBOUT7					GP OUT
#define LCD_CS2_PIN				(1<<7U)	// lcd_cs2 pin, PBOUT7					GP OUT
#endif
#ifdef MICROTIPS_MTGS12864KFGHSGW01
#define LCD_CS2_PORT_OUT		PBOUT	// lcd_rs pin, PBOUT6					GP OUT
#define LCD_CS2_PIN				(1<<6U)	// lcd_rs pin, PBOUT6					GP OUT
#define LCD_CS1_PORT_OUT		PBOUT	// lcd_rs pin, PBOUT7					GP OUT
#define LCD_CS1_PIN				(1<<7U)	// lcd_rs pin, PBOUT7					GP OUT
#endif
#ifdef PD6_CONTROL_BL_EN
#define LCD_BL_EN_PORT_OUT		PDOUT	// backlight en pin, PDOUT6				GP OUT
#define LCD_BL_EN_PIN			(1<<6U)	// backlight en pin, PDOUT6				GP OUT
#endif
#ifdef PD6_CONTROL_LCD_RESET
#define LCD_RESET_PORT_OUT		PDOUT	// reset lcd pin, PDOUT6				GP OUT
#define LCD_RESET_PIN			(1<<6U)	// reset lcd pin, PDOUT6				GP OUT
#endif

// ONE WIRE BUS SELECTION OUTPUTS PINS
#define ONE_WIRE_MUX_PORT_OUT	PHOUT	// A0,A1,A2 							GP OUT
#define ONE_WIRE_MUX_A0_PIN		(1<<0U)	// A0									GP OUT
#define ONE_WIRE_MUX_A1_PIN		(1<<1U)	// A1									GP OUT
#define ONE_WIRE_MUX_A2_PIN		(1<<2U)	// A2									GP OUT

// TRANSPORT BUTTONS FROM USER INTERFACE
#define PLAY_PORT_IN			PHIN	// play, PH3IN							GP IN 
#define PLAY_PIN				(1<<3U)	// play, PH3IN							GP IN
#define REC_PORT_IN				PEIN	// record, PH3IN						GP IN 
#define REC_PIN					(1<<5U)	// record, PH3IN						GP IN
#define STOP_PORT_IN			PAIN	// stop, PA1IN							GP IN 
#define STOP_PIN				(1<<1U)	// stop, PA1IN							GP IN
#define FW_PORT_IN				PAIN	// forward, PA0IN						GP IN 
#define FW_PIN					(1<<0U)	// forward, PA0IN						GP IN
#define BW_PORT_IN				PEIN	// backward, PE4IN						GP IN 
#define BW_PIN					(1<<4U)	// backward, PE4IN						GP IN

// BUTTONS FROM USER INTERFACE
#define OK_PORT_IN				PBIN	// cancel, PB0IN						GP IN 
#define OK_PIN					(1<<2U)	// cancel, PB0IN						GP IN
#define CANCEL_PORT_IN			PBIN	// ok, PB2IN							GP IN
#define CANCEL_PIN				(1<<0U)	// ok, PB2IN							GP IN
#define UP_SW_PORT_IN			PGIN	// shift, PG3IN							GP IN 
#define UP_SW_PIN				(1<<3U)	// shift, PG3IN							GP IN 
#define SHIFT_PORT_IN			PFIN	// down sw, PF7IN						GP IN
#define SHIFT_PIN				(1<<7U)	// down sw, PF7IN						GP IN
#define DOWN_SW_PORT_IN			PAIN	// up sw, PA3IN							GP IN
#define DOWN_SW_PIN				(1<<3U)	// up sw, PA3IN							GP IN

// ENCODER 1 + ENCODER 1 SWITCH
#define ENCODER1_SW_PORT_IN		PBIN	// encoder1 sw, PB1IN					GP IN
#define ENCODER1_SW_PIN			(1<<1U)	// encoder1 sw, PB1IN					GP IN
#define ENCODER1_A_PORT_IN		PDIN	// encoder1 A, PD2IN					GP IN WITH ITR
#define ENCODER1_A_PIN			(1<<0U)	// encoder1 A, PD2IN					GP IN WITH ITR
#define ENCODER1_B_PORT_IN		PDIN	// encoder1 B, PD3IN					GP IN WITH ITR
#define ENCODER1_B_PIN			(1<<1U)	// encoder1 B, PD3IN					GP IN WITH ITR

// ENCODER 2
#define ENCODER2_A_PORT_IN		PDIN	// encoder2 A, PD2IN					GP IN WITH ITR
#define ENCODER2_A_PIN			(1<<2U)	// encoder2 A, PD2IN					GP IN WITH ITR
#define ENCODER2_B_PORT_IN		PDIN	// encoder2 B, PD3IN					GP IN WITH ITR
#define ENCODER2_B_PIN			(1<<3U)	// encoder2 B, PD3IN					GP IN WITH ITR

// LEDS OUTPUTS PINS
#define LED_RECORD_PORT_OUT		PEOUT	// led record, PEOUT7					GP OUT
#define LED_RECORD_PIN			(1<<7U)	// led record, PEOUT7					GP OUT
#define LED_PLAY_PORT_OUT		PEOUT	// led record, PEOUT6					GP OUT
#define LED_PLAY_PIN			(1<<6U)	// led record, PEOUT6					GP OUT

// SERIAL UART AND I2C COMUNICATION
#define TX1_TO_EZ_PORT_OUT			PDOUT	// tx to ez pin, PDOUT5					ALT FCT
#define TX1_TO_EZ_PIN				(1<<5U)	// tx to ez pin, PDOUT5					ALT FCT
#define RX1_FROM_EZ_PORT_IN			PDIN	// rx from ez pin, PDIN4				ALT FCT
#define RX1_FROM_EZ_PIN				(1<<4U)	// rx from ez pin, PDIN4				ALT FCT
#define TX0_TO_ONE_WIRE_PORT_OUT	PAOUT	// tx to one wire, PAOUT5				ALT FCT
#define TX0_TO_ONE_WIRE_PIN			(1<<5U)	// tx to one wire, PAOUT5				ALT FCT
#define RX0_FROM_ONE_WIRE_PORT_IN	PAIN	// rx from one wire, PAIN4				ALT FCT
#define RX0_FROM_ONE_WIRE_PIN		(1<<4U)	// rx from one wire, PAIN4				ALT FCT
#define I2C_SCL_PORT_OUT			PAOUT	// i2c scl pin, PAOUT6					ALT FCT
#define I2C_SCL_PIN					(1<<6U)	// i2c scl pin, PAOUT6					ALT FCT
#define I2C_SDA_PORT_OUT			PAOUT	// i2c sda pin, PAOUT7					ALT FCT
#define I2C_SDA_PORT_IN				PAIN	// i2c sda pin, PAIN7					ALT FCT
#define I2C_SDA_PIN					(1<<7U)	// i2c sda pin, PAOUT7					ALT FCT
#define ONE_WIRE_EN_PORT_OUT		PAOUT	// one wire enable pin, PAOUT2			ALT FCT
#define ONE_WIRE_EN_PIN				(1<<2U)	// one wire enable pin, PAOUT2			ALT FCT

#endif



