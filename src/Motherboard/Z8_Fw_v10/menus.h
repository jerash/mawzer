#ifndef MENUS
#define MENUS

///////////////////////////////////////////////////////////
// Local Definition
///////////////////////////////////////////////////////////

// Nb of menus lines to display on LCD
#define MENUS_MAX_DEPTH         4U
#define UNDEFINED_VALUE         255U
#define NO_BIT                  255U
#define MUST_TOGGLE_SINGLE_BIT  255U

#define MIDI_OUTS_NB    		2U // out1,2, usb out
#define MERGER_OUTS_NB  		2U // out1, usb out
#define MIDI_INS_NB     		2U // in, usb

#define MENU_MAX_VALUES			5U

// Display value type
enum {DISP_NUMERIC, DISP_NUMERIC_PLUS_1, DISP_STRING, DISP_NUMERIC_PLUS_30, DISP_EDIT_STRING};
enum {READ_REG_VALUE, WRITE_REG_VALUE};

// different menus depht
enum {TOP_LEVEL_MENU, MENU_DEPTH_1, MENU_DEPTH_2, MENU_DEPTH_3 };
// Param of key pressed
enum {UP_PRESSED, DOWN_PRESSED};
// Action associated to Up & Down keys
enum {CHANGE_MENU_INDEX, CHANGE_MENU_VALUE};
// The different tables IDs affected by menus
enum {SUB_MENU, TABLE_GLOBAL_PARAMS, TABLE_CTRL_ASSIGN, TABLE_MODULE_NAME, TABLE_PRESET_NAME};

/////////////////////// THE GENERIC MENU STRUCTURE //////////////////////////////////////
typedef struct {
	rom uchar MenuStr[16U];    		// Menu name to display on LCD
    uchar menu_depth;               // the menu depth level
	uchar value_max;				// max value for current variable to modify (nb of submenus if submenu)
	uchar value_index;      		// index into table of registers to modify (all var to modify are into a table)
    uchar table_id;                 // id on the table to modify
    uchar bit_pos_msk;              // Bit(s) to modify on byte value_index of table_id (mask) 
	uchar display_type;				// DISP_STR, DISP_VAL, DIPS_VAL_1 (how to display the value)
	rom uchar *MenuValueStr;		// String to be displayed instead of the numerical value, if display_type = DISP_STR
	uchar SubMenuBaseIndex;			// index of the first submenu for this menu
    uchar NextMenuExist;            // TRUE if a next menu exist on same depth
} MENU;



///////////////////////////////////////////////////////////
// Function Prototypes
///////////////////////////////////////////////////////////
void 	CheckMenuSwitches(void);
void 	MenuEnter(void);
void 	DisplayMenus(void);
uchar 	GetBitPositionFromMask(uchar bits_mask, uchar action);
uchar 	MenuGetValue(void);
void 	MenuSetValue(uchar next_value);
void 	DisplayMenuValue(void);
void 	ButtonCancelClick(void);
void 	ButtonOkClick(void);
void 	ChangeCurrentMenu(uchar direction);
void 	ExitMenusMode(void);
void 	GetFirstMenuIndex(void);
void 	GetLastMenuIndex(void);


#endif