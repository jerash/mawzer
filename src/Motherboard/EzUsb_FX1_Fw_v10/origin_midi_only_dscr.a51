;-----------------------------------------------------------------------------
;;   File:     origin_dscr.a51 MIDI ONLY
;;   Contents: This file contains the "Origin" descriptor data tables.  
;;
;;   AUTHOR: P.WICKER
;;
;;   Copyright (c) 2005 ARTURIA SA. All rights reserved
;;-----------------------------------------------------------------------------

Iso_Buffer_Length equ   0x4000;  ;; ???????
HS_EP_BUFFER_LENGTH  equ   0x0002;  ;; HighSpeed EndPoint length= 0x200 
FS_EP_BUFFER_LENGTH  equ   0x4000;  ;; FullSpeed EndPoint length= 0x40

CS_INTERFACE      equ   0x24    ;; CS_INTERFACE descriptor.
CS_ENDPOINT       equ   0x25    ;; Descriptor type: CS_ENDPOINT
   
; Descriptor types definition
DSCR_DEVICE       equ   1       ;; Descriptor type: Device
DSCR_CONFIG       equ   2       ;; Descriptor type: Configuration
DSCR_STRING       equ   3       ;; Descriptor type: String
DSCR_INTRFC       equ   4       ;; Descriptor type: Interface
DSCR_ENDPNT       equ   5       ;; Descriptor type: Endpoint
DSCR_DEVQUAL      equ   6       ;; Descriptor type: Device Qualifier

; Sizes of fixed size descriptors
DSCR_DEVICE_LEN   equ   18
DSCR_CONFIG_LEN   equ   9
DSCR_INTRFC_LEN   equ   9
DSCR_ENDPNT_LEN   equ   7
DSCR_DEVQUAL_LEN  equ   10

; Endpoint types definition
ET_CONTROL        equ   0        ;; Endpoint type: Control
ET_ISO            equ   1        ;; Endpoint type: Isochronous
ET_BULK           equ   2        ;; Endpoint type: Bulk
ET_INT            equ   3        ;; Endpoint type: Interrupt

; MIDI function jacks and elements IDs
JACK_USB_MIDI_IN_ID    equ 1      ;; Embedded JACK connected to MIDI_EP_OUT
JACK_USB_MIDI_OUT_ID   equ 2      ;; Embedded JACK connected to MIDI_EP_IN
JACK_SER_MIDI_IN_ID    equ 3      ;; External MIDI IN JACK
JACK_SER_MIDI_OUT_ID   equ 4      ;; External MIDI OUT JACK
JACK_SER_MIDI_THRU_ID  equ 5      ;; External MIDI THRU JACK
ELEM_ARTURIA_SYNTH_ID  equ 6      ;; MIDI merger and synth are modeled as 
                                  ;; a single "CUSTOM" ELEMENT. All the jacks
                                  ;; are connected to this element

; ARTURIA_SYNTH element pins IDs
ARTURIA_SYNTH_OUTPIN_USB_MIDI  equ 1
ARTURIA_SYNTH_OUTPIN_SER_MIDI  equ 2

; AUDIO FUNCTION (AF) TERMINALS IDs
IT_EPOUT_AUDIO_IN_1CH_TO_AF_ID  equ 7   ;; Receives the mono 24 bits stream from the host
OT_AF_TO_ARTURIA_SYNTH_ID       equ 8   ;; Forwards the mono 24 bits stream to the synth
IT_ARTURIA_SYNTH_TO_AF_ID       equ 9   ;; Receives the 2/10 channels 24 bits stream from the synth
OT_AF_TO_EPIN_AUDIO_OUT_10CH_ID equ 10  ;; Outputs the 10 channels 24 bits stream to the host
OT_AF_TO_EPIN_AUDIO_OUT_2CH_ID equ 10   ;; Outputs the 2 channels 24 bits stream to the host

