; USB MIDI DESCRIPTORS
#include "usb_defs.h"

; ==========================================================================
; Copyright (C) MAWZER - Jerash Labs 2006
; ==========================================================================

DSCR_DEVICE	equ	1	;; Descriptor type: Device
DSCR_CONFIG	equ	2	;; Descriptor type: Configuration
DSCR_STRING	equ	3	;; Descriptor type: String
DSCR_INTRFC	equ	4	;; Descriptor type: Interface
DSCR_ENDPNT	equ	5	;; Descriptor type: Endpoint
DSCR_DEVQUAL      equ   6       ;; Descriptor type: Device Qualifier

ET_CONTROL	equ	0	;; Endpoint type: Control
ET_ISO		equ	1	;; Endpoint type: Isochronous
ET_BULK		equ	2	;; Endpoint type: Bulk
ET_INT		equ	3	;; Endpoint type: Interrupt

DSCR_DEVICE_LEN   equ   18
DSCR_CONFIG_LEN   equ   9
DSCR_INTRFC_LEN   equ   9
DSCR_ENDPNT_LEN   equ   7
DSCR_DEVQUAL_LEN  equ   10

CS_INTERFACE      equ   0x24    ;; CS_INTERFACE descriptor.
CS_ENDPOINT       equ   0x25    ;; Descriptor type: CS_ENDPOINT

; ENDPOINT ADDRESSES: EPIN/OUT are named from the host point of view
;                     MIDI/AUDIO/IN/OUT are named from the device point of view
EPOUT_ADDR_MIDI_IN        equ 0x02 ;; Receives the MIDI data sent by the USB host
EPIN_ADDR_MIDI_OUT        equ 0x84 ;; Sends MIDI data to the USB host


//public DeviceDscr, ConfigDscr, StringDscr, UserDscr
public DeviceDscr, DeviceQualDscr, HighSpeedConfigDscr, FullSpeedConfigDscr, StringDscr, UserDscr


DSCR	SEGMENT	CODE PAGE
//CSEG AT 100H

;;-----------------------------------------------------------------------------
;; Global Variables
;;-----------------------------------------------------------------------------

		rseg DSCR		;; locate the descriptor table anywhere below 8K

DeviceDscr:	
		db	DSCR_DEVICE_LEN		;; Descriptor length
		db	DSCR_DEVICE	;; Decriptor type
		db	0x10, 0x01	;; Specification Version (BCD)
		db	0x00  		;; Device class
		db	0x00		;; Device sub-class
		db	0x00		;; Device sub-sub-class
		db	64			;; Maximum packet size
		db	0xE1, 0x0F	;; Vendor ID
		db	0x01, 0x00	;; Product ID
		db	0x00, 0x00	;; Product version ID
		db	1			;; Manufacturer string index
		db	2			;; Product string index
		db	0			;; Serial number string index
		db	1			;; Number of configurations
deviceDscrEnd:

DeviceQualDscr:
      db   DSCR_DEVQUAL_LEN     ;; Descriptor length
      db   DSCR_DEVQUAL         ;; Decriptor type
      db   0x00, 0x02           ;; Specification Version (BCD)
      db   00H                  ;; Device class
      db   00H                  ;; Device sub-class
      db   00H                  ;; Device sub-sub-class
      db   0x40                 ;; Maximum packet size
      db   0x01                 ;; Number of configurations
      db   0x00                 ;; Reserved

;>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
;>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
;>>>>>>>>>> DESCRIPTORS FOR THE FULL-SPEED (1.1) CONFIGURATION

org (($ / 2) +1) * 2

FullSpeedConfigDscr:   
		db	FullSpeedConfigDscrEnd-FullSpeedConfigDscr		;; Descriptor length
		db	DSCR_CONFIG	;; Descriptor type
		db	LOW(FullSpeedConfigDscrEnd-FullSpeedConfigDscr)   ;; Config + End Points length (LSB)
		db	HIGH(FullSpeedConfigDscrEnd-FullSpeedConfigDscr)  ;; Config + End Points length (MSB)
		db	2		;; Number of interfaces 
		db	1		;; configuration value, Interface number 
		db	0		;; Configuration string
		db	11000000b	;; Attributes (b7 - buspwr, b6 - selfpwr, b5 - rwu)
		;JDS power requirements set to 100 mA
		db	0x50		;; Power requirement (100ma div 2)
