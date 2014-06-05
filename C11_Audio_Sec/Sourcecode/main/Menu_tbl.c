#include "macro.h"

#include "setup_menu.h"
#include "setup_func.h"
#include "task.h"

#include "..\cs495300\dspdrv.h"

#include "..\driver\channel.h"
#include "..\driver\eeprom.h"
 
char strFl[] = "FL";
char strCen[] = "CEN";
char strFr[] = "FR";
char strSr[] = "SR";
char strSbr[] = "SBR";
char strSbl[] = "SBL";
char strSl[] = "SL";
char strSw[] = "SW";

char strListenProfile[] = "LISTENING PROFILES"; 
char strInput[] = "INPUTS";
char strZone2[] = "ZONE 2";
char strOther[] = "ADVANCED SETTINGS";  

char strOn[] = "On";
char strOff[] = "Off";

char strBack[] = "BACK";

char strGotoMain[] = "GOTO MAIN MENU";

char strAssignedInput[]="Assigned Inputs";
char strChangeInputNames[] = "Change input names";
char strSetAnalogInLevel[] = "Set Analog In Level";
char strSetAnalogInConfig[] = "Set Analog In Config";
char strHDMIinAudio[]="Set HDMI in audio";

const short volume_table[MAX_VOLUME] = {
		-90,		-89,		-88,		-87,		-86,		-85,		-84,		-83,		-82,		-81,	//0..9
		-80,		-79,		-78,		-77,		-76,		-75,		-74,		-73,		-72,		-71,	//10..19
		-70,		-69,		-68,		-67,		-66,		-65,		-64,		-63,		-62,		-61,	//20..29
		-60,		-59,		-58,		-57,		-56,		-55,		-54,		-53,		-52,		-51,	//30..39
		-50,		-49,		-48,		-47,		-46,		-45,		-44,		-43,		-42,		-41,	//40..49
		-40,		-39,		-38,		-37,		-36,		-35,		-34,		-33,		-32,		-31,	//50..59
		-30,		-29,		-28,		-27,		-26,		-25,		-24,		-23,		-22,		-21,	//60..69
		-20,		-19,		-18,		-17,		-16,		-15,		-14,		-13,		-12,		-11,	//70..79
		-10,		-9,  		-8,  		-7,  		-6,  		-5,  		-4,  		-3,  		-2,  		-1,  	//80..89
		   0,  	 1,		  2,		  3,		  4,		   5,		  6,		  7,		  8,		  9,	//90..99
		 10,		11,		 12,		 13,		 14,		  15,									//100..105
};

const float balance_table[] = 
{
		-15.0,		-14.5,		-14.0,		-13.5,		-13.0,	//0..4
		-12.5,		-12.0,		-11.5,		-11.0,		-10.5,	//5..9
		-10.0,		-9.5,		-9.0,		-8.5,		-8.0,	//10..14
		-7.5,		-7.0,		-6.5,		-6.0,		-5.5,	//15..19
		-5.0,		-4.5,		-4.0,		-3.5,		-3.0,	//20..24
		-2.5,		-2.0,		-1.5,		-1.0,		-0.5,	//25..29
		0,			0.5,			1.0,			1.5,			2.0,		//30..34
		2.5, 			3.0,			3.5,			4.0,			4.5,		//35..39
		5.0,			5.5,			6.0,			6.5,			7.0,		//40..44
		7.5,			8.0,			8.5,			9.0,			9.5,		//45..49
		10.0,		10.5,		11.0,		11.5,		12.0,	//50..54
		12.5,		13.0,		13.5,		14.0,		14.5,	//55..59
		15.0,													//60
};

MENU_OPTION opCrossOver[] =
{
	{0x00, "FULL"},
	{0x01, "40Hz"},
	{0x03, "50Hz"},
	{0x05, "60Hz"},
	{0x07, "70Hz"},
	{0x09, "80Hz"},
	{0x0A, "90Hz"},
	{0x0B, "100Hz"},
	{0x0C, "110Hz"},	
	{0x0D, "120Hz"},
	{0x0E, "130Hz"},
	{0x10, "150Hz"},
};

MENU_OPTION opAssignedIput[]=
{
	{ASSIGNED_INPUT_PAGE,NULL}
};

MENU_OPTION opChangeInputNames[] = 
{
	{CHANGE_NAME_PAGE, NULL}
};

MENU_OPTION opSetAnalogInLevel[] = 
{
	{SET_ANALOG_LEVEL_PAGE, NULL}
};

MENU_OPTION opSetAnalogInConfig[] = 
{
	{SET_ANALOG_IN_PAGE, NULL}
};

MENU_OPTION opSetHDMIInAudio[]=
{
	{SET_HDMI_IN_AUDIO,NULL}
};

MENU_OPTION opListenProfilePage[] =
{
	{LISTENPROFILE_PAGE, NULL}
}; 

 MENU_OPTION opMovieSpeaker[]=
{
	{LIS_PRO_MOVIE_PAGE,NULL}
};

MENU_OPTION opMusicSpeaker[]=
{
	{LIS_PRO_MUSIC_PAGE,NULL}
};

MENU_OPTION opSpkConfigPage[] =
{
	{SPK_CONFIG_PAGE, NULL}
}; 

MENU_OPTION opSpkDistancePage[] =
{
	{SPK_DISTANCE_PAGE,NULL}
}; 

MENU_OPTION opSpkBalancePage[] =
{
	{SPK_BALANCE_PAGE,NULL}
}; 

MENU_OPTION opSpkCrossoverPage[] =
{
	{SPK_CROSSOVER_PAGE,NULL}
};

MENU_OPTION opSpkEqPage[] =
{
	{SPK_EQ_PAGE,NULL}
};

MENU_OPTION opSpkPhasePage[] =
{
	{SPK_PHASE_PAGE,NULL}
};

MENU_OPTION opSpkConfigPage_music[] =
{
	{SPK_CONFIG_PAGE_MUSIC, NULL}
};

MENU_OPTION opSpkDistancePage_music[] =
{
	{SPK_DISTANCE_PAGE_MUSIC,NULL}
};

MENU_OPTION opSpkBalancePage_music[] =
{
	{SPK_BALANCE_PAGE_MUSIC,NULL}
};

MENU_OPTION opSpkCrossoverPage_music[] =
{
	{SPK_CROSSOVER_PAGE_MUSIC,NULL}
};

MENU_OPTION opSpkEqPage_music[] =
{
	{SPK_EQ_PAGE_MUSIC,NULL}
};

MENU_OPTION opSpkPhasePage_music[] =
{
	{SPK_PHASE_PAGE_MUSIC,NULL}
}; 

MENU_OPTION opSpkPanoramaPage_music[]=
{
	{SPK_PANORAMA_PAGE_MUSIC,NULL}
};

MENU_OPTION opMainPage[1] =
{
	{MAIN_PAGE, NULL}
};

MENU_OPTION opZone2Page[1] =
{
	{ZONE2_PAGE, NULL}
};

MENU_OPTION opCrossOverPage[1] =
{
	{CROSSOVER_PAGE, NULL}
};

MENU_OPTION opDelayPage[1] =
{
	{DELAY_PAGE, NULL}
};

MENU_OPTION opAvSynPage[1] =
{
	{AV_SYN_PAGE, NULL}
};

MENU_OPTION opBalancePage[1] =
{
	{BLANCE_PAGE, NULL}
};

MENU_OPTION opInputPage[1] =
{
	{INPUT_PAGE, NULL}
};

MENU_OPTION opHDMIPage[1]=
{
	{SET_HDMI_IN_AUDIO,NULL}
};

MENU_OPTION opOtherPage[1] =
{
	{OTHER_PAGE, NULL}
};

MENU_OPTION opSignalPage[1] =
{
	{SIGNAL_PAGE, NULL}
};

MENU_OPTION opLogoPage[] = 
{
	{LOGO_PAGE, NULL}
};

MENU_OPTION opPasswordPage[] = 
{
	{PASSWORD_PAGE, NULL}
};

char strMovieSpeaker[]="Memory 1";
char strMusicSpeaker[]="Memory 2";  
char strMovieSpeakerSetting[]="MOVIE SPK Setting"; 
char strMusicSpeakerSetting[]="MUSIC SPK Setting"; 
MENU_ITEM ListenProfileItem[] =
{
	//pCap	uType	uOptionNum	pOptionArray	uPara	pAction
	{strMovieSpeaker, GOTO_PAGE_TYPE,
		ARRAY_SIZE(opMovieSpeaker), opMovieSpeaker, 0, SetupMenuDisplay},
	{strMusicSpeaker,GOTO_PAGE_TYPE,
		ARRAY_SIZE(opMusicSpeaker), opMusicSpeaker, 0, SetupMenuDisplay},	
	{strGotoMain, GOTO_PAGE_TYPE,
		ARRAY_SIZE(opMainPage), opMainPage, 0, SetupMenuDisplay}
}; 

