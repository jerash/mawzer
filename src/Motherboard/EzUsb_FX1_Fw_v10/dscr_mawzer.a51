;;-----------------------------------------------------------------------------
;;   File:      dscr.a51
;;   Contents:  This file contains descriptor data tables.
;;
;; $Archive: /USB/Examples/Fx2lp/bulkloop/dscr.a51 $
;; $Date: 9/01/03 8:51p $
;; $Revision: 3 $
;;
;;
;;-----------------------------------------------------------------------------
;; Copyright 2003, Cypress Semiconductor Corporation
;;-----------------------------------------------------------------------------;;-----------------------------------------------------------------------------
; USB MIDI DESCRIPTORS
#include "usb_defs.h"
   
DSCR_DEVICE   equ   1   ;; Descriptor type: Device
DSCR_CONFIG   equ   2   ;; Descriptor type: Configuration
DSCR_STRING   equ   3   ;; Descriptor type: String
DSCR_INTRFC   equ   4   ;; Descriptor type: Interface
DSCR_ENDPNT   equ   5   ;; Descriptor type: Endpoint
DSCR_DEVQUAL  equ   6   ;; Descriptor type: Device Qualifier

DSCR_DEVICE_LEN   equ   18
DSCR_CONFIG_LEN   equ    9
DSCR_INTRFC_LEN   equ    9
DSCR_ENDPNT_LEN   equ    7
DSCR_DEVQUAL_LEN  equ   10

ET_CONTROL   equ   0   ;; Endpoint type: Control
ET_ISO       equ   1   ;; Endpoint type: Isochronous
ET_BULK      equ   2   ;; Endpoint type: Bulk
ET_INT       equ   3   ;; Endpoint type: Interrupt

CS_INTERFACE      equ   0x24    ;; CS_INTERFACE descriptor.
CS_ENDPOINT       equ   0x25    ;; Descriptor type: CS_ENDPOINT

; ENDPOINT ADDRESSES: EPIN/OUT are named from the host point of view
;                     MIDI/AUDIO/IN/OUT are named from the device point of view
EPOUT_ADDR_MIDI_IN        equ 0x02 ;; Receives the MIDI data sent by the USB host
EPIN_ADDR_MIDI_OUT        equ 0x84 ;; Sends MIDI data to the USB host


public      DeviceDscr, DeviceQualDscr, HighSpeedConfigDscr, FullSpeedConfigDscr, StringDscr, UserDscr

DSCR   SEGMENT   CODE PAGE

;;-----------------------------------------------------------------------------
;; Global Variables
;;-----------------------------------------------------------------------------
      rseg DSCR      ;; locate the descriptor table in on-part memory.

;;CSEG AT 100H

DeviceDscr:   
      db   DSCR_DEVICE_LEN      ;; Descriptor length
      db   DSCR_DEVICE   ;; Decriptor type
      dw   0002H      ;; Specification Version (BCD)
      db   00H        ;; Device class
      db   00H         ;; Device sub-class
      db   00H         ;; Device sub-sub-class
      db   64         ;; Maximum packet size
		// db	0xE1, 0x0F	;; Vendor ID
		db	0x23, 0x01	;; Vendor ID
		db	0x01, 0x00	;; Product ID
		db	0x00, 0x00	;; Product version ID
      db   1         ;; Manufacturer string index
      db   2         ;; Product string index
      db   0         ;; Serial number string index
      db   1         ;; Number of configurations

DeviceQualDscr:
      db   DSCR_DEVQUAL_LEN   ;; Descriptor length
      db   DSCR_DEVQUAL   ;; Decriptor type
      dw   0002H      ;; Specification Version (BCD)
      db   00H        ;; Device class
      db   00H         ;; Device sub-class
      db   00H         ;; Device sub-sub-class
      db   64         ;; Maximum packet size
      db   1         ;; Number of configurations
      db   0         ;; Reserved

;;org (($ / 2) +1) * 2

HighSpeedConfigDscr:   
      db   DSCR_CONFIG_LEN               ;; Descriptor length
      db   DSCR_CONFIG                  ;; Descriptor type
      db   (HighSpeedConfigDscrEnd-HighSpeedConfigDscr) mod 256 ;; Total Length (LSB)
      db   (HighSpeedConfigDscrEnd-HighSpeedConfigDscr)  /  256 ;; Total Length (MSB)
      	db   2      ;; Number of interfaces
      db   1      ;; Configuration number
      db   0      ;; Configuration string
      	db   11000000b   ;; Attributes (b7 - buspwr, b6 - selfpwr, b5 - rwu)
      db   25      ;; Power requirement (div 2 ma)