; ENDPOINT ADDRESSES: EPIN/OUT are named from the host point of view
;                     MIDI/AUDIO/IN/OUT are named from the device point of view
EPOUT_ADDR_MIDI_IN        equ 0x04 ;; Receives the MIDI data sent by the USB host
EPIN_ADDR_MIDI_OUT        equ 0x86 ;; Sends MIDI data to the USB host
EPOUT_ADDR_AUDIO_IN_1CH   equ 0x02 ;; Receives the mono 24 bits stream from the host
EPIN_ADDR_AUDIO_OUT_10CH  equ 0x88 ;; Outputs the 10 channels 24 bits stream to the host
EPIN_ADDR_AUDIO_OUT_2CH   equ 0x88 ;; Outputs the 2 channels 24 bits stream to the host


EMBEDDED_JACK          equ 1
EXTERNAL_JACK          equ 2

MIDI_IN_JACK           equ 2
MIDI_OUT_JACK          equ 3
ELEMENT                equ 4

public DeviceDscr, DeviceQualDscr, HighSpeedConfigDscr, FullSpeedConfigDscr, StringDscr, UserDscr

;DSCR   SEGMENT   CODE PAGE

;;-----------------------------------------------------------------------------
;; Global Variables
;;-----------------------------------------------------------------------------
;      rseg DSCR                 ;; locate the descriptor table in on-part memory.

CSEG AT 80H

DeviceDscr:   
      db   DSCR_DEVICE_LEN      ;; Descriptor length
      db   DSCR_DEVICE          ;; Decriptor type
      db   0x00, 0x02           ;; Specification Version (BCD)
      db   00H                  ;; Device class(0): Reserved, defined in Interface Descriptor
      db   00H                  ;; Device sub-class, unused
      db   00H                  ;; Device sub-sub-class, unused
      db   0x40                 ;; Maximum packet size
      dw   0x3412               ;; Vendor ID
      dw   0x0100               ;; Product ID
      dw   0x0100               ;; Product version ID, BCD device
      db   0x01                 ;; Manufacturer string index
      db   0x02                 ;; Product string index
      db   0x03                 ;; Serial number string index
      db   0x01                 ;; Number of configurations

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
;>>>>>>>>>> DESCRIPTORS FOR THE HIGH-SPEED (2.0) CONFIGURATION

org (($ / 2) +1) * 2

FullSpeedConfigDscr:   
  
      db   DSCR_CONFIG_LEN      ;; Descriptor length
      db   DSCR_CONFIG          ;; Descriptor type
      db   Low(FullSpeedConfigDscrEnd-FullSpeedConfigDscr)     ;; mod 256 ;; Total Length (LSB)
      db   High(FullSpeedConfigDscrEnd-FullSpeedConfigDscr)    ;; /256    ;; Total Length (MSB)
      db   2                    ;; Number of interfaces: 1 AC, 1 MS
      db   1                    ;; Configuration number
      db   0                    ;; Configuration string
      db   11000000b            ;; Attributes (b7 - buspwr, b6 - selfpwr, b5 - rwu)
      db   25                   ;; Power requirement 50 mA

;========= STANDARD INTERFACE DESCRIPTOR
        db    DSCR_INTRFC_LEN   ;; Descriptor length
        db    DSCR_INTRFC       ;; Descriptor type
        db    0                 ;; index of this interface
        db    0                 ;; index of this setting
        db    0                 ;; Number of endpoints used by this interface (excluding ep0).
                                ;; No interrupt endpoint hence the number set to 0. 
        db    1                 ;; Interface class  (AUDIO). It uses the default pipe
                                ;; (endpoint 0) for all communication purposes.
        db    1                 ;; Interface sub class AudioControl (Audio Control)
        db    0                 ;; Interface protocol
        db    0                 ;; Interface descriptor string index

;========= CLASS SPECIFIC AUDIO CONTROL INTERFACE: HEADER
FS_CS_AC_IF_DESCR:
FS_CS_AC_1:
        db    FS_CS_AC_1_END - FS_CS_AC_1 ;; 8 mandatory bytes + 1 byte for each streaming IF
        db    CS_INTERFACE      ;; Descriptor type
        db    0x01              ;; Header, sub-type
        db    0x00, 0x01        ;; revision of this class specification
        db    low(FS_CS_AC_IF_DESCR_END - FS_CS_AC_IF_DESCR)
        db    high(FS_CS_AC_IF_DESCR_END - FS_CS_AC_IF_DESCR)
        db    1                 ;; number of streaming interface, 1 MS, 2 AS
  ;;      db    1                 ;; audio streaming "IF 1" belongs to this AC IF
  ;;      db    2                 ;; audio streaming "IF 2" belongs to this AC IF
        db    1                 ;; MIDI streaming "IF 3" belongs to this AC IF        
