#ifndef IOS_FAD
#define IOS_FAD

///////////////////////////////////////////////////////////
// Local Definition
///////////////////////////////////////////////////////////
   
#define MODULE_PORT_A_DIR 			0x90    	// Port A direction 1=in 0=out
#define MODULE_PORT_A_ALT			0xF0  		// Port A alternate functions enable (i2c, serial 0)
//#define  MODULE_PORT_A_DEFAULT  0X38      	// Default out values on port, cs1 is high, rw low, e low

#define MODULE_PORT_B_DIR 		 	0x10	   	// Port B direction 1=in 0=out
#define MODULE_PORT_B_ALT 	 		0x01    	// Port B alternate functions enable 1=in 0=out

#define MODULE_PORT_C_DIR 			0x07    	// Port C direction 1=in 0=out
#define MODULE_PORT_C_DEFAULT	 	0X00     	// Default out values on port, cs2 is high

#endif