;; Interface Descriptor
		db	DSCR_INTRFC_LEN	;; Descriptor length
		db	DSCR_INTRFC	;; Descriptor type
		db	0		;; Zero-based index of this interface
		db	0		;; index of this setting
		db	0		;; Number of end points
		db	1		;; Interface class  (AUDIO)
		db	1		;; Interface sub class  (AC:audio control)
		db	0		;; Interface sub sub class
		db	0		;; Interface descriptor string index

;; Interface Descriptor
		db	DSCR_INTRFC_LEN		;; Descriptor length
		db	CS_INTERFACE		;; Descriptor type
		db	1					;; descriptor sub-type
		db	0x00, 0x01	;; revision of this class specification
		db	0x09, 0x00	;; Total size of class-specific descriptors
		db	1		;; number of streaming interface
		db	1		;; midi streaming interf 1 belongs to this audiocontrol interf			   ???

;; Interface Descriptor
		db	DSCR_INTRFC_LEN	;; Descriptor length
		db	DSCR_INTRFC	;; Descriptor type
		db	1		;; Zero-based index of this interface
		db	0		;; index of this Alternate setting
		db	2		;; Number of end points 
		db	1		;; Interface class  (AUDIO)
		db	3		;; Interface sub class  (MIDISTREAMING)
		db	0		;; Interface sub sub class
		db	0		;; Interface descriptor string index

;; Interface Descriptor
HighSpeedMSCIntrfcDscr:
		db	7		;; Descriptor length
		db	CS_INTERFACE		;; Descriptor type
		db	1		;; Zero-based index of this interface
		db	0x00, 0x01	;; revision of this class specification
		db	LOW(HS_CS_MS_IF_DSCR_TOTAL_SIZE);; Total size of class-specific descriptors		   ???
		db	HIGH(HS_CS_MS_IF_DSCR_TOTAL_SIZE)		;;										   ???

;; Endpoint Descriptor MIDI IN EMBEDDED
 		db	6		;; Descriptor length
		db	CS_INTERFACE		;; Descriptor type (CS_INTERFACE)
		db	0x02		;; MIDI_IN_JACK subtype
		db	0x01		;; EMBEDDED
		db	0x01		;; ID of this jack
		db	0		;; unused

;; Endpoint Descriptor MIDI IN EXTERNAL
 		db	6		;; Descriptor length
		db	CS_INTERFACE		;; Descriptor type (CS_INTERFACE)
		db	0x02		;; MIDI_IN_JACK subtype
		db	0x02		;; EXTERNAL
		db	0x02		;; ID of this jack
		db	0		;; unused

;; Endpoint Descriptor MIDI OUT EMBEDDED
		db	9		;; Descriptor length
		db	CS_INTERFACE		;; Descriptor type (CS_INTERFACE)
		db	0x03		;; MIDI_OUT_JACK subtype
		db	0x01		;; EMBEDDED
		db	0x03		;; ID of this jack
		db	0x01		;; number of input pins of this jack
		db	0x02		;; ID of the entity to which this pin is connected
		db	0x01		;; Output Pin number of the entity to which this input pin is connected
		db	0		;; unused

;; Endpoint Descriptor MIDI OUT EXTERNAL
		db	9		;; Descriptor length
		db	CS_INTERFACE		;; Descriptor type (CS_INTERFACE)
		db	0x03		;; MIDI_OUT_JACK subtype
		db	0x02		;; EXTERNAL
		db	0x04		;; ID of this jack
		db	0x01		;; number of input pins of this jack
		db	0x01		;; ID of the entity to which this pin is connected
		db	0x01		;; Output Pin number of the entity to which this input pin is connected
		db	0		;; unused

;; Endpoint Descriptor BULK OUT
		db	9	;; Descriptor length
		db	DSCR_ENDPNT	;; Descriptor type
		db	EPOUT_ADDR_MIDI_IN		;; Out Endpoint 4 
		db	0x02		;; Bulk, not shared
		db	USB_EP_BUFFER_LENGHT, 0x00	;; 64 bytes per packet
		db	0x00		;; ignore for bulk
		db	0x00		;; unused
		db	0x00		;; unused

