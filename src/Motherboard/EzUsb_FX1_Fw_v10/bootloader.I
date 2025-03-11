
#line 1 "bootloader.c" /0
 
 
 
 
 
 
  
#line 1 "fx2.h" /0
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 


 
 
 
 
 
 


 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 

 
 
 
 
 
 
 
 

 
 
 
 
 

 
 

 
 
 
 
 
 
 typedef unsigned char   BYTE;
 typedef unsigned short   WORD;
 typedef unsigned long   DWORD;
 typedef bit            BOOL;
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 typedef struct
 {
 BYTE   length;
 BYTE   type;
 }DSCR;
 
 typedef struct             
 {
 BYTE   length;          
 BYTE   type;          
 BYTE   spec_ver_minor;    
 BYTE   spec_ver_major;    
 BYTE   dev_class;       
 BYTE   sub_class;       
 BYTE   protocol;       
 BYTE   max_packet;       
 WORD   vendor_id;       
 WORD   product_id;       
 WORD   version_id;       
 BYTE   mfg_str;       
 BYTE   prod_str;       
 BYTE   serialnum_str;    
 BYTE   configs;       
 }DEVICEDSCR;
 
 typedef struct             
 {
 BYTE   length;          
 BYTE   type;          
 BYTE   spec_ver_minor;    
 BYTE   spec_ver_major;    
 BYTE   dev_class;       
 BYTE   sub_class;       
 BYTE   protocol;       
 BYTE   max_packet;       
 BYTE   configs;       
 BYTE  reserved0;
 }DEVICEQUALDSCR;
 
 typedef struct
 {
 BYTE   length;          
 BYTE   type;          
 WORD   config_len;       
 BYTE   interfaces;       
 BYTE   index;          
 BYTE   config_str;       
 BYTE   attrib;          
 BYTE   power;          
 }CONFIGDSCR;
 
 typedef struct
 {
 BYTE   length;          
 BYTE   type;          
 BYTE   index;          
 BYTE   alt_setting;    
 BYTE   ep_cnt;          
 BYTE   class;          
 BYTE   sub_class;       
 BYTE   protocol;       
 BYTE   interface_str;    
 }INTRFCDSCR;
 
 typedef struct
 {
 BYTE   length;          
 BYTE   type;          
 BYTE   addr;          
 BYTE   ep_type;       
 BYTE   mp_L;          
 BYTE   mp_H;
 BYTE   interval;       
 }ENDPNTDSCR;
 
 typedef struct
 {
 BYTE   length;          
 BYTE   type;          
 }STRINGDSCR;
 
 typedef struct
 {
 BYTE   cntrl;          
 BYTE   bytes;          
 }EPIOC;
 
 typedef struct 
 {
 BYTE   length;
 BYTE   *dat;
 BYTE   count;
 BYTE   status;
 }I2CPCKT;
 
 
 
 
 extern code BYTE   USB_AutoVector;
 
 extern WORD   pDeviceDscr;
 extern WORD   pDeviceQualDscr;
 extern WORD	  pHighSpeedConfigDscr;
 extern WORD	  pFullSpeedConfigDscr;	
 extern WORD   pConfigDscr;
 extern WORD   pOtherConfigDscr;
 extern WORD   pStringDscr;
 
 extern code DEVICEDSCR        DeviceDscr;
 extern code DEVICEQUALDSCR    DeviceQualDscr;
 extern code CONFIGDSCR        HighSpeedConfigDscr;
 extern code CONFIGDSCR        FullSpeedConfigDscr;
 extern code STRINGDSCR        StringDscr;
 extern code DSCR              UserDscr;
 
 extern I2CPCKT   I2CPckt;
 
 
 
 
 
 extern void EZUSB_Renum(void);
 extern void EZUSB_Discon(BOOL renum);
 
 extern void EZUSB_Susp(void);
 extern void EZUSB_Resume(void);
 
 extern void EZUSB_Delay1ms(void);
 extern void EZUSB_Delay(WORD ms);
 
 extern CONFIGDSCR xdata*   EZUSB_GetConfigDscr(BYTE ConfigIdx);
 extern INTRFCDSCR xdata*   EZUSB_GetIntrfcDscr(BYTE ConfigIdx, BYTE IntrfcIdx, BYTE AltSetting);
 extern STRINGDSCR xdata*   EZUSB_GetStringDscr(BYTE StrIdx);
 extern DSCR xdata*      EZUSB_GetDscr(BYTE index, DSCR* dscr, BYTE type);
 
 extern void EZUSB_InitI2C(void);
 extern BOOL EZUSB_WriteI2C_(BYTE addr, BYTE length, BYTE xdata *dat);
 extern BOOL EZUSB_ReadI2C_(BYTE addr, BYTE length, BYTE xdata *dat);
 extern BOOL EZUSB_WriteI2C(BYTE addr, BYTE length, BYTE xdata *dat);
 extern BOOL EZUSB_ReadI2C(BYTE addr, BYTE length, BYTE xdata *dat);
 extern void EZUSB_WaitForEEPROMWrite(BYTE addr);
 
 extern void modify_endpoint_stall(BYTE epid, BYTE stall);
 
 
#line 7 "bootloader.c" /0
 
  
#line 1 "fx2regs.h" /0
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
#line 35 "fx2regs.h" /1
  
  
 