FullSpeedConfigDscrEnd:

; B3.1
;; removed otherwise win Me crashes
FullSpeedStandardAcIntrfcDscr:
		db	FullSpeedStandardAcIntrfcDscrEnd-FullSpeedStandardAcIntrfcDscr	;; Descriptor length
		db	DSCR_INTRFC	;; Descriptor type
		db	0		;; Zero-based index of this interface
		db	0		;; index of this setting
		db	0		;; Number of end points
		db	1		;; Interface class  (AUDIO)
		db	1		;; Interface sub class  (AC:audio control)
		db	0		;; Interface sub sub class
		db	0		;; Interface descriptor string index
FullSpeedStandardAcIntrfcDscrEnd:

; B3.2
FullSpeedClassSpecificAcIntrfcDscr:
		db	FullSpeedClassSpecificAcIntrfcDscrEnd-FullSpeedClassSpecificAcIntrfcDscr		;; Descriptor length
		db	CS_INTERFACE		;; Descriptor type
		db	1		;; descriptor sub-type
		db	0x00, 0x01	;; revision of this class specification
		db	0x09, 0x00	;; Total size of class-specific descriptors
		db	1		;; number of streaming interface
		db	1		;; midi streaming interf 1 belongs to this audiocontrol interf
FullSpeedClassSpecificAcIntrfcDscrEnd:

;========= MIDI STREAMING INTERFACE ==========================================

;B4.1
FullSpeedMidiStreamIntrfcDscr:
		db	FullSpeedMidiStreamIntrfcDscrEnd-FullSpeedMidiStreamIntrfcDscr	;; Descriptor length
		db	DSCR_INTRFC	;; Descriptor type
		db	1		;; Zero-based index of this interface
		db	0		;; index of this Alternate setting
		db	2		;; Number of end points 
		db	1		;; Interface class  (AUDIO)
		db	3		;; Interface sub class  (MIDISTREAMING)
		db	0		;; Interface sub sub class
		db	0		;; Interface descriptor string index
FullSpeedMidiStreamIntrfcDscrEnd:

;B4.2
FullSpeedMSCIntrfcDscr:
		db	FullSpeedMSCIntrfcDscrEnd-FullSpeedMSCIntrfcDscr		;; Descriptor length
		db	CS_INTERFACE		;; Descriptor type
		db	1		;; Zero-based index of this interface
		db	0x00, 0x01	;; revision of this class specification
		db	LOW(FS_CS_MS_IF_DSCR_TOTAL_SIZE);; Total size of class-specific descriptors
		db	HIGH(FS_CS_MS_IF_DSCR_TOTAL_SIZE)
FullSpeedMSCIntrfcDscrEnd:

;B4.3 //////////////////// MIDI IN 1 /////////////////////////////
FSMIDIInEmDscr:
		db	FSMIDIInEmDscrEnd-FSMIDIInEmDscr		;; Descriptor length
		db	CS_INTERFACE		;; Descriptor type (CS_INTERFACE)
		db	0x02		;; MIDI_IN_JACK subtype
		db	0x01		;; EMBEDDED
		db	0x01		;; ID of this jack
		db	0		;; unused
FSMIDIInEmDscrEnd:

FSMIDIInExDscr:
		db	FSMIDIInExDscrEnd-FSMIDIInExDscr		;; Descriptor length
		db	CS_INTERFACE		;; Descriptor type (CS_INTERFACE)
		db	0x02		;; MIDI_IN_JACK subtype
		db	0x02		;; EXTERNAL
		db	0x02		;; ID of this jack
		db	0		;; unused
FSMIDIInExDscrEnd:


;B4.4 //////////////////// MIDI OUT 1 /////////////////////////////
FSMIDIOutEmDscr:
		db	FSMIDIOutEmDscrEnd-FSMIDIOutEmDscr		;; Descriptor length
		db	CS_INTERFACE		;; Descriptor type (CS_INTERFACE)
		db	0x03		;; MIDI_OUT_JACK subtype
		db	0x01		;; EMBEDDED
		db	0x03		;; ID of this jack
		db	0x01		;; number of input pins of this jack
		db	0x02		;; ID of the entity to which this pin is connected
		db	0x01		;; Output Pin number of the entity to which this input pin is connected
		db	0		;; unused
FSMIDIOutEmDscrEnd:

FSMIDIOutExDscr:
		db	FSMIDIOutExDscrEnd-FSMIDIOutExDscr		;; Descriptor length
		db	CS_INTERFACE		;; Descriptor type (CS_INTERFACE)
		db	0x03		;; MIDI_OUT_JACK subtype
		db	0x02		;; EXTERNAL
		db	0x04		;; ID of this jack
		db	0x01		;; number of input pins of this jack
		db	0x01		;; ID of the entity to which this pin is connected
		db	0x01		;; Output Pin number of the entity to which this input pin is connected
		db	0		;; unused
FSMIDIOutExDscrEnd:

;B4.4 //////////////////// MIDI OUT 2 /////////////////////////////
;MIDIOutEm2Dscr:
;		db	MIDIOutEm2DscrEnd-MIDIOutEm2Dscr	;; Descriptor length
;		db	0x24		;; Descriptor type (CS_INTERFACE)
;		db	0x03		;; MIDI_OUT_JACK subtype
;		db	0x01		;; EMBEDDED
;		db	0x05		;; ID of this jack
;		db	0x01		;; number of input pins of this jack
;		db	0x02		;; ID of the entity to which this pin is connected
;		db	0x01		;; Output Pin number of the entity to which this input pin is connected
;		db	0		;; unused
;MIDIOutEm2DscrEnd:

;MIDIOutEx2Dscr:
;		db	MIDIOutEx2DscrEnd-MIDIOutEx2Dscr	;; Descriptor length
;		db	0x24		;; Descriptor type (CS_INTERFACE)
;		db	0x03		;; MIDI_OUT_JACK subtype
;		db	0x02		;; EXTERNAL
;		db	0x06		;; ID of this jack
;		db	0x01		;; number of input pins of this jack
;		db	0x01		;; ID of the entity to which this pin is connected
;		db	0x01		;; Output Pin number of the entity to which this input pin is connected
;		db	0		;; unused
;MIDIOutEx2DscrEnd:

;B5.1
FSStdBulkOutEPDscr:
		db	FSStdBulkOutEPDscrEnd-FSStdBulkOutEPDscr	;; Descriptor length
		db	DSCR_ENDPNT	;; Descriptor type
		db	EPOUT_ADDR_MIDI_IN		;; Out Endpoint 4 
		db	0x02		;; Bulk, not shared
		db	USB_EP_BUFFER_LENGHT, 0x00	;; 64 bytes per packet
		db	0x00		;; ignore for bulk
		db	0x00		;; unused
		db	0x00		;; unused
FSStdBulkOutEPDscrEnd:

;B5.2
FSMSBulkOutEPDscr:
		db	FSMSBulkOutEPDscrEnd-FSMSBulkOutEPDscr	;; Descriptor length
		db	CS_ENDPOINT		;; Descriptor type (CS_ENDPOINT)
		db	0x01		;; MS_GENERAL
		db	0x01		;; number of embedded MIDI IN Jacks (???)
		db	0x01		;; ID of embedded MIDI In Jack
FSMSBulkOutEPDscrEnd:

;B6.1
FSStdBulkInEPDscr:
		db	FSStdBulkInEPDscrEnd-FSStdBulkInEPDscr	;; Descriptor length
		db	DSCR_ENDPNT	;; Descriptor type
		db	EPIN_ADDR_MIDI_OUT		;; In Endpoint 2 (see usb_defs.h)
		db	0x02		;; Bulk, not shared
		db	USB_EP_BUFFER_LENGHT, 0x00	;; 64 bytes per packet
		db	0x00		;; ignore for bulk
		db	0x00		;; unused
		db	0x00		;; unused