;; Endpoint Descriptor BULK OUT CLASS SPECIFIC
		db	5	;; Descriptor length
		db	CS_ENDPOINT		;; Descriptor type (CS_ENDPOINT)
		db	0x01		;; MS_GENERAL
		db	0x01		;; number of embedded MIDI IN Jacks (???)
		db	0x01		;; ID of embedded MIDI In Jack

;; Endpoint Descriptor BULK IN
		db	9	;; Descriptor length
		db	DSCR_ENDPNT	;; Descriptor type
		db	EPIN_ADDR_MIDI_OUT		;; In Endpoint 4 (see usb_defs.h)
		db	0x02		;; Bulk, not shared
		db	USB_EP_BUFFER_LENGHT, 0x00	;; 64 bytes per packet
		db	0x00		;; ignore for bulk
		db	0x00		;; unused
		db	0x00		;; unused

;; Endpoint Descriptor BULK IN CLASS SPECIFIC
		db	5	;; Descriptor length
		db	CS_ENDPOINT		;; Descriptor type (CS_ENDPOINT)
		db	0x01		;; MS_GENERAL
		db	0x01		;; number of embedded MIDI Out Jacks
		db	0x03		;; ID of embedded MIDI Out Jack

HighSpeedConfigDscrEnd:   

HS_CS_MS_IF_DSCR_TOTAL_SIZE    EQU    $-HighSpeedMSCIntrfcDscr

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

;;org (($ / 2) +1) * 2

FullSpeedConfigDscr:   
      db   DSCR_CONFIG_LEN               ;; Descriptor length
      db   DSCR_CONFIG                  ;; Descriptor type
      db   (FullSpeedConfigDscrEnd-FullSpeedConfigDscr) mod 256 ;; Total Length (LSB)
      db   (FullSpeedConfigDscrEnd-FullSpeedConfigDscr)  /  256 ;; Total Length (MSB)
      	db   2      ;; Number of interfaces
      db   1      ;; Configuration number
      db   0      ;; Configuration string
      	db   11000000b   ;; Attributes (b7 - buspwr, b6 - selfpwr, b5 - rwu)
      db   25      ;; Power requirement (div 2 ma)

;; Interface Descriptor
		db	DSCR_INTRFC_LEN	;; Descriptor length
		db	DSCR_INTRFC	;; Descriptor type
		db	0		;; Zero-based index of this interface
		db	0		;; index of this setting
		db	0		;; Number of end points
		db	1		;; Interface class  (AUDIO)
		db	1		;; Interface sub class  (AC:audio control)
		db	0		;; Interface sub sub class
		db	0		;; Interface descriptor string index

;; Interface Descriptor
		db	DSCR_INTRFC_LEN		;; Descriptor length
		db	CS_INTERFACE		;; Descriptor type
		db	1					;; descriptor sub-type
		db	0x00, 0x01	;; revision of this class specification
		db	0x09, 0x00	;; Total size of class-specific descriptors
		db	1		;; number of streaming interface
		db	1		;; midi streaming interf 1 belongs to this audiocontrol interf			   ???

;; Interface Descriptor
		db	DSCR_INTRFC_LEN	;; Descriptor length
		db	DSCR_INTRFC	;; Descriptor type
		db	1		;; Zero-based index of this interface
		db	0		;; index of this Alternate setting
		db	2		;; Number of end points 
		db	1		;; Interface class  (AUDIO)
		db	3		;; Interface sub class  (MIDISTREAMING)
		db	0		;; Interface sub sub class
		db	0		;; Interface descriptor string index

;; Interface Descriptor
FullSpeedMSCIntrfcDscr:
		db	7		;; Descriptor length
		db	CS_INTERFACE		;; Descriptor type
		db	1		;; Zero-based index of this interface
		db	0x00, 0x01	;; revision of this class specification
		db	LOW(FS_CS_MS_IF_DSCR_TOTAL_SIZE);; Total size of class-specific descriptors		   ???
		db	HIGH(FS_CS_MS_IF_DSCR_TOTAL_SIZE)					;;							   ???

