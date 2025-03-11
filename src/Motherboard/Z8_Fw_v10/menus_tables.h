#ifndef MENUS_TABLES
#define MENUS_TABLES

#define ON_OFF_NB         2U
rom uchar OffOnTxtPtr[]   = {ON_OFF_NB,6,"OFF","ON"};

#define WIDGET_MODE_NB	  2U
rom uchar WidgetModesTxtPtr[]  = {WIDGET_MODE_NB,7,"JUMP","JUMPLESS"};

#define MTC_MODE_NB	  3U
rom uchar MtcModesTxtPtr[]  = {MTC_MODE_NB,18,24,"MIDI CLOCK+MMC","SMPTE","OFF"};

#define VIDEO_MODE_NB     2U
rom uchar VideoModeTxtPtr[] 	= { VIDEO_MODE_NB,9,"NORMAL","INVERT" };

#define AUTOASSIGN_NB     2U
rom uchar AutoAssignTxtPtr[] 	= { AUTOASSIGN_NB,9,"CANCEL","CONFIRM"};

// #define SMPTE_FRAME_RATE_NB	  4U
rom uchar SmpteFrameRateTxtPtr[]  = { SMPTE_FRAME_RATE_NB,10,16,23,"24FPS","25FPS","30FPSD","30FPS" };

#define EVENT_TYPE_ASSIGN_NB     2U
rom uchar EventTypeAssignTxtPtr[] 	= { EVENT_TYPE_ASSIGN_NB,5,"CC","NOTE"};

#define SWITCH_MODE_ASSIGN_NB     2U
rom uchar SwitchModeAssignTxtPtr[] 	= { SWITCH_MODE_ASSIGN_NB,7,"PUSH","TOGGLE"};

#define MIDI_EVENT_PARAM_NB		 2U	 // event type, event nb

////////////////////////// all menus and sub menus indexes ///////////////////////////////
enum {
// TOP LEVEL MENUS
SYSTEM_CONFIG_INDEX_MENU, MODULE_CONFIG_INDEX_MENU, MTC_CONFIG,
// SUB LEVEL MENUS (SYSTEM CONFIG)
MIDI_CHANNEL_INDEX_MENU, MIDI_THRU_INDEX_MENU, WIDGET_MODE_INDEX_MENU,
VIDEO_MODE_INDEX_MENU, PRESET_NAME_INDEX_MENU,
// SUB LEVEL MENUS (MODULE CONFIG)
MODULE_AUTOASSIGN_INDEX_MENU, MODULE_ASSIGN_INDEX_MENU, MODULE_NAME_INDEX_MENU,
// SUB LEVEL MENUS (MTC CONFIG)
MTC_MODE_INDEX_MENU, ENCODER_TEMPO_LOCK_INDEX_MENU, SMPTE_FRAME_RATE_INDEX_MENU,
PRESET_NAME_DUMMY_INDEX_ASSIGN,
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// MENUS ASSIGN TABLES INDEX ///////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
// TOP LEVEL MENUS
MIDI_EVENT_INDEX_ASSIGN, MIN_VALUE_INDEX_ASSIGN, MAX_VALUE_INDEX_ASSIGN,
LCD_STRING_INDEX_ASSIGN, SWITCH_MODE_INDEX_ASSIGN, MODULE_NAME_DUMMY_INDEX_ASSIGN,
// SUB LEVEL MENUS (MIDI EVENT)
EVENT_TYPE_INDEX_ASSIGN, // must be first index of manual assign
EVENT_NB_INDEX_ASSIGN, MIDI_CHANNEL_INDEX_ASSIGN,
LCD_STRING_DUMMY_INDEX_ASSIGN
};

#define MENUS_ASSIGN_FIRST_INDEX 	MIDI_EVENT_INDEX_ASSIGN