char strSetSpeakerConfig[] = "Set Speaker Config";
char strSetDistance[] = "Set Distance";
char strSetBalance[] = "Set Balance";
char strSetCrossoverPoints[] = "Set Crossover Points";
char strSetRoomEq[] = "Set Room EQ";
char strSetSpeakerPhase[] = "Set Speaker Phase";
MENU_ITEM MovieSpeakerItem[] =
{
	//pCap	uType	uOptionNum	pOptionArray	uPara	pAction
	{strSetSpeakerConfig, GOTO_PAGE_TYPE,
		ARRAY_SIZE(opSpkConfigPage), opSpkConfigPage, 0, SetupMenuDisplay},

	{strSetDistance, GOTO_PAGE_TYPE,
		ARRAY_SIZE(opSpkDistancePage), opSpkDistancePage, 0, SetupMenuDisplay},

	{strSetBalance, GOTO_PAGE_TYPE,
		ARRAY_SIZE(opSpkBalancePage), opSpkBalancePage, 0, SetupMenuDisplay},

	{strSetCrossoverPoints, GOTO_PAGE_TYPE,
		ARRAY_SIZE(opSpkCrossoverPage), opSpkCrossoverPage, 0, SetupMenuDisplay},

	{strSetRoomEq, GOTO_PAGE_TYPE,
		ARRAY_SIZE(opSpkEqPage), opSpkEqPage, 0, SetupMenuDisplay},

	{strSetSpeakerPhase, GOTO_PAGE_TYPE,
		ARRAY_SIZE(opSpkPhasePage), opSpkPhasePage, 0, SetupMenuDisplay},

	{strBack, GOTO_PAGE_TYPE,
		ARRAY_SIZE(opListenProfilePage), opListenProfilePage, 0, SetupMenuDisplay}
}; 

char strSetSpeakerConfig_music[] = "Set Speaker Config";
char strSetDistance_music[] = "Set Distance";
char strSetBalance_music[] = "Set Balance";
char strSetCrossoverPoints_music[] = "Set Crossover Points";
char strSetRoomEq_music[] = "Set Room EQ";
char strSetSpeakerPhase_music[] = "Set Speaker Phase";
char strSetPanorama_music[]="Panorama Settings";
MENU_ITEM MusicSpeakerItem[] =
{
	//pCap	uType	uOptionNum	pOptionArray	uPara	pAction
	{strSetSpeakerConfig_music, GOTO_PAGE_TYPE,
		ARRAY_SIZE(opSpkConfigPage_music), opSpkConfigPage_music, 0, SetupMenuDisplay},

	{strSetDistance_music, GOTO_PAGE_TYPE,
		ARRAY_SIZE(opSpkDistancePage_music), opSpkDistancePage_music, 0, SetupMenuDisplay},

	{strSetBalance_music, GOTO_PAGE_TYPE,
		ARRAY_SIZE(opSpkBalancePage_music), opSpkBalancePage_music, 0, SetupMenuDisplay},

	{strSetCrossoverPoints_music, GOTO_PAGE_TYPE,
		ARRAY_SIZE(opSpkCrossoverPage_music), opSpkCrossoverPage_music, 0, SetupMenuDisplay},

	{strSetRoomEq_music, GOTO_PAGE_TYPE,
		ARRAY_SIZE(opSpkEqPage_music), opSpkEqPage_music, 0, SetupMenuDisplay},

	{strSetSpeakerPhase_music, GOTO_PAGE_TYPE,
		ARRAY_SIZE(opSpkPhasePage_music), opSpkPhasePage_music, 0, SetupMenuDisplay},
		
	{strSetPanorama_music,GOTO_PAGE_TYPE,
		ARRAY_SIZE(opSpkPanoramaPage_music),opSpkPanoramaPage_music,0,SetupMenuDisplay},

	{strBack, GOTO_PAGE_TYPE,
		ARRAY_SIZE(opListenProfilePage), opListenProfilePage, 1, SetupMenuDisplay}
}; 

MENU_OPTION SpkConfigOption[] = 
{
	{0x00,"No"},
	{0x01,"Yes"},
};

MENU_ITEM SpkConfigItem[] =
{
	//pCap	uType	uOptionNum	pOptionArray	uPara	pAction
	{strFl,GENERAL_ITEM_TYPE,
		ARRAY_SIZE(SpkConfigOption),SpkConfigOption, CH_L, SpkConfigSetup},

	{strCen,GENERAL_ITEM_TYPE,
		ARRAY_SIZE(SpkConfigOption),SpkConfigOption, CH_C, SpkConfigSetup},

	{strFr,GENERAL_ITEM_TYPE,
		ARRAY_SIZE(SpkConfigOption),SpkConfigOption, CH_R, SpkConfigSetup},

	{strSr,GENERAL_ITEM_TYPE,
		ARRAY_SIZE(SpkConfigOption),SpkConfigOption, CH_RS, SpkConfigSetup},

	{strSbr,GENERAL_ITEM_TYPE,
		ARRAY_SIZE(SpkConfigOption),SpkConfigOption, CH_RB, SpkConfigSetup},

	{strSbl,GENERAL_ITEM_TYPE,
		ARRAY_SIZE(SpkConfigOption),SpkConfigOption, CH_LB, SpkConfigSetup},

	{strSl,GENERAL_ITEM_TYPE,
		ARRAY_SIZE(SpkConfigOption),SpkConfigOption, CH_LS, SpkConfigSetup},

	{strSw,GENERAL_ITEM_TYPE,
		ARRAY_SIZE(SpkConfigOption),SpkConfigOption, CH_SW, SpkConfigSetup},

	{strBack, GOTO_PAGE_TYPE,
		ARRAY_SIZE(opMovieSpeaker), opMovieSpeaker, 0, SetupMenuDisplay} 
};

MENU_ITEM SpkConfigItem_Music[] =
{
	//pCap	uType	uOptionNum	pOptionArray	uPara	pAction   
	{strFl,GENERAL_ITEM_TYPE,
		ARRAY_SIZE(SpkConfigOption),SpkConfigOption, CH_L, SpkConfigSetup},

	{strCen,GENERAL_ITEM_TYPE,
		ARRAY_SIZE(SpkConfigOption),SpkConfigOption, CH_C, SpkConfigSetup},

	{strFr,GENERAL_ITEM_TYPE,
		ARRAY_SIZE(SpkConfigOption),SpkConfigOption, CH_R, SpkConfigSetup},

	{strSr,GENERAL_ITEM_TYPE,
		ARRAY_SIZE(SpkConfigOption),SpkConfigOption, CH_RS, SpkConfigSetup},

	{strSbr,GENERAL_ITEM_TYPE,
		ARRAY_SIZE(SpkConfigOption),SpkConfigOption, CH_RB, SpkConfigSetup},

	{strSbl,GENERAL_ITEM_TYPE,
		ARRAY_SIZE(SpkConfigOption),SpkConfigOption, CH_LB, SpkConfigSetup},

	{strSl,GENERAL_ITEM_TYPE,
		ARRAY_SIZE(SpkConfigOption),SpkConfigOption, CH_LS, SpkConfigSetup},

	{strSw,GENERAL_ITEM_TYPE,
		ARRAY_SIZE(SpkConfigOption),SpkConfigOption, CH_SW, SpkConfigSetup},

	{strBack, GOTO_PAGE_TYPE,
		ARRAY_SIZE(opMusicSpeaker), opMusicSpeaker, 0, SetupMenuDisplay}
};

MENU_ITEM SpkDistanceItem[] =
{
	//pCap	uType	uOptionNum	pOptionArray	uPara	pAction
	{strFl, DELAY_DISTANCE_ITEM_TYPE,
		101, NULL, CH_L, SpkDistanceSetup},

	{strCen, DELAY_DISTANCE_ITEM_TYPE,
		101, NULL, CH_C, SpkDistanceSetup},

	{strFr, DELAY_DISTANCE_ITEM_TYPE,
		101, NULL, CH_R, SpkDistanceSetup},

	{strSr, DELAY_DISTANCE_ITEM_TYPE,
		101, NULL, CH_RS, SpkDistanceSetup},

	{strSbr, DELAY_DISTANCE_ITEM_TYPE,
		101, NULL, CH_RB, SpkDistanceSetup},

	{strSbl, DELAY_DISTANCE_ITEM_TYPE,
		101, NULL, CH_LB, SpkDistanceSetup},

	{strSl, DELAY_DISTANCE_ITEM_TYPE,
		101, NULL, CH_LS, SpkDistanceSetup},

	{strSw, DELAY_DISTANCE_ITEM_TYPE,
		101, NULL, CH_SW, SpkDistanceSetup},

	{strBack, GOTO_PAGE_TYPE,
		ARRAY_SIZE(opMovieSpeaker), opMovieSpeaker, 1, SetupMenuDisplay} 
};

MENU_ITEM SpkDistanceItem_Music[] =
{
	//pCap	uType	uOptionNum	pOptionArray	uPara	pAction
	{strFl, DELAY_DISTANCE_ITEM_TYPE,
		101, NULL, CH_L, SpkDistanceSetup},

	{strCen, DELAY_DISTANCE_ITEM_TYPE,
		101, NULL, CH_C, SpkDistanceSetup},

	{strFr, DELAY_DISTANCE_ITEM_TYPE,
		101, NULL, CH_R, SpkDistanceSetup},

	{strSr, DELAY_DISTANCE_ITEM_TYPE,
		101, NULL, CH_RS, SpkDistanceSetup},

	{strSbr, DELAY_DISTANCE_ITEM_TYPE,
		101, NULL, CH_RB, SpkDistanceSetup},

	{strSbl, DELAY_DISTANCE_ITEM_TYPE,
		101, NULL, CH_LB, SpkDistanceSetup},

	{strSl, DELAY_DISTANCE_ITEM_TYPE,
		101, NULL, CH_LS, SpkDistanceSetup},

	{strSw, DELAY_DISTANCE_ITEM_TYPE,
		101, NULL, CH_SW, SpkDistanceSetup},

	{strBack, GOTO_PAGE_TYPE,
		ARRAY_SIZE(opMusicSpeaker), opMusicSpeaker, 1, SetupMenuDisplay}
};