FS_CS_AC_1_END:
FS_CS_AC_IF_DESCR_END:

;========= MIDI STREAMING INTERFACE ==========================================

;========= STANDARD MIDI STREAMING INTERFACE
        db    DSCR_INTRFC_LEN   ;; Descriptor length
        db    DSCR_INTRFC       ;; Descriptor type
        db    1                 ;; Zero-based index of this interface
        db    0                 ;; Zero-based index of this Alternate setting
        db    2                 ;; Number of endpoints (1 IN + 1 OUT)
        db    1                 ;; Interface class  (AUDIO)
        db    3                 ;; Interface sub class  (MIDISTREAMING)
        db    0                 ;; bInterfaceProtocol, not used
        db    0                 ;; iInterface string index, not used

;========= CLASS SPECIFIC MIDI STREAMING INTERFACE: HEADER
FS_CS_MS_IF_DSCR:
        db    7                 ;; Descriptor length
        db    CS_INTERFACE      ;; Descriptor type
        db    1                 ;; MS_HEADER subtype.
        db    0x00, 0x01        ;; revision of this class specification
        db    low(FS_CS_MS_IF_DSCR_TOTAL_SIZE) ;; Total size of class-specific descriptors
                                            ;; ie this header + JACKs and ELEMENTs descr.
        db    high(FS_CS_MS_IF_DSCR_TOTAL_SIZE)

;========= EMBEDDED JACK IN CONNECTED TO MIDI ENDPOINT OUT
        db    6                      ;; Descriptor length
        db    CS_INTERFACE           ;; Descriptor type (CS_INTERFACE)
        db    MIDI_IN_JACK           ;; JACK IN subtype
        db    EMBEDDED_JACK
        db    JACK_USB_MIDI_IN_ID    ;; ID of this jack
        db    4                      ;; String descr index "USB_MIDI_IN"

;========= EMBEDDED JACK OUT CONNECTED TO MIDI ENDPOINT IN
        db    6 + 2 + 1              ;; 6 + 2 * pins number + 1 string index
        db    CS_INTERFACE           ;; Descriptor type (CS_INTERFACE)
        db    MIDI_OUT_JACK          ;; JACK OUT subtype
        db    EMBEDDED_JACK
        db    JACK_USB_MIDI_OUT_ID   ;; ID of this jack
        db    1                      ;; number of input pins of this jack
        db    ELEM_ARTURIA_SYNTH_ID  ;; ID of the entity to which this pin is connected
        db    ARTURIA_SYNTH_OUTPIN_USB_MIDI ;; Output Pin number of the entity to which
                                            ;; this input pin is connected
        db    5                      ;; String descr index "USB_MIDI_OUT"

;========= EXTERNAL JACK IN CONNECTED TO THE ARTURIA_SYNTH ELEMENT
        db    6                      ;; Descriptor length
        db    CS_INTERFACE           ;; Descriptor type (CS_INTERFACE)
        db    MIDI_IN_JACK           ;; JACK IN subtype
        db    EXTERNAL_JACK
        db    JACK_SER_MIDI_IN_ID    ;; ID of this jack
        db    6                      ;; String descr index "SER_MIDI_IN"

;========= EXTERNAL JACK OUT CONNECTED TO THE ARTURIA_SYNTH ELEMENT
        db    6 + 2 + 1              ;; 6 + 2 * pins number + 1 string index
        db    CS_INTERFACE           ;; Descriptor type (CS_INTERFACE)
        db    MIDI_OUT_JACK          ;; JACK OUT subtype
        db    EXTERNAL_JACK
        db    JACK_SER_MIDI_OUT_ID   ;; ID of this jack
        db    1                      ;; number of input pins of this jack
        db    ELEM_ARTURIA_SYNTH_ID  ;; ID of the entity to which this pin is connected
        db    ARTURIA_SYNTH_OUTPIN_SER_MIDI ;; Output Pin number of the entity to which
                                            ;; this input pin is connected
        db    7                      ;; String descr index "SER_MIDI_OUT"