FSStdBulkInEPDscrEnd:

;B6.2
FSMSBulkInEPDscr:
		db	FSMSBulkInEPDscrEnd-FSMSBulkInEPDscr	;; Descriptor length
		db	CS_ENDPOINT		;; Descriptor type (CS_ENDPOINT)
		db	0x01		;; MS_GENERAL
//		db	0x02		;; number of embedded MIDI Out Jacks
		db	0x01		;; number of embedded MIDI Out Jacks
		db	0x03		;; ID of embedded MIDI Out Jack
//		db	0x05		;; ID of embedded MIDI Out Jack
FSMSBulkInEPDscrEnd:

//MSCClassSize	EQU	$-MSCIntrfcDscr
FS_CS_MS_IF_DSCR_TOTAL_SIZE    EQU    $-FullSpeedMSCIntrfcDscr  
//FullSpeedConfigDscrEnd:

;>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
;>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
;>>>>>>>>>> DESCRIPTORS FOR THE HIGH-SPEED (2.0) CONFIGURATION

org (($ / 2) +1) * 2

HighSpeedConfigDscr:   
		db	HighSpeedConfigDscrEnd-HighSpeedConfigDscr		;; Descriptor length
		db	DSCR_CONFIG	;; Descriptor type
		db	LOW(StringDscr-HighSpeedConfigDscr)   ;; Config + End Points length (LSB)
		db	HIGH(StringDscr-HighSpeedConfigDscr)  ;; Config + End Points length (MSB)
		db	2		;; Number of interfaces 
		db	1		;; configuration value, Interface number 
		db	3		;; Configuration string
		db	11000000b	;; Attributes (b7 - buspwr, b6 - selfpwr, b5 - rwu)
		;JDS power requirements set to 100 mA
		db	0		;; Power requirement (100ma div 2)
HighSpeedConfigDscrEnd:

; B3.1
;; removed otherwise win Me crashes
HighSpeedStandardAcIntrfcDscr:
		db	HighSpeedStandardAcIntrfcDscrEnd-HighSpeedStandardAcIntrfcDscr	;; Descriptor length
		db	DSCR_INTRFC	;; Descriptor type
		db	0		;; Zero-based index of this interface
		db	0		;; index of this setting
		db	0		;; Number of end points
		db	1		;; Interface class  (AUDIO)
		db	1		;; Interface sub class  (AC:audio control)
		db	0		;; Interface sub sub class
		db	0		;; Interface descriptor string index
HighSpeedStandardAcIntrfcDscrEnd:

; B3.2
HighSpeedClassSpecificAcIntrfcDscr:
		db	HighSpeedClassSpecificAcIntrfcDscrEnd-HighSpeedClassSpecificAcIntrfcDscr		;; Descriptor length
		db	CS_INTERFACE		;; Descriptor type
		db	1		;; descriptor sub-type
		db	0x00, 0x01	;; revision of this class specification
		db	0x09, 0x00	;; Total size of class-specific descriptors
		db	1		;; number of streaming interface
		db	1		;; midi streaming interf 1 belongs to this audiocontrol interf
HighSpeedClassSpecificAcIntrfcDscrEnd:

;========= MIDI STREAMING INTERFACE ==========================================

;B4.1
HighSpeedMidiStreamIntrfcDscr:
		db	HighSpeedMidiStreamIntrfcDscrEnd-HighSpeedMidiStreamIntrfcDscr	;; Descriptor length
		db	DSCR_INTRFC	;; Descriptor type
		db	1		;; Zero-based index of this interface
		db	0		;; index of this Alternate setting
		db	2		;; Number of end points 
		db	1		;; Interface class  (AUDIO)
		db	3		;; Interface sub class  (MIDISTREAMING)
		db	0		;; Interface sub sub class
		db	0		;; Interface descriptor string index