MENU_OPTION OpTestMode[] = 
{
	{0x00,strOff},
	{0x01,"MANUAL"},
	{0x02,"AUTO"},
};

MENU_ITEM SpkSetBalanceItem[] =
{
	//pCap	uType	uOptionNum	pOptionArray	uPara	pAction
	{"TEST MODE",GENERAL_ITEM_TYPE,
		ARRAY_SIZE(OpTestMode), OpTestMode, NULL, SpkBlanceTestMode},

	{strFl, BALANCE_ITEM_TYPE,
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_L, SpkBlanceSetup},

	{strCen, BALANCE_ITEM_TYPE,
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_C, SpkBlanceSetup},

	{strFr, BALANCE_ITEM_TYPE,
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_R, SpkBlanceSetup},

	{strSr, BALANCE_ITEM_TYPE,
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_RS, SpkBlanceSetup},

	{strSbr, BALANCE_ITEM_TYPE,
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_RB, SpkBlanceSetup},

	{strSbl, BALANCE_ITEM_TYPE,
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_LB, SpkBlanceSetup},

	{strSl, BALANCE_ITEM_TYPE,
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_LS, SpkBlanceSetup},

	{strSw, BALANCE_ITEM_TYPE,
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_SW, SpkBlanceSetup},

	{strBack, GOTO_PAGE_TYPE,
		ARRAY_SIZE(opMovieSpeaker), opMovieSpeaker, 2, SetupMenuDisplay} 
};

MENU_ITEM SpkSetBalanceItem_Music[] =
{
	//pCap	uType	uOptionNum	pOptionArray	uPara	pAction
	{"TEST MODE",GENERAL_ITEM_TYPE,
		ARRAY_SIZE(OpTestMode), OpTestMode, NULL, SpkBlanceTestMode},

	{strFl, BALANCE_ITEM_TYPE,
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_L, SpkBlanceSetup},

	{strCen, BALANCE_ITEM_TYPE,
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_C, SpkBlanceSetup},

	{strFr, BALANCE_ITEM_TYPE,
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_R, SpkBlanceSetup},

	{strSr, BALANCE_ITEM_TYPE,
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_RS, SpkBlanceSetup},

	{strSbr, BALANCE_ITEM_TYPE,
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_RB, SpkBlanceSetup},

	{strSbl, BALANCE_ITEM_TYPE,
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_LB, SpkBlanceSetup},

	{strSl, BALANCE_ITEM_TYPE,
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_LS, SpkBlanceSetup},

	{strSw, BALANCE_ITEM_TYPE,
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_SW, SpkBlanceSetup},

	{strBack, GOTO_PAGE_TYPE,
		ARRAY_SIZE(opMusicSpeaker), opMusicSpeaker, 2, SetupMenuDisplay}
};

MENU_ITEM SpkSetCrossoverPointsItem[] =
{
	//pCap	uType	uOptionNum	pOptionArray	uPara	pAction
	{strFl, GENERAL_ITEM_TYPE,
		ARRAY_SIZE(opCrossOver), opCrossOver, CH_L, SpkCrossoverSetup},

	{strCen, GENERAL_ITEM_TYPE,
		ARRAY_SIZE(opCrossOver), opCrossOver, CH_C, SpkCrossoverSetup},

	{strFr, GENERAL_ITEM_TYPE,
		ARRAY_SIZE(opCrossOver), opCrossOver, CH_R, SpkCrossoverSetup},

	{strSr, GENERAL_ITEM_TYPE,
		ARRAY_SIZE(opCrossOver), opCrossOver, CH_RS, SpkCrossoverSetup},

	{strSbr, GENERAL_ITEM_TYPE,
		ARRAY_SIZE(opCrossOver), opCrossOver, CH_RB, SpkCrossoverSetup},

	{strSbl, GENERAL_ITEM_TYPE,
		ARRAY_SIZE(opCrossOver), opCrossOver, CH_LB, SpkCrossoverSetup},

	{strSl, GENERAL_ITEM_TYPE,
		ARRAY_SIZE(opCrossOver), opCrossOver, CH_LS, SpkCrossoverSetup},

	{strSw, GENERAL_ITEM_TYPE,
		ARRAY_SIZE(opCrossOver), opCrossOver, CH_SW, SpkCrossoverSetup},

	{strBack, GOTO_PAGE_TYPE,
		ARRAY_SIZE(opMovieSpeaker), opMovieSpeaker, 3, SetupMenuDisplay}
};

MENU_ITEM SpkSetCrossoverPointsItem_Music[] =
{
	//pCap	uType	uOptionNum	pOptionArray	uPara	pAction
	{strFl, GENERAL_ITEM_TYPE,
		ARRAY_SIZE(opCrossOver), opCrossOver, CH_L, SpkCrossoverSetup},

	{strCen, GENERAL_ITEM_TYPE,
		ARRAY_SIZE(opCrossOver), opCrossOver, CH_C, SpkCrossoverSetup},

	{strFr, GENERAL_ITEM_TYPE,
		ARRAY_SIZE(opCrossOver), opCrossOver, CH_R, SpkCrossoverSetup},

	{strSr, GENERAL_ITEM_TYPE,
		ARRAY_SIZE(opCrossOver), opCrossOver, CH_RS, SpkCrossoverSetup},

	{strSbr, GENERAL_ITEM_TYPE,
		ARRAY_SIZE(opCrossOver), opCrossOver, CH_RB, SpkCrossoverSetup},

	{strSbl, GENERAL_ITEM_TYPE,
		ARRAY_SIZE(opCrossOver), opCrossOver, CH_LB, SpkCrossoverSetup},

	{strSl, GENERAL_ITEM_TYPE,
		ARRAY_SIZE(opCrossOver), opCrossOver, CH_LS, SpkCrossoverSetup},

	{strSw, GENERAL_ITEM_TYPE,
		ARRAY_SIZE(opCrossOver), opCrossOver, CH_SW, SpkCrossoverSetup},

	{strBack, GOTO_PAGE_TYPE,
		ARRAY_SIZE(opMusicSpeaker), opMusicSpeaker, 3, SetupMenuDisplay}
};