;========= EXTERNAL JACK THRU CONNECTED TO JACK_USB_MIDI_IN_ID
        db    6 + 2 + 1              ;; 6 + 2 * pins number + 1 string index
        db    CS_INTERFACE           ;; Descriptor type (CS_INTERFACE)
        db    MIDI_OUT_JACK          ;; JACK OUT subtype
        db    EXTERNAL_JACK
        db    JACK_SER_MIDI_THRU_ID  ;; ID of this jack
        db    1                      ;; number of input pins of this jack
        db    JACK_USB_MIDI_IN_ID    ;; ID of the entity to which this pin is connected
        db    1                      ;; ????? output pin number of a JACK IN
        db    8                      ;; String descr index "SER_MIDI_THRU"

;========= ARTURIA_SYNTH ELEMENT: custom, 2 input pins, 2 output pins
        db    9 + 2*2 + 2            ;; 9 + 2 * input pins number + output pins number
        db    CS_INTERFACE           ;; Descriptor type (CS_INTERFACE)
        db    ELEMENT                ;; Descriptor subtype
        db    ELEM_ARTURIA_SYNTH_ID  ;; ID of this entity
        db    2                      ;; Number of input pins
        db    JACK_USB_MIDI_IN_ID    ;;
        db    1                      ;; ????? output pin number of a JACK IN
        db    JACK_SER_MIDI_IN_ID    ;;
        db    1                      ;; ????? output pin number of a JACK IN
        db    2                      ;; Number of output pins
        db    IT_ARTURIA_SYNTH_TO_AF_ID ;; bInTerminalLink ID
        db    OT_AF_TO_ARTURIA_SYNTH_ID ;; bOutTerminalLink ID
        db    1                      ;; Element capabilities bit field size (in bytes)
        db    1                      ;; "CUSTOM" element
        db    9                      ;; String descr index "ORIGIN"


;========= "OUT" ENDPOINT USED BY THE USB HOST TO SEND MIDI TO THE DEVICE
;========= STANDARD BULK ENDPOINT DESCRIPTOR
        db    9                    ;; Descriptor length
        db    DSCR_ENDPNT          ;; Descriptor type
        db    EPOUT_ADDR_MIDI_IN   ;; 0x04
        db    ET_BULK              ;; Bulk, not shared
        dw    FS_EP_BUFFER_LENGTH     ;; 64 bytes per packet
        db    0x00                 ;; ignore for bulk
        db    0x00                 ;; unused
        db    0x00                 ;; unused
;
;========= CLASS SPECIFIC BULK ENDPOINT DESCRIPTOR
        db    4 + 1                ;; Descriptor length: 4 + 1 * connected jack IN
        db    CS_ENDPOINT          ;; Descriptor type (CS_ENDPOINT)
        db    0x01                 ;; MS_GENERAL subtype
        db    0x01                 ;; number of embedded MIDI IN Jacks
        db    JACK_USB_MIDI_IN_ID  ;; ID of embedded MIDI In Jack

;========= "IN" ENDPOINT USED BY THE USB HOST TO GET MIDI FROM THE DEVICE
;========= STANDARD BULK ENDPOINT DESCRIPTOR
        db    9                    ;; Descriptor length
        db    DSCR_ENDPNT          ;; Descriptor type
        db    EPIN_ADDR_MIDI_OUT   ;; 0x86
        db    ET_BULK              ;; Bulk, not shared
        dw    FS_EP_BUFFER_LENGTH     ;; 64 bytes per packet
        db    0x00                 ;; ignore for bulk
        db    0x00                 ;; unused
        db    0x00                 ;; unused
;
;========= CLASS SPECIFIC BULK ENDPOINT DESCRIPTOR
        db    4 + 1                ;; Descriptor length: 4 + 1 * connected jack IN
        db    CS_ENDPOINT          ;; Descriptor type (CS_ENDPOINT)
        db    0x01                 ;; MS_GENERAL
        db    0x01                 ;; number of embedded MIDI OUT Jacks
        db    JACK_USB_MIDI_OUT_ID ;; ID of embedded MIDI Out Jack

