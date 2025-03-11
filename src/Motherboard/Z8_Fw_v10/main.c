#include <eZ8.h>
#include "pins_defs.h"
#include "commondef.h"
#include <common_z8_ez.h>
#include <ctrldef.h>
#include "main.h"
#include "hardware.h"
#include "lcd_low_level.h"
#include "i2c_low_level.h"
#include "serial.h"
#include "itrs.h"
#include "lcd.h"
#include "onewire.h"
#include "autotest.h"
#include "digital.h"
#include "realtime.h"
#include "menus.h"
#include "smpte.h"
#include "assign.h"

// set the option bits
FLASH_OPTION1 = 0xFF;
FLASH_OPTION2 = 0xFF;

///////////////////////////////////////////////////////////////////////////////////////
// GLOBAL VARS
///////////////////////////////////////////////////////////////////////////////////////
extern rom uchar bmp_home[];
extern volatile uchar Z8_STATUS_4;
extern volatile uchar Z8_STATUS_5;

//dbg extern uchar Z8_STATUS_6;

///////////////////////////////////////////////////////////////////////////////////////////
// Main program beings here 
///////////////////////////////////////////////////////////////////////////////////////////
void main(void)
{	
START:
	// Disable Interrupts
	DI();		
	// Initialize IOs directions, default values and alternate functions
	InitIos();
	// Initialize LCD display
	InitLcd();
	// Initialize I2C Controller periferal
	InitI2C(); 
	// Initialize Serial periferals I2C and UARTS 0 & 1
	InitSerial(); 
	// Initialize Interrupt controller + vectors
	InitItrs();
	// Initialize default values
	InitInitialValues();
	// Init SMPTE frame
	InitSmpteTime();
	// Init modules
	InitModules();
	// Init all timers
	InitTimers();
	// Enable Interrupts	
	EI();	
	// Sync to ez board
	SyncToEzBoard();
	// Check if factory default, or autotest request
	CheckBootStatus();
	// Display home screen
	DisplayBmp(0, 0, LCD_INVERT, bmp_home);
	// Display firmware version
	DisplayRomVersion(LCD_ZONE_FW_VERSION_X, LCD_ZONE_FW_VERSION_Y);
	// wait 1.5sec on home screen
	delay_msec(1500);
	// Search and display for all connected modules
	SearchModules();
	// wait 1.5sec on modules found
	delay_msec(1500);
	// Get all structures from the connected modules
	GetAllStructFromModules();
  	// Send current group to all modules found
	SendCurrentGroupToAllModules();
	// Clear display
	FillLcdZone(0, 0, LCD_X_PIXEL_MAX, LCD_Y_PAGE_MAX, FILL_BLANK);
	// Init LCD zones
	Z8_STATUS_5 |= must_reinit_all_lcd_z5;

	// Main loop start from there
	while(1) {
				// Check if a param has changed from ITRs
				CheckParamChangedFromItr();

				// Scan current module
				ScanCurrentModule();

				// Check realtime midi events
				CheckRealtimeMidi();

				// Check transport bar
				CheckTransportButtons();

				// Check other UI buttons
				CheckButtons();

				// Check if a control reg for EZ board must be saved and sent
				CheckCtrlRegToSendToEz();

				// Each 5 seconds check ctrl regs to save to eeprom
				CheckCtrlRegToSave5Sec();

				// Check for Serial input buffer
				CheckSerialInBuffer();

				// Check if we are waiting a control to assign
				AssignWaitControl();

				// Check menu switches
				CheckMenuSwitches();
		
				// no more first pass flag
				Z8_STATUS_4 &= ~first_pass_z4;

//DBG				if(Z8_STATUS_6 & shift_state_z6)
//DBG					goto START;

	}

} // END PROGRAM


