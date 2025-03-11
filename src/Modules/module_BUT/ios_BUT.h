#ifndef IOS_BUT
#define IOS_BUT

///////////////////////////////////////////////////////////
// Local Definition
///////////////////////////////////////////////////////////
   
#define MODULE_PORT_A_DIR 			0x92    	// Port A direction 1=in 0=out (A1 sw in, A0 A3 led out)  
#define MODULE_PORT_A_ALT			0xF0  		// Port A alternate functions enable (i2c, serial 0)

#define MODULE_PORT_B_DIR 		 	0x02	   	// Port B direction 1=in 0=out (B0 led out, B1 sw in)
#define MODULE_PORT_B_ALT 	 		0x00    	// Port B alternate functions enable 1=in 0=out

#define MODULE_PORT_C_DIR 			0x06    	// Port C direction 1=in 0=out (C0 led out, C1 C2 sw in)
#define MODULE_PORT_C_DEFAULT	 	0X00     	// Default out values on port, cs2 is high

#endif