FS_CS_MS_IF_DSCR_TOTAL_SIZE    EQU    $-FS_CS_MS_IF_DSCR   
FullSpeedConfigDscrEnd:


org (($ / 2) +1) * 2   
HighSpeedConfigDscr:   
      db   DSCR_CONFIG_LEN      ;; Descriptor length
      db   DSCR_CONFIG          ;; Descriptor type
      db   Low(HighSpeedConfigDscrEnd-HighSpeedConfigDscr)     ;; mod 256 ;; Total Length (LSB)
      db   High(HighSpeedConfigDscrEnd-HighSpeedConfigDscr)    ;; /256    ;; Total Length (MSB)
      db   2                    ;; Number of interfaces: 1 AC, 1 MS
      db   1                    ;; Configuration number
      db   3                    ;; Configuration string
      db   11000000b            ;; Attributes (b7 - buspwr, b6 - selfpwr, b5 - rwu)
      db   0                   ;; Power requirement 50 mA

;========= STANDARD INTERFACE DESCRIPTOR
        db    DSCR_INTRFC_LEN   ;; Descriptor length
        db    DSCR_INTRFC       ;; Descriptor type
        db    0                 ;; index of this interface
        db    0                 ;; index of this setting
        db    0                 ;; Number of endpoints used by this interface (excluding ep0).
                                ;; No interrupt endpoint hence the number set to 0. 
        db    1                 ;; Interface class  (AUDIO). It uses the default pipe
                                ;; (endpoint 0) for all communication purposes.
        db    1                 ;; Interface sub class AudioControl (Audio Control)
        db    0                 ;; Interface protocol
        db    0                 ;; Interface descriptor string index

;========= CLASS SPECIFIC AUDIO CONTROL INTERFACE: HEADER
HS_CS_AC_IF_DESCR:
HS_CS_AC_1:
        db    HS_CS_AC_1_END - HS_CS_AC_1 ;; 8 mandatory bytes + 1 byte for each streaming IF
        db    CS_INTERFACE      ;; Descriptor type
        db    0x01              ;; Header, sub-type
        db    0x00, 0x01        ;; revision of this class specification
        db    low(HS_CS_AC_IF_DESCR_END - HS_CS_AC_IF_DESCR)
        db    high(HS_CS_AC_IF_DESCR_END - HS_CS_AC_IF_DESCR)
 ;;       db    3                 ;; number of streaming interface, 1 MS, 2 AS
        db    1                 ;; number of streaming interface, 1 MS, 2 AS
 ;;       db    1                 ;; audio streaming "IF 1" belongs to this AC IF
 ;;       db    2                 ;; audio streaming "IF 2" belongs to this AC IF
        db    1                 ;; MIDI streaming "IF 3" belongs to this AC IF        
HS_CS_AC_1_END:
HS_CS_AC_IF_DESCR_END:

;========= MIDI STREAMING INTERFACE ==========================================
;========= MIDI STREAMING INTERFACE ==========================================

;========= STANDARD MIDI STREAMING INTERFACE
        db    DSCR_INTRFC_LEN   ;; Descriptor length
        db    DSCR_INTRFC       ;; Descriptor type
        db    1                 ;; Zero-based index of this interface
        db    0                 ;; Zero-based index of this Alternate setting
        db    2                 ;; Number of endpoints (1 IN + 1 OUT)
        db    1                 ;; Interface class  (AUDIO)
        db    3                 ;; Interface sub class  (MIDISTREAMING)
        db    0                 ;; bInterfaceProtocol, not used
        db    0                 ;; iInterface string index, not used

;========= CLASS SPECIFIC MIDI STREAMING INTERFACE: HEADER
HS_CS_MS_IF_DSCR:
        db    7                 ;; Descriptor length
        db    CS_INTERFACE      ;; Descriptor type
        db    1                 ;; MS_HEADER subtype.
        db    0x00, 0x01        ;; revision of this class specification
        db    low(HS_CS_MS_IF_DSCR_TOTAL_SIZE) ;; Total size of class-specific descriptors
                                            ;; ie this header + JACKs and ELEMENTs descr.
        db    high(HS_CS_MS_IF_DSCR_TOTAL_SIZE)

