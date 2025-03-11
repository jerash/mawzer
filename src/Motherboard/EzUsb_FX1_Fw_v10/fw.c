//-----------------------------------------------------------------------------
//   File:      fw.c
//   Contents:  Firmware frameworks task dispatcher and device request parser
//
// $Archive: /USB/Examples/FX2LP/bulkext/fw.c $
// $Date: 3/23/05 2:53p $
// $Revision: 8 $
//
//
// ==========================================================================
// Copyright (C) MAWZER - Jerash Labs 2006
// ==========================================================================

#include "fx2.h"
#include "fx2regs.h"
#include "syncdly.h"            // SYNCDELAY macro
#include "pins_def.h"
#include "common_def.h"
#include <common_z8_ez.h>
#include "main.h"
#include "usb.h"
#include "bootloader.h"
#include "serial.h"
#include "hardware.h"
#include "fw.h"

//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------
#define DELAY_COUNT   0x9248*8L  // Delay for 8 sec at 24Mhz, 4 sec at 48
#define _IFREQ  48000            // IFCLK constant for Synchronization Delay
#define _CFREQ  48000            // CLKOUT constant for Synchronization Delay

//#define NO_RENUM 

//-----------------------------------------------------------------------------
// Random Macros
//-----------------------------------------------------------------------------
#define   min(a,b) (((a)<(b))?(a):(b))
#define   max(a,b) (((a)>(b))?(a):(b))

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------
volatile BOOL   GotSUD;
BOOL      Rwuen;
BOOL      Selfpwr;
volatile BOOL   Sleep;                  // Sleep mode enable flag

WORD   pDeviceDscr;   // Pointer to Device Descriptor; Descriptors may be moved
WORD   pDeviceQualDscr;
WORD   pHighSpeedConfigDscr;
WORD   pFullSpeedConfigDscr;   
WORD   pConfigDscr;
WORD   pOtherConfigDscr;   
WORD   pStringDscr;   

//-----------------------------------------------------------------------------
// Prototypes
//-----------------------------------------------------------------------------
void SetupCommand(void);
void TD_Init(void);
void TD_Poll(void);
BOOL TD_Suspend(void);
BOOL TD_Resume(void);

BOOL DR_GetDescriptor(void);
BOOL DR_SetConfiguration(void);
BOOL DR_GetConfiguration(void);
BOOL DR_SetInterface(void);
BOOL DR_GetInterface(void);
BOOL DR_GetStatus(void);
BOOL DR_ClearFeature(void);
BOOL DR_SetFeature(void);
BOOL DR_VendorCmnd(void);

// this table is used by the epcs macro 
const char code  EPCS_Offset_Lookup_Table[] =
{
   0,    // EP1OUT
   1,    // EP1IN
   2,    // EP2OUT
   2,    // EP2IN
   3,    // EP4OUT
   3,    // EP4IN
   4,    // EP6OUT
   4,    // EP6IN
   5,    // EP8OUT
   5,    // EP8IN
};


// From MAIN.C
extern unsigned char bdata CTRL_REGS_7BITS_TO_Z8[CTRL_REG_MAX_FROM_EZ_TO_Z8];
extern unsigned char bdata CTRL_REGS_7BITS_FROM_Z8[CTRL_REG_NB_FROM_Z8_TO_EZ];
extern bit usb_state_from_ez0_bit;
extern bit usb_enumrated_from_ez0_bit;
extern xdata Midi_In_Struct MIDI_In_From_Con, MIDI_In_From_Usb;
//extern bit merger_src_midi_in_state_z_ez0_bit;
//extern bit merger_src_usb_in_state_z_ez0_bit;
extern bit midi_thru_ez0_bit;
extern bit must_start_bootloader_from_ez0_bit;

// macro for generating the address of an endpoint's control and status register (EPnCS)
#define epcs(EP) (EPCS_Offset_Lookup_Table[(EP & 0x7E) | (EP > 128)] + 0xE6A1)

