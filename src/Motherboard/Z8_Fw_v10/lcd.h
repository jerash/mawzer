#ifndef LCD
#define LCD

///////////////////////////////////////////////////////////
// Local definitions
///////////////////////////////////////////////////////////
#define CS1_MAX_X 		63U 		// chip 1 x range 0 - 63 
#define CS2_MAX_X 		127U 	// chip 2 x range 64 - 127 
#define LCD_X_PIXEL_MAX	128U
#define LCD_Y_PAGE_MAX	8U

#define DISPLAY_ON 		0x3FU 	// LCD Display ON 
#define DISPLAY_OFF 	0x3EU 	// LCD Display OFF 
#define SET_Y 			0x40U 	// LCD SET Y command, add y - 0 to 63 (line) 
#define SET_X 			0xB8U 	// LCD SET X command, add x - 0 to 7  (page) 
#define SET_RAM_0 		0xC0U 	// LCD RAM, used for scrolling add y - 0 to 63 

// Bits Mask to access LCD commands

#define DIGITS_2			0x00U
#define DIGITS_3			(1<<0U)
#define LCD_COMMAND			0x00U
#define LCD_DATA			(1<<1U)

//#ifdef SUNLIKE_SG12864HBWBGBK
#define LCD_NORMAL			0x00U
#define LCD_INVERT			(1<<2U)
//#endif
//#ifdef MICROTIPS_MTGS12864KFGHSGW01 
//#define LCD_NORMAL			(1<<2)
//#define LCD_INVERT			0x00
//#endif

#define LCD_LEFT				0U
#define LCD_RIGHT				64U

#define BUSY_FLAG				0x80U

#define FILL_BLANK				0xFFU
#define FONT_START_OFFSET		4U // bytes used to format the font
#define BMP_START_OFFSET		2U // bytes used to format the bmp

//#define INIT_LCD_CHANNEL			(1<<0U)
#define INIT_LCD_MIDI_ACTIVITY		(1<<1U)
#define INIT_LCD_TEMPO				(1<<2U)
#define INIT_LCD_LIVE_SCR_TOP_FIXED (1<<3U)
#define INIT_LCD_GROUP				(1<<4U)
#define INIT_LCD_REALTIME_BMP		(1<<5U)
#define INIT_LCD_LIVE_SCR_BOT_FIXED (1<<6U)
#define INIT_LCD_USB				(1<<12U)
#define INIT_LCD_ALL_ZONES			0xFFFF

#define LCD_ZONE_MOD_FOUND_X		64U
#define LCD_ZONE_MOD_FOUND_Y		3U
#define LCD_ZONE_SEARCH_MOD_X	 	0U
#define LCD_ZONE_SEARCH_MOD_Y	 	6U
#define LCD_ZONE_FW_VERSION_X		66U
#define LCD_ZONE_FW_VERSION_Y		5U
#define LCD_ZONE_MIDI_ACTIVITY_X	17U
#define LCD_ZONE_MIDI_ACTIVITY_Y	0U
//#define LCD_ZONE_CHANNEL_X			2U
//#define LCD_ZONE_CHANNEL_Y			1U
#define LCD_ZONE_GROUP_X			37U
#define LCD_ZONE_GROUP_Y			0U
#define LCD_ZONE_GROUP_NAME_X		55U
#define LCD_ZONE_GROUP_NAME_Y		0U
#define LCD_ZONE_USB_X				1U
#define LCD_ZONE_USB_Y				0U
#define LCD_ZONE_TEMPO_X			55U
#define LCD_ZONE_TEMPO_Y			1U
#define LCD_ZONE_BPM_X				91U
#define LCD_ZONE_BPM_Y				1U
#define LCD_ZONE_VALUE_X			82U
#define LCD_ZONE_VALUE_Y			5U
#define LCD_ZONE_TRANSPORT_X		37U
#define LCD_ZONE_TRANSPORT_Y		1U

#define LCD_ZONE_INCRUST_X			0U
#define LCD_ZONE_INCRUST_Y			4U
#define LCD_ZONE_INCRUST_X_PIX		128U
#define LCD_ZONE_INCRUST_Y_PAGE		4U

#define LCD_ZONE_MENUS_BASE_X		10U
#define LCD_ZONE_MENUS_BASE_Y		2U

#define LCD_ZONE_EVENT_TYPE_X		56U
#define LCD_ZONE_EVENT_TYPE_Y		7U
#define LCD_ZONE_CHANNEL_MODULE_X	21U
#define LCD_ZONE_CHANNEL_MODULE_Y	5U
#define LCD_ZONE_CC_NB_X			40U
#define LCD_ZONE_CC_NB_Y			5U
#define LCD_ZONE_MODULE_NAME_X		0U
#define LCD_ZONE_MODULE_NAME_Y		4U
#define LCD_ZONE_HOOK_ARROW_X		119U
#define LCD_ZONE_HOOK_ARROW_Y		4U
#define LCD_ZONE_OW_ACTIVITY_X		41U
#define LCD_ZONE_OW_ACTIVITY_Y		7U
#define LCD_ZONE_STRING_X			63U
#define LCD_ZONE_STRING_Y			4U

///////////////////////////////////////////////////////////
// Function Prototypes
///////////////////////////////////////////////////////////
void DisplayInputOverflowIncrust(void);
void DisplayEmptyIncrust(void);
void Display_Invalid_Sysex_Incrust(void);
void Display_Invalid_Onewire_Serial_Incrust(void);
void DisplayInputOverflowIncrust(void);
void DisplayEmptyIncrust(void);
void DisplayRomVersion(uchar x, uchar y);
void rom_to_lcd_text_copy(rom uchar *src);
void UpdateLcdZones(int update_zones);
void DisplayMidiActivityBmp(void);
void DisplayCurrentValue(void);
void DisplayRealtimeBitmap(void);
void DisplayModuleName(uchar x, uchar y);
void UpdateLcdHookState(uchar next_hook_state);
void CreateStdLcdStr(uchar cc_nb);
void Display_Invalid_Midi_Struct_Incrust(void);
void DisplayModuleInfos(void);

#endif