;========= EMBEDDED JACK IN CONNECTED TO MIDI ENDPOINT OUT
        db    6                      ;; Descriptor length
        db    CS_INTERFACE           ;; Descriptor type (CS_INTERFACE)
        db    MIDI_IN_JACK           ;; JACK IN subtype
        db    EMBEDDED_JACK
        db    JACK_USB_MIDI_IN_ID    ;; ID of this jack
        db    4                      ;; String descr index "USB_MIDI_IN"

;========= EXTERNAL JACK IN CONNECTED TO THE ARTURIA_SYNTH ELEMENT
        db    6                      ;; Descriptor length
        db    CS_INTERFACE           ;; Descriptor type (CS_INTERFACE)
        db    MIDI_IN_JACK           ;; JACK IN subtype
        db    EXTERNAL_JACK
        db    JACK_SER_MIDI_IN_ID    ;; ID of this jack
        db    6                      ;; String descr index "SER_MIDI_IN"



;========= EMBEDDED JACK OUT CONNECTED TO MIDI ENDPOINT IN
        db    6 + 2 + 1              ;; 6 + 2 * pins number + 1 string index
        db    CS_INTERFACE           ;; Descriptor type (CS_INTERFACE)
        db    MIDI_OUT_JACK          ;; JACK OUT subtype
        db    EMBEDDED_JACK
        db    JACK_USB_MIDI_OUT_ID   ;; ID of this jack
        db    1                      ;; number of input pins of this jack
        db    JACK_SER_MIDI_IN_ID  ;; ID of the entity to which this pin is connected
        db    1;ARTURIA_SYNTH_OUTPIN_SER_MIDI ;; Output Pin number of the entity to which
                                            ;; this input pin is connected
        db    5                      ;; String descr index "USB_MIDI_OUT"



;========= EXTERNAL JACK OUT CONNECTED TO THE ARTURIA_SYNTH ELEMENT
        db    6 + 2 + 1              ;; 6 + 2 * pins number + 1 string index
        db    CS_INTERFACE           ;; Descriptor type (CS_INTERFACE)
        db    MIDI_OUT_JACK          ;; JACK OUT subtype
        db    EXTERNAL_JACK
        db    JACK_SER_MIDI_OUT_ID   ;; ID of this jack
        db    1                      ;; number of input pins of this jack
        db    JACK_USB_MIDI_IN_ID  ;; ID of the entity to which this pin is connected
        db    1;ARTURIA_SYNTH_OUTPIN_USB_MIDI ;; Output Pin number of the entity to which
                                            ;; this input pin is connected
        db    7                      ;; String descr index "SER_MIDI_OUT"



;========= "OUT" ENDPOINT USED BY THE USB HOST TO SEND MIDI TO THE DEVICE
;========= STANDARD BULK ENDPOINT DESCRIPTOR
        db    9                    ;; Descriptor length
        db    DSCR_ENDPNT          ;; Descriptor type
        db    EPOUT_ADDR_MIDI_IN   ;; 0x04
        db    ET_BULK              ;; Bulk, not shared
        dw    0x4000;HS_EP_BUFFER_LENGTH     ;; 512 bytes per packet
        db    0x00                 ;; ignore for bulk
        db    0x00                 ;; unused
        db    0x00                 ;; unused
;
;========= CLASS SPECIFIC BULK ENDPOINT DESCRIPTOR
        db    4 + 1                ;; Descriptor length: 4 + 1 * connected jack IN
        db    CS_ENDPOINT          ;; Descriptor type (CS_ENDPOINT)
        db    0x01                 ;; MS_GENERAL subtype
        db    0x01                 ;; number of embedded MIDI IN Jacks
        db    JACK_USB_MIDI_IN_ID  ;; ID of embedded MIDI In Jack

;========= "IN" ENDPOINT USED BY THE USB HOST TO GET MIDI FROM THE DEVICE
;========= STANDARD BULK ENDPOINT DESCRIPTOR
        db    9                    ;; Descriptor length
        db    DSCR_ENDPNT          ;; Descriptor type
        db    EPIN_ADDR_MIDI_OUT   ;; 0x86
        db    ET_BULK              ;; Bulk, not shared
        dw    0x4000;HS_EP_BUFFER_LENGTH     ;; 512 bytes per packet
        db    0x00                 ;; ignore for bulk
        db    0x00                 ;; unused
        db    0x00                 ;; unused
