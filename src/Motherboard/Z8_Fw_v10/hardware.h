#ifndef HARDWARE
#define HARDWARE


///////////////////////////////////////////////////////////
// Local definitions
///////////////////////////////////////////////////////////
#define PORT_ALT_FCT				2U
#define PORT_DIR					1U
#define TEN							0x80U	// timer enable bit mask

#define CTRL_UNDEF					0xFFU
#define GENERIC_BUFF_SIZE			128U	// V1.5 128U

#define MIDI_ACTIVITY_PERSISTENCY	5U	// bitmap persictency in multiple of 10ms
#define BUTTON_DEBOUNCE_CPT			15U	// button debounce counter in multiple of 10ms


///////////////////////////////////////////////////////////
// Function Prototypes
///////////////////////////////////////////////////////////
void InitIos(void);
void InitTimers(void);
void ReinitTimer1(void);
void ReinitTimer2(void);
void InitInitialValues(void);
void delay_usec(uchar delay_time);
void delay_msec(unsigned int delay_time);
void CheckParamChangedFromItr(void);
void CheckCtrlRegToSendToEz(void);
void CheckCtrlRegToSave5Sec(void);
void InitParamStructsInRam(void);
void SyncToEzBoard(void);

#endif