HighSpeedMidiStreamIntrfcDscrEnd:

;B4.2
HighSpeedMSCIntrfcDscr:
		db	HighSpeedMSCIntrfcDscrEnd-HighSpeedMSCIntrfcDscr		;; Descriptor length
		db	CS_INTERFACE		;; Descriptor type
		db	1		;; Zero-based index of this interface
		db	0x00, 0x01	;; revision of this class specification
		db	LOW(HS_CS_MS_IF_DSCR_TOTAL_SIZE);; Total size of class-specific descriptors
		db	HIGH(HS_CS_MS_IF_DSCR_TOTAL_SIZE)
HighSpeedMSCIntrfcDscrEnd:

;B4.3 //////////////////// MIDI IN 1 /////////////////////////////
HSMIDIInEmDscr:
		db	HSMIDIInEmDscrEnd-HSMIDIInEmDscr		;; Descriptor length
		db	CS_INTERFACE		;; Descriptor type (CS_INTERFACE)
		db	0x02		;; MIDI_IN_JACK subtype
		db	0x01		;; EMBEDDED
		db	0x01		;; ID of this jack
		db	0		;; unused
HSMIDIInEmDscrEnd:

HSMIDIInExDscr:
		db	HSMIDIInExDscrEnd-HSMIDIInExDscr		;; Descriptor length
		db	CS_INTERFACE		;; Descriptor type (CS_INTERFACE)
		db	0x02		;; MIDI_IN_JACK subtype
		db	0x02		;; EXTERNAL
		db	0x02		;; ID of this jack
		db	0		;; unused
HSMIDIInExDscrEnd:


;B4.4 //////////////////// MIDI OUT 1 /////////////////////////////
HSMIDIOutEmDscr:
		db	HSMIDIOutEmDscrEnd-HSMIDIOutEmDscr		;; Descriptor length
		db	CS_INTERFACE		;; Descriptor type (CS_INTERFACE)
		db	0x03		;; MIDI_OUT_JACK subtype
		db	0x01		;; EMBEDDED
		db	0x03		;; ID of this jack
		db	0x01		;; number of input pins of this jack
		db	0x02		;; ID of the entity to which this pin is connected
		db	0x01		;; Output Pin number of the entity to which this input pin is connected
		db	0		;; unused
HSMIDIOutEmDscrEnd:

HSMIDIOutExDscr:
		db	HSMIDIOutExDscrEnd-HSMIDIOutExDscr		;; Descriptor length
		db	CS_INTERFACE		;; Descriptor type (CS_INTERFACE)
		db	0x03		;; MIDI_OUT_JACK subtype
		db	0x02		;; EXTERNAL
		db	0x04		;; ID of this jack
		db	0x01		;; number of input pins of this jack
		db	0x01		;; ID of the entity to which this pin is connected
		db	0x01		;; Output Pin number of the entity to which this input pin is connected
		db	0		;; unused
HSMIDIOutExDscrEnd:

;B4.4 //////////////////// MIDI OUT 2 /////////////////////////////
;MIDIOutEm2Dscr:
;		db	MIDIOutEm2DscrEnd-MIDIOutEm2Dscr	;; Descriptor length
;		db	0x24		;; Descriptor type (CS_INTERFACE)
;		db	0x03		;; MIDI_OUT_JACK subtype
;		db	0x01		;; EMBEDDED
;		db	0x05		;; ID of this jack
;		db	0x01		;; number of input pins of this jack
;		db	0x02		;; ID of the entity to which this pin is connected
;		db	0x01		;; Output Pin number of the entity to which this input pin is connected
;		db	0		;; unused
;MIDIOutEm2DscrEnd:

;MIDIOutEx2Dscr:
;		db	MIDIOutEx2DscrEnd-MIDIOutEx2Dscr	;; Descriptor length
;		db	0x24		;; Descriptor type (CS_INTERFACE)
;		db	0x03		;; MIDI_OUT_JACK subtype
;		db	0x02		;; EXTERNAL
;		db	0x06		;; ID of this jack
;		db	0x01		;; number of input pins of this jack
;		db	0x01		;; ID of the entity to which this pin is connected
;		db	0x01		;; Output Pin number of the entity to which this input pin is connected
;		db	0		;; unused
;MIDIOutEx2DscrEnd:

;B5.1
HSStdBulkOutEPDscr:
		db	HSStdBulkOutEPDscrEnd-HSStdBulkOutEPDscr	;; Descriptor length
		db	DSCR_ENDPNT	;; Descriptor type
		db	EPOUT_ADDR_MIDI_IN		;; Out Endpoint 4 
		db	0x02		;; Bulk, not shared
		db	USB_EP_BUFFER_LENGHT, 0x00	;; 64 bytes per packet
		db	0x00		;; ignore for bulk
		db	0x00		;; unused
		db	0x00		;; unused
HSStdBulkOutEPDscrEnd:

;B5.2
HSMSBulkOutEPDscr:
		db	HSMSBulkOutEPDscrEnd-HSMSBulkOutEPDscr	;; Descriptor length
		db	CS_ENDPOINT		;; Descriptor type (CS_ENDPOINT)
		db	0x01		;; MS_GENERAL
		db	0x01		;; number of embedded MIDI IN Jacks (???)
		db	0x01		;; ID of embedded MIDI In Jack
HSMSBulkOutEPDscrEnd:

;B6.1
HSStdBulkInEPDscr:
		db	HSStdBulkInEPDscrEnd-HSStdBulkInEPDscr	;; Descriptor length
		db	DSCR_ENDPNT	;; Descriptor type
		db	EPIN_ADDR_MIDI_OUT		;; In Endpoint 2 (see usb_defs.h)
		db	0x02		;; Bulk, not shared
		db	USB_EP_BUFFER_LENGHT, 0x00	;; 64 bytes per packet
		db	0x00		;; ignore for bulk
		db	0x00		;; unused
		db	0x00		;; unused
HSStdBulkInEPDscrEnd:

;B6.2
HSMSBulkInEPDscr:
		db	HSMSBulkInEPDscrEnd-HSMSBulkInEPDscr	;; Descriptor length
		db	CS_ENDPOINT		;; Descriptor type (CS_ENDPOINT)
		db	0x01		;; MS_GENERAL
//		db	0x02		;; number of embedded MIDI Out Jacks
		db	0x01		;; number of embedded MIDI Out Jacks
		db	0x03		;; ID of embedded MIDI Out Jack
//		db	0x05		;; ID of embedded MIDI Out Jack
HSMSBulkInEPDscrEnd:

//MSCClassSize	EQU	$-MSCIntrfcDscr
HS_CS_MS_IF_DSCR_TOTAL_SIZE    EQU    $-HighSpeedMSCIntrfcDscr

//HighSpeedConfigDscrEnd:


;<<<<<<<<<< END OF DESCRIPTORS FOR THE FULL AND HIGH-SPEED CONFIGURATION
;<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
;<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

;==================================================================================================================
;                     string descriptors
;==================================================================================================================

org (($ / 2) +1) * 2


;; string language descriptor (english)
StringDscr:
StringDscr0:
		db	StringDscr0End-StringDscr0		;; String descriptor length
		db	DSCR_STRING
		db	0x09,0x04
StringDscr0End:

StringDscr1:	
		db	StringDscr1End-StringDscr1		;; String descriptor length
		db	DSCR_STRING
		db	'J',00
		db	'e',00
		db	'r',00
		db	'a',00
		db	's',00
		db	'h',00
		db	'L',00
		db	'a',00
		db	'b',00
		db	's',00
StringDscr1End:

StringDscr2:	
		db	StringDscr2End-StringDscr2		;; Descriptor length
		db	DSCR_STRING
		db	'M',00
		db	'a',00
		db	'w',00
		db	'z',00
		db	'e',00
		db	'r',00
StringDscr2End:

UserDscr:		
		dw	0x0000
		end
		