/////////////////////// THE WHOLE MENUS STRUCTURES DEFINITION //////////////////////////////////////
rom MENU menutable[] = {
      /////////////////////////////////////////////////////////////////////////////////////////////////////
      //////////// TOP LEVEL MENUS
      /////////////////////////////////////////////////////////////////////////////////////////////////////
      { // index 0 [CALL TO A SUBMENU]
        "SYSTEM CONFIG >",         	// Menu associated string
        TOP_LEVEL_MENU,         	// menu_depth           : the menu depth
        NULL,      					// value_max            : 12 sub menus from this global menu
        UNDEFINED_VALUE,        	// value_index          : No table index for a global menu
        SUB_MENU,               	// table_id             : ID of the table to modify
        NO_BIT,                 	// bit_pos_msk          : position of the bit to modify
        DISP_STRING,            	// display_type         : The value of this menu is a string type (sub menus names)
        NULL,                   	// MenuValueStr         : Dynamically filled
        MIDI_CHANNEL_INDEX_MENU,	// SubMenuBaseIndex     : index of the first submenu for this menu
        TRUE                    	// NextMenuExist        : TRUE if a next menu exist on same depth
      },
      { // index 0 [CALL TO A SUBMENU]
        "MODULE CONFIG >",         	// Menu associated string
        TOP_LEVEL_MENU,         	// menu_depth           : the menu depth
        NULL,      					// value_max            : 12 sub menus from this global menu
        UNDEFINED_VALUE,        	// value_index          : No table index for a global menu
        SUB_MENU,               	// table_id             : ID of the table to modify
        NO_BIT,                 	// bit_pos_msk          : position of the bit to modify
        DISP_STRING,            	// display_type         : The value of this menu is a string type (sub menus names)
        NULL,                   	// MenuValueStr         : Dynamically filled
        MODULE_AUTOASSIGN_INDEX_MENU,  		// SubMenuBaseIndex     : index of the first submenu for this menu
        TRUE                    	// NextMenuExist        : TRUE if a next menu exist on same depth
      },
      { // index 0 [CALL TO A SUBMENU]
        "MTC CONFIG >",         	// Menu associated string
        TOP_LEVEL_MENU,         	// menu_depth           : the menu depth
        NULL,      					// value_max            : 12 sub menus from this global menu
        UNDEFINED_VALUE,        	// value_index          : No table index for a global menu
        SUB_MENU,               	// table_id             : ID of the table to modify
        NO_BIT,                 	// bit_pos_msk          : position of the bit to modify
        DISP_STRING,            	// display_type         : The value of this menu is a string type (sub menus names)
        NULL,                   	// MenuValueStr         : Dynamically filled
        MTC_MODE_INDEX_MENU,   		// SubMenuBaseIndex     : index of the first submenu for this menu
        FALSE                    	// NextMenuExist        : TRUE if a next menu exist on same depth
      },
      /////////////////////////////////////////////////////////////////////////////////////////////////////
      //////////// MENUS LEVEL 1
      /////////////////////////////////////////////////////////////////////////////////////////////////////
      { // index 0 [CALL TO A SUBMENU]
        "MIDI CHANNEL >",         	// Menu associated string
        MENU_DEPTH_1,         		// menu_depth           : the menu depth
        CHANNEL_NB,      			// value_max            : 12 sub menus from this global menu
        CHANNEL_INDEX,        		// value_index          : No table index for a global menu
        TABLE_GLOBAL_PARAMS,       	// table_id             : ID of the table to modify
        NO_BIT,                 	// bit_pos_msk          : position of the bit to modify
        DISP_NUMERIC_PLUS_1,       	// display_type         : The value of this menu is a string type (sub menus names)
        NULL,                   	// MenuValueStr         : Dynamically filled
        UNDEFINED_VALUE,			// SubMenuBaseIndex     : index of the first submenu for this menu
        TRUE                    	// NextMenuExist        : TRUE if a next menu exist on same depth
      },
      { // index 0 [CALL TO A SUBMENU]
        "MIDI THRU >",         		// Menu associated string
        MENU_DEPTH_1,         		// menu_depth           : the menu depth
        ON_OFF_NB,      			// value_max            : 12 sub menus from this global menu
        CTRL_REG_0_7BITS_TO_EZ_INDEX,       		// value_index          : No table index for a global menu
        TABLE_GLOBAL_PARAMS,       	// table_id             : ID of the table to modify
        midi_thru_ez0,              // bit_pos_msk          : position of the bit to modify
        DISP_STRING,       			// display_type         : The value of this menu is a string type (sub menus names)
        OffOnTxtPtr,               	// MenuValueStr         : Dynamically filled
        UNDEFINED_VALUE,     		// SubMenuBaseIndex     : index of the first submenu for this menu
        TRUE                    	// NextMenuExist        : TRUE if a next menu exist on same depth
      },
      { // index 0 [CALL TO A SUBMENU]
        "WIDGET MODE >",         		// Menu associated string
        MENU_DEPTH_1,         		// menu_depth           : the menu depth
        WIDGET_MODE_NB,    			// value_max            : 12 sub menus from this global menu
        Z8_STATUS_0_NVM_INDEX, 		// value_index          : No table index for a global menu
        TABLE_GLOBAL_PARAMS,       	// table_id             : ID of the table to modify
        hook_zjump_mode_z0nvm,     	// bit_pos_msk          : position of the bit to modify
        DISP_STRING,       			// display_type         : The value of this menu is a string type (sub menus names)
        WidgetModesTxtPtr,         	// MenuValueStr         : Dynamically filled
        UNDEFINED_VALUE,     		// SubMenuBaseIndex     : index of the first submenu for this menu
        TRUE                    	// NextMenuExist        : TRUE if a next menu exist on same depth
      },
      { // index 0 [CALL TO A SUBMENU]
        "VIDEO MODE >",         		// Menu associated string
        MENU_DEPTH_1,         		// menu_depth           : the menu depth
        VIDEO_MODE_NB,    			// value_max            : 12 sub menus from this global menu
        Z8_STATUS_0_NVM_INDEX, 		// value_index          : No table index for a global menu
        TABLE_GLOBAL_PARAMS,       	// table_id             : ID of the table to modify
        new_video_mode_z0nvm,     	// bit_pos_msk          : position of the bit to modify
        DISP_STRING,       			// display_type         : The value of this menu is a string type (sub menus names)
        VideoModeTxtPtr,         	// MenuValueStr         : Dynamically filled
        UNDEFINED_VALUE,     		// SubMenuBaseIndex     : index of the first submenu for this menu
        TRUE                    	// NextMenuExist        : TRUE if a next menu exist on same depth
      },
      { // index 0 [CALL TO A SUBMENU]
        "PRESET NAME >",         		// Menu associated string
        MENU_DEPTH_1,         		// menu_depth           : the menu depth
        1,    					// value_max            : 12 sub menus from this global menu
        UNDEFINED_VALUE, 		// value_index          : No table index for a global menu
        SUB_MENU,       	// table_id             : ID of the table to modify
        NO_BIT,     	// bit_pos_msk          : position of the bit to modify
        DISP_STRING,       			// display_type         : The value of this menu is a string type (sub menus names)
        NULL,         	// MenuValueStr         : Dynamically filled
        PRESET_NAME_DUMMY_INDEX_ASSIGN,     		// SubMenuBaseIndex     : index of the first submenu for this menu
        FALSE                    	// NextMenuExist        : TRUE if a next menu exist on same depth
      },
     { // index 0 [CALL TO A SUBMENU]
        "AUTO ASSIGN >",         		// Menu associated string
        MENU_DEPTH_1,         		// menu_depth           : the menu depth
        AUTOASSIGN_NB,    			// value_max            : 12 sub menus from this global menu
        Z8_STATUS_1_NVM_INDEX, 		// value_index          : No table index for a global menu
        TABLE_GLOBAL_PARAMS,       	// table_id             : ID of the table to modify
        must_autoassign_z1nvm,     	// bit_pos_msk          : position of the bit to modify
        DISP_STRING,       			// display_type         : The value of this menu is a string type (sub menus names)
        AutoAssignTxtPtr,         	// MenuValueStr         : Dynamically filled
        UNDEFINED_VALUE,     		// SubMenuBaseIndex     : index of the first submenu for this menu
        TRUE                    	// NextMenuExist        : TRUE if a next menu exist on same depth
      },
     { // index 0 [CALL TO A SUBMENU]
        "MANUAL ASSIGN >",         		// Menu associated string
        MENU_DEPTH_1,         		// menu_depth           : the menu depth
        NULL,   		 			// value_max            : 12 sub menus from this global menu
        UNDEFINED_VALUE, 			// value_index          : No table index for a global menu
        SUB_MENU,       			// table_id             : ID of the table to modify
        NO_BIT,     				// bit_pos_msk          : position of the bit to modify
        DISP_STRING,       			// display_type         : The value of this menu is a string type (sub menus names)
        NULL,         				// MenuValueStr         : Dynamically filled
        MIDI_EVENT_INDEX_ASSIGN ,  	// SubMenuBaseIndex     : index of the first submenu for this menu
        TRUE                    	// NextMenuExist        : TRUE if a next menu exist on same depth
      },
     { // index 0 [CALL TO A SUBMENU]
        "MODULE NAME >",         		// Menu associated string
        MENU_DEPTH_1,         		// menu_depth           : the menu depth
        1,   		 				// value_max            : 12 sub menus from this global menu
        UNDEFINED_VALUE, 			// value_index          : No table index for a global menu
        SUB_MENU, 					// table_id             : ID of the table to modify
        NO_BIT,     				// bit_pos_msk          : position of the bit to modify
        DISP_STRING,    			// display_type         : The value of this menu is a string type (sub menus names)
        NULL,         				// MenuValueStr         : Dynamically filled
        MODULE_NAME_DUMMY_INDEX_ASSIGN ,  			// SubMenuBaseIndex     : index of the first submenu for this menu
        FALSE                    	// NextMenuExist        : TRUE if a next menu exist on same depth
      },
      { // index 0 [CALL TO A SUBMENU]
        "MTC MODE >",         		// Menu associated string
        MENU_DEPTH_1,         		// menu_depth           : the menu depth
        MTC_MODE_NB,    			// value_max            : 12 sub menus from this global menu
        Z8_STATUS_0_NVM_INDEX, 		// value_index          : No table index for a global menu
        TABLE_GLOBAL_PARAMS,       	// table_id             : ID of the table to modify
        MTC_MODE_MASK_BIT,     		// bit_pos_msk          : position of the bit to modify
        DISP_STRING,       			// display_type         : The value of this menu is a string type (sub menus names)
        MtcModesTxtPtr,         	// MenuValueStr         : Dynamically filled
        UNDEFINED_VALUE,     		// SubMenuBaseIndex     : index of the first submenu for this menu
        TRUE                    	// NextMenuExist        : TRUE if a next menu exist on same depth
      },
       { // index 0 [CALL TO A SUBMENU]
        "ENCODER LOCK >",      		// Menu associated string
        MENU_DEPTH_1,         		// menu_depth           : the menu depth
        ON_OFF_NB,    				// value_max            : 12 sub menus from this global menu
        Z8_STATUS_0_NVM_INDEX, 		// value_index          : No table index for a global menu
        TABLE_GLOBAL_PARAMS,       	// table_id             : ID of the table to modify
        encoder_tempo_lock_z0nvm,  	// bit_pos_msk          : position of the bit to modify
        DISP_STRING,       			// display_type         : The value of this menu is a string type (sub menus names)
        OffOnTxtPtr,         		// MenuValueStr         : Dynamically filled
        UNDEFINED_VALUE,     		// SubMenuBaseIndex     : index of the first submenu for this menu
        TRUE                    	// NextMenuExist        : TRUE if a next menu exist on same depth
      },
       { // index 0 [CALL TO A SUBMENU]
        "SMPTE RATE >",      		// Menu associated string
        MENU_DEPTH_1,         		// menu_depth           : the menu depth
        SMPTE_FRAME_RATE_NB,    	// value_max            : 12 sub menus from this global menu
        Z8_STATUS_1_NVM_INDEX, 		// value_index          : No table index for a global menu
        TABLE_GLOBAL_PARAMS,       	// table_id             : ID of the table to modify
        SMPTE_FRAME_RATE_MASK_BIT,  // bit_pos_msk          : position of the bit to modify
        DISP_STRING,       			// display_type         : The value of this menu is a string type (sub menus names)
        SmpteFrameRateTxtPtr,      	// MenuValueStr         : Dynamically filled
        UNDEFINED_VALUE,     		// SubMenuBaseIndex     : index of the first submenu for this menu
        FALSE                    	// NextMenuExist        : TRUE if a next menu exist on same depth
      },
     { // index 10
        // ASSIGN USER MODE SUB MENUS  1
        "EDIT NAME",      			// Menu associated string
        MENU_DEPTH_2,           	// menu_depth           : the menu depth
        GROUP_NAME_REAL_SIZE,   	// value_max            : 12 sub menus from this global menu
        0,							// value_index          : No table index for a global menu
        TABLE_PRESET_NAME, 			// table_id             : ID of the table to modify
        NO_BIT,     				// bit_pos_msk          : position of the bit to modify
        DISP_EDIT_STRING,    		// display_type         : The value of this menu is a string type (sub menus names)
        NULL,         				// MenuValueStr         : Dynamically filled
 	    UNDEFINED_VALUE,    		// SubMenuBaseIndex     : not used since no sub menu
        FALSE                   	// NextMenuExist        : TRUE if a next menu exist on same depth
      },
	  ///////////////////////////////////////////////////////////////////////////////////////////////////////////
	  ///////////////////////////// MENUS ASSIGN TABLES INDEX ///////////////////////////////////////////////////
	  ///////////////////////////////////////////////////////////////////////////////////////////////////////////
	  {
        // ASSIGN STD MODE SUB MENUS  0
        "MIDI CONFIG >",         	// Menu associated string
        MENU_DEPTH_2,          	 	// menu_depth           : the menu depth
        MIDI_EVENT_PARAM_NB, 		// value_max            : This global menu value is 0 - 3
        UNDEFINED_VALUE,     		// value_index          : current_channel is CtrlReg[2]
        SUB_MENU,	    			// table_id             : ID of the table to modify
        NO_BIT, 				  	// bit_pos_msk          : position of the bit to modify
        DISP_STRING,    			// display_type         : The value of this menu is a string type (sub menus names)
        NULL,  					    // MenuValueStr         : No value since numeric
        EVENT_TYPE_INDEX_ASSIGN,   	// SubMenuBaseIndex     : not used since no sub menu
        TRUE                    	// NextMenuExist        : TRUE if a next menu exist on same depth
      },
 	  { // index 10
        // ASSIGN USER MODE SUB MENUS 3
        "MIN VALUE >",         	// Menu associated string
        MENU_DEPTH_2,           // menu_depth           : the menu depth
        128, 					// value_max            : This global menu value is 0 - 3
        ctrl_value_min_index,   // value_index          : current_channel is CtrlReg[2]
        TABLE_CTRL_ASSIGN,	    // table_id             : ID of the table to modify
        NO_BIT,       			// bit_pos_msk          : position of the bit to modify
        DISP_NUMERIC,    		// display_type         : The value of this menu is a string type (sub menus names)
        NULL, 			        // MenuValueStr         : No value since numeric
        UNDEFINED_VALUE,        // SubMenuBaseIndex     : not used since no sub menu
        TRUE                    // NextMenuExist        : TRUE if a next menu exist on same depth
      },
      { // index 10
        // ASSIGN USER MODE SUB MENUS 4
        "MAX VALUE >",         	// Menu associated string
        MENU_DEPTH_2,           // menu_depth           : the menu depth
        128, 					// value_max            : This global menu value is 0 - 3
        ctrl_value_max_index,   // value_index          : current_channel is CtrlReg[2]
        TABLE_CTRL_ASSIGN,	    // table_id             : ID of the table to modify
        NO_BIT,       			// bit_pos_msk          : position of the bit to modify
        DISP_NUMERIC,    		// display_type         : The value of this menu is a string type (sub menus names)
        NULL, 			        // MenuValueStr         : No value since numeric
        UNDEFINED_VALUE,        // SubMenuBaseIndex     : not used since no sub menu
        TRUE                   // NextMenuExist        : TRUE if a next menu exist on same depth
      },
      { // index 10
        // ASSIGN USER MODE SUB MENUS  1
        "EDIT NAME >",      	// Menu associated string
        MENU_DEPTH_2,           // menu_depth           : the menu depth
        1,   		 				// value_max            : 12 sub menus from this global menu
        UNDEFINED_VALUE, 			// value_index          : No table index for a global menu
        SUB_MENU, 					// table_id             : ID of the table to modify
        NO_BIT,     				// bit_pos_msk          : position of the bit to modify
        DISP_STRING,    			// display_type         : The value of this menu is a string type (sub menus names)
        NULL,         				// MenuValueStr         : Dynamically filled
        LCD_STRING_DUMMY_INDEX_ASSIGN , 	// SubMenuBaseIndex     : index of the first submenu for this menu
        TRUE                    	// NextMenuExist        : TRUE if a next menu exist on same depth
      },
      { // index 10
        // ASSIGN USER MODE SUB MENUS  1
        "BUTTON MODE >",      	// Menu associated string
        MENU_DEPTH_2,           // menu_depth           : the menu depth
        SWITCH_MODE_ASSIGN_NB,  // value_max            : This global menu value is 0 - 3
        ctrl_status_1_index, 	// value_index          : table index to modify is 0
        TABLE_CTRL_ASSIGN,      // table_id             : ID of the table to modify
        switch_ctrl_mode,  		// bit_pos_msk          : position of the bit to modify
        DISP_STRING,            // display_type         : The value of this menu is a string type (sub menus names)
        SwitchModeAssignTxtPtr,        // MenuValueStr         : string values are sine/square/random/triangle
        UNDEFINED_VALUE,    	// SubMenuBaseIndex     : not used since no sub menu
        FALSE                   // NextMenuExist        : TRUE if a next menu exist on same depth
      },
      { // index 10
        // ASSIGN USER MODE SUB MENUS  1
        "EDIT NAME",      			// Menu associated string
        MENU_DEPTH_2,           	// menu_depth           : the menu depth
        MODULE_NAME_LENGHT,   		// value_max            : 12 sub menus from this global menu
        UNDEFINED_VALUE,			// value_index          : No table index for a global menu
        TABLE_MODULE_NAME, 			// table_id             : ID of the table to modify
        NO_BIT,     				// bit_pos_msk          : position of the bit to modify
        DISP_EDIT_STRING,    		// display_type         : The value of this menu is a string type (sub menus names)
        NULL,         				// MenuValueStr         : Dynamically filled
 	    UNDEFINED_VALUE,    		// SubMenuBaseIndex     : not used since no sub menu
        FALSE                   	// NextMenuExist        : TRUE if a next menu exist on same depth
      },
	  {
        // ASSIGN STD MODE SUB MENUS  0
        "MIDI EVENT >",         	// Menu associated string
        MENU_DEPTH_3,           // menu_depth           : the menu depth
        EVENT_TYPE_ASSIGN_NB, 	// value_max            : This global menu value is 0 - 3
        message_type_index,     // value_index          : current_channel is CtrlReg[2]
        TABLE_CTRL_ASSIGN,	    // table_id             : ID of the table to modify
        NO_BIT, 			    // bit_pos_msk          : position of the bit to modify
        DISP_STRING,    		// display_type         : The value of this menu is a string type (sub menus names)
        EventTypeAssignTxtPtr,  // MenuValueStr         : No value since numeric
        UNDEFINED_VALUE,        // SubMenuBaseIndex     : not used since no sub menu
        TRUE                    // NextMenuExist        : TRUE if a next menu exist on same depth
      },
      { // index 10
        // ASSIGN STD MODE SUB MENUS  1
        "EVENT NB >",         	// Menu associated string // V1.5 numero
        MENU_DEPTH_3,           // menu_depth           : the menu depth
        128,			        // value_max            : This global menu value is 0 - 3
        message_nb_index,    	// value_index          : table index to modify is 0
        TABLE_CTRL_ASSIGN,      // table_id             : ID of the table to modify
        NO_BIT,       			 // bit_pos_msk          : position of the bit to modify
        DISP_NUMERIC,           // display_type         : The value of this menu is a string type (sub menus names)
        NULL,                   // MenuValueStr         : string values are sine/square/random/triangle
        UNDEFINED_VALUE,    	// SubMenuBaseIndex     : not used since no sub menu
        TRUE                   // NextMenuExist        : TRUE if a next menu exist on same depth
      },
      { // index 0 [CALL TO A SUBMENU]
        "MIDI CHANNEL >",         	// Menu associated string
        MENU_DEPTH_3,         		// menu_depth           : the menu depth
        CHANNEL_NB,      			// value_max            : 12 sub menus from this global menu
        midi_channel_index,        	// value_index          : No table index for a global menu
        TABLE_CTRL_ASSIGN,       	// table_id             : ID of the table to modify
        NO_BIT,                 	// bit_pos_msk          : position of the bit to modify
        DISP_NUMERIC_PLUS_1,       	// display_type         : The value of this menu is a string type (sub menus names)
        NULL,                   	// MenuValueStr         : Dynamically filled
        UNDEFINED_VALUE,			// SubMenuBaseIndex     : index of the first submenu for this menu
        FALSE                    	// NextMenuExist        : TRUE if a next menu exist on same depth
      },
      { // index 10
        // ASSIGN USER MODE SUB MENUS  1
        "EDIT NAME",      			// Menu associated string
        MENU_DEPTH_3,           	// menu_depth           : the menu depth
        LCD_STR_MAX_LENGHT,   		// value_max            : 12 sub menus from this global menu
        lcd_string_index,			// value_index          : No table index for a global menu
        TABLE_CTRL_ASSIGN, 			// table_id             : ID of the table to modify
        NO_BIT,     				// bit_pos_msk          : position of the bit to modify
        DISP_EDIT_STRING,    		// display_type         : The value of this menu is a string type (sub menus names)
        NULL,         				// MenuValueStr         : Dynamically filled
 	    UNDEFINED_VALUE,    		// SubMenuBaseIndex     : not used since no sub menu
        FALSE                   	// NextMenuExist        : TRUE if a next menu exist on same depth
      }
  };

#endif