//-----------------------------------------------------------------------------
// Code
//-----------------------------------------------------------------------------

// Task dispatcher
void main(void)
{
   DWORD   i;
   WORD   offset;
   DWORD   DevDescrLen;
   DWORD   j=0;
   WORD   IntDescrAddr;
   WORD   ExtDescrAddr;

   // Initialize Global States
   Sleep = FALSE;               // Disable sleep mode
   Rwuen = FALSE;               // Disable remote wakeup
   Selfpwr = FALSE;            // Disable self powered
   GotSUD = FALSE;               // Clear "Got setup data" flag

   // turn off Clock 24 out pin
   CPUCS &= ~bmCLKOE;							 
  
    // Initialize user device
   TD_Init();

   // DGB
   OED = 3;
 
   // clear usb flags at starttup
   // this reg must be cleared before the usb is active since it contains all usb flags
   CTRL_REGS_7BITS_TO_Z8[0]	= 0;

   // The following section of code is used to relocate the descriptor table. 
   // The frameworks uses SUDPTRH and SUDPTRL to automate the SETUP requests
   // for descriptors.  These registers only work with memory locations
   // in the EZ-USB internal RAM.  Therefore, if the descriptors are located
   // in external RAM, they must be copied to in internal RAM.  
   // The descriptor table is relocated by the frameworks ONLY if it is found 
   // to be located in external memory.
   pDeviceDscr = (WORD)&DeviceDscr;
   pDeviceQualDscr = (WORD)&DeviceQualDscr;
   pHighSpeedConfigDscr = (WORD)&HighSpeedConfigDscr;
   pFullSpeedConfigDscr = (WORD)&FullSpeedConfigDscr;
   pStringDscr = (WORD)&StringDscr;

   // Is the descriptor table in external RAM (> 16Kbytes)?  If yes,
   // then relocate.
   // Note that this code only checks if the descriptors START in 
   // external RAM.  It will not work if the descriptor table spans
   // internal and external RAM.
   if ((WORD)&DeviceDscr & 0xC000)
   {
      // first, relocate the descriptors
      IntDescrAddr = INTERNAL_DSCR_ADDR;
      ExtDescrAddr = (WORD)&DeviceDscr;
      DevDescrLen = (WORD)&UserDscr - (WORD)&DeviceDscr + 2;
      for (i = 0; i < DevDescrLen; i++)
         *((BYTE xdata *)IntDescrAddr+i) = *((BYTE xdata *)ExtDescrAddr+i);

      // update all of the descriptor pointers
      pDeviceDscr = IntDescrAddr;
      offset = (WORD)&DeviceDscr - INTERNAL_DSCR_ADDR;
      pDeviceQualDscr -= offset;
      pConfigDscr -= offset;
      pOtherConfigDscr -= offset;
      pHighSpeedConfigDscr -= offset;
      pFullSpeedConfigDscr -= offset;
      pStringDscr -= offset;
   }

   EZUSB_IRQ_ENABLE();            // Enable USB interrupt (INT2)
   EZUSB_ENABLE_RSMIRQ();         // Wake-up interrupt

   //INTSETUP |= (bmAV2EN | bmAV4EN);     // Enable INT 2 & 4 autovectoring
   INTSETUP |= bmAV2EN;

   USBIE |= bmSUDAV | bmSUTOK | bmSUSP | bmURES | bmHSGRANT;   // Enable selected interrupts
   EA = 1;                  // Enable 8051 interrupts

#ifndef NO_RENUM
   // Renumerate if necessary.  Do this by checking the renum bit.  If it
   // is already set, there is no need to renumerate.  The renum bit will
   // already be set if this firmware was loaded from an eeprom.
   if(!(USBCS & bmRENUM))
   {
       EZUSB_Discon(TRUE);   // renumerate
   }
#endif

   // unconditionally re-connect.  If we loaded from eeprom we are
   // disconnected and need to connect.  If we just renumerated this
   // is not necessary but doesn't hurt anything
   USBCS &=~bmDISCON;

   CKCON = (CKCON&(~bmSTRETCH)) | FW_STRETCH_VALUE; // Set stretch

   // clear the Sleep flag.
   //Sleep = FALSE;

   // CHIP IS NOW READY TO OPERATE, CHECK IF BOOTLOADER IS REQUESTED
   // WAIT ALSO FOR THE Z8 INCOMING CONFIGURATION BYTES

   /* Check if USB is attached to host */
   Check_If_Usb_Attached();

   // Change the serial port Baud rate from OCD baud rate to 31250 kb/sec
   // TIMER 1 : BAUD RATE GENERATOR FOR SERIAL PORTS 1
   TH1  = 256-SERIAL_TO_Z8_BAUD_RELOAD;  	// Setup Baudrate reload value
   TL1  = 256-SERIAL_TO_Z8_BAUD_RELOAD;  	// Setup Baudrate overflow value

Check_Usb();

   // synchronize the EZ to Z8 board
   SyncToZ8Board();

   // Task Dispatcher
   while(TRUE)               // Main Loop
   {
		Check_Usb();

		IOD ^=1;

		// Call to main user loop there
		// TD_Poll();
		// Check if USB is attached to host 
		Check_If_Usb_Attached();
	
		// Check if MIDI In buffer contains complete event 
		Check_Midi_In_Buffer(&MIDI_In_From_Con, /*merger_src_midi_in_state_z_ez0_bit*/midi_thru_ez0_bit);
	
		// Check Overflow flags
		Check_Overflow_Flags();
			
		// Check Serial buffer from Z8 board 
		Check_Serial_In_From_Z8_Buffer();

		// Check if bootloader must be started
		if(must_start_bootloader_from_ez0_bit)
			Check_Bootloader();		

		// Check both MIDI in and USB in realtime buffers 
		Check_Realtime();

		// Check if USB In buffer contains complete event 
		if(usb_state_from_ez0_bit)
		{
	        // Check if USB MIDI In buffer contains complete event 
			Ep2out_Function();
	
			Check_Midi_In_Buffer(&MIDI_In_From_Usb, /*merger_src_usb_in_state_z_ez0_bit*/midi_thru_ez0_bit);
		}
   }
}

