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
void		HandlePickingUpMorrisInstructionNote( SOLDIERTYPE *pSoldier, INT32 iIndex );
//ppp

//*******************************************************************
//
// Functions
//
//*******************************************************************




void HandlePickingUpMorrisInstructionNote( SOLDIERTYPE *pSoldier, INT32 iIndex )
{
	INT8	bID=-1;

	if( iIndex != MORRIS_INSTRUCTION_NOTE )
	{
		return;
	}

	//if the note has already been picked up by a new merc, or the not ehas been displayed
	if( gJa25SaveStruct.ubMorrisNoteState == MN__PICKED_UP_BY_NEW_MERC ||
			gJa25SaveStruct.ubMorrisNoteState == MN__FINISHED )
	{
		return;
	}

	bID = RandomSoldierIdFromNewMercsOnPlayerTeam();

	//if this is a qualified merc on the team
	if( IsSoldierQualifiedMerc( pSoldier ) )
	{
		//Remember we said the quote
		gJa25SaveStruct.ubMorrisNoteState = MN__PICKED_UP_BY_NEW_MERC;

		//Delaying the merc to say the note
		DelayedMercQuote( pSoldier->ubProfile, DQ__NEW_MERC_SAY_NOTE_QUOTES, GetWorldTotalSeconds()+1 );
	}

	//else if there is a new merc on the team
	else if( bID != -1 )
	{
		UINT32 uiTime=0;

		if( gJa25SaveStruct.ubMorrisNoteState == MN__PICKED_UP_BY_OLD_MERC_SAID_QUOTE_ALREADY )
		{
			return;
		}

		//the merc is not a new merc
		gJa25SaveStruct.ubMorrisNoteState = MN__PICKED_UP_BY_OLD_MERC_SAID_QUOTE_ALREADY;

		TacticalCharacterDialogue( &Menptr[ bID ], QUOTE_RENEWING_CAUSE_BUDDY_1_ON_TEAM );

		gJa25SaveStruct.bNewMercProfileIDForSayingMorrisNote = bID;

		//if we are in turnbased combat
		if( (gTacticalStatus.uiFlags & TURNBASED) && (gTacticalStatus.uiFlags & INCOMBAT) )
		{
			uiTime = GetWorldTotalSeconds() + 3;
		}
		else
		{
			uiTime = GetWorldTotalSeconds() + 10;
		}

		DelayedMercQuote( NOBODY, DQ__MORRIS_NOTE_NEW_MERC_DELAY, uiTime );
	}

	else
	{
		//
		// Create a merc popup box that will display a message telling player what to do
		//

		gJa25SaveStruct.bNewMercProfileIDForSayingMorrisNote = pSoldier->ubProfile;

		DelayedMercQuote( pSoldier->ubProfile, DQ__MORRIS_NOTE_DISPLAY_NOTE_1, GetWorldTotalSeconds() + 1 );

		gJa25SaveStruct.ubMorrisNoteState = MN__FINISHED;
	}
}

void HandleInitialEventsInHeliCrash()
{
	SOLDIERTYPE *pSoldier=NULL;
	INT32 cnt;

	//first, loop through all the mercs and injure them
	cnt = gTacticalStatus.Team[ OUR_TEAM ].bFirstID;
	for ( pSoldier = MercPtrs[ cnt ]; cnt <= gTacticalStatus.Team[ OUR_TEAM ].bLastID; cnt++,pSoldier++)
	{    
		//if the merc is alive
		if( pSoldier->bActive )
		{
			pSoldier->bLife -= ( 3 + Random( 5 ) );
			pSoldier->sBreathRed = ( 15 + Random( 15 ) ) * 100;
			pSoldier->bActionPoints = 0;
		}
	}
}
void DisplayCommanderMorrisNote( SOLDIERTYPE *pSoldier )
{
	CHAR16	zString[1024];

	if( gJa25SaveStruct.ubDisplayCommanderMorrisNote == DMN__FINISHED )
	{
		return;
	}

	if( pSoldier == NULL )
	{
		return;
	}

	//if it si the first part of the note to be displayed
	if( gJa25SaveStruct.ubDisplayCommanderMorrisNote == DMN__NOT_TO_DISPLAY_IT )
	{
		gJa25SaveStruct.ubDisplayCommanderMorrisNote = DMN__DISPLAY_PART_1; 

		swprintf( zString, zNewTacticalMessages[ TCTL_MSG__END_GAME_POPUP_TXT_1 ], pSoldier->name );
	}

	//if it si the second part of the note to be displayed
	else if( gJa25SaveStruct.ubDisplayCommanderMorrisNote == DMN__DISPLAY_PART_1 )
	{
		gJa25SaveStruct.ubDisplayCommanderMorrisNote = DMN__DISPLAY_PART_2; 

		swprintf( zString, zNewTacticalMessages[ TCTL_MSG__END_GAME_POPUP_TXT_2 ], pSoldier->name, pSoldier->name );
	}

	//Display it
	ExecuteTacticalTextBox( 110, zString );
}