#line 38 "fx2regs.h" /0
 
 
 
 
  extern xdata volatile BYTE GPIF_WAVE_DATA    ; 
  extern xdata volatile BYTE RES_WAVEDATA_END  ; 
 
 
 
  extern xdata volatile BYTE CPUCS             ; 
  extern xdata volatile BYTE IFCONFIG          ; 
  extern xdata volatile BYTE PINFLAGSAB        ; 
  extern xdata volatile BYTE PINFLAGSCD        ; 
  extern xdata volatile BYTE FIFORESET         ; 
  extern xdata volatile BYTE BREAKPT           ; 
  extern xdata volatile BYTE BPADDRH           ; 
  extern xdata volatile BYTE BPADDRL           ; 
  extern xdata volatile BYTE UART230           ; 
  extern xdata volatile BYTE FIFOPINPOLAR      ; 
  extern xdata volatile BYTE REVID             ; 
  extern xdata volatile BYTE REVCTL            ; 
 
 
 
  extern xdata volatile BYTE EP1OUTCFG         ; 
  extern xdata volatile BYTE EP1INCFG          ; 
  extern xdata volatile BYTE EP2CFG            ; 
  extern xdata volatile BYTE EP4CFG            ; 
  extern xdata volatile BYTE EP6CFG            ; 
  extern xdata volatile BYTE EP8CFG            ; 
  extern xdata volatile BYTE EP2FIFOCFG        ; 
  extern xdata volatile BYTE EP4FIFOCFG        ; 
  extern xdata volatile BYTE EP6FIFOCFG        ; 
  extern xdata volatile BYTE EP8FIFOCFG        ; 
  extern xdata volatile BYTE EP2AUTOINLENH     ; 
  extern xdata volatile BYTE EP2AUTOINLENL     ; 
  extern xdata volatile BYTE EP4AUTOINLENH     ; 
  extern xdata volatile BYTE EP4AUTOINLENL     ; 
  extern xdata volatile BYTE EP6AUTOINLENH     ; 
  extern xdata volatile BYTE EP6AUTOINLENL     ; 
  extern xdata volatile BYTE EP8AUTOINLENH     ; 
  extern xdata volatile BYTE EP8AUTOINLENL     ; 
  extern xdata volatile BYTE EP2FIFOPFH        ; 
  extern xdata volatile BYTE EP2FIFOPFL        ; 
  extern xdata volatile BYTE EP4FIFOPFH        ; 
  extern xdata volatile BYTE EP4FIFOPFL        ; 
  extern xdata volatile BYTE EP6FIFOPFH        ; 
  extern xdata volatile BYTE EP6FIFOPFL        ; 
  extern xdata volatile BYTE EP8FIFOPFH        ; 
  extern xdata volatile BYTE EP8FIFOPFL        ; 
  extern xdata volatile BYTE EP2ISOINPKTS      ; 
  extern xdata volatile BYTE EP4ISOINPKTS      ; 
  extern xdata volatile BYTE EP6ISOINPKTS      ; 
  extern xdata volatile BYTE EP8ISOINPKTS      ; 
  extern xdata volatile BYTE INPKTEND          ; 
  extern xdata volatile BYTE OUTPKTEND         ; 
 
 
 
  extern xdata volatile BYTE EP2FIFOIE         ; 
  extern xdata volatile BYTE EP2FIFOIRQ        ; 
  extern xdata volatile BYTE EP4FIFOIE         ; 
  extern xdata volatile BYTE EP4FIFOIRQ        ; 
  extern xdata volatile BYTE EP6FIFOIE         ; 
  extern xdata volatile BYTE EP6FIFOIRQ        ; 
  extern xdata volatile BYTE EP8FIFOIE         ; 
  extern xdata volatile BYTE EP8FIFOIRQ        ; 
  extern xdata volatile BYTE IBNIE             ; 
  extern xdata volatile BYTE IBNIRQ            ; 
  extern xdata volatile BYTE NAKIE             ; 
  extern xdata volatile BYTE NAKIRQ            ; 
  extern xdata volatile BYTE USBIE             ; 
  extern xdata volatile BYTE USBIRQ            ; 
  extern xdata volatile BYTE EPIE              ; 
  extern xdata volatile BYTE EPIRQ             ; 
  extern xdata volatile BYTE GPIFIE            ; 
  extern xdata volatile BYTE GPIFIRQ           ; 
  extern xdata volatile BYTE USBERRIE          ; 
  extern xdata volatile BYTE USBERRIRQ         ; 
  extern xdata volatile BYTE ERRCNTLIM         ; 
  extern xdata volatile BYTE CLRERRCNT         ; 
  extern xdata volatile BYTE INT2IVEC          ; 
  extern xdata volatile BYTE INT4IVEC          ; 
  extern xdata volatile BYTE INTSETUP          ; 
 
 
 
  extern xdata volatile BYTE PORTACFG          ; 
  extern xdata volatile BYTE PORTCCFG          ; 
  extern xdata volatile BYTE PORTECFG          ; 
  extern xdata volatile BYTE I2CS              ; 
  extern xdata volatile BYTE I2DAT             ; 
  extern xdata volatile BYTE I2CTL             ; 
  extern xdata volatile BYTE XAUTODAT1         ; 
  extern xdata volatile BYTE XAUTODAT2         ; 
 
 
 
 
 
 
  extern xdata volatile BYTE USBCS             ; 
  extern xdata volatile BYTE SUSPEND           ; 
  extern xdata volatile BYTE WAKEUPCS          ; 
  extern xdata volatile BYTE TOGCTL            ; 
  extern xdata volatile BYTE USBFRAMEH         ; 
  extern xdata volatile BYTE USBFRAMEL         ; 
  extern xdata volatile BYTE MICROFRAME        ; 
  extern xdata volatile BYTE FNADDR            ; 
 
 
 
  extern xdata volatile BYTE EP0BCH            ; 
  extern xdata volatile BYTE EP0BCL            ; 
  extern xdata volatile BYTE EP1OUTBC          ; 
  extern xdata volatile BYTE EP1INBC           ; 
  extern xdata volatile BYTE EP2BCH            ; 
  extern xdata volatile BYTE EP2BCL            ; 
  extern xdata volatile BYTE EP4BCH            ; 
  extern xdata volatile BYTE EP4BCL            ; 
  extern xdata volatile BYTE EP6BCH            ; 
  extern xdata volatile BYTE EP6BCL            ; 
  extern xdata volatile BYTE EP8BCH            ; 
  extern xdata volatile BYTE EP8BCL            ; 
  extern xdata volatile BYTE EP0CS             ; 
  extern xdata volatile BYTE EP1OUTCS          ; 
  extern xdata volatile BYTE EP1INCS           ; 
  extern xdata volatile BYTE EP2CS             ; 
  extern xdata volatile BYTE EP4CS             ; 
  extern xdata volatile BYTE EP6CS             ; 
  extern xdata volatile BYTE EP8CS             ; 
  extern xdata volatile BYTE EP2FIFOFLGS       ; 
  extern xdata volatile BYTE EP4FIFOFLGS       ; 
  extern xdata volatile BYTE EP6FIFOFLGS       ; 
  extern xdata volatile BYTE EP8FIFOFLGS       ; 
  extern xdata volatile BYTE EP2FIFOBCH        ; 
  extern xdata volatile BYTE EP2FIFOBCL        ; 
  extern xdata volatile BYTE EP4FIFOBCH        ; 
  extern xdata volatile BYTE EP4FIFOBCL        ; 
  extern xdata volatile BYTE EP6FIFOBCH        ; 
  extern xdata volatile BYTE EP6FIFOBCL        ; 
  extern xdata volatile BYTE EP8FIFOBCH        ; 
  extern xdata volatile BYTE EP8FIFOBCL        ; 
  extern xdata volatile BYTE SUDPTRH           ; 
  extern xdata volatile BYTE SUDPTRL           ; 
  extern xdata volatile BYTE SUDPTRCTL         ; 
  extern xdata volatile BYTE SETUPDAT[8]       ; 
 
 
 
  extern xdata volatile BYTE GPIFWFSELECT      ; 
  extern xdata volatile BYTE GPIFIDLECS        ; 
  extern xdata volatile BYTE GPIFIDLECTL       ; 
  extern xdata volatile BYTE GPIFCTLCFG        ; 
  extern xdata volatile BYTE GPIFADRH          ; 
  extern xdata volatile BYTE GPIFADRL          ; 
 
  extern xdata volatile BYTE GPIFTCB3          ; 
  extern xdata volatile BYTE GPIFTCB2          ; 
  extern xdata volatile BYTE GPIFTCB1          ; 
  extern xdata volatile BYTE GPIFTCB0          ; 
 
 
 
 
 
 
 
 
 
 
  extern xdata volatile BYTE EP2GPIFFLGSEL     ; 
  extern xdata volatile BYTE EP2GPIFPFSTOP     ; 
  extern xdata volatile BYTE EP2GPIFTRIG       ; 
  extern xdata volatile BYTE EP4GPIFFLGSEL     ; 
  extern xdata volatile BYTE EP4GPIFPFSTOP     ; 
  extern xdata volatile BYTE EP4GPIFTRIG       ; 
  extern xdata volatile BYTE EP6GPIFFLGSEL     ; 
  extern xdata volatile BYTE EP6GPIFPFSTOP     ; 
  extern xdata volatile BYTE EP6GPIFTRIG       ; 
  extern xdata volatile BYTE EP8GPIFFLGSEL     ; 
  extern xdata volatile BYTE EP8GPIFPFSTOP     ; 
  extern xdata volatile BYTE EP8GPIFTRIG       ; 
  extern xdata volatile BYTE XGPIFSGLDATH      ; 
  extern xdata volatile BYTE XGPIFSGLDATLX     ; 
  extern xdata volatile BYTE XGPIFSGLDATLNOX   ; 
  extern xdata volatile BYTE GPIFREADYCFG      ; 
  extern xdata volatile BYTE GPIFREADYSTAT     ; 
  extern xdata volatile BYTE GPIFABORT         ; 
 
 
 
  extern xdata volatile BYTE FLOWSTATE         ; 
  extern xdata volatile BYTE FLOWLOGIC         ; 
  extern xdata volatile BYTE FLOWEQ0CTL        ; 
  extern xdata volatile BYTE FLOWEQ1CTL        ; 
  extern xdata volatile BYTE FLOWHOLDOFF       ; 
  extern xdata volatile BYTE FLOWSTB           ; 
  extern xdata volatile BYTE FLOWSTBEDGE       ; 
  extern xdata volatile BYTE FLOWSTBHPERIOD    ; 
  extern xdata volatile BYTE GPIFHOLDAMOUNT    ; 
  extern xdata volatile BYTE UDMACRCH          ; 
  extern xdata volatile BYTE UDMACRCL          ; 
  extern xdata volatile BYTE UDMACRCQUAL       ; 
 
 
 
 
 
 
  extern xdata volatile BYTE DBUG              ; 
  extern xdata volatile BYTE TESTCFG           ; 
  extern xdata volatile BYTE USBTEST           ; 
  extern xdata volatile BYTE CT1               ; 
  extern xdata volatile BYTE CT2               ; 
  extern xdata volatile BYTE CT3               ; 
  extern xdata volatile BYTE CT4               ; 
 
 
 
  extern xdata volatile BYTE EP0BUF[64]        ; 
  extern xdata volatile BYTE EP1OUTBUF[64]     ; 
  extern xdata volatile BYTE EP1INBUF[64]      ; 
  extern xdata volatile BYTE EP2FIFOBUF[1024]  ; 
  extern xdata volatile BYTE EP4FIFOBUF[1024]  ; 
  extern xdata volatile BYTE EP6FIFOBUF[1024]  ; 
  extern xdata volatile BYTE EP8FIFOBUF[1024]  ; 
 
 
 
  extern xdata volatile BYTE ECCCFG            ; 
  extern xdata volatile BYTE ECCRESET          ; 
  extern xdata volatile BYTE ECC1B0            ; 
  extern xdata volatile BYTE ECC1B1            ; 
  extern xdata volatile BYTE ECC1B2            ; 
  extern xdata volatile BYTE ECC2B0            ; 
  extern xdata volatile BYTE ECC2B1            ; 
  extern xdata volatile BYTE ECC2B2            ; 
 
 
  extern xdata volatile BYTE GPCR2             ; 
 
 
 
 






 
 
 sfr IOA     = 0x80;
 
 sbit PA0    = 0x80 + 0;
 sbit PA1    = 0x80 + 1;
 sbit PA2    = 0x80 + 2;
 sbit PA3    = 0x80 + 3;
 
 sbit PA4    = 0x80 + 4;
 sbit PA5    = 0x80 + 5;
 sbit PA6    = 0x80 + 6;
 sbit PA7    = 0x80 + 7;
 sfr SP      = 0x81;
 sfr DPL     = 0x82;
 sfr DPH     = 0x83;
 sfr DPL1    = 0x84;
 sfr DPH1    = 0x85;
 sfr DPS     = 0x86;
 
 
 sfr PCON    = 0x87;
 
 
 
 
 
 
 sfr TCON    = 0x88;
 
 sbit IT0    = 0x88+0;
 sbit IE0    = 0x88+1;
 sbit IT1    = 0x88+2;
 sbit IE1    = 0x88+3;
 sbit TR0    = 0x88+4;
 sbit TF0    = 0x88+5;
 sbit TR1    = 0x88+6;
 sbit TF1    = 0x88+7;
 sfr TMOD    = 0x89;
 
 
 
 
 
 
 
 
 
 sfr TL0     = 0x8A;
 sfr TL1     = 0x8B;
 sfr TH0     = 0x8C;
 sfr TH1     = 0x8D;
 sfr CKCON   = 0x8E;
 
 
 
 
 
 
 
 sfr SPC_FNC = 0x8F;  
 
 
 sfr IOB     = 0x90;
 
 sbit PB0    = 0x90 + 0;
 sbit PB1    = 0x90 + 1;
 sbit PB2    = 0x90 + 2;
 sbit PB3    = 0x90 + 3;
 
 sbit PB4    = 0x90 + 4;
 sbit PB5    = 0x90 + 5;
 sbit PB6    = 0x90 + 6;
 sbit PB7    = 0x90 + 7;
 sfr EXIF    = 0x91;  
 
 
 
 
 
 sfr MPAGE  = 0x92;
 sfr SCON0  = 0x98;
 
 sbit RI    = 0x98+0;
 sbit TI    = 0x98+1;
 sbit RB8   = 0x98+2;
 sbit TB8   = 0x98+3;
 sbit REN   = 0x98+4;
 sbit SM2   = 0x98+5;
 sbit SM1   = 0x98+6;
 sbit SM0   = 0x98+7;
 sfr SBUF0  = 0x99;
 
 
 
 
 
 
 
 sfr AUTOPTRH1     = 0x9A; 
 sfr AUTOPTRL1     = 0x9B; 
 sfr AUTOPTRH2     = 0x9D;
 sfr AUTOPTRL2     = 0x9E; 
 
 sfr IOC        = 0xA0;
 
 sbit PC0    = 0xA0 + 0;
 sbit PC1    = 0xA0 + 1;
 sbit PC2    = 0xA0 + 2;
 sbit PC3    = 0xA0 + 3;
 
 sbit PC4    = 0xA0 + 4;
 sbit PC5    = 0xA0 + 5;
 sbit PC6    = 0xA0 + 6;
 sbit PC7    = 0xA0 + 7;
 sfr INT2CLR    = 0xA1;
 sfr INT4CLR    = 0xA2;
 
 sfr IE     = 0xA8;
 
 sbit EX0   = 0xA8+0;
 sbit ET0   = 0xA8+1;
 sbit EX1   = 0xA8+2;
 sbit ET1   = 0xA8+3;
 sbit ES0   = 0xA8+4;
 sbit ET2   = 0xA8+5;
 sbit ES1   = 0xA8+6;
 sbit EA    = 0xA8+7;
 
 sfr EP2468STAT     = 0xAA;
 
 
 
 
 
 
 
 
 
 
 sfr EP24FIFOFLGS   = 0xAB;
 sfr EP68FIFOFLGS   = 0xAC;
 sfr AUTOPTRSETUP  = 0xAF;
 
 
 
 
 
 sfr IOD     = 0xB0;
 
 sbit PD0    = 0xB0 + 0;
 sbit PD1    = 0xB0 + 1;
 sbit PD2    = 0xB0 + 2;
 sbit PD3    = 0xB0 + 3;
 
 sbit PD4    = 0xB0 + 4;
 sbit PD5    = 0xB0 + 5;
 sbit PD6    = 0xB0 + 6;
 sbit PD7    = 0xB0 + 7;
 sfr IOE     = 0xB1;
 sfr OEA     = 0xB2;
 sfr OEB     = 0xB3;
 sfr OEC     = 0xB4;
 sfr OED     = 0xB5;
 sfr OEE     = 0xB6;
 
 sfr IP     = 0xB8;
 
 sbit PX0   = 0xB8+0;
 sbit PT0   = 0xB8+1;
 sbit PX1   = 0xB8+2;
 sbit PT1   = 0xB8+3;
 sbit PS0   = 0xB8+4;
 sbit PT2   = 0xB8+5;
 sbit PS1   = 0xB8+6;
 
 sfr EP01STAT    = 0xBA;
 sfr GPIFTRIG    = 0xBB;
 
 sfr GPIFSGLDATH     = 0xBD;
 sfr GPIFSGLDATLX    = 0xBE;
 sfr GPIFSGLDATLNOX  = 0xBF;
 
 sfr SCON1  = 0xC0;
 
 sbit RI1   = 0xC0+0;
 sbit TI1   = 0xC0+1;
 sbit RB81  = 0xC0+2;
 sbit TB81  = 0xC0+3;
 sbit REN1  = 0xC0+4;
 sbit SM21  = 0xC0+5;
 sbit SM11  = 0xC0+6;
 sbit SM01  = 0xC0+7;
 sfr SBUF1  = 0xC1;
 sfr T2CON  = 0xC8;
 
 sbit CP_RL2 = 0xC8+0;
 sbit C_T2  = 0xC8+1;
 sbit TR2   = 0xC8+2;
 sbit EXEN2 = 0xC8+3;
 sbit TCLK  = 0xC8+4;
 sbit RCLK  = 0xC8+5;
 sbit EXF2  = 0xC8+6;
 sbit TF2   = 0xC8+7;
 sfr RCAP2L = 0xCA;
 sfr RCAP2H = 0xCB;
 sfr TL2    = 0xCC;
 sfr TH2    = 0xCD;
 sfr PSW    = 0xD0;
 
 sbit P     = 0xD0+0;
 sbit FL    = 0xD0+1;
 sbit OV    = 0xD0+2;
 sbit RS0   = 0xD0+3;
 sbit RS1   = 0xD0+4;
 sbit F0    = 0xD0+5;
 sbit AC    = 0xD0+6;
 sbit CY    = 0xD0+7;
 sfr EICON  = 0xD8;  
 
 sbit INT6  = 0xD8+3;
 sbit RESI  = 0xD8+4;
 sbit ERESI = 0xD8+5;
 sbit SMOD1 = 0xD8+7;
 sfr ACC    = 0xE0;
 sfr EIE    = 0xE8;  
 
 sbit EUSB    = 0xE8+0;
 sbit EI2C    = 0xE8+1;
 sbit EIEX4   = 0xE8+2;
 sbit EIEX5   = 0xE8+3;
 sbit EIEX6   = 0xE8+4;
 sfr B      = 0xF0;
 sfr EIP    = 0xF8;  
 
 sbit PUSB    = 0xF8+0;
 sbit PI2C    = 0xF8+1;
 sbit EIPX4   = 0xF8+2;
 sbit EIPX5   = 0xF8+3;
 sbit EIPX6   = 0xF8+4;
 


 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 

 
 
 
 
 
 
 
 
 
 
 
