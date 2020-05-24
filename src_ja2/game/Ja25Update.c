#ifdef PRECOMPILEDHEADERS
	#include "JA2 All.h"
	#include "Ja25Update.h"
	#include "PreBattle Interface.h"
	#include "Strategic AI.h"
	#include "Ja25 Strategic Ai.h"
#endif


//** Defines *******************************************************************

//Starting money
#define JA2_5_STARTING_MONEY_EASY			40000
#define JA2_5_STARTING_MONEY_MEDIUM		35000
#define JA2_5_STARTING_MONEY_HARD			30000


#define	JA2SAVE_CONV_FILENAME		"..\\SavedGames\\Ja2.dts"

typedef struct
{
	INT8		bMercID;
	INT8		bPercentStatIncrease;
} STAT_CHANGE;

#define	NUM_MERCS_TO_USE_IN_PERCENT_STAT_INCREASE			6

#define	MAX_STAT_INCREASE															30

//******************************************************************************


//** GLOBAL VARIABLES ****************************************************************

STAT_CHANGE gJa2PlayerStatIncrease[NUM_MERCS_TO_USE_IN_PERCENT_STAT_INCREASE];
UINT8		gubTotalNumberMercsOnJa2PlayersTeam;
UINT32	guiTotalStatIncreaseFromMercsOnJa2PlayersTeam;


extern UINT32	guiJA2EncryptionSet;

extern	void GiveItemsToPC( UINT8 ubProfileId );

//******************************************************************************


//** Prototypes ****************************************************************

//******************************************************************************

void GivePlayerStartingJa25Money()
{
	INT32	iStartingCash=0;

	switch( gGameOptions.ubDifficultyLevel )
	{
		case DIF_LEVEL_EASY:
			iStartingCash	= JA2_5_STARTING_MONEY_EASY;
			break;

		case DIF_LEVEL_MEDIUM:
			iStartingCash	= JA2_5_STARTING_MONEY_MEDIUM;
			break;

		case DIF_LEVEL_HARD:
			iStartingCash	= JA2_5_STARTING_MONEY_HARD;
			break;

		default:
			Assert(0);
	}


	//add the players starting money
	AddTransactionToPlayersBook( ANONYMOUS_DEPOSIT, 0, GetWorldTotalMin(), iStartingCash );
}