MENU_ITEM SpkSetRoomEqItem[] =
{
	//pCap	uType	uOptionNum	pOptionArray	uPara	pAction
	{"FL  80Hz", BALANCE_ITEM_TYPE,
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_L | EQ_0  , SpkEqSetup},  
            
	{"FL  160Hz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_L | EQ_1 , SpkEqSetup},

	{"FL  250Hz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_L | EQ_2 , SpkEqSetup},
            
	{"FL  500Hz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_L | EQ_3 , SpkEqSetup},
		
    	{"FL  1kHz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_L | EQ_4 , SpkEqSetup},

	{"FL  2kHz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_L | EQ_5 , SpkEqSetup},
		
	{"FL  4kHz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_L | EQ_6, SpkEqSetup},

	{"FL  8kHz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_L | EQ_7, SpkEqSetup},

	{"FL  12kHz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_L | EQ_8, SpkEqSetup},

	{"FL  16kHz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_L | EQ_9, SpkEqSetup},
		
	{"CEN 80Hz", BALANCE_ITEM_TYPE,
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_C | EQ_0  , SpkEqSetup},
            
	{"CEN 160Hz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_C | EQ_1 , SpkEqSetup},
		
   	{"CEN 250Hz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_C | EQ_2 , SpkEqSetup},
		
	{"CEN 500Hz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_C | EQ_3 , SpkEqSetup},

	{"CEN 1kHz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_C | EQ_4 , SpkEqSetup},
	
	{"CEN 2kHz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_C | EQ_5 , SpkEqSetup},
           
	{"CEN 4kHz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_C | EQ_6, SpkEqSetup},

	{"CEN 8kHz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_C | EQ_7, SpkEqSetup},	
            
	{"CEN 12kHz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_C | EQ_8, SpkEqSetup},

	{"CEN 16kHz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_C | EQ_9, SpkEqSetup},

	{"FR  80Hz", BALANCE_ITEM_TYPE,
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_R | EQ_0  , SpkEqSetup},
            
	{"FR  160Hz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_R | EQ_1 , SpkEqSetup},

	{"FR  250Hz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_R | EQ_2 , SpkEqSetup},	
            
	{"FR  500Hz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_R | EQ_3 , SpkEqSetup},

	{"FR  1kHz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_R | EQ_4 , SpkEqSetup},		

	{"FR  2kHz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_R | EQ_5 , SpkEqSetup},			
            
	{"FR  4kHz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_R | EQ_6, SpkEqSetup},

	{"FR  8kHz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_R | EQ_7, SpkEqSetup},		
            
	{"FR  12kHz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_R | EQ_8, SpkEqSetup},

	{"FR  16kHz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_R | EQ_9, SpkEqSetup},

	{"SR  80Hz", BALANCE_ITEM_TYPE,
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_RS | EQ_0  , SpkEqSetup},
            
	{"SR  160Hz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_RS | EQ_1 , SpkEqSetup},

	{"SR  250Hz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_RS | EQ_2 , SpkEqSetup},		
            
	{"SR  500Hz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_RS | EQ_3 , SpkEqSetup},
		
	{"SR  1kHz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_RS | EQ_4 , SpkEqSetup},		

	{"SR  2kHz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_RS | EQ_5 , SpkEqSetup},				
            
	{"SR  4kHz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_RS | EQ_6, SpkEqSetup},

	{"SR  8kHz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_RS | EQ_7, SpkEqSetup},		
            
	{"SR  12kHz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_RS | EQ_8, SpkEqSetup},

	{"SR  16kHz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_RS | EQ_9, SpkEqSetup},

	{"SBR 80Hz", BALANCE_ITEM_TYPE,
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_RB | EQ_0  , SpkEqSetup},
            
	{"SBR 160Hz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_RB | EQ_1 , SpkEqSetup},

	{"SBR 250Hz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_RB | EQ_2 , SpkEqSetup},		
            
	{"SBR 500Hz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_RB | EQ_3 , SpkEqSetup},

	{"SBR 1kHz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_RB | EQ_4 , SpkEqSetup},		

	{"SBR 2kHz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_RB | EQ_5 , SpkEqSetup},			
            
	{"SBR 4kHz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_RB | EQ_6, SpkEqSetup},

	{"SBR 8kHz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_RB | EQ_7, SpkEqSetup},		
            
	{"SBR 12Hz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_RB | EQ_8, SpkEqSetup},

	{"SBR 16kHz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_RB | EQ_9, SpkEqSetup},

	{"SBL 80Hz", BALANCE_ITEM_TYPE,
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_LB | EQ_0  , SpkEqSetup},
            
	{"SBL 160Hz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_LB | EQ_1 , SpkEqSetup},

	{"SBL 250Hz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_LB | EQ_2 , SpkEqSetup},		
            
	{"SBL 500Hz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_LB | EQ_3 , SpkEqSetup},

	{"SBL 1kHz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_LB | EQ_4 , SpkEqSetup},	

	{"SBL 2kHz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_LB | EQ_5 , SpkEqSetup},			
            
	{"SBL 4kHz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_LB | EQ_6, SpkEqSetup},

	{"SBL 8kHz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_LB | EQ_7, SpkEqSetup},		
            
	{"SBL 12kHz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_LB | EQ_8, SpkEqSetup},

	{"SBL 16kHz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_LB | EQ_9, SpkEqSetup},
		
	{"SL  80Hz", BALANCE_ITEM_TYPE,
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_LS | EQ_0  , SpkEqSetup},
            
	{"SL  160Hz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_LS | EQ_1 , SpkEqSetup},

	{"SL  250Hz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_LS | EQ_2 , SpkEqSetup},		
            
	{"SL  500Hz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_LS | EQ_3 , SpkEqSetup},

	{"SL  1kHz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_LS | EQ_4 , SpkEqSetup},		

	{"SL  2kHz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_LS | EQ_5 , SpkEqSetup},				
            
	{"SL  4kHz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_LS | EQ_6, SpkEqSetup},

	{"SL  8kHz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_LS | EQ_7, SpkEqSetup},		
            
	{"SL  12kHz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_LS | EQ_8, SpkEqSetup},

	{"SL  16kHz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_LS | EQ_9, SpkEqSetup},	
		
	{"SW  25Hz", BALANCE_ITEM_TYPE,
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_SW | EQ_0  , SpkEqSetup},

	{"SW  40Hz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_SW | EQ_1 , SpkEqSetup},

	{"SW  50Hz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_SW | EQ_2 , SpkEqSetup},

	{"SW  63Hz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_SW | EQ_3, SpkEqSetup},

	{"SW  70Hz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_SW | EQ_4, SpkEqSetup},		

	{"SW  80Hz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_SW | EQ_5, SpkEqSetup},

	{"SW  90Hz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_SW | EQ_6, SpkEqSetup},		

	{"SW  100Hz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_SW | EQ_7, SpkEqSetup},

	{"SW  110Hz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_SW | EQ_8, SpkEqSetup},

	{"SW  125Hz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_SW | EQ_9, SpkEqSetup},		
		
	{strBack, GOTO_PAGE_TYPE,
		ARRAY_SIZE(opMovieSpeaker), opMovieSpeaker, 4, SetupMenuDisplay} 
};

MENU_ITEM SpkSetRoomEqItem_Music[] =
{
	//pCap	uType	uOptionNum	pOptionArray	uPara	pAction
	{"FL  80Hz", BALANCE_ITEM_TYPE,
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_L | EQ_0  , SpkEqSetup},
            
	{"FL  160Hz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_L | EQ_1 , SpkEqSetup},

	{"FL  250Hz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_L | EQ_2 , SpkEqSetup},
            
	{"FL  500Hz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_L | EQ_3 , SpkEqSetup},
		
    	{"FL  1kHz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_L | EQ_4 , SpkEqSetup},

	{"FL  2kHz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_L | EQ_5 , SpkEqSetup},
		
	{"FL  4kHz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_L | EQ_6, SpkEqSetup},

	{"FL  8kHz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_L | EQ_7, SpkEqSetup},

	{"FL  12kHz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_L | EQ_8, SpkEqSetup},

	{"FL  16kHz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_L | EQ_9, SpkEqSetup},

	{"CEN 80Hz", BALANCE_ITEM_TYPE,
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_C | EQ_0  , SpkEqSetup},
            
	{"CEN 160Hz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_C | EQ_1 , SpkEqSetup},
		
   	{"CEN 250Hz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_C | EQ_2 , SpkEqSetup},
		
	{"CEN 500Hz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_C | EQ_3 , SpkEqSetup},

	{"CEN 1kHz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_C | EQ_4 , SpkEqSetup},
	
	{"CEN 2kHz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_C | EQ_5 , SpkEqSetup},
           
	{"CEN 4kHz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_C | EQ_6, SpkEqSetup},

	{"CEN 8kHz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_C | EQ_7, SpkEqSetup},	
            
	{"CEN 12kHz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_C | EQ_8, SpkEqSetup},

	{"CEN 16kHz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_C | EQ_9, SpkEqSetup},

	{"FR  80Hz", BALANCE_ITEM_TYPE,
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_R | EQ_0  , SpkEqSetup},
            
	{"FR  160Hz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_R | EQ_1 , SpkEqSetup},

	{"FR  250Hz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_R | EQ_2 , SpkEqSetup},	
            
	{"FR  500Hz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_R | EQ_3 , SpkEqSetup},

	{"FR  1kHz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_R | EQ_4 , SpkEqSetup},		

	{"FR  2kHz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_R | EQ_5 , SpkEqSetup},			
            
	{"FR  4kHz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_R | EQ_6, SpkEqSetup},

	{"FR  8kHz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_R | EQ_7, SpkEqSetup},		
            
	{"FR  12kHz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_R | EQ_8, SpkEqSetup},

	{"FR  16kHz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_R | EQ_9, SpkEqSetup},

	{"SR  80Hz", BALANCE_ITEM_TYPE,
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_RS | EQ_0  , SpkEqSetup},
            
	{"SR  160Hz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_RS | EQ_1 , SpkEqSetup},

	{"SR  250Hz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_RS | EQ_2 , SpkEqSetup},		
            
	{"SR  500Hz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_RS | EQ_3 , SpkEqSetup},
		
	{"SR  1kHz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_RS | EQ_4 , SpkEqSetup},		

	{"SR  2kHz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_RS | EQ_5 , SpkEqSetup},				
            
	{"SR  4kHz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_RS | EQ_6, SpkEqSetup},

	{"SR  8kHz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_RS | EQ_7, SpkEqSetup},		
            
	{"SR  12kHz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_RS | EQ_8, SpkEqSetup},

	{"SR  16kHz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_RS | EQ_9, SpkEqSetup},

	{"SBR 80Hz", BALANCE_ITEM_TYPE,
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_RB | EQ_0  , SpkEqSetup},
            
	{"SBR 160Hz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_RB | EQ_1 , SpkEqSetup},

	{"SBR 250Hz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_RB | EQ_2 , SpkEqSetup},		
            
	{"SBR 500Hz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_RB | EQ_3 , SpkEqSetup},

	{"SBR 1kHz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_RB | EQ_4 , SpkEqSetup},		

	{"SBR 2kHz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_RB | EQ_5 , SpkEqSetup},			
            
	{"SBR 4kHz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_RB | EQ_6, SpkEqSetup},

	{"SBR 8kHz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_RB | EQ_7, SpkEqSetup},		
            
	{"SBR 12Hz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_RB | EQ_8, SpkEqSetup},

	{"SBR 16kHz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_RB | EQ_9, SpkEqSetup},

	{"SBL 80Hz", BALANCE_ITEM_TYPE,
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_LB | EQ_0  , SpkEqSetup},
            
	{"SBL 160Hz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_LB | EQ_1 , SpkEqSetup},

	{"SBL 250Hz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_LB | EQ_2 , SpkEqSetup},		
            
	{"SBL 500Hz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_LB | EQ_3 , SpkEqSetup},

	{"SBL 1kHz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_LB | EQ_4 , SpkEqSetup},	

	{"SBL 2kHz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_LB | EQ_5 , SpkEqSetup},			
            
	{"SBL 4kHz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_LB | EQ_6, SpkEqSetup},

	{"SBL 8kHz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_LB | EQ_7, SpkEqSetup},		
            
	{"SBL 12kHz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_LB | EQ_8, SpkEqSetup},

	{"SBL 16kHz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_LB | EQ_9, SpkEqSetup},
		
	{"SL  80Hz", BALANCE_ITEM_TYPE,
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_LS | EQ_0  , SpkEqSetup},
            
	{"SL  160Hz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_LS | EQ_1 , SpkEqSetup},

	{"SL  250Hz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_LS | EQ_2 , SpkEqSetup},		
            
	{"SL  500Hz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_LS | EQ_3 , SpkEqSetup},

	{"SL  1kHz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_LS | EQ_4 , SpkEqSetup},		

	{"SL  2kHz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_LS | EQ_5 , SpkEqSetup},				
            
	{"SL  4kHz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_LS | EQ_6, SpkEqSetup},

	{"SL  8kHz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_LS | EQ_7, SpkEqSetup},		
            
	{"SL  12kHz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_LS | EQ_8, SpkEqSetup},

	{"SL  16kHz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_LS | EQ_9, SpkEqSetup},	
		
	{"SW  25Hz", BALANCE_ITEM_TYPE,
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_SW | EQ_0  , SpkEqSetup},

	{"SW  40Hz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_SW | EQ_1 , SpkEqSetup},

	{"SW  50Hz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_SW | EQ_2 , SpkEqSetup},

	{"SW  63Hz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_SW | EQ_3, SpkEqSetup},

	{"SW  70Hz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_SW | EQ_4, SpkEqSetup},		

	{"SW  80Hz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_SW | EQ_5, SpkEqSetup},

	{"SW  90Hz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_SW | EQ_6, SpkEqSetup},		

	{"SW  100Hz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_SW | EQ_7, SpkEqSetup},

	{"SW  110Hz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_SW | EQ_8, SpkEqSetup},

	{"SW  125Hz", BALANCE_ITEM_TYPE,            
		ARRAY_SIZE(balance_table), (void *)balance_table, CH_SW | EQ_9, SpkEqSetup},		

	{strBack, GOTO_PAGE_TYPE,
		ARRAY_SIZE(opMusicSpeaker), opMusicSpeaker, 4, SetupMenuDisplay}
};

MENU_OPTION SpkPhaseOption[] = 
{
	{0x00,"In phase"},
	{0x01,"Outphase"},
};

MENU_ITEM SpkSetSpeakerPhaseItem[] =
{
	//pCap	uType	uOptionNum	pOptionArray	uPara	pAction
	{"FL Speaker", GENERAL_ITEM_TYPE,
		ARRAY_SIZE(SpkPhaseOption), SpkPhaseOption, CH_L, SpkPhaseSetup},
	{"CEN Speaker", GENERAL_ITEM_TYPE,
		ARRAY_SIZE(SpkPhaseOption), SpkPhaseOption, CH_C, SpkPhaseSetup},
	{"FR Speaker", GENERAL_ITEM_TYPE,
		ARRAY_SIZE(SpkPhaseOption), SpkPhaseOption, CH_R, SpkPhaseSetup},
	{"SR Speaker", GENERAL_ITEM_TYPE,
		ARRAY_SIZE(SpkPhaseOption), SpkPhaseOption, CH_RS, SpkPhaseSetup},
	{"SBR Speaker", GENERAL_ITEM_TYPE,
		ARRAY_SIZE(SpkPhaseOption), SpkPhaseOption, CH_RB, SpkPhaseSetup},
	{"SBL Speaker", GENERAL_ITEM_TYPE,
		ARRAY_SIZE(SpkPhaseOption), SpkPhaseOption, CH_LB, SpkPhaseSetup},
	{"SL Speaker", GENERAL_ITEM_TYPE,
		ARRAY_SIZE(SpkPhaseOption), SpkPhaseOption, CH_LS, SpkPhaseSetup},	
	{"SW Speaker", GENERAL_ITEM_TYPE,
		ARRAY_SIZE(SpkPhaseOption), SpkPhaseOption, CH_SW,SpkPhaseSetup},		
	{strBack, GOTO_PAGE_TYPE,
		ARRAY_SIZE(opMovieSpeaker), opMovieSpeaker, 5, SetupMenuDisplay} 
};

MENU_ITEM SpkSetSpeakerPhaseItem_Music[] =
{
	//pCap	uType	uOptionNum	pOptionArray	uPara	pAction
	{"FL Speaker", GENERAL_ITEM_TYPE,
		ARRAY_SIZE(SpkPhaseOption), SpkPhaseOption, CH_L, SpkPhaseSetup},    
	{"CEN Speaker", GENERAL_ITEM_TYPE,
		ARRAY_SIZE(SpkPhaseOption), SpkPhaseOption, CH_C, SpkPhaseSetup},
	{"FR Speaker", GENERAL_ITEM_TYPE,
		ARRAY_SIZE(SpkPhaseOption), SpkPhaseOption, CH_R, SpkPhaseSetup},
	{"SR Speaker", GENERAL_ITEM_TYPE,
		ARRAY_SIZE(SpkPhaseOption), SpkPhaseOption, CH_RS, SpkPhaseSetup},
	{"SBR Speaker", GENERAL_ITEM_TYPE,
		ARRAY_SIZE(SpkPhaseOption), SpkPhaseOption, CH_RB, SpkPhaseSetup},
	{"SBL Speaker", GENERAL_ITEM_TYPE,
		ARRAY_SIZE(SpkPhaseOption), SpkPhaseOption, CH_LB, SpkPhaseSetup},
	{"SL Speaker", GENERAL_ITEM_TYPE,
		ARRAY_SIZE(SpkPhaseOption), SpkPhaseOption, CH_LS, SpkPhaseSetup},	
	{"SW Speaker", GENERAL_ITEM_TYPE,
		ARRAY_SIZE(SpkPhaseOption), SpkPhaseOption, CH_SW, SpkPhaseSetup},	

	{strBack, GOTO_PAGE_TYPE,
		ARRAY_SIZE(opMusicSpeaker), opMusicSpeaker, 5, SetupMenuDisplay}  
};

char strDimension[] = "Dimension";
char strPanorama[]="Panorama";
char strWidth[]="Width";
char strDelay[]="Delay";

MENU_OPTION SpkDimensionOption[] = 
{
	{0x00,"-3"},
	{0x01,"-2"},
	{0x02,"-1"},
	{0x03,"0"},
	{0x04,"1"},
	{0x05,"2"},
	{0x06,"3"},
};

MENU_OPTION SpkPanoramaOption[]=
{
	{0x00,"Off"},	
	{0x01,"On"},		
};

MENU_OPTION SpkWidthOption[]=
{
	{0x00,"0"},	
	{0x01,"1"},	
	{0x02,"2"},	
	{0x03,"3"},	
	{0x04,"4"},	
	{0x05,"5"},	
	{0x06,"6"},	
	{0x07,"7"},	
};

MENU_OPTION SpkDelayOption[]=
{
	{0x00,"0ms"},
	{0x01,"1ms"},
	{0x02,"2ms"},
	{0x03,"3ms"},
	{0x04,"4ms"},
	{0x05,"5ms"},
	{0x06,"6ms"},
	{0x07,"7ms"},
	{0x08,"8ms"},
	{0x09,"9ms"},
	{0x10,"10ms"},
	{0x11,"11ms"},
	{0x12,"12ms"},
	{0x13,"13ms"},
	{0x14,"14ms"},
	{0x15,"15ms"},
};

MENU_ITEM SpkSetSpeakerPanoramaItem_Music[]=
{
	//pCap	uType	uOptionNum	pOptionArray	uPara	pAction
	{strDimension,GENERAL_ITEM_TYPE,
		ARRAY_SIZE(SpkDimensionOption), SpkDimensionOption,0,SpkDimensionSetup},
		
	{strPanorama,GENERAL_ITEM_TYPE,
		ARRAY_SIZE(SpkPanoramaOption),SpkPanoramaOption,0,SpkPanoramaSetup},
		
	{strWidth,GENERAL_ITEM_TYPE,
		ARRAY_SIZE(SpkWidthOption),SpkWidthOption,0,SpkWidthSetup},
		
	{strDelay,GENERAL_ITEM_TYPE,
		ARRAY_SIZE(SpkDelayOption),SpkDelayOption,0,SpkDelaySetup},
		
	{strBack,GOTO_PAGE_TYPE,
		ARRAY_SIZE(opMusicSpeaker), opMusicSpeaker, 6, SetupMenuDisplay}
};

char strInput7CH1[] = "7.1 In";
char strInput1[] = "INPUT1";
char strInput2[] = "INPUT2";
char strInput3[] = "INPUT3";
char strInput4[] = "INPUT4";
char strInput5[] = "INPUT5";
char strInput6[] = "INPUT6";
char strInput7[] = "INPUT7";
char strInput8[] = "INPUT8";
char strInputHDM1[]= "HDMI 1";
char strInputHDM2[]= "HDMI 2";
char strInputHDM3[]= "HDMI 3";
char strInputHDM4[]= "HDMI 4";
char strarcchannel[]= "TV SOUND";
char strInputTuner[] = "Tuner";

MENU_ITEM InputItem[] =
{
	//pCap	uType	uOptionNum	pOptionArray	uPara	pAction
	{strAssignedInput,GOTO_PAGE_TYPE,
		ARRAY_SIZE(opAssignedIput),opAssignedIput,0,SetupMenuDisplay},
		
	{strChangeInputNames, GOTO_PAGE_TYPE,
		ARRAY_SIZE(opChangeInputNames), opChangeInputNames, 0, SetupMenuDisplay},
		
	//change by cjm 2009-03-30
	{strSetAnalogInConfig, GOTO_PAGE_TYPE,
		ARRAY_SIZE(opSetAnalogInConfig), opSetAnalogInConfig, 0, SetupMenuDisplay},	

	//change by cjm 2009-03-30
	{strSetAnalogInLevel, GOTO_PAGE_TYPE,
		ARRAY_SIZE(opSetAnalogInLevel), opSetAnalogInLevel, 0, SetupMenuDisplay},

//	{strHDMIinAudio,GOTO_PAGE_TYPE,
//		ARRAY_SIZE(opSetHDMIInAudio),opSetHDMIInAudio,0,SetupMenuDisplay},

	{strGotoMain, GOTO_PAGE_TYPE,
		ARRAY_SIZE(opMainPage), opMainPage, 1, SetupMenuDisplay}
};

const short AnalogInLevelTable[] = 
{
		-18,		-17,		-16,		-15,		-14,		//0..4
		-13,		-12,		-11,		-10,		-9,		//5..9
		-8,  		-7,  		-6,  		-5,  		-4,		//10..14
		-3,  		-2,  		-1,  		  0,  		  1,		//15..19
		  2,		  3,		  4,		  5,		  6,		//20..24
		  7,		  8,		  9,		10,		11,		//25..29
		12										//30
};

MENU_OPTION Input1Option[] = 
{
	{0x01,"XLR Digital"},
	{0x02,"XLR Analog"},
};

MENU_OPTION Input2Option[]=
{
	{0x00,"Toslink 2"},
	{0x01,"Coaxial 2"},
	{0x02,"Analog 2"},
};


MENU_OPTION Input3Option[]=
{
	{0x00,"Toslink 3"},
	{0x01,"Coaxial 3"},
	{0x02,"Analog 3"},
};

MENU_OPTION Input4Option[]=
{
	{0x00,"Toslink 4"},
	{0x01,"Coaxial 4"},
	{0x02,"Analog 4"},
};

MENU_OPTION Input5Option[]=
{
	{0x00,"Toslink 5"},
	{0x01,"Coaxial 5"},
	{0x02,"Analog 5"},
};

MENU_OPTION Input6Option[]=
{
	{0x00,"Toslink 6"},
	{0x01,"Coaxial 6"},
	{0x02,"Analog 6"},
};

MENU_OPTION Input7Option[]=
{
	{0x00,"Toslink 7"},
	{0x01,"Coaxial 7"},
	{0x02,"Analog 7"},
};

MENU_OPTION Input8Option[]=
{
	{0x00,"Toslink 8"},
	{0x01,"Coaxial 8"},
	{0x02,"Analog 8"},
};

MENU_ITEM AssignedInputItem[]=
{
	//pCap	uType	uOptionNum	pOptionArray	uPara	pAction
	{strInput1,GENERAL_ITEM_TYPE,
		ARRAY_SIZE(Input1Option),Input1Option,INPUT_1,AssignedInputSetup},
		
	{strInput2,GENERAL_ITEM_TYPE,
		ARRAY_SIZE(Input2Option),Input2Option,INPUT_2,AssignedInputSetup},

	{strInput3,GENERAL_ITEM_TYPE,
		ARRAY_SIZE(Input3Option),Input3Option,INPUT_3,AssignedInputSetup},

	{strInput4,GENERAL_ITEM_TYPE,
		ARRAY_SIZE(Input4Option),Input4Option,INPUT_4,AssignedInputSetup},

	{strInput5,GENERAL_ITEM_TYPE,
		ARRAY_SIZE(Input5Option),Input5Option,INPUT_5,AssignedInputSetup},

	{strInput6,GENERAL_ITEM_TYPE,
		ARRAY_SIZE(Input6Option),Input6Option,INPUT_6,AssignedInputSetup},

	{strInput7,GENERAL_ITEM_TYPE,
		ARRAY_SIZE(Input7Option),Input7Option,INPUT_7,AssignedInputSetup},

	{strInput8,GENERAL_ITEM_TYPE,
		ARRAY_SIZE(Input8Option),Input8Option,INPUT_8,AssignedInputSetup},

	{strBack, GOTO_PAGE_TYPE,
		ARRAY_SIZE(opInputPage), opInputPage, 0, SetupMenuDisplay}
};

MENU_ITEM InputNameItem[] = 
{
	//pCap	uType	uOptionNum	pOptionArray	uPara	pAction
	{strInput7CH1, STRING_ITEM_TYPE,
		INPUT_NAME_LENGTH, (void *)SrcNameCh71, INPUT_CH71, InputNameSetup},

	{strInput1, STRING_ITEM_TYPE,
		INPUT_NAME_LENGTH, (void *)SrcNameInput1, INPUT_1, InputNameSetup},

	{strInput2, STRING_ITEM_TYPE,
		INPUT_NAME_LENGTH, (void *)SrcNameInput2, INPUT_2, InputNameSetup},

	{strInput3, STRING_ITEM_TYPE,
		INPUT_NAME_LENGTH, (void *)SrcNameInput3, INPUT_3, InputNameSetup},

	{strInput4, STRING_ITEM_TYPE,
		INPUT_NAME_LENGTH, (void *)SrcNameInput4, INPUT_4, InputNameSetup},

	{strInput5, STRING_ITEM_TYPE,
		INPUT_NAME_LENGTH, (void *)SrcNameInput5, INPUT_5, InputNameSetup},

	{strInput6, STRING_ITEM_TYPE,
		INPUT_NAME_LENGTH, (void *)SrcNameInput6, INPUT_6, InputNameSetup},

	{strInput7, STRING_ITEM_TYPE,
		INPUT_NAME_LENGTH, (void *)SrcNameInput7, INPUT_7, InputNameSetup},

	{strInput8, STRING_ITEM_TYPE,
		INPUT_NAME_LENGTH, (void *)SrcNameInput8, INPUT_8, InputNameSetup},

	{strInputHDM1,STRING_ITEM_TYPE,
		INPUT_NAME_LENGTH,(void *)SrcNameInputHDMI1,9,InputNameSetup},

	{strInputHDM2,STRING_ITEM_TYPE,
		INPUT_NAME_LENGTH,(void *)SrcNameInputHDMI2,10,InputNameSetup},

	{strInputHDM3,STRING_ITEM_TYPE,
		INPUT_NAME_LENGTH,(void *)SrcNameInputHDMI3,11,InputNameSetup},

	{strInputHDM4,STRING_ITEM_TYPE,
		INPUT_NAME_LENGTH,(void *)SrcNameInputHDMI4,12,InputNameSetup},
		
	{strarcchannel,STRING_ITEM_TYPE,
		INPUT_NAME_LENGTH,(void *)SrcNameInputARC,13,InputNameSetup},
		
	{strBack, GOTO_PAGE_TYPE,
		ARRAY_SIZE(opInputPage), opInputPage, 1, SetupMenuDisplay}  
};

MENU_ITEM AnalogInLevelItem[] =
{
	//pCap	uType	uOptionNum	pOptionArray	uPara	pAction
	{strInput1, VOLUME_ITEM_TYPE,
		ARRAY_SIZE(AnalogInLevelTable), (void *)AnalogInLevelTable, INPUT_1, AnalogLevelSetup},

	{strInput2, VOLUME_ITEM_TYPE,
		ARRAY_SIZE(AnalogInLevelTable), (void *)AnalogInLevelTable, INPUT_2, AnalogLevelSetup},

	{strInput3, VOLUME_ITEM_TYPE,
		ARRAY_SIZE(AnalogInLevelTable), (void *)AnalogInLevelTable, INPUT_3, AnalogLevelSetup},

	{strInput4, VOLUME_ITEM_TYPE,
		ARRAY_SIZE(AnalogInLevelTable), (void *)AnalogInLevelTable, INPUT_4, AnalogLevelSetup},

	{strInput5, VOLUME_ITEM_TYPE,
		ARRAY_SIZE(AnalogInLevelTable), (void *)AnalogInLevelTable, INPUT_5, AnalogLevelSetup},

	{strInput6, VOLUME_ITEM_TYPE,
		ARRAY_SIZE(AnalogInLevelTable), (void *)AnalogInLevelTable, INPUT_6, AnalogLevelSetup},

	{strInput7, VOLUME_ITEM_TYPE,
		ARRAY_SIZE(AnalogInLevelTable), (void *)AnalogInLevelTable, INPUT_7, AnalogLevelSetup},

	{strInput8, VOLUME_ITEM_TYPE,
		ARRAY_SIZE(AnalogInLevelTable), (void *)AnalogInLevelTable, INPUT_8, AnalogLevelSetup},

	{strBack, GOTO_PAGE_TYPE,
		ARRAY_SIZE(opInputPage), opInputPage, 3, SetupMenuDisplay} //change by cjm 2009-03-30
};

MENU_OPTION AnalogInOption[] = 
{
	{0x00,"Bypass"},
	{0x01,"DSP"},
};

MENU_ITEM AnalogInConfigItem[] =
{
	//pCap	uType	uOptionNum	pOptionArray	uPara	pAction
	{strInput1, GENERAL_ITEM_TYPE,
		ARRAY_SIZE(AnalogInOption), AnalogInOption, INPUT_1, AnalogInSetup},

	{strInput2, GENERAL_ITEM_TYPE,
		ARRAY_SIZE(AnalogInOption), AnalogInOption, INPUT_2, AnalogInSetup},

	{strInput3, GENERAL_ITEM_TYPE,
		ARRAY_SIZE(AnalogInOption), AnalogInOption, INPUT_3, AnalogInSetup},

	{strInput4, GENERAL_ITEM_TYPE,
		ARRAY_SIZE(AnalogInOption), AnalogInOption, INPUT_4, AnalogInSetup},

	{strInput5, GENERAL_ITEM_TYPE,
		ARRAY_SIZE(AnalogInOption), AnalogInOption, INPUT_5, AnalogInSetup},

	{strInput6, GENERAL_ITEM_TYPE,
		ARRAY_SIZE(AnalogInOption), AnalogInOption, INPUT_6, AnalogInSetup},

	{strInput7, GENERAL_ITEM_TYPE,
		ARRAY_SIZE(AnalogInOption), AnalogInOption, INPUT_7, AnalogInSetup},

	{strInput8, GENERAL_ITEM_TYPE,
		ARRAY_SIZE(AnalogInOption), AnalogInOption, INPUT_8, AnalogInSetup},

	{strInput7CH1, GENERAL_ITEM_TYPE,
		ARRAY_SIZE(AnalogInOption), AnalogInOption, INPUT_CH71, AnalogInSetup},

	{strInputTuner, GENERAL_ITEM_TYPE,
		ARRAY_SIZE(AnalogInOption), AnalogInOption, INPUT_TUNER, AnalogInSetup},

	{strBack, GOTO_PAGE_TYPE,
		ARRAY_SIZE(opInputPage), opInputPage, 2, SetupMenuDisplay} //change by cjm 2009-03-30
};

char strHDMI1[] = "HDMI1";
char strHDMI2[] = "HDMI2";
char strHDMI3[] = "HDMI3"; // jian add in 10-09-13
char strHDMI4[] = "HDMI4";

MENU_OPTION HDMIInAudioOption[] = 
{
	{0x00,"Bitstream"},
	{0x01,"LPCM"},
};

MENU_ITEM HDMIInAudioItem[]=
{
	//pCap	uType	uOptionNum	pOptionArray	uPara	pAction
	{strHDMI1,GENERAL_ITEM_TYPE,
		ARRAY_SIZE(HDMIInAudioOption),HDMIInAudioOption,HDMI_1,HDMIInAudioSetup},
	{strHDMI2,GENERAL_ITEM_TYPE,
		ARRAY_SIZE(HDMIInAudioOption),HDMIInAudioOption,HDMI_2,HDMIInAudioSetup},
	{strHDMI3,GENERAL_ITEM_TYPE,
		ARRAY_SIZE(HDMIInAudioOption),HDMIInAudioOption,HDMI_3,HDMIInAudioSetup},
	{strHDMI4,GENERAL_ITEM_TYPE,
		ARRAY_SIZE(HDMIInAudioOption),HDMIInAudioOption,HDMI_4,HDMIInAudioSetup},

	{strBack,GOTO_PAGE_TYPE,
		ARRAY_SIZE(opInputPage),opInputPage,4,SetupMenuDisplay}    
};

MENU_OPTION opZONE2OUT[] =
{
	{INPUT_1, strInput1},
	{INPUT_2, strInput2},
	{INPUT_3, strInput3},
	{INPUT_4, strInput4},
	{INPUT_5, strInput5},
	{INPUT_6, strInput6},
	{INPUT_7, strInput7},
	{INPUT_8, strInput8},
	{INPUT_CH71, strInput7CH1},
	{INPUT_TUNER | 0x20, "FM"},
	{INPUT_TUNER | 0x10, "AM"},
	{INPUT_TUNER | 0x40, "HDFM"},
	{INPUT_TUNER | 0X80, "HDAM"},
	{INPUT_HDMI1, "HDMI 1"},
	{INPUT_HDMI2, "HDMI 2"},
	{ARC_CHANNEL, "TV SOUND"},
	
};

MENU_ITEM Zone2Item[] =
{
	//pCap	uType	uOptionNum	pOptionArray	uPara	pAction
	{"Zone 2 Input",GENERAL_ITEM_TYPE,
		ARRAY_SIZE(opZONE2OUT), opZONE2OUT, NULL, Zone2InputSetup},

	{"Zone 2 Volume", VOLUME_ITEM_TYPE,
		ARRAY_SIZE(volume_table), (void *)volume_table, NULL, Zone2VolumeSetup},

	{strGotoMain, GOTO_PAGE_TYPE,
		ARRAY_SIZE(opMainPage), opMainPage, 2, SetupMenuDisplay}
};

char strSetMaxVolume[]="Max Volume Out";
char strLate[] = "Late";
char strBright[] = "Bright";
char strPassword[] = "Password";
char strHDMIAudioOut[]="HDMI Audio Out";
char strSetZ1MasterVolume[]="Z1 Master Volume"; 
char strSetZ2MasterVolume[]="Z2 Master Volume";
char strSetCDPureAudio[]="CD Pure Audio";
char strPasswordMode[] = "Password Mode";
char strPasswordChange[] = "Change";
char strAutoInputSeek[] = "Auto Input Seek";
char strIrControls[] ="IR Controls";
char strZone2IrControls[] ="ZONE2 IR Ctrls";
char strTriggers[] ="TRIGGER 1";
char strTriggers1[] ="TRIGGER 2";  // jian add in 10-09-14
char strTriggers2[] ="TRIGGER 3";// jian add in 10-09-14
char strRestoreDefault[] = "Restore Default";
char strAvSyncDelay[] = "AV SYNC Delay";
char strEditCustomName[] = "Edit Custom Name";

MENU_OPTION opLate[] =
{
	{0x00, strOff},
	{0x01, "Half"},
	{0x02, "Full"},
};

MENU_OPTION opPassword[] = 
{
	{0x00, strOff},
	{0x01, strOn},
};

MENU_OPTION opHDMIAudioOut[]=
{
	{0x01, strOff},
	{0x00, strOn},
};

MENU_OPTION opCDPureAudio[]=
{
	{0x00,strOff},
	{0x01,strOn},
};

MENU_OPTION opBright[] =
{
	{0x00, strOff},
	{0x01, "Low"},
	{0x02, "Mid"},
	{0x03, "High"},
};

MENU_OPTION opAutoInputSeek[] =
{
	{0x00, strOn},
	{0x01, strOff},
};

MENU_OPTION opIrControls[] =
{
	{0x00, "Front"},
	{0x01, "Rear"},
	{0x02, "Both"},
};

MENU_OPTION opIrMode[] =
{
	{0x00, "Normal"},
	{0x01, "Invert"},
};

#if 0
MENU_OPTION opTriggers[] =
{
	{0x00, "ZONE 1"},
	{0x01, "ZONE 2"},
	{0x02, "ZONE 1 & 2"},
};
#endif
MENU_OPTION opTriggers[] =
{
	{0x00, "OFF"},
	{0x01, "ZONE 1"},
	{0x02, "ZONE 2"},
};

MENU_ITEM OtherItem[] =
{
	//pCap	uType	uOptionNum	pOptionArray	uPara	pAction
	{strPassword, GOTO_PAGE_TYPE,
		ARRAY_SIZE(opPasswordPage), opPasswordPage, 0, SetupMenuDisplay},

	{strSetZ1MasterVolume,VOLUME_ITEM_TYPE,
		ARRAY_SIZE(volume_table),(void *)volume_table,0,MasterVolumeLevel},

	{strSetZ2MasterVolume,VOLUME_ITEM_TYPE,
		ARRAY_SIZE(volume_table),(void *)volume_table,0,Z2MasterVolumeLevel}, 		

	{strSetCDPureAudio,GENERAL_ITEM_TYPE,
		ARRAY_SIZE(opCDPureAudio),opCDPureAudio,NULL,CDPureAudio},

	{strHDMIAudioOut,GENERAL_ITEM_TYPE,
		ARRAY_SIZE(opHDMIAudioOut),opHDMIAudioOut,NULL,HDMIAUIDOOUT},

	{strAvSyncDelay, AV_DELAY_ITEM_TYPE, 
		101, NULL, NULL, AvSyncDelaySetup},

	{strBright, GENERAL_ITEM_TYPE,
		ARRAY_SIZE(opBright), opBright, NULL, BrightSetup},

	{strEditCustomName, GOTO_PAGE_TYPE,
		ARRAY_SIZE(opLogoPage), opLogoPage, 0, SetupMenuDisplay},

	{strIrControls, GENERAL_ITEM_TYPE,
		ARRAY_SIZE(opIrControls), opIrControls, NULL, IrControlSetup},

	{strZone2IrControls, GENERAL_ITEM_TYPE,
		ARRAY_SIZE(opIrControls), opIrControls, NULL, Zone2IrControlSetup},

	{"REAR IR MODE", GENERAL_ITEM_TYPE,
		ARRAY_SIZE(opIrMode), opIrMode, NULL, RearIrModeSetup},

	{"ZONE2 IR MODE", GENERAL_ITEM_TYPE,
		ARRAY_SIZE(opIrMode), opIrMode, NULL, Zone2IrModeSetup},

	{strTriggers, GENERAL_ITEM_TYPE,
		ARRAY_SIZE(opTriggers), opTriggers, NULL, TriggerSetup},

	{strTriggers1, GENERAL_ITEM_TYPE,
		ARRAY_SIZE(opTriggers), opTriggers, NULL, TriggerSetup1},

	{strTriggers2, GENERAL_ITEM_TYPE,
		ARRAY_SIZE(opTriggers), opTriggers, NULL, TriggerSetup2},

	{strAutoInputSeek, GENERAL_ITEM_TYPE,
		ARRAY_SIZE(opAutoInputSeek), opAutoInputSeek, NULL, AutoInputSeekSetup},

	{strLate, GENERAL_ITEM_TYPE,
		ARRAY_SIZE(opLate), opLate, NULL, LateSetup},

	{strSetMaxVolume,VOLUME_ITEM_TYPE, 
		ARRAY_SIZE(volume_table),(void *)volume_table,0,SetMaxVolumeLevel},

	{strRestoreDefault, NONE_OPTION_ITEM_TYPE,
		0, NULL, NULL, RestoreDefaultSetup},

	{strGotoMain, GOTO_PAGE_TYPE,
		ARRAY_SIZE(opMainPage), opMainPage, 3, SetupMenuDisplay}
};

MENU_ITEM PasswordItem[] = 
{
	//pCap	uType	uOptionNum	pOptionArray	uPara	pAction
	{strPasswordMode, GENERAL_ITEM_TYPE,
		ARRAY_SIZE(opPassword), opPassword, NULL, PasswordModeSetup},

	{strPasswordChange, PASSWORD_ITEM_TYPE,
		PASSWORD_LENGTH, /*(void *)cur_input_pwd*/NULL, NULL, PasswordSetup},

	{strBack, GOTO_PAGE_TYPE,
		ARRAY_SIZE(opOtherPage), opOtherPage, 0, SetupMenuDisplay}
};

MENU_ITEM EditCustomNameItem[] = 
{
	//pCap	uType	uOptionNum	pOptionArray	uPara	pAction
	{" ", STRING_ITEM_TYPE,
		PRODUCT_NAME_LENGTH, (void *)C11Logo, NULL, LogoSetup},

	{strBack, GOTO_PAGE_TYPE,
		ARRAY_SIZE(opOtherPage), opOtherPage, 7, SetupMenuDisplay} 
};

char strExit[] = "EXIT SETUP MENU";
MENU_ITEM MainPageItem[] =
{
	{strListenProfile,GOTO_PAGE_TYPE,
		ARRAY_SIZE(opListenProfilePage),opListenProfilePage,0,SetupMenuDisplay}, 

	{strInput, GOTO_PAGE_TYPE,
		ARRAY_SIZE(opInputPage), opInputPage, 0, SetupMenuDisplay},

	{strZone2, GOTO_PAGE_TYPE,
		ARRAY_SIZE(opZone2Page), opZone2Page, 0, SetupMenuDisplay},

	{strOther, GOTO_PAGE_TYPE,
		ARRAY_SIZE(opOtherPage), opOtherPage, 0, SetupMenuDisplay},

	{strExit, GOTO_PAGE_TYPE, NULL, NULL, NULL, SetupExitMenu}
};

char StrSetupMenu[] = "SETUP MENU";    
const MENU_PAGE MenuPageList[MENUPAGELIST_SIZE] =
{
	//	pCap		  uID		 uType	               uItemNum	  			pItemArray	
	{StrSetupMenu, MAIN_PAGE, OPTION_TYPE, ARRAY_SIZE(MainPageItem), MainPageItem}, //0

	{strListenProfile,LISTENPROFILE_PAGE,OPTION_TYPE,ARRAY_SIZE(ListenProfileItem),ListenProfileItem}, // 1
	{strMovieSpeakerSetting,LIS_PRO_MOVIE_PAGE,OPTION_TYPE,ARRAY_SIZE(MovieSpeakerItem),MovieSpeakerItem},  // 2
	{strMusicSpeakerSetting,LIS_PRO_MUSIC_PAGE,OPTION_TYPE,ARRAY_SIZE(MusicSpeakerItem),MusicSpeakerItem},  // 3
	
	{strSetSpeakerConfig, SPK_CONFIG_PAGE, OPTION_TYPE, ARRAY_SIZE(SpkConfigItem), SpkConfigItem}, // 4
	{strSetDistance, SPK_DISTANCE_PAGE, OPTION_TYPE, ARRAY_SIZE(SpkDistanceItem), SpkDistanceItem}, //5
	{strSetBalance, SPK_BALANCE_PAGE, OPTION_TYPE, ARRAY_SIZE(SpkSetBalanceItem), SpkSetBalanceItem}, //6
	{strSetCrossoverPoints, SPK_CROSSOVER_PAGE, OPTION_TYPE, ARRAY_SIZE(SpkSetCrossoverPointsItem), SpkSetCrossoverPointsItem}, //7
	{strSetRoomEq, SPK_EQ_PAGE, OPTION_TYPE, ARRAY_SIZE(SpkSetRoomEqItem), SpkSetRoomEqItem}, //8
	{strSetSpeakerPhase, SPK_PHASE_PAGE, OPTION_TYPE, ARRAY_SIZE(SpkSetSpeakerPhaseItem), SpkSetSpeakerPhaseItem}, //9

	{strSetSpeakerConfig_music, SPK_CONFIG_PAGE_MUSIC, OPTION_TYPE, ARRAY_SIZE(SpkConfigItem_Music), SpkConfigItem_Music}, //10
	{strSetDistance_music, SPK_DISTANCE_PAGE_MUSIC, OPTION_TYPE, ARRAY_SIZE(SpkDistanceItem_Music), SpkDistanceItem_Music}, //11
	{strSetBalance_music, SPK_BALANCE_PAGE_MUSIC, OPTION_TYPE, ARRAY_SIZE(SpkSetBalanceItem_Music), SpkSetBalanceItem_Music},  //12
	{strSetCrossoverPoints_music, SPK_CROSSOVER_PAGE_MUSIC, OPTION_TYPE, ARRAY_SIZE(SpkSetCrossoverPointsItem_Music), SpkSetCrossoverPointsItem_Music}, //13
	{strSetRoomEq_music, SPK_EQ_PAGE_MUSIC, OPTION_TYPE, ARRAY_SIZE(SpkSetRoomEqItem_Music), SpkSetRoomEqItem_Music}, //14
	{strSetSpeakerPhase_music, SPK_PHASE_PAGE_MUSIC, OPTION_TYPE, ARRAY_SIZE(SpkSetSpeakerPhaseItem_Music), SpkSetSpeakerPhaseItem_Music}, //15
	{strSetPanorama_music,SPK_PANORAMA_PAGE_MUSIC,OPTION_TYPE,ARRAY_SIZE(SpkSetSpeakerPanoramaItem_Music),SpkSetSpeakerPanoramaItem_Music}, //16

	{strInput, INPUT_PAGE, OPTION_TYPE, ARRAY_SIZE(InputItem), InputItem}, //17
	{strAssignedInput,ASSIGNED_INPUT_PAGE,OPTION_TYPE,ARRAY_SIZE(AssignedInputItem),AssignedInputItem}, //18
	{strChangeInputNames, CHANGE_NAME_PAGE, OPTION_TYPE, ARRAY_SIZE(InputNameItem), InputNameItem}, //19
	{strSetAnalogInConfig, SET_ANALOG_IN_PAGE, OPTION_TYPE, ARRAY_SIZE(AnalogInConfigItem), AnalogInConfigItem}, //20 //change by cjm 2009-03-30
	{strSetAnalogInLevel, SET_ANALOG_LEVEL_PAGE, OPTION_TYPE, ARRAY_SIZE(AnalogInLevelItem), AnalogInLevelItem}, //21 //change by cjm 2009-03-30
//	{strHDMIinAudio,SET_HDMI_IN_AUDIO,OPTION_TYPE,ARRAY_SIZE(HDMIInAudioItem),HDMIInAudioItem}, //22
// jian remove it in 10-11-12
	{strZone2, ZONE2_PAGE, OPTION_TYPE, ARRAY_SIZE(Zone2Item), Zone2Item}, //23

	{strOther, OTHER_PAGE, OPTION_TYPE, ARRAY_SIZE(OtherItem), OtherItem}, //24
	{strEditCustomName, LOGO_PAGE, OPTION_TYPE, ARRAY_SIZE(EditCustomNameItem), EditCustomNameItem}, //25
	{strPassword, PASSWORD_PAGE, OPTION_TYPE, ARRAY_SIZE(PasswordItem), PasswordItem}, //26
};