#line 8 "bootloader.c" /0
 
  
#line 1 "common_def.h" /0
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 typedef struct {
 BYTE start_index;							 
 volatile BYTE end_index;					 
 BYTE merger_state;							 
 volatile BYTE current_index;				 
 volatile BYTE input_status;					 
 BYTE buffer[128];	 
 } Midi_In_Struct;
 
 
 
 
 
 typedef struct {
 BYTE realtime[8]; 
 BYTE realtime_start_index;					 
 BYTE realtime_end_index;					 
 volatile BYTE input_status;					 
 } Midi_In_Realtime_Struct;
 
 
 
 
 
 
 
 
#line 9 "bootloader.c" /0
 
  
#line 1 "..\..\COMMON_EZ_Z8\COMMON_Z8_EZ.H" /0
 
 
 
 typedef unsigned char uchar;
 typedef unsigned int  uint;
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
#line 67 "..\..\COMMON_EZ_Z8\COMMON_Z8_EZ.H" /1
  
 
  
  
 
 
 
 
 
#line 76 "..\..\COMMON_EZ_Z8\COMMON_Z8_EZ.H" /0
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
#line 97 "..\..\COMMON_EZ_Z8\COMMON_Z8_EZ.H" /1
  
  
  
 
#line 101 "..\..\COMMON_EZ_Z8\COMMON_Z8_EZ.H" /0
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
#line 167 "..\..\COMMON_EZ_Z8\COMMON_Z8_EZ.H" /1
  
  
  
  
 
