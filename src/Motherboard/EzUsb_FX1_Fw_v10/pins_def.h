#ifndef PINS_DEF_H		//Header sentry
#define PINS_DEF_H

// PINS FUNCTIONS DEFINITIONS

// PORT BIT DEFINITIONS : OUTPUTS
//#define USB_LED_PORT_OUT		OUTB	// USB Led output pin
//#define USB_LED_PIN			bmBIT6	// USB Led
#define Z8_RESET_PORT_OUT		IOE		// Reset control of Z8 board
#define Z8_RESET_PIN			bmBIT1	// Reset control of Z8 board
#define OCD_EN_PORT_OUT			IOE		// OCD Enable output port
#define OCD_EN_PIN				bmBIT0	// OCD Enable output pin
#define FORCE_OCD_Z8_PORT_OUT	IOE		// Reset control of Z8 board
#define FORCE_OCD_Z8_PIN		bmBIT3	// Reset control of Z8 board

// PORT BIT DEFINITIONS : INPUTS
#define VUSB_PORT_IN			IOE		// usb power supply monitor input port
#define VUSB_PIN				bmBIT2	// usb power supply monitor input pin


// OPTIONAL PINS ACTUALLY NOT USED


#endif