BOOL HighSpeedCapable()
{
   // this function determines if the chip is high-speed capable.
   // FX2 and FX2LP are high-speed capable. FX1 is not - it does
   // not have a high-speed transceiver.

   if (GPCR2 & bmFULLSPEEDONLY)
      return FALSE;
   else
      return TRUE;
}   

// Device request parser
void SetupCommand(void)
{
   void   *dscr_ptr;

   switch(SETUPDAT[1])
   {
      case SC_GET_DESCRIPTOR:                  // *** Get Descriptor
         if(DR_GetDescriptor())
            switch(SETUPDAT[3])         
            {
               case GD_DEVICE:            // Device
                  SUDPTRH = MSB(pDeviceDscr);
                  SUDPTRL = LSB(pDeviceDscr);
                  break;
               case GD_DEVICE_QUALIFIER:            // Device Qualifier
			   	  // only retuen a device qualifier if this is a high speed
				  // capable chip.
			   	  if (HighSpeedCapable())
				  {
	                  SUDPTRH = MSB(pDeviceQualDscr);
	                  SUDPTRL = LSB(pDeviceQualDscr);
				  }
				  else
				  {
					  EZUSB_STALL_EP0();
				  }
				  break;
               case GD_CONFIGURATION:         // Configuration
                  SUDPTRH = MSB(pConfigDscr);
                  SUDPTRL = LSB(pConfigDscr);
                  break;
               case GD_OTHER_SPEED_CONFIGURATION:  // Other Speed Configuration
                  SUDPTRH = MSB(pOtherConfigDscr);
                  SUDPTRL = LSB(pOtherConfigDscr);
                  break;
               case GD_STRING:            // String
                  if(dscr_ptr = (void *)EZUSB_GetStringDscr(SETUPDAT[2]))
                  {
                     SUDPTRH = MSB(dscr_ptr);
                     SUDPTRL = LSB(dscr_ptr);
                  }
                  else 
                     EZUSB_STALL_EP0();   // Stall End Point 0
                  break;
               default:            // Invalid request
                  EZUSB_STALL_EP0();      // Stall End Point 0
            }
         break;
      case SC_GET_INTERFACE:                  // *** Get Interface
         DR_GetInterface();
         break;
      case SC_SET_INTERFACE:                  // *** Set Interface
         DR_SetInterface();
         break;
      case SC_SET_CONFIGURATION:               // *** Set Configuration
         DR_SetConfiguration();
         break;
      case SC_GET_CONFIGURATION:               // *** Get Configuration
         DR_GetConfiguration();
         break;
      case SC_GET_STATUS:                  // *** Get Status
         if(DR_GetStatus())
            switch(SETUPDAT[0])
            {
               case GS_DEVICE:            // Device
                  EP0BUF[0] = ((BYTE)Rwuen << 1) | (BYTE)Selfpwr;
                  EP0BUF[1] = 0;
                  EP0BCH = 0;
                  EP0BCL = 2;
                  break;
               case GS_INTERFACE:         // Interface
                  EP0BUF[0] = 0;
                  EP0BUF[1] = 0;
                  EP0BCH = 0;
                  EP0BCL = 2;
                  break;
               case GS_ENDPOINT:         // End Point
                  EP0BUF[0] = *(BYTE xdata *) epcs(SETUPDAT[4]) & bmEPSTALL;
                  EP0BUF[1] = 0;
                  EP0BCH = 0;
                  EP0BCL = 2;
                  break;
               default:            // Invalid Command
                  EZUSB_STALL_EP0();      // Stall End Point 0
            }
         break;
      case SC_CLEAR_FEATURE:                  // *** Clear Feature
         if(DR_ClearFeature())
            switch(SETUPDAT[0])
            {
               case FT_DEVICE:            // Device
                  if(SETUPDAT[2] == 1)
                     Rwuen = FALSE;       // Disable Remote Wakeup
                  else
                     EZUSB_STALL_EP0();   // Stall End Point 0
                  break;
               case FT_ENDPOINT:         // End Point
                  if(SETUPDAT[2] == 0)
                  {
                     *(BYTE xdata *) epcs(SETUPDAT[4]) &= ~bmEPSTALL;
                     EZUSB_RESET_DATA_TOGGLE( SETUPDAT[4] );
                  }
                  else
                     EZUSB_STALL_EP0();   // Stall End Point 0
                  break;
            }
         break;
      case SC_SET_FEATURE:                  // *** Set Feature
         if(DR_SetFeature())
            switch(SETUPDAT[0])
            {
               case FT_DEVICE:            // Device
                  if(SETUPDAT[2] == 1)
                     Rwuen = TRUE;      // Enable Remote Wakeup
                  else if(SETUPDAT[2] == 2)
                     // Set Feature Test Mode.  The core handles this request.  However, it is
                     // necessary for the firmware to complete the handshake phase of the
                     // control transfer before the chip will enter test mode.  It is also
                     // necessary for FX2 to be physically disconnected (D+ and D-)
                     // from the host before it will enter test mode.
                     break;
                  else
                     EZUSB_STALL_EP0();   // Stall End Point 0
                  break;
               case FT_ENDPOINT:         // End Point
                  *(BYTE xdata *) epcs(SETUPDAT[4]) |= bmEPSTALL;
                  break;
               default:
                  EZUSB_STALL_EP0();      // Stall End Point 0
            }
         break;
      default:                     // *** Invalid Command
         if(DR_VendorCmnd())
            EZUSB_STALL_EP0();            // Stall End Point 0
   }

   // Acknowledge handshake phase of device request
   EP0CS |= bmHSNAK;
}

// Wake-up interrupt handler
void resume_isr(void) interrupt WKUP_VECT
{
   EZUSB_CLEAR_RSMIRQ();
}