#line 172 "..\..\COMMON_EZ_Z8\COMMON_Z8_EZ.H" /0
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 enum { FRAME_IS_COMMON_PARAM, FRAME_IS_LCD_CTRL_ASSIGN, FRAME_IS_FIRMWARE_OR_PURE_MIDI, FRAME_IS_MIDI_CTRL_ASSIGN};
 
 
 enum { 	 
 CTRL_REG_0_7BITS_TO_EZ_INDEX, CTRL_REG_1_7BITS_TO_EZ_INDEX, 
 LAST_CTRL_REG_7BITS_TO_EZ_INDEX,
 
 Z8_STATUS_0_NVM_INDEX, Z8_STATUS_1_NVM_INDEX, CHANNEL_INDEX, GROUP_INDEX, INT_TEMPO_INDEX, 
 
 CTRL_REG_0_7B_FROM_EZ_INDEX, LCD_MESSAGE_TO_Z8_INDEX };
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
#line 10 "bootloader.c" /0
 
  
#line 1 "pins_def.h" /0
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
#line 11 "bootloader.c" /0
 
  
#line 1 "periph.h" /0
 
 
 
 BOOL TD_Resume(void); 			 
 void TD_Init(void);
 
 
 
#line 12 "bootloader.c" /0
 
  
#line 1 "bootloader.h" /0
 
 
 
 enum { BOOTLOADER_OFF, SEND_IDENTITY_SYSEX, WAIT_EZ_FW_DATA, WAIT_Z8_FW_DATA, TERMINATE_FW_UPGRADE };
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 enum { FW_BL_SRC_UNKNOWN, FW_BL_SRC_MIDI_IN, FW_BL_SRC_USB_IN };
 
 
 
 
 
 
 void Check_Bootloader(void);
 void SendIdentityBootLoader(void);
 void SendFwAcknoledge(unsigned char result);
 void AssignJlabsSysexHeader(void);
 void SendDispatchFwSysex(void);
 void WaitNextBootLoaderState(unsigned char awaited_state);
 void DecodeFirmwareSysex(Midi_In_Struct *midi_struct);
 
 BYTE EEPROMWriteVerify(WORD addr, BYTE length, BYTE xdata *buf);  
 void WriteSingleRegOCD(WORD address, BYTE value);
 void InitFlashFrqAndUnlock(void);
 
 
 
