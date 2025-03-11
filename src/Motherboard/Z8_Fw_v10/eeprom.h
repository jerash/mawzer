#ifndef EEPROM
#define EEPROM

///////////////////////////////////////////////////////////
// Local Definition
///////////////////////////////////////////////////////////
#define EEPROM_I2C_ADR_POS				1U
#define EEPROM_ZERO_I2C_ADR				(0x00U)		// EEprom 0 I2C adress
#define EEPROM_ONE_I2C_ADR				(0x01U<<1U)	// EEprom 1 I2C adress
#define EEPROM_TWO_I2C_ADR				(0x02U<<1U)	// EEprom 2 I2C adress 

#define EEPROM_ZERO_FULL_SIZE			32768	// Contains control reg + scene + motion + misc
#define EEPROM_ONE_FULL_SIZE			32768	// Contains both MIDI and LCD strings + structures
#define EEPROM_TWO_FULL_SIZE			32768	// Contains both MIDI and LCD strings + structures 
#define EEPROM_ONE_TWO_FULL_SIZE		(EEPROM_ONE_FULL_SIZE + EEPROM_TWO_FULL_SIZE)	

#define EEPROM_PAGE_SIZE				64U		// Page size is 64 bytes
#define I2C_SLAVE_ADR_EE				0xA0U	// I2C EEPROM control Write Address
#define GROUP_NAME_EE_LENGHT			16U		// group name are 16 bytes strings in EEPROM
#define GROUP_NAME_REAL_SIZE			10U

///////////////////////////////////// EEPROM CONTROL MAPPING //////////////////////////////////////////////////////// 

// ******************************************************************************************************************
// ************************************************** EEPROM 0 ******************************************************
// ******************************************************************************************************************

//////////////////////////////////////////  GROUP NAMES MAPPING ////////////////////////////////////////////////

// Group names are located in eeprom 0 at the beginning
#define GROUP_NAME_BASE_EE_ADR				 0
// check if coherent :
#if (GROUP_NAME_BASE_EE_ADR % 64)
	#error _GROUP_NAME_BASE_EE_ADR_ is not aligned on a eeprom page size
#endif

#define GROUP_NAME_END_EE_ADR				(GROUP_NAME_BASE_EE_ADR + (GROUP_NB * GROUP_NAME_EE_LENGHT))

// Control and status registers are located in first eeprom after group names realigned on eeprom page size
// Group names are 16 byte lenght : the next mapping is control regs which MUST be alligned on a eeprom page
// Depending on group nb : the first address of control regs must be offseted
#if (GROUP_NAME_END_EE_ADR % 64)
	#define CTRL_REG_OFFSET_TO_ADD			(EEPROM_PAGE_SIZE-(GROUP_NAME_END_EE_ADR % 64))
#else
	#define CTRL_REG_OFFSET_TO_ADD			0
#endif

/////////////////////////////////  CONTROL REGISTERS MAPPING /////////////////////////////////////////////////
// Control and status registers are located in first eeprom after group names realigned on eeprom page size
// Group names are 16 byte lenght : the next mapping is control regs which MUST be alligned on a eeprom page
// Depending on group nb : the first address of control regs must be offseted

#define CTRL_REG_START_EE_ADR				 (GROUP_NAME_END_EE_ADR + CTRL_REG_OFFSET_TO_ADD)

// check if coherent :
#if (CTRL_REG_START_EE_ADR % 64)
	#error CTRL_REG_START_EE_ADR is not aligned on a eeprom page size
#endif

// start with all control registers
// the first ones must be sent to EZ board at startup and on change
#define CTRL_REG_0_7BITS_TO_EZ_EE_ADR		(CTRL_REG_START_EE_ADR)
#define CTRL_REG_1_7BITS_TO_EZ_EE_ADR		(CTRL_REG_START_EE_ADR + 1)
#define LAST_CTRL_REG_7BITS_TO_EZ_EE_ADR	(CTRL_REG_START_EE_ADR + 2)
// the followings are only used by Z8 board
#define Z8_STATUS_0_EE_ADR					(CTRL_REG_START_EE_ADR + 3)
#define Z8_STATUS_1_EE_ADR					(CTRL_REG_START_EE_ADR + 4)
#define MIDI_CHANNEL_EE_ADR					(CTRL_REG_START_EE_ADR + 5)
#define GROUP_EE_ADR						(CTRL_REG_START_EE_ADR + 6)
#define INTERNAL_TEMPO_EE_ADR				(CTRL_REG_START_EE_ADR + 7)
// end of control regs
#define CTRL_REG_REAL_END_EE_ADR			(INTERNAL_TEMPO_EE_ADR + 1)
#define CTRL_REG_REAL_NB					(CTRL_REG_REAL_END_EE_ADR - CTRL_REG_START_EE_ADR) // 3
#define CTRL_REG_PAGE_NB					1U	// Control registers use 1 page total
#define CTRL_REG_END_EE_ADR					(CTRL_REG_START_EE_ADR + (CTRL_REG_PAGE_NB * EEPROM_PAGE_SIZE))	// 64

// check if coherent :
#if (CTRL_REG_REAL_NB >= 64)
	#error _CTRL_REG_REAL_NB_ is higher than 64
#endif

#if (CTRL_REG_END_EE_ADR % 64)
	#error _CTRL_REG_END_EE_ADR_ is not aligned on a eeprom page size
#endif




// ******************************************************************************************************************
// ************************************************** EEPROM 1 ******************************************************
// ******************************************************************************************************************

// All MIDI and LCD strings are located in 2 last EEPROMs of 32 KB each, 
// >> Use a linear 16b address, when address > 32768, auto select second EEPROM
#define MIDI_STR_EE_BASE_ADR				 0
// total size for group 0
#define MIDI_STR_GROUP_0_EE_END_ADR			(MODULES_MAX_NB * CTRL_MAX_NB_PER_MODULE * CTRL_STRUCT_TOTAL_SIZE)
#define MIDI_STR_EE_LENGHT_PER_GROUP		(MIDI_STR_GROUP_0_EE_END_ADR - MIDI_STR_EE_BASE_ADR)

#define STRUCT_EE_LGT_PER_GRP_IN_MODUL		(CTRL_MAX_NB_PER_MODULE * CTRL_STRUCT_TOTAL_SIZE)

// ******************************************************************************************************************
// ************************************************** EEPROM 2 ******************************************************
// ******************************************************************************************************************


///////////////////////////////////////////////////////////
// Function Prototypes
///////////////////////////////////////////////////////////


void LockEEprom10Ms(void);
void GetCtrlStructEEaddFromIndexGroup(void);

#endif