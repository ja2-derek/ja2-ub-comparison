#ifdef PRECOMPILEDHEADERS
	#include "Tactical All.h"
	#include "Ja25_Tactical.h"
	#include "_Ja25EnglishText.h"
	#include "Ja25 Strategic Ai.h"
	#include "end game.h"
#else
#endif


//*******************************************************************
//
// Local Defines
//
//*******************************************************************
;

//*******************************************************************
//
// Global Variables
//
//******************************************************************

//*******************************************************************
//
// Function Prototypes
//
//*******************************************************************
//ppp

//*******************************************************************
//
// Functions
//
//*******************************************************************




void HandleFadingOutToEndGameCinematics()
{
	gFadeOutDoneCallback = DoneFadeOutEndCinematic;

	FadeOutGameScreen( );	

	// UnLock UI!
	guiPendingOverrideEvent = LU_ENDUILOCK;
	HandleTacticalUI( );
}