#line 13 "bootloader.c" /0
 
  
#line 1 "serial.h" /0
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 enum {WAIT_SOF, FRAME_STATUS_BYTE, WAIT_EOF };  
 enum {WAIT_FOR_CORRECT_DATA, IN_SYSEX, SYSEX_RECEIVED, NORMAL, FIFO_FULL };  
 enum {CN0, CN1 };	  
 enum {SERIAL, USB };  
 
 
 
 
 
 
 
 
 
 void Init_Serial_Ports(void);
 void Send_To_Midi_Out_Con(BYTE data_to_send);
 void Check_MIDI_Buff_Send_To_Z8_Board(Midi_In_Struct *midi_struct, BYTE end_index_local);
 void Send_To_Serial_Z8(BYTE data_to_send);
 void Send_Single_Framed_byte_To_Z8( BYTE SYSEX_ID2, BYTE byte_to_send);
 
 
#line 14 "bootloader.c" /0
 
  
#line 1 "hardware.h" /0
 
 
 
 
 
 
 
 void Init_Variables(void);
 void Init_IOs(void);
 void SyncToZ8Board(void);
 
 
 
#line 15 "bootloader.c" /0
 
  
#line 1 "usb.h" /0
 
 
 
 
 
 
 void Check_If_Usb_Attached(void);
 void Check_Usb(void);
 void Create_Send_USB_Frame_From_MIDI_Buffer( BYTE CN, Midi_In_Struct *midi_struct, BYTE end_index_local);
 void Create_Send_Realtime_USB_Frame_From_MIDI_Buffer( BYTE CN, BYTE realtime_data);
 void Ep2out_Function(void);
 
