#ifndef _SETUP_MENU_H
#define _SETUP_MENU_H

typedef enum{
EXECUTE_MODE,
READ_VALUE_MODE,
WRITE_VALUE_MODE,
LOAD_DEFAULT_MODE,
INQUIRE_RESTRICT	//Inquire whether the item is restricted, only the item of RESTRICT-ITEM 
					//If restricted, return the ordinal of option that should show,
					//else return 0
}FUNC_MODE;

typedef enum{
	DUMMY_ITEM_TYPE,	// show bank
	GOTO_PAGE_TYPE,	// pointer to another menu page
	GENERAL_ITEM_TYPE,	//general with option list
	NONE_OPTION_ITEM_TYPE,	//none option, eg. reset
	STRING_ITEM_TYPE,	//only show a string
	DELAY_DISTANCE_ITEM_TYPE,
	RESTRICTED_ITEM_TYPE,	//2Use the last option to save the string show when restricted.
						//2And the option number of item euals array's number sub 1. 
	AV_DELAY_ITEM_TYPE,
	VOLUME_ITEM_TYPE,
//	IMMEDIATE_EXEC_ITEM_TYPE,
	BALANCE_ITEM_TYPE,
	PASSWORD_ITEM_TYPE,
}ITEM_TYPE;

typedef enum{
	OPTION_TYPE,
	NO_KEY_TYPE
}PAGE_TYPE;

typedef enum{
	MAIN_PAGE,	/*0*/
	AUTO_SURROUND_PAGE,	/*1*/
	TONE_PAGE,	/*2*/
	
	//SPEAKER_PAGE,	/*3*/
	LISTENPROFILE_PAGE, /*3*/  /*add cjm 2007-07-12*/
	LIS_PRO_MOVIE_PAGE, /*4*/ /*add cjm 2007-07-12*/
	LIS_PRO_MUSIC_PAGE, /*5*/ /*add cjm 2007-07-12*/
	
	SPK_CONFIG_PAGE,		/*6*/
	SPK_DISTANCE_PAGE,	/*7*/
	SPK_BALANCE_PAGE,		/*8*/
	SPK_CROSSOVER_PAGE,	/*9*/
	SPK_EQ_PAGE,			/*10*/
	SPK_PHASE_PAGE,		/*11*/

	SPK_CONFIG_PAGE_MUSIC,	  /*12*/ /*add cjm 2007-07-12*/
	SPK_DISTANCE_PAGE_MUSIC,	  /*13*/ /*add cjm 2007-07-12*/
	SPK_BALANCE_PAGE_MUSIC,	  /*14*/ /*add cjm 2007-07-12*/
	SPK_CROSSOVER_PAGE_MUSIC, /*15*/ 	/*add cjm 2007-07-12*/
	SPK_EQ_PAGE_MUSIC,		  /*16*/ /*add cjm 2007-07-12*/
	SPK_PHASE_PAGE_MUSIC,	  /*17*/ /*add cjm 2007-07-12*/
	SPK_PANORAMA_PAGE_MUSIC, /*18*/ /*add cjm 2007-07-12*/

	INPUT_PAGE,				/*19*/
	ASSIGNED_INPUT_PAGE,		/*20*/  /*add cjm 2007-07-18*/
	CHANGE_NAME_PAGE,		/*21*/
	SET_ANALOG_IN_PAGE,		/*22*/ /*change by cjm 2009-03-30*/
	SET_ANALOG_LEVEL_PAGE,	/*23*/ /*change by cjm 2009-03-30*/
	SET_HDMI_IN_AUDIO,		/*24*/

	RADIO_PAGE,				/*25*/

	ZONE2_PAGE,				/*26*/

	CROSSOVER_PAGE,			/*27*/
	DELAY_PAGE,				/*28*/
	AV_SYN_PAGE,				/*29*/
	BLANCE_PAGE,				/*30*/
	OTHER_PAGE,				/*31*/
	SIGNAL_PAGE,				/*32*/
	LOGO_PAGE,					/*33*/
	PASSWORD_PAGE,			/*34*/
}PAGE_ID;

/*
  If there enough Ram, Then we can define all the function with two parameter bMode, bValue and needn't to change the funtion body
  */
typedef SETUP_T (*SETUPFUNC)(SETUP_MODE_T, SETUP_VALUE_T, SETUP_VALUE_T);

typedef struct{
	unsigned char uValue;		/*zlc 0112 add*/
	char  *pCap;		
}MENU_OPTION, * pMENU_OPTION;

typedef struct{
	char* pCap;
	unsigned char uType;
				/* possible value: 
				GENERAL_ITEM_TYPE : general with option list 
				GOTO_PAGE_TYPE     : pointer to another menu page.  
				DUMMY_ITEM_TYPE    : dummy, for displaying blanks. 
				STRING_BAR             : Show string only.
				*/ 
	unsigned char uOptionNum;	/*zlc 0112 change*/
	MENU_OPTION * pOptionArray;
	unsigned char uPara;
	SETUPFUNC pAction;
}MENU_ITEM, *pMENU_ITEM;

typedef struct{
	char* pCap;
	char * input;
	SETUPFUNC pAction;
}MENU_INPUT_ITEM, *pMENU_INPUT_ITEM;

typedef struct{
	char *pCap;
	unsigned char uID;
	unsigned char  uType;
	unsigned char uItemNum;
	MENU_ITEM * pItemArray;
}MENU_PAGE, *pMENU_PAGE;
//<
////////////////////////////////////////////////////////////////////////////////

#define PASSWORD_LENGTH 4
////////////////////////////////////////////////////////////////////////////////
//>                             Global variables
#define MENUPAGELIST_SIZE 		27 	/*add cjm 2007-07-12*/
extern const MENU_PAGE MenuPageList[MENUPAGELIST_SIZE];

extern const float balance_table[];
extern const short volume_table[];
extern const short AnalogInLevelTable[];
//<
////////////////////////////////////////////////////////////////////////////////
extern char strOff[];
////////////////////////////////////////////////////////////////////////////////
//>                             Functions declaration
SETUP_T SetupExitMenu(SETUP_MODE_T bMode, SETUP_VALUE_T bValue, SETUP_VALUE_T resValue);
SETUP_T SetupMenuDisplay(SETUP_MODE_T bMode, SETUP_VALUE_T bValue, SETUP_VALUE_T resValue);

MENU_PAGE* MenuFindPage(unsigned char uID);

extern void SetupInit(void);
extern KEYT SetupKeyService(KEYT key);
extern void setupload_memroy1(uchar memory_mode);
//<
////////////////////////////////////////////////////////////////////////////////
#endif