void HandleCommanderMorrisNewMercWantsNoteDelayedSpeech()
{
	INT8 bID=-1;

	//if the note has already been picked up by a new merc, or the note has been displayed
	if( gJa25SaveStruct.ubMorrisNoteState == MN__PICKED_UP_BY_NEW_MERC ||
			gJa25SaveStruct.ubMorrisNoteState == MN__FINISHED )
	{
		return;
	}

	//if the original merc who said the quote is valid
	bID = gJa25SaveStruct.bNewMercProfileIDForSayingMorrisNote;
	if( bID != -1 && IsSoldierQualifiedMerc( &Menptr[ bID ] ) )
	{
	}
	else
	{
		bID = RandomSoldierIdFromNewMercsOnPlayerTeam();

		if( bID != -1 )
		{
			gJa25SaveStruct.bNewMercProfileIDForSayingMorrisNote = bID;
		}
		else
		{
			return;
		}
	}

	TacticalCharacterDialogue( &Menptr[ bID ], QUOTE_RENEWING_CAUSE_BUDDY_1_ON_TEAM );
}


void HandleShowingRadioLocatorsInMorrisArea()
{
	//Reveal the switch in Morris area
	HandleOpenControlPanelToRevealSwitchInMorrisArea();

	//control panel locater
	BeginMultiPurposeLocator( 15070, 0, TRUE );

	//if we are not in combat
	if( !((gTacticalStatus.uiFlags & TURNBASED) && (gTacticalStatus.uiFlags & INCOMBAT)) )
	{
		DelayedMercQuote( 14744, DQ__SHOW_RADIO_LOCATOR, GetWorldTotalSeconds() + 3 );
	}
}

void HandleNewMercSayingContentsOfMorrisNote( SOLDIERTYPE *pSoldier )
{
	TacticalCharacterDialogue( pSoldier, QUOTE_MERC_LEAVING_ALSUCO_SOON );

	TacticalCharacterDialogue( pSoldier, QUOTE_MERC_GONE_UP_IN_PRICE );
	SpecialCharacterDialogueEvent( DIALOGUE_SPECIAL_EVENT_JERRY_MILO, JM_SE__SHOW_RADIO_LOCATOR, 0, 0, 0, DIALOGUE_TACTICAL_UI );
}

void HandlePlayerHittingSwitchToLaunchMissles()
{
	INT32 cnt;
	SOLDIERTYPE *pSoldier=NULL;

	//
	// The player Just won the game, remeber this
	//
	gGameSettings.fPlayerFinishedTheGame = TRUE;
	SaveGameSettings();

	//Update the quest log
	InternalEndQuest( QUEST_DESTROY_MISSLES, -1, -1, TRUE );

	//lock the interface
	guiPendingOverrideEvent = LU_BEGINUILOCK;
	HandleTacticalUI( );

	//
	// Loop through all the mercs in the sector and make them run to the elevator
	//
	cnt = gTacticalStatus.Team[ OUR_TEAM ].bFirstID;
	for ( pSoldier = MercPtrs[ cnt ]; cnt <= gTacticalStatus.Team[ OUR_TEAM ].bLastID; cnt++,pSoldier++)
	{       
		// if the soldier was in the complex
		if( pSoldier->bActive && pSoldier->bLife >= OKLIFE && pSoldier->bInSector &&
				pSoldier->sSectorX == 15 && pSoldier->sSectorY == 12 && pSoldier->bSectorZ == 3 )
		{
			if( PythSpacesAway( pSoldier->sGridNo, 14268 ) < PythSpacesAway( pSoldier->sGridNo, 15708 ) )
				EVENT_InternalGetNewSoldierPath( pSoldier, 14268, RUNNING, TRUE, TRUE );
			else
				EVENT_InternalGetNewSoldierPath( pSoldier, 15708, RUNNING, TRUE, TRUE );
		}
	}

	//Start a timer so we can fade out
	DelayedMercQuote( NOBODY, DQ__FADE_OUT_TO_END_GAME_CINEMATIC, GetWorldTotalSeconds()+2 );
}
void HandleFadingOutToEndGameCinematics()
{
	gFadeOutDoneCallback = DoneFadeOutEndCinematic;

	FadeOutGameScreen( );	

	// UnLock UI!
	guiPendingOverrideEvent = LU_ENDUILOCK;
	HandleTacticalUI( );
}


void HandleOpenControlPanelToRevealSwitchInMorrisArea()
{
	//if the note has already been picked up by a new merc, or the note has been displayed
	if( gJa25SaveStruct.ubMorrisNoteState == MN__PICKED_UP_BY_NEW_MERC ||
			gJa25SaveStruct.ubMorrisNoteState == MN__FINISHED )
	{
		STRUCTURE *		pStructure;
		INT32					iItemIndex;
		ITEM_POOL *		pItemPool;
		

		// first, find the switch item and turn off its trap level

		if ( ItemTypeExistsAtLocation( 15231, SWITCH, 0, &iItemIndex ) )
		{
			gWorldItems[ iItemIndex ].o.bTrap = 0;
			GetItemPool( 15231, &pItemPool, 0 );
			if ( pItemPool )
			{
				SetItemPoolVisibilityOn( pItemPool, ANY_VISIBILITY_VALUE, FALSE );
			}
		}

		//Open up the Control Panel so the switch is visibile
		pStructure = FindStructure( 15231, STRUCTURE_GENERIC );
		if( pStructure == NULL )
		{
			return;
		}

		if( SwapStructureForPartnerForcingGraphicalChange( 15231, pStructure ) == NULL )
		{
			//an error occured
			return;
		}
	}
}