#line 16 "bootloader.c" /0
 
 
 volatile BYTE bootloader_state;
 bit must_program_z8;
 
 
 
 
 extern bit usb_state_from_ez0_bit;
 extern bit z8_board_fully_updated_from_ez0_bit;
 extern xdata Midi_In_Struct  MIDI_In_From_Con, MIDI_In_From_Usb, Serial_In_From_Z8;
 
 BYTE fw_bl_src;
 
 
 extern unsigned char code identity[];
 
 
 extern bit tx_to_serial_z8_busy;
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 void Check_Bootloader(void)
 {
 
 TD_Init();
 
 z8_board_fully_updated_from_ez0_bit = 1;
 
 
 
 bootloader_state = SEND_IDENTITY_SYSEX; 		
 
 
 SendIdentityBootLoader();
 
 must_program_z8 = 0;
 
 
 bootloader_state = WAIT_EZ_FW_DATA;				
 
 WaitNextBootLoaderState(bootloader_state);
 
 if(must_program_z8)
 {
 
  IOE |=  0x02;
 
 EZUSB_Delay(200);
 
 
 SCON1 = 0x40;		 
 
 
  IOE |= 0x01;
 
 
  IOE &= ~0x08;
 
 
 EZUSB_Delay(2000);
 
 
  IOE |= 0x08;
 
 
 
 
 
 Send_To_Serial_Z8(0x80);
 EZUSB_Delay1ms();
 
 
 Send_To_Serial_Z8(0x04);
 Send_To_Serial_Z8(0x81);
 EZUSB_Delay(100);
 
 InitFlashFrqAndUnlock();
 
 
 WriteSingleRegOCD(0x0FF9, 0x00);
 
 
 WriteSingleRegOCD(0x0FF8, 0x63);
 
 EZUSB_Delay(4000);
 
 InitFlashFrqAndUnlock();
 
 
 WaitNextBootLoaderState(bootloader_state);
 
 EZUSB_Delay(50);
 
 
 WriteSingleRegOCD(0x0FF8, 0x00);
 
 
  IOE   &=  ~0x01;
 
 
  IOE &=  ~0x02;
 }
 
 
 
 while(1)
 { 
 Check_Usb();
 Ep2out_Function();
 }
 }
 
 
 
 
 
 
 
 
 void SendIdentityBootLoader(void)
 {
 unsigned char i;
 
 
 Serial_In_From_Z8.end_index   = 6U + 4 + 11 + 1;
 
 AssignJlabsSysexHeader();
 
 
 Serial_In_From_Z8.buffer[6]   = FRAME_IS_COMMON_PARAM | (1<<6U) | (1<<5U);
 
 
 Serial_In_From_Z8.buffer[7]   = 11;
 
 Serial_In_From_Z8.buffer[8]   = 0x7F;
 
 Serial_In_From_Z8.buffer[9]   = 0;
 
 for(i=0; i<11 ; i++)
 Serial_In_From_Z8.buffer[10+i] = identity[i];
 
 Serial_In_From_Z8.buffer[21]   = 0xF7U;
 
 SendDispatchFwSysex();
 }
 
 
 
 
 
 
 
 void SendFwAcknoledge(unsigned char result)
 {
 
 
 Serial_In_From_Z8.end_index   = 6U + 1 + 1;
 
 AssignJlabsSysexHeader();
 
 
 Serial_In_From_Z8.buffer[6]   =  FRAME_IS_FIRMWARE_OR_PURE_MIDI | (1<<6U) | (1<<5U) | result;
 
 
 Serial_In_From_Z8.buffer[7]   = 0xF7U;
 
 SendDispatchFwSysex();
 
 }
 
 
 
 
 
 
 
 void AssignJlabsSysexHeader(void)
 {
 
 Serial_In_From_Z8.buffer[0]   = 0xF0U;
 Serial_In_From_Z8.buffer[1]   = 0x00U;
 Serial_In_From_Z8.buffer[2]   = 0x20U;
 Serial_In_From_Z8.buffer[3]   = 0x4FU;
 Serial_In_From_Z8.buffer[4]   = 0x00U;
 Serial_In_From_Z8.buffer[5]   = 0x01U;
 }
 
 
 
 
 
 
 
 void SendDispatchFwSysex(void)
 {
 
 BYTE start_index_local = 0;
 
 
 if(fw_bl_src != FW_BL_SRC_USB_IN)
 {
 while (start_index_local != Serial_In_From_Z8.end_index)
 {
 Send_To_Midi_Out_Con(Serial_In_From_Z8.buffer[start_index_local]);
 start_index_local++;  
 }
 }
 
 
 if(fw_bl_src  != FW_BL_SRC_MIDI_IN)
 if(usb_state_from_ez0_bit)
 {
 Serial_In_From_Z8.start_index 	= 0;
 
 Create_Send_USB_Frame_From_MIDI_Buffer(CN0, &Serial_In_From_Z8, Serial_In_From_Z8.end_index);
 }
 
 
 Serial_In_From_Z8.start_index 		= 0;
 Serial_In_From_Z8.end_index   		= 0;
 Serial_In_From_Z8.current_index		= 0;
 }
 
 
 
 
 
 
 
 void WaitNextBootLoaderState(unsigned char current_state)
 {
 
 do
 {
 Check_If_Usb_Attached();		
 Check_Usb();
 
 
 if(fw_bl_src != FW_BL_SRC_USB_IN)
 
 Check_MIDI_Buff_Send_To_Z8_Board(&MIDI_In_From_Con, MIDI_In_From_Con.end_index);
 
 
 if(fw_bl_src != FW_BL_SRC_MIDI_IN)	
 {
 
 if(usb_state_from_ez0_bit)
 {
 Check_MIDI_Buff_Send_To_Z8_Board(&MIDI_In_From_Usb, MIDI_In_From_Usb.end_index);
 Ep2out_Function();
 }
 }
 }
 while(bootloader_state == current_state);
 }
 
 
 
 
 
 
 
 void DecodeFirmwareSysex(Midi_In_Struct *midi_struct)
 {
 BYTE SYSXID0;
 WORD Address;
 BYTE i, j, tmp1, tmp2, tmp3, lenght_div_2;
 BYTE result = 0;
 bit must_send_ack = 0;
 BYTE fw_checksum = 0;
 
 SYSXID0 = midi_struct->buffer[6];
 
 
 
 if((SYSXID0 & (0x03U | (1<<4U))) == ( FRAME_IS_FIRMWARE_OR_PURE_MIDI | (1<<4U)))
 {
 must_send_ack = 1;
 
 
 if(SYSXID0 & (1<<2U))
 {
 
 bootloader_state++;
 result = (1<<4U);
 
 if(SYSXID0 & (1<<5U))
 {
 must_program_z8 = 1;
 
 
 }
 }
 else
 {
 
 
 Address  =  midi_struct->buffer[8]  << 14;
 Address += (midi_struct->buffer[9]  << 7);
 Address += (midi_struct->buffer[10]);
 
 
 fw_checksum = 0;
 j= 11;
 for(i=0; i < midi_struct->buffer[7]; i=i+2)
 {
 tmp1 = midi_struct->buffer[11+i];
 tmp2 = midi_struct->buffer[11+i+1];
 fw_checksum = (fw_checksum + tmp1) & 0x7F;
 fw_checksum = (fw_checksum + tmp2) & 0x7F;
 midi_struct->buffer[j] = ( (tmp1 << 4) + (tmp2 & 0x0F)) ^ 0x55;
 j++;
 }
 
 
 if(fw_checksum == midi_struct->buffer[11+i])
 {
 lenght_div_2 = midi_struct->buffer[7] >> 1;
 
 
 if(!(SYSXID0 & (1<<5U)))
 {
 if(Address < 16384)
 
 {
  EUSB = 0;
 CKCON |= 0x07;  
 
 result = EEPROMWriteVerify(Address, lenght_div_2, &midi_struct->buffer[11]);
 CKCON &= 0xF8;  
  EUSB = 1;
 }
 }
 
 else
 {
 
 result = (1<<4U);		
 
 
 
 Send_To_Serial_Z8(0x0A);
 
 Send_To_Serial_Z8(Address >> 8);
 
 Send_To_Serial_Z8(Address & 0xFF);
 
 Send_To_Serial_Z8(0x00);
 
 Send_To_Serial_Z8(lenght_div_2);
 
 for(i=0; i < lenght_div_2; i++)
 {
 Send_To_Serial_Z8(midi_struct->buffer[11+i]);
 }		
 
 
 while(tx_to_serial_z8_busy);
 
 
 EZUSB_Delay(50);  
 
 
 
 Send_To_Serial_Z8(0x0B);
 
 Send_To_Serial_Z8(Address >> 8);
 
 Send_To_Serial_Z8(Address & 0xFF);
 
 Send_To_Serial_Z8(0x00);
 
 Send_To_Serial_Z8(lenght_div_2);
 
 
 while(tx_to_serial_z8_busy);
 
 
 SCON1 |= 0x50;		 
 
 
 while(Serial_In_From_Z8.current_index < lenght_div_2);
 
 
 
 
 
 
 
 
 
 
 
 
 for(i=0; i < lenght_div_2; i++)
 {
 
 tmp3 = Serial_In_From_Z8.buffer[i];
 
 
 
 
 
 
 
 
 
 if(tmp3 != midi_struct->buffer[11+i])
 {
 
 result = 0;
 break;
 }
 }	
 
 
 SCON1 &= 0xEF;		 
 
 }  
 
 
 
 
 }  
 }  
 }  
 
 
 
 
 
 
 
 
 if(must_send_ack)
 SendFwAcknoledge(result);	
 
 }
 
 
 
 
 
 
 
 BYTE EEPROMWriteVerify(WORD addr, BYTE length, BYTE xdata *buf)
 {
 BYTE 		j;
 BYTE xdata 	ee_str[3];
 BYTE xdata  data_to_verify;
 
























 
 for(j=0;j<length;++j)
 {
 ee_str[0] = (BYTE)(((WORD)(addr) >> 8) & 0xff);	 
 ee_str[1] = (BYTE)((WORD)(addr) & 0xff);	 
 ee_str[2] = buf[j];		 
 
 EZUSB_WriteI2C((0x50 | 0x01), 3, ee_str);
 
 
 
 
 EZUSB_WaitForEEPROMWrite((0x50 | 0x01));
 
 
 
 EZUSB_WriteI2C((0x50 | 0x01), 2, ee_str);
 
 EZUSB_ReadI2C((0x50 | 0x01), 0x01, &data_to_verify); 
 
 
 if(data_to_verify != buf[j])
 return(0);
 addr++;
 }
 
 return((1<<4U));
 
 }
 
 
 
 
 
 
 
 void WriteSingleRegOCD(WORD address, BYTE value) 
 {
 Send_To_Serial_Z8(0x08);
 Send_To_Serial_Z8(address >> 8);
 Send_To_Serial_Z8(address & 0xFF);
 Send_To_Serial_Z8(1);
 Send_To_Serial_Z8(value);
 EZUSB_Delay(50);
 }
 
 
 
 
 
 
 
 void InitFlashFrqAndUnlock(void)
 {
 
 WriteSingleRegOCD(0x0FF8, 0x00);
 
 WriteSingleRegOCD(0x0FFA, (20000000ul / 1000) >> 8);
 WriteSingleRegOCD(0x0FFB , (20000000ul / 1000) & 0xFF);
 EZUSB_Delay(50);
 
 WriteSingleRegOCD(0x0FF8, 0x73);
 WriteSingleRegOCD(0x0FF8, 0x8C);
 EZUSB_Delay(50);
 }
