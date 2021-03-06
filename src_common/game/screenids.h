#ifndef __SCREEN_IDS
#define __SCREEN_IDS


#include "CustomMap.h"


typedef enum ScreenTypes
{
	EDIT_SCREEN,
	SAVING_SCREEN,
	LOADING_SCREEN,
	ERROR_SCREEN,
	INIT_SCREEN,
	GAME_SCREEN,
	ANIEDIT_SCREEN,
	PALEDIT_SCREEN,
	DEBUG_SCREEN,
	MAP_SCREEN,
	LAPTOP_SCREEN,
	LOADSAVE_SCREEN,
	MAPUTILITY_SCREEN,
	FADE_SCREEN,
	MSG_BOX_SCREEN,
	MAINMENU_SCREEN,
	AUTORESOLVE_SCREEN,
	SAVE_LOAD_SCREEN,
	OPTIONS_SCREEN,
	SHOPKEEPER_SCREEN,
	SEX_SCREEN,
	GAME_INIT_OPTIONS_SCREEN,
	DEMO_EXIT_SCREEN,
	INTRO_SCREEN,
	CREDIT_SCREEN,

#ifdef ENABLE_CUSTOM_MAP_INTERFACE
	CUSTOM_MAP_LOAD_SCREEN,
	CUSTOM_MAP_BRIEFING_SCREEN,
	CUSTOM_MAP_MERC_CHOOSING_SCREEN,
#endif


	QUEST_DEBUG_SCREEN,

	IMPORT_SCREEN,

	MAX_SCREENS
};

#endif
