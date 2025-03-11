#ifndef IOS_ENC
#define IOS_ENC

///////////////////////////////////////////////////////////
// Local Definition
///////////////////////////////////////////////////////////
   
#define MODULE_PORT_A_DIR 			0x9B    	// Port A direction 1=in 0=out (A0, A1, A3 encoder digital in)
#define MODULE_PORT_A_ALT			0xF0  		// Port A alternate functions enable (i2c, serial 0)

#define MODULE_PORT_B_DIR 		 	0x13	   	// Port B direction 1=in 0=out (B0, B1 encoder digital in)
#define MODULE_PORT_B_ALT 	 		0x00    	// Port B alternate functions enable 1=in 0=out

#define MODULE_PORT_C_DIR 			0x07    	// Port C direction 1=in 0=out (C0, C1, C2 encoder digital in)
#define MODULE_PORT_C_DEFAULT	 	0X00     	// Default out values on port, cs2 is high

#endif
