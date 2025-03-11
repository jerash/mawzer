#ifndef HARDWARE
#define HARDWARE


///////////////////////////////////////////////////////////
// Local definitions
///////////////////////////////////////////////////////////
#define PORT_ALT_FCT			2U
#define PORT_DIR				1U
#define TEN						0x80U	// timer enable bit mask

#define Z8_FREQ 20000000ul   	// 20.0 MHz
#define BRG0 	Z8_FREQ/((unsigned long)BAUD1*16)
 
#define TX_BUFF_LENGTH				128U

// TX_STATUS bits definitions
#define tx0_busy_txstat			(1<<6U)
#define tx0_full_txstat			(1<<5U)


///////////////////////////////////////////////////////////
// Function Prototypes
///////////////////////////////////////////////////////////
void GetModuleSubID();
void InitIos(void);
void InitTimers(void);
void ReinitTimer1(void);
void delay_usec(uchar);
void delay_msec(unsigned int);
void InitSerial(void);
void SendToTx0(uchar);
void InitInitialValues(void);

#endif