;; Endpoint Descriptor MIDI IN EMBEDDED
 		db	6		;; Descriptor length
		db	CS_INTERFACE		;; Descriptor type (CS_INTERFACE)
		db	0x02		;; MIDI_IN_JACK subtype
		db	0x01		;; EMBEDDED
		db	0x01		;; ID of this jack
		db	0		;; unused

;; Endpoint Descriptor MIDI IN EXTERNAL
 		db	6		;; Descriptor length
		db	CS_INTERFACE		;; Descriptor type (CS_INTERFACE)
		db	0x02		;; MIDI_IN_JACK subtype
		db	0x02		;; EXTERNAL
		db	0x02		;; ID of this jack
		db	0		;; unused

;; Endpoint Descriptor MIDI OUT EMBEDDED
		db	9		;; Descriptor length
		db	CS_INTERFACE		;; Descriptor type (CS_INTERFACE)
		db	0x03		;; MIDI_OUT_JACK subtype
		db	0x01		;; EMBEDDED
		db	0x03		;; ID of this jack
		db	0x01		;; number of input pins of this jack
		db	0x02		;; ID of the entity to which this pin is connected
		db	0x01		;; Output Pin number of the entity to which this input pin is connected
		db	0		;; unused

;; Endpoint Descriptor MIDI OUT EXTERNAL
		db	9		;; Descriptor length
		db	CS_INTERFACE		;; Descriptor type (CS_INTERFACE)
		db	0x03		;; MIDI_OUT_JACK subtype
		db	0x02		;; EXTERNAL
		db	0x04		;; ID of this jack
		db	0x01		;; number of input pins of this jack
		db	0x01		;; ID of the entity to which this pin is connected
		db	0x01		;; Output Pin number of the entity to which this input pin is connected
		db	0		;; unused

;; Endpoint Descriptor BULK OUT
		db	9	;; Descriptor length
		db	DSCR_ENDPNT	;; Descriptor type
		db	EPOUT_ADDR_MIDI_IN		;; Out Endpoint 4 
		db	0x02		;; Bulk, not shared
		db	USB_EP_BUFFER_LENGHT, 0x00	;; 64 bytes per packet
		db	0x00		;; ignore for bulk
		db	0x00		;; unused
		db	0x00		;; unused

;; Endpoint Descriptor BULK OUT CLASS SPECIFIC
		db	5	;; Descriptor length
		db	CS_ENDPOINT		;; Descriptor type (CS_ENDPOINT)
		db	0x01		;; MS_GENERAL
		db	0x01		;; number of embedded MIDI IN Jacks (???)
		db	0x01		;; ID of embedded MIDI In Jack

;; Endpoint Descriptor BULK IN
		db	9	;; Descriptor length
		db	DSCR_ENDPNT	;; Descriptor type
		db	EPIN_ADDR_MIDI_OUT		;; In Endpoint 4 (see usb_defs.h)
		db	0x02		;; Bulk, not shared
		db	USB_EP_BUFFER_LENGHT, 0x00	;; 64 bytes per packet
		db	0x00		;; ignore for bulk
		db	0x00		;; unused
		db	0x00		;; unused

;; Endpoint Descriptor BULK IN CLASS SPECIFIC
		db	5	;; Descriptor length
		db	CS_ENDPOINT		;; Descriptor type (CS_ENDPOINT)
		db	0x01		;; MS_GENERAL
		db	0x01		;; number of embedded MIDI Out Jacks
		db	0x03		;; ID of embedded MIDI Out Jack
FullSpeedConfigDscrEnd:   

FS_CS_MS_IF_DSCR_TOTAL_SIZE    EQU    $-FullSpeedMSCIntrfcDscr

;;org (($ / 2) +1) * 2

StringDscr:

StringDscr0:   
      db   StringDscr0End-StringDscr0      ;; String descriptor length
      db   DSCR_STRING
      db   09H,04H
StringDscr0End:

StringDscr1:   
      db   StringDscr1End-StringDscr1      ;; String descriptor length
      db   DSCR_STRING
      db   'J',00
      db   'e',00
      db   'r',00
      db   'a',00
      db   's',00
      db   'h',00
      db   'L',00
StringDscr1End:

StringDscr2:   
      db   StringDscr2End-StringDscr2      ;; Descriptor length
      db   DSCR_STRING
      db   'M',00
      db   'a',00
      db   'w',00
      db   'z',00
      db   'e',00
      db   'r',00
StringDscr2End:

UserDscr:      
      dw   0000H
      end
      