;
;========= CLASS SPECIFIC BULK ENDPOINT DESCRIPTOR
        db    4 + 1                ;; Descriptor length: 4 + 1 * connected jack IN
        db    CS_ENDPOINT          ;; Descriptor type (CS_ENDPOINT)
        db    0x01                 ;; MS_GENERAL
        db    0x01                 ;; number of embedded MIDI OUT Jacks
        db    JACK_USB_MIDI_OUT_ID ;; ID of embedded MIDI Out Jack

HS_CS_MS_IF_DSCR_TOTAL_SIZE    EQU    $-HS_CS_MS_IF_DSCR
      
HighSpeedConfigDscrEnd:

;<<<<<<<<<< END OF DESCRIPTORS FOR THE FULL AND HIGH-SPEED CONFIGURATION
;<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
;<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

;==================================================================================================================
;                     string descriptors
;==================================================================================================================

org (($ / 2) +1) * 2

StringDscr:

StringDscr0:   
      db   StringDscr0End-StringDscr0      ;; String descriptor length
      db   DSCR_STRING
      db   09H,04H
StringDscr0End:

StringDscr1:   
      db   StringDscr1End-StringDscr1      ;; String descriptor length
      db   DSCR_STRING
      db   'A',00
      db   'r',00
      db   't',00
      db   'u',00
      db   'r',00
      db   'i',00
      db   'a',00
StringDscr1End:

StringDscr2:   
      db   StringDscr2End-StringDscr2      ;; Descriptor length
      db   DSCR_STRING
      db   'O',00
      db   'r',00
      db   'i',00
      db   'g',00
      db   'i',00
      db   'n',00
StringDscr2End:

StringDscr3:   
      db   StringDscr3End-StringDscr3      ;; Descriptor length
      db   DSCR_STRING
      db   '1',00
      db   '.',00
      db   '0',00
      db   '0',00
StringDscr3End:

StringDscr4:   
      db   StringDscr4End-StringDscr4      ;; Descriptor length
      db   DSCR_STRING
      db   'U',00
      db   'S',00
      db   'B',00
      db   '_',00
      db   'M',00
      db   'I',00
      db   'D',00
      db   'I',00
      db   '_',00
      db   'I',00
      db   'N',00
StringDscr4End:

StringDscr5:   
      db   StringDscr5End-StringDscr5      ;; Descriptor length
      db   DSCR_STRING
      db   'U',00
      db   'S',00
      db   'B',00
      db   '_',00
      db   'M',00
      db   'I',00
      db   'D',00
      db   'I',00
      db   '_',00
      db   'O',00
      db   'U',00
      db   'T',00
StringDscr5End:

StringDscr6:   
      db   StringDscr6End-StringDscr6      ;; Descriptor length
      db   DSCR_STRING
      db   'S',00
      db   'E',00
      db   'R',00
      db   '_',00
      db   'M',00
      db   'I',00
      db   'D',00
      db   'I',00
      db   '_',00
      db   'I',00
      db   'N',00
StringDscr6End:

StringDscr7:   
      db   StringDscr7End-StringDscr7      ;; Descriptor length
      db   DSCR_STRING
      db   'S',00
      db   'E',00
      db   'R',00
      db   '_',00
      db   'M',00
      db   'I',00
      db   'D',00
      db   'I',00
      db   '_',00
      db   'O',00
      db   'U',00
      db   'T',00
StringDscr7End:

StringDscr8:   
      db   StringDscr8End-StringDscr8      ;; Descriptor length
      db   DSCR_STRING
      db   'S',00
      db   'E',00
      db   'R',00
      db   '_',00
      db   'M',00
      db   'I',00
      db   'D',00
      db   'I',00
      db   '_',00
      db   'T',00
      db   'H',00
      db   'R',00
      db   'U',00
StringDscr8End:

StringDscr9:   
      db   StringDscr9End-StringDscr9      ;; Descriptor length
      db   DSCR_STRING
      db   'O',00
      db   'R',00
      db   'I',00
      db   'G',00
      db   'I',00
      db   'N',00
StringDscr9End:


UserDscr:      
      dw   0000H
      end
      